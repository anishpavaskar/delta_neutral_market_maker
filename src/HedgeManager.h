#pragma once
#include <mutex>
#include <map>
#include <memory>
#include "MyIBClient.h"

class HedgeManager {
public:
    HedgeManager(std::shared_ptr<MyIBClient> ib);
    void onFill(const Contract& contract, double filled);
    double getInventory(const std::string& symbol);
private:
    std::mutex mu_;
    std::map<std::string,double> positions_;
    std::shared_ptr<MyIBClient> ib_;
    int orderId_;
};
