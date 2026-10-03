#include <vector>
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <algorithm>

using namespace std;

class Stock {
private:
    std::string companyName;
    std::string ticker;
    double currentPrice;
    string lastUpdated; //track last network sync time
    vector <double> priceHistory;

public:
    Stock(std::string t, std::string name, double price, string timestamp = "Never")
        :ticker(t), companyName(name), currentPrice(price), lastUpdated(timestamp)
    {
        priceHistory.push_back(price);
    }

    std::string getTicker() const { return ticker; }
    string getCompanyName() const { return companyName; }
    double getPrice() const { return currentPrice; }
    string getTimestamp() const { return lastUpdated; }
    const vector<double>& getHistory() const { return priceHistory; }

    void syncLivePrice(double newPrice, string currentTimestamp) {
        currentPrice = newPrice;
        lastUpdated = currentTimestamp;
        priceHistory.push_back(newPrice);
        if (priceHistory.size() > 5) {
            priceHistory.erase(priceHistory.begin());
        }
    }

    double calculatePrediction() const {
        if (priceHistory.size() < 2) {
            return currentPrice;
        }
        double sum = 0;
        for (double p : priceHistory) {
            sum += p;
        }
        double movingAverage = sum / priceHistory.size();
        double lastDelta = currentPrice - priceHistory[priceHistory.size() - 2];
        return movingAverage + (lastDelta * 0.5);
    }

    //converter of info to JSON

    string toJSON()const {
        stringstream json;
        json << "{"
            << "\"ticker\":\"" << ticker << "\","
            << "\"companyName\":\"" << companyName << "\","
            << "\"price\":" << currentPrice << ","
            << "\"forecast\":" << calculatePrediction() << ","
            << "\"lastUpdated\":\"" << lastUpdated << "\""
            << "}";
        return json.str();
    }

    void displayRow() const {
        double forecast = calculatePrediction();
        cout << left << setw(10) << ticker
            << setw(25) << companyName
            << "$" << fixed << setprecision(2) << setw(11) << currentPrice
            << "$" << fixed << setprecision(2) << setw(14) << forecast
            << right << setw(11) << lastUpdated << endl;

    }
};

//************************************
// PHASE 5 SERVER BROADCAST UTILITIES
//************************************

//funct that generates timestamps
string getCurrentTimeStr() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    struct tm timeinfo;

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&timeinfo, &now_c);
#else
    localtime_r(&now_c, &timeinfo);
#endif

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
    return std::string(buffer);
}

void fetchLiveMarketPrices(vector<Stock>& watchlist) {
    string currentTimestamp = getCurrentTimeStr();

    for (auto& stock : watchlist) {
        double changePercent = ((rand() % 300) - 150) / 100.0;
        double multiplier = 1.0 + (changePercent / 100.0);
        double linkedLivePrice = stock.getPrice() * multiplier;

        //push data changes
        stock.syncLivePrice(linkedLivePrice, currentTimestamp);
    }
}

void saveWatchlist(const vector<Stock>& watchlist) {
    ofstream outFile("watchlist.txt");

    if (!outFile) return;

    for (const auto& stock : watchlist) {
        outFile << stock.getTicker() << "|"
            << stock.getCompanyName() << "|"
            << stock.getPrice() << "|"
            << stock.getTimestamp() << endl;
        for (double h : stock.getHistory()) {
            outFile << "," << h;
        }
        outFile << endl;
    }
    outFile.close();
}

void loadWatchlist(vector<Stock>& watchlist) {
    ifstream inFile("watchlist.txt");

    if (!inFile) {
        watchlist.push_back(Stock("AAPL", "Apple Inc.", 241.50, "Static Default"));
        watchlist.push_back(Stock("TSLA", "Tesla Inc.", 260.85, "Static Default"));
        watchlist.push_back(Stock("MSFT", "Microsoft Corp.", 415.20, "Static Default"));
        return;
    }

    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string ticker, name, priceStr, timestamp, historyBlock;

        if (getline(ss, ticker, '|') &&
            getline(ss, name, '|') &&
            getline(ss, priceStr, '|') &&
            getline(ss, timestamp, '|')) {
            Stock tempStock(ticker, name, stod(priceStr), timestamp);
            if (getline(ss, historyBlock)) {
                stringstream histSS(historyBlock);
                string valStr;
                while (getline(histSS, valStr, ',')) {
                    if (!valStr.empty()) {
                        tempStock.syncLivePrice(stod(valStr), timestamp);
                    }
                }
            }
            watchlist.push_back(tempStock);
        }
    }

    inFile.close();
}

//API packaging logic

string packageWatchlistToJSON(const vector<Stock>& watchlist) {
    stringstream jsonList;
    jsonList << "[\n";

    for (size_t i = 0; i < watchlist.size(); ++i) {
        jsonList << "  " << watchlist[i].toJSON();
        if (i < watchlist.size() - 1) {
            jsonList << ",\n";
        }
    }
    jsonList << "\n]";
    return jsonList.str();
}

int main()
{
    //rand tool for testing, price changes tracker
    srand(static_cast<unsigned int>(time(0)));

    vector<Stock> watchlist;
    loadWatchlist(watchlist);

    //int choice = 0;

    std::cout << "==========================================\n";
    std::cout << "  STOCK TRACKING ENGINE - PHASE 5 \n";
    std::cout << "==========================================\n";

    fetchLiveMarketPrices(watchlist);

    cout << "Server Listening Loop Active on http://localhost:8080\n";
    cout << "Simulating Browser Network Call (HTTP GET /api/watchlist)....\n\n";

    string webResponsePacket = packageWatchlistToJSON(watchlist);
    cout << webResponsePacket << "\n";
    cout << "========================================================================\n";

    return 0;
}