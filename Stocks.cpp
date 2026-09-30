#include <vector>
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <cstdlib>

using namespace std;

class Stock {
private:
    std::string companyName;
    std::string ticker;
    double currentPrice;
    string lastUpdated; //track last network sync time

public:
    Stock(std::string t, std::string name, double price, string timestamp = "Never")
        :ticker(t), companyName(name), currentPrice(price), lastUpdated(timestamp) {}

    std::string getTicker() const { return ticker; }
    string getCompanyName() const { return companyName; }
    double getPrice() const { return currentPrice; }
    string getTimestamp() const { return lastUpdated; }

    void syncLivePrice(double newPrice, string currentTimestamp) {
        currentPrice = newPrice;
        lastUpdated = currentTimestamp;
    }

    void displayRow() const {
        std::cout << std::left << std::setw(10) << ticker <<
            std::setw(25) << companyName
            << setw(12) << ("$" + to_string(currentPrice).substr(0, to_string(currentPrice).find(".") + 3))
            << lastUpdated << endl;

    }
};

//part of phase 3, simulated network engine

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
        string ticker, name, priceStr, timestamp;

        if (getline(ss, ticker, '|') &&
            getline(ss, name, '|') &&
            getline(ss, priceStr, '|') &&
            getline(ss, timestamp, '|')) {

            watchlist.push_back(Stock(ticker, name, stod(priceStr), timestamp));
        }
    }

    inFile.close();
}

int main()
{
    //rand tool for testing price changes track
    srand(static_cast<unsigned int>(time(0)));

    vector<Stock> watchlist;

    //phase 2 auto load a watchlist
    loadWatchlist(watchlist);

    int choice = 0;

    std::cout << "==========================================\n";
    std::cout << "  STOCK TRACKING ENGINE - PHASE 2 \n";
    std::cout << "==========================================\n";

    while (choice != 4) {
        cout << "\n--- Main Menu ---\n";
        cout << "1. View Active Watchlist\n";
        cout << "2. Sync Live Market Prices.\n";
        cout << "3. Add new Stock Profile\n";
        cout << "4. Save & Exit. \n";
        cout << "Select and option: ";

        if (!(std::cin >> choice)) {
            cout << "Invalid numeric input. Resetting menu context.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
        case 1:
            std::cout << "\n------------------------------------------\n";
            std::cout << std::left << std::setw(10) << "TICKER"
                << std::setw(25) << "COMPANY NAME"
                << setw(12) << "PRICE"
                << "Last updated sync\n";
            std::cout << "------------------------------------------\n";
            for (const auto& stock : watchlist) {
                stock.displayRow();
            }
            std::cout << "------------------------------------------\n";
            break;

        case 2:
            cout << "\nConnecting to network... Fetching market data...";
            fetchLiveMarketPrices(watchlist);
            cout << "Sync complete! Data refreshed successfully.";
            break;

        case 3: {
            std::string tick, name;
            double price;
            std::cout << "Enter ticker: ";
            std::cin >> tick;
            std::cin.ignore(); // Clean buffer
            std::cout << "Enter company name: ";
            std::getline(std::cin, name);
            std::cout << "Enter current price: ";
            std::cin >> price;

            watchlist.push_back(Stock(tick, name, price, getCurrentTimeStr()));
            std::cout << "\nProfile initialized and appended to tracker.\n";
            break;
        }

        case 4:
            saveWatchlist(watchlist);
            cout << "Data saved. Shutting down safely. Active session closed.\n";
            break;

        default:
            std::cout << "Invalid menu item choice. Try again.\n";
        }
    }

    return 0;
}