#include "MyIBClient.h"
#include "Logger.h"

MyIBClient::MyIBClient()
    : client_(EClientSocket::create(this)), connected_(false) {}

void MyIBClient::connect(const char* host, int port, int clientId) {
    client_->eConnect(host, port, clientId);
    reader_ = std::thread([this]{
        while (client_->isConnected()) client_->checkMessages();
    });
    connected_ = true;
    Logger::instance()->info("Connected to IB at {}:{}", host, port);
}

void MyIBClient::reqMarketDepth(int reqId, const Contract& c, int numRows) {
    client_->reqMktDepth(reqId, c, numRows, TagValueListSPtr());
}

void MyIBClient::reqMktData(int reqId, const Contract& c) {
    client_->reqMktData(reqId, c, "", false, false, TagValueListSPtr());
}

void MyIBClient::placeLimitOrder(int orderId, const Contract& c, const Order& o) {
    client_->placeOrder(orderId, c, o);
}

void MyIBClient::updateMktDepthL2(TickerId, int, int, int side,
                                  double price, int size, bool) {
    // Implementation delegated to OrderBookManager via callback
}

void MyIBClient::tickPrice(TickerId, TickType, double, const TickAttrib&) {
    // Handle price ticks if needed
}

void MyIBClient::orderStatus(OrderId oid, const std::string& status, double filled,
                             double, double, int, int, double, int, const std::string&, double) {
    Logger::instance()->info("Order {} status {} filled {}", oid, status, filled);
}
