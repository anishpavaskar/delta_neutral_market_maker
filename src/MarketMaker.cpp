#include "MarketMaker.h"
#include "Logger.h"
#include <cmath>
#include <chrono>
#include <boost/asio.hpp>

MarketMaker::MarketMaker(std::shared_ptr<MyIBClient> ib,
                         std::shared_ptr<OrderBookManager> obm,
                         std::shared_ptr<HedgeManager> hm)
    : ib_(ib), obm_(obm), hm_(hm),
      gamma_(Config::instance().getDouble("gamma")),
      sigma_(Config::instance().getDouble("sigma")),
      kappa_(Config::instance().getDouble("kappaInit")),
      orderId_(Config::instance().getInt("startOrderId")),
      size_(Config::instance().getInt("orderSize")) {}

void MarketMaker::start(const Contract& c) {
    contract_ = c;
    ib_->reqMarketDepth(1, c, 5);
    ib_->reqMktData(2, c);
    loopThread_ = std::thread([this]{ run(); });
}

void MarketMaker::run() {
    boost::asio::io_context io;
    boost::asio::steady_timer timer(io);
    auto interval = std::chrono::milliseconds(Config::instance().getInt("quoteIntervalMs"));
    std::function<void()> tick = [this,&timer,interval](){
        computeAndSendQuotes();
        timer.expires_after(interval);
        timer.async_wait([&](auto){ tick(); });
    };
    timer.expires_after(interval);
    timer.async_wait([&](auto){ tick(); });
    io.run();
}

void MarketMaker::computeAndSendQuotes() {
    double mid = obm_->getMidPrice(contract_.symbol);
    double inv = hm_->getInventory(contract_.symbol);
    // risk limit
    double limit = 0.025 * Config::instance().getDouble("portfolioNAV");
    if (std::abs(inv * mid) >= limit) return;
    if (qp_.throttleActive()) return;

    double t = timeFraction();
    double dt = 1.0 - t;
    double r  = mid - inv * gamma_ * sigma_ * sigma_ * dt;
    double common = 0.5 * gamma_ * sigma_ * sigma_ * dt;
    double delta = common + (1.0/gamma_) * log(1.0 + gamma_/kappa_);
    sendQuote(r - delta, r + delta);
}

void MarketMaker::sendQuote(double bid, double ask) {
    Order o;
    o.action = "BUY"; o.orderType = "LMT"; o.totalQuantity = size_; o.lmtPrice = bid;
    if (!qp_.isStale(orderId_)) {
        ib_->placeLimitOrder(orderId_, contract_, o);
        qp_.registerSent(orderId_);
    }
    o.action = "SELL"; o.lmtPrice = ask;
    if (!qp_.isStale(orderId_+1)) {
        ib_->placeLimitOrder(orderId_+1, contract_, o);
        qp_.registerSent(orderId_+1);
    }
    orderId_ += 2;
}

double MarketMaker::timeFraction() {
    using namespace std::chrono;
    static auto start = high_resolution_clock::now();
    duration<double> diff = high_resolution_clock::now() - start;
    return fmod(diff.count(), 60.0) / 60.0;
}
