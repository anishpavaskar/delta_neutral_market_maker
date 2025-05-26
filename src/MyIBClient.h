#pragma once
#include "EWrapper.h"
#include "EClientSocket.h"
#include <thread>
#include <atomic>

using namespace IBApi;

class MyIBClient : public EWrapper {
public:
    MyIBClient();
    void connect(const char* host, int port, int clientId);
    void reqMarketDepth(int reqId, const Contract& contract, int numRows);
    void reqMktData(int reqId, const Contract& contract);
    void placeLimitOrder(int orderId, const Contract& contract, const Order& order);

    // EWrapper callbacks
    void updateMktDepthL2(TickerId id, int position, int operation, int side,
                          double price, int size, bool isSmartDepth) override;
    void tickPrice(TickerId tickerId, TickType field, double price,
                   const TickAttrib& attribs) override;
    void orderStatus(OrderId orderId, const std::string& status, double filled,
                     double remaining, double avgFillPrice, int permId,
                     int parentId, double lastFillPrice, int clientId,
                     const std::string& whyHeld, double mktCapPrice) override;
private:
    EClient* client_;
    std::thread reader_;
    std::atomic<bool> connected_;
};
