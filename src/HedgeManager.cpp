#include "HedgeManager.h"
#include "Logger.h"

HedgeManager::HedgeManager(std::shared_ptr<MyIBClient> ib)
    : ib_(ib), orderId_(Config::instance().getInt("startHedgeOrderId")) {}

void HedgeManager::onFill(const Contract& c, double filled) {
    std::lock_guard<std::mutex> lock(mu_);
    positions_[c.symbol] += filled;
    Order h;
    h.action = (filled>0?"SELL":"BUY");
    h.orderType = "MKT";
    h.totalQuantity = std::abs((int)filled);
    ib_->placeOrder(orderId_++, c, h);
    Logger::instance()->info("Hedged {} shares of {}", h.totalQuantity, c.symbol);
}

double HedgeManager::getInventory(const std::string& sym) {
    std::lock_guard<std::mutex> lock(mu_);
    return positions_[sym];
}
