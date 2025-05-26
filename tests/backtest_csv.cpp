#include <gtest/gtest.h>
#include <fstream>
#include <sstream>

TEST(BacktestCSVTest, PnLMonotonic) {
    std::ifstream file("backtest_sample.csv");
    ASSERT_TRUE(file.is_open());
    std::string line;
    // skip header
    std::getline(file, line);
    double prevPnL = -1e9;
    int count = 0;
    while (std::getline(file, line) && count < 100) {
        std::stringstream ss(line);
        std::string time, mid, bid, ask, inv, pnl;
        std::getline(ss, time, ',');
        std::getline(ss, mid, ',');
        std::getline(ss, bid, ',');
        std::getline(ss, ask, ',');
        std::getline(ss, inv, ',');
        std::getline(ss, pnl, ',');
        double dPnL = std::stod(pnl);
        EXPECT_GE(dPnL, prevPnL) << "PnL decreased at row " << count;
        prevPnL = dPnL;
        ++count;
    }
    EXPECT_EQ(count, 100);
}
