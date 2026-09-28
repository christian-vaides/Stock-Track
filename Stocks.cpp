
#include <vector>
#include <iostream>
#include <string>
#include <iomanip>

class Stock {
private:
    std::string companyName;
    std::string ticker;
    double CurrentPrice;

public:
    Stock(std::string t, std::string name, double price)
    :ticker(t), companyName(name), CurrentPrice(price){}

    std::string getTicker() const { return ticker;}

    void displayRow() const {
        std::cout << std::left << std::setw(10) << ticker <<
            std::setw(25) << companyName << "$" <<
            std::fixed << std::setprecision(2) << CurrentPrice << '\n';
    }
};

int main()
{
    std::vector<Stock> watchlist;

    watchlist.push_back(Stock("APPL", "Apple Inc.", 241.50));
    watchlist.push_back(Stock("TSLA", "Tesla Inc.", 260.85));
    watchlist.push_back(Stock("MSFT", "Microsoft corp", 415.20));

    int choice = 0;

    std::cout << "==========================================\n";
    std::cout << "  STOCK TRACKING ENGINE - PHASE 1 \n";
    std::cout << "==========================================\n";

    while (choice != 3) {
        std::cout << "\n--- Main Menu ---\n";
        std::cout << "1. View Current Watchlist\n";
        std::cout << "2. Quick Add Mock Stock\n";
        std::cout << "3. Exit Engine\n";
        std::cout << "Select an option: ";

        if (!(std::cin >> choice)) {
            std::cout << "Invalid numeric input. Resetting menu context.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
        case 1:
            std::cout << "\n------------------------------------------\n";
            std::cout << std::left << std::setw(10) << "TICKER"
                << std::setw(25) << "COMPANY NAME"
                << "PRICE\n";
            std::cout << "------------------------------------------\n";
            for (const auto& stock : watchlist) {
                stock.displayRow();
            }
            std::cout << "------------------------------------------\n";
            break;

        case 2: {
            std::string tick, name;
            double price;
            std::cout << "Enter uppercase ticker (e.g., NVDA): ";
            std::cin >> tick;
            std::cin.ignore(); // Clean buffer
            std::cout << "Enter company name: ";
            std::getline(std::cin, name);
            std::cout << "Enter mock starting price: ";
            std::cin >> price;

            watchlist.push_back(Stock(tick, name, price));
            std::cout << "\nSuccess: Added " << tick << " to active memory tracking loop.\n";
            break;
        }

        case 3:
            std::cout << "\nShutting down engine thread safety safely. Active session closed.\n";
            break;

        default:
            std::cout << "Invalid menu item choice. Try again.\n";
        }
    }

    return 0;
}
