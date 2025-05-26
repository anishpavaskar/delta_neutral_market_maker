#include "Config.h"
#include "Logger.h"
#include "MyIBClient.h"
#include "OrderBookManager.h"
#include "HedgeManager.h"
#include "MarketMaker.h"

int main() {
    Config::instance().load("config.yaml");
    Logger::instance()->info("Starting MarketMaker...");

    auto ib = std::make_shared<MyIBClient>();
    ib->connect(
        Config::instance().getString("ibHost").c_str(),
        Config::instance().getInt("ibPort"), 0
    );

    auto obm = std::make_shared<OrderBookManager>();
    auto hm  = std::make_shared<HedgeManager>(ib);
    MarketMaker mm(ib, obm, hm);

    Contract c;
    c.symbol   = "AAPL";
    c.secType  = "STK";
    c.currency = "USD";
    c.exchange = "SMART";
    mm.start(c);

    std::this_thread::sleep_for(std::chrono::hours(24));
    return 0;
}
