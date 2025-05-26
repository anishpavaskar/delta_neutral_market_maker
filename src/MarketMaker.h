#pragma once
#include <memory>
#include <thread>
#include "Config.h"
#include "QuoteProtection.h"
#include "OrderBookManager.h"
#include "HedgeManager.h"
#include "MyIBClient.h"

class MarketMaker {
public:
    MarketMaker(std::shared_ptr<MyIBClient> ib,
                std::shared_ptr<OrderBookManager> obm,
                std::shared_ptr<HedgeManager> hm);
    void start(const Contract& contract);
private:
    void run();
    void computeAndSendQuotes();
    void sendQuote(double bid, double ask);
    double timeFraction();

    std::shared_ptr<MyIBClient> ib_;
    std::shared_ptr<OrderBookManager> obm_;
    std::shared_ptr<HedgeManager> hm_;
    QuoteProtection qp_;
    std::thread loopThread_;
    Contract contract_;
    double gamma_, sigma_, kappa_;
    int orderId_, size_;
};
