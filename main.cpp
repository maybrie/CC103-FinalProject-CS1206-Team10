#include <algorithm>
#include <deque>
#include <iostream>
#include <unordered_set>
#include <stack>
#include <string>
#include <vector>

using namespace std;

struct Booking {
    int movieIndex;
    int seat;
    int ticketID;
};

struct BookingRecord {
    string customerName;
    string movieName;
    string date;
    string showtime;
    int seat;
    string type;
    string paymentInfo;
    double finalPrice;
    int ticketID;
    string status;
};

struct User {
    string name;
    bool vip;
    int seq;
};

stack<Booking> bookingHistory;
vector<BookingRecord> bookingLedger;
deque<User> userQueue;
unordered_set<string> usedNames;
User currentServedUser = {"", false, 0};
int userSeq = 0;
int ticketID = 1000;

const string ADMIN_NAME = "Admin";
const string ADMIN_PASSWORD = "Admin@123";
const int VIP_SEAT_COUNT = 3;
const int DEFAULT_SEAT_COUNT = 10;

vector<string> movies = {};
vector<string> times = {};
vector<int> moviePrices = {};
vector<string> showingUntil = {};
vector<int> vipSeatCounts = {};
vector<vector<bool>> seats(movies.size(), vector<bool>(DEFAULT_SEAT_COUNT, false));

string trimCopy(const string& text);

char toLowerAscii(char c) {
    if (c >= 'A' && c <= 'Z') return static_cast<char>(c - 'A' + 'a');
    return c;
}

char toUpperAscii(char c) {
    if (c >= 'a' && c <= 'z') return static_cast<char>(c - 'a' + 'A');
    return c;
}

void clearInputLine() {
    cin.ignore(static_cast<streamsize>(1000000), '\n');
}

string trimCopy(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

string normalizeName(const string& text) {
    string normalized = trimCopy(text);
    transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char c) {
        return toLowerAscii(static_cast<char>(c));
    });
    return normalized;
}

int readInt() {
    int val;
    while (!(cin >> val)) {
        cin.clear();
        clearInputLine();
        cout << "Invalid input. Try again: ";
    }
    clearInputLine();
    return val;
}

char readChar() {
    char c;
    cin.get(c);
    return c;
}

string readString() {
    string s;
    getline(cin >> ws, s);
    return s;
}

bool isReservedAdminName(const string& name) {
    return normalizeName(name) == normalizeName(ADMIN_NAME);
}

bool authenticateAdmin() {
    string password;
    cout << "Enter admin password: ";
    password = readString();
    return password == ADMIN_PASSWORD;
}

bool verifyAdminAction() {
    return true;
}

void viewVipSeats(int m) {
    if (m < 0 || m >= (int)vipSeatCounts.size()) return;
    cout << "\n--- VIP Seats for " << movies[m] << " ---\n";
    int vipLimit = vipSeatCounts[m];
    if (vipLimit == 0) {
        cout << "No VIP seats configured for this movie.\n";
        return;
    }
    for (int i = 0; i < vipLimit && i < (int)seats[m].size(); i++) {
        cout << "Seat " << i + 1 << ": " << (seats[m][i] ? "Booked" : "Available") << '\n';
    }
}

void viewRegularSeats(int m) {
    if (m < 0 || m >= (int)vipSeatCounts.size()) return;
    cout << "\n--- Regular Seats for " << movies[m] << " ---\n";
    int vipLimit = vipSeatCounts[m];
    if (vipLimit >= (int)seats[m].size()) {
        cout << "No regular seats configured for this movie.\n";
        return;
    }
    for (int i = vipLimit; i < (int)seats[m].size(); i++) {
        cout << "Seat " << i + 1 << ": " << (seats[m][i] ? "Booked" : "Available") << '\n';
    }
}

void viewSeatsByType(int m, const string& type) {
    if (type == "VIP") viewVipSeats(m);
    else viewRegularSeats(m);
}

void sortBooked(int m) {
    vector<int> booked;
    for (int i = 0; i < (int)seats[m].size(); i++) {
        if (seats[m][i]) booked.push_back(i + 1);
    }

    if (booked.empty()) {
        cout << "No booked seats for " << movies[m] << " yet.\n";
        return;
    }

    sort(booked.begin(), booked.end());
    cout << "Sorted booked seats for " << movies[m] << ": ";
    for (int seatNumber : booked) cout << seatNumber << ' ';
    cout << '\n';
}

void appendBookingRecord(const string& customerName, int movieIndex, int seatNumber, const string& type, double finalPrice, const string& date, const string& paymentInfo) {
    bookingLedger.push_back({customerName, movies[movieIndex], date, times[movieIndex], seatNumber, type, paymentInfo, finalPrice, ticketID, "CONFIRMED"});
}

void bookSeats(int count, int m, const string& type, const string& customerName) {
    if (count == 0) return;

    viewSeatsByType(m, type);

    cout << "\nEnter seat number to book: ";
    int seat = readInt();

    int maxSeat = (int)seats[m].size();
    int vipLimit = vipSeatCounts[m];
    if (seat < 1 || seat > maxSeat) {
        cout << "Invalid seat number. Must be 1-" << maxSeat << ".\n";
        bookSeats(count, m, type, customerName);
        return;
    }

    if (type == "VIP" && seat > vipLimit) {
        cout << "VIP customers can only book seats 1-" << vipLimit << ".\n";
        bookSeats(count, m, type, customerName);
        return;
    }

    if (type == "Regular" && seat <= vipLimit) {
        cout << "Seats 1-" << vipLimit << " are reserved for VIP customers.\n";
        bookSeats(count, m, type, customerName);
        return;
    }

    if (seats[m][seat - 1]) {
        cout << "Seat " << seat << " is already booked. Please choose another.\n";
        bookSeats(count, m, type, customerName);
        return;
    }

    int basePrice = moviePrices[m];
    double finalPrice = basePrice;
    if (type == "VIP") finalPrice = basePrice * 0.90;

    cout << "\n========== BOOKING SUMMARY ==========" << '\n';
    cout << "Customer   : " << customerName << '\n';
    cout << "Movie      : " << movies[m] << '\n';
    cout << "Showtime   : " << times[m] << '\n';
    cout << "Seat       : " << seat << '\n';
    cout << "Type       : " << type << '\n';
    cout << "Base Price : PHP " << basePrice << '\n';
    if (type == "VIP") cout << "VIP Disc.  : 10%\n";
    cout << "Final Price: PHP " << finalPrice << '\n';
    cout << "=====================================" << '\n';

    string date;
    cout << "\nEnter booking date (YYYY-MM-DD): ";
    date = readString();
    if (date.empty()) date = "N/A";

    cout << "\nSelect Payment Method:\n";
    cout << "1. GCash\n";
    cout << "2. Card\n";
    cout << "Choice: ";
    int pay = readInt();
    if (pay < 1 || pay > 2) {
        cout << "Invalid payment method. Booking cancelled.\n";
        return;
    }

    string paymentInfo = "";
    if (pay == 1) {
        paymentInfo = "GCash";
    } else {
        cout << "\nSelect Card Type:\n";
        cout << "1. GoTyme\n";
        cout << "2. MariBank\n";
        cout << "3. UnionBank\n";
        cout << "4. Other\n";
        cout << "Choice: ";
        int cardChoice = readInt();
        switch (cardChoice) {
            case 1: paymentInfo = "Card (GoTyme)"; break;
            case 2: paymentInfo = "Card (MariBank)"; break;
            case 3: paymentInfo = "Card (UnionBank)"; break;
            case 4: {
                cout << "Enter card name: ";
                string other = readString();
                paymentInfo = "Card (" + (other.empty() ? string("Unknown") : other) + ")";
                break;
            }
            default:
                cout << "Invalid card choice. Booking cancelled.\n";
                return;
        }
    }

    cout << "\nDate       : " << date << '\n';
    cout << "Payment    : " << paymentInfo << "\n";

    cout << "\nConfirm booking? (y/n): ";
    char confirm = readChar();
    clearInputLine();
    if (tolower(static_cast<unsigned char>(confirm)) != 'y') {
        cout << "Booking cancelled.\n";
        return;
    }

    seats[m][seat - 1] = true;
    bookingHistory.push({m, seat, ticketID});
    appendBookingRecord(customerName, m, seat, type, finalPrice, date, paymentInfo);

    cout << "Booking successful! Ticket ID: " << ticketID << '\n';
    ticketID++;
    if (count > 1) {
        cout << count - 1 << " seat(s) remaining to book.\n";
        bookSeats(count - 1, m, type, customerName);
    }
}

void undoBooking() {
    if (bookingHistory.empty()) {
        cout << "No booking found to undo.\n";
        return;
    }

    Booking last = bookingHistory.top();
    bookingHistory.pop();
    seats[last.movieIndex][last.seat - 1] = false;

    for (auto it = bookingLedger.rbegin(); it != bookingLedger.rend(); ++it) {
        if (it->ticketID == last.ticketID) {
            it->status = "CANCELLED";
            break;
        }
    }

    cout << "Undo successful: Seat " << last.seat << " for " << movies[last.movieIndex] << " has been released.\n";
}

void adminDashboard() {
    if (bookingLedger.empty()) {
        cout << "No booking records found.\n";
        return;
    }
    
    cout << "\n============ ADMIN DASHBOARD ============\n";
    for (const BookingRecord& record : bookingLedger) {
        cout << "Ticket ID  : " << record.ticketID << '\n';
        cout << "Customer   : " << record.customerName << '\n';
        cout << "Movie      : " << record.movieName << '\n';
        cout << "Date       : " << record.date << '\n';
        cout << "Showtime   : " << record.showtime << '\n';
        cout << "Seat       : " << record.seat << '\n';
        cout << "Type       : " << record.type << '\n';
        cout << "Payment    : " << record.paymentInfo << '\n';
        cout << "Final Price: PHP " << record.finalPrice << '\n';
        cout << "Status     : " << record.status << '\n';
        cout << "--------------------------------------\n";
    }
    cout << "=========================================\n";
}

void addMovie() {
    cout << "\n--- Add New Movie ---\n";
    cout << "Movie name: ";
    string name = readString();
    if (trimCopy(name).empty()) {
        cout << "Movie name cannot be empty.\n";
        return;
    }

    cout << "Showtime: ";
    string showtime = readString();
    if (trimCopy(showtime).empty()) {
        cout << "Showtime cannot be empty.\n";
        return;
    }

    cout << "Ticket price (PHP): ";
    int price = readInt();
    if (price <= 0) {
        cout << "Price must be greater than 0.\n";
        return;
    }

    cout << "Showing until (YYYY-MM-DD): ";
    string untilDate = readString();
    if (trimCopy(untilDate).empty()) untilDate = "N/A";

    cout << "Number of VIP seats (minimum " << VIP_SEAT_COUNT << "): ";
    int vipCount = readInt();
    if (vipCount < VIP_SEAT_COUNT) {
        cout << "VIP seat count must be at least " << VIP_SEAT_COUNT << ".\n";
        return;
    }

    cout << "Number of regular seats (can be 0 or more): ";
    int regularCount = readInt();
    if (regularCount < 0) {
        cout << "Regular seat count cannot be negative.\n";
        return;
    }

    int totalSeats = vipCount + regularCount;

    movies.push_back(name);
    times.push_back(showtime);
    moviePrices.push_back(price);
    showingUntil.push_back(untilDate);
    vipSeatCounts.push_back(vipCount);
    seats.push_back(vector<bool>(totalSeats, false));

    cout << "Movie added successfully with " << vipCount << " VIP seats and " << regularCount << " regular seats.\n";
}

void editMovie() {
    if (movies.empty()) {
        cout << "No movies available to edit.\n";
        return;
    }

    cout << "\n--- Edit Movie ---\n";
    for (int i = 0; i < (int)movies.size(); i++) {
        cout << "  " << i + 1 << ". " << movies[i] << " | " << times[i] << " | PHP " << moviePrices[i]
             << " | Showing until: " << showingUntil[i] << " | Seats: " << seats[i].size() << '\n';
    }

    cout << "Select movie (1-" << movies.size() << "): ";
    int m = readInt();
    if (m < 1 || m > (int)movies.size()) {
        cout << "Invalid selection.\n";
        return;
    }
    int idx = m - 1;

    cout << "\nWhat do you want to edit?\n";
    cout << "1. Movie Name\n";
    cout << "2. Showtime\n";
    cout << "3. Price\n";
    cout << "4. Showing Until Date\n";
    cout << "5. VIP Seat Count\n";
    cout << "Choice: ";
    int choice = readInt();

    if (choice == 1) {
        cout << "New movie name: ";
        string value = readString();
        if (trimCopy(value).empty()) {
            cout << "Movie name cannot be empty.\n";
            return;
        }
        movies[idx] = value;
    } else if (choice == 2) {
        cout << "New showtime: ";
        string value = readString();
        if (trimCopy(value).empty()) {
            cout << "Showtime cannot be empty.\n";
            return;
        }
        times[idx] = value;
    } else if (choice == 3) {
        cout << "New price (PHP): ";
        int value = readInt();
        if (value <= 0) {
            cout << "Price must be greater than 0.\n";
            return;
        }
        moviePrices[idx] = value;
    } else if (choice == 4) {
        cout << "New showing-until date (YYYY-MM-DD): ";
        string value = readString();
        showingUntil[idx] = trimCopy(value).empty() ? "N/A" : value;
    } else if (choice == 5) {
        cout << "Current VIP seat count: " << vipSeatCounts[idx] << "\n";
        cout << "New VIP seat count (minimum " << VIP_SEAT_COUNT << "): ";
        int newVipCount = readInt();
        if (newVipCount < VIP_SEAT_COUNT) {
            cout << "VIP seat count must be at least " << VIP_SEAT_COUNT << ".\n";
            return;
        }
        int currentVipCount = vipSeatCounts[idx];
        if (newVipCount > (int)seats[idx].size()) {
            cout << "Cannot set VIP count higher than total seats (" << seats[idx].size() << ").\n";
            return;
        }
        vipSeatCounts[idx] = newVipCount;
    } else {
        cout << "Invalid option.\n";
        return;
    }

    cout << "Movie updated successfully.\n";
}

void deleteMovie() {
    if (movies.empty()) {
        cout << "No movies available to delete.\n";
        return;
    }
    if (movies.size() == 1) {
        cout << "At least one movie must remain in the system.\n";
        return;
    }

    cout << "\n--- Delete Movie ---\n";
    for (int i = 0; i < (int)movies.size(); i++) {
        cout << "  " << i + 1 << ". " << movies[i] << '\n';
    }

    cout << "Select movie (1-" << movies.size() << "): ";
    int m = readInt();
    if (m < 1 || m > (int)movies.size()) {
        cout << "Invalid selection.\n";
        return;
    }
    int idx = m - 1;

    cout << "Delete '" << movies[idx] << "'? (y/n): ";
    char confirm = readChar();
    clearInputLine();
    if (tolower(static_cast<unsigned char>(confirm)) != 'y') {
        cout << "Delete cancelled.\n";
        return;
    }

    movies.erase(movies.begin() + idx);
    times.erase(times.begin() + idx);
    moviePrices.erase(moviePrices.begin() + idx);
    showingUntil.erase(showingUntil.begin() + idx);
    vipSeatCounts.erase(vipSeatCounts.begin() + idx);
    seats.erase(seats.begin() + idx);

    while (!bookingHistory.empty()) bookingHistory.pop();
    cout << "Movie deleted successfully. Booking undo history was cleared to keep indices valid.\n";
}

void addSeatsToMovie() {
    if (movies.empty()) {
        cout << "No movies available.\n";
        return;
    }

    cout << "\n--- Add Seats ---\n";
    for (int i = 0; i < (int)movies.size(); i++) {
        cout << "  " << i + 1 << ". " << movies[i] << " (Current seats: " << seats[i].size() << ")\n";
    }

    cout << "Select movie (1-" << movies.size() << "): ";
    int m = readInt();
    if (m < 1 || m > (int)movies.size()) {
        cout << "Invalid selection.\n";
        return;
    }
    int idx = m - 1;

    cout << "\nAdd seats to " << movies[idx] << "\n";
    cout << "Current VIP seats: " << vipSeatCounts[idx] << ", Current regular seats: " << (seats[idx].size() - vipSeatCounts[idx]) << ", Total: " << seats[idx].size() << "\n";
    cout << "\n1. Add VIP seats\n";
    cout << "2. Add regular seats\n";
    cout << "Choice: ";
    int choice = readInt();

    if (choice == 1) {
        cout << "How many VIP seats to add? ";
        int addCount = readInt();
        if (addCount <= 0) {
            cout << "Count must be greater than 0.\n";
            return;
        }
        int currentVipCount = vipSeatCounts[idx];
        // Insert new VIP seats at the boundary between VIP and regular seats.
        seats[idx].insert(seats[idx].begin() + currentVipCount, addCount, false);
        vipSeatCounts[idx] = currentVipCount + addCount;
        cout << "VIP seats added successfully. " << movies[idx] << " now has " << vipSeatCounts[idx] << " VIP seats.\n";
    } else if (choice == 2) {
        cout << "How many regular seats to add? ";
        int addCount = readInt();
        if (addCount <= 0) {
            cout << "Count must be greater than 0.\n";
            return;
        }
        seats[idx].insert(seats[idx].end(), addCount, false);
        cout << "Regular seats added successfully. " << movies[idx] << " now has " << (seats[idx].size() - vipSeatCounts[idx]) << " regular seats.\n";
    } else {
        cout << "Invalid choice.\n";
    }
}

void manageMovies() {
    int choice;
    do {
        cout << "\n----- Manage Movies -----\n";
        cout << "1. Add Movie\n";
        cout << "2. Edit Movie\n";
        cout << "3. Delete Movie\n";
        cout << "4. Back\n";
        cout << "Choice: ";
        choice = readInt();

        switch (choice) {
        case 1:
            addMovie();
            break;
        case 2:
            editMovie();
            break;
        case 3:
            deleteMovie();
            break;
        case 4:
            return;
        default:
            cout << "Invalid choice. Please enter 1-4.\n";
        }
    } while (true);
}

void serveUser() {
    if (!userQueue.empty()) {
        int vipIndex = -1;
        for (size_t i = 0; i < userQueue.size(); ++i) {
            if (userQueue[i].vip) {
                vipIndex = static_cast<int>(i);
                break;
            }
        }

        if (vipIndex != -1) {
            currentServedUser = userQueue[vipIndex];
            cout << "Now serving VIP: " << currentServedUser.name << '\n';
            userQueue.erase(userQueue.begin() + vipIndex);
        } else {
            currentServedUser = userQueue.front();
            cout << "Now serving Regular: " << currentServedUser.name << '\n';
            userQueue.pop_front();
        }
    } else {
        // Clear the currently served user when serve is clicked and queue is empty
        currentServedUser = {"", false, 0};
        cout << "No users in queue. Not serving anyone now.\n";
    }
}

void enqueueCustomer(const string& name, bool isVip) {
    userSeq++;
    userQueue.push_back({name, isVip, userSeq});
    usedNames.insert(normalizeName(name));
    if (isVip) cout << name << " joined the VIP queue.\n";
    else cout << name << " joined the Regular queue.\n";
}

bool servedCustomerSession(const string& name, bool isVip) {
    int choice;
    do {
        cout << "\n----- Served Customer Booking Menu -----\n";
        cout << "Customer: " << name << '\n';
        cout << "1. View Movies & Prices\n";
        cout << "2. View Available Seats\n";
        cout << "3. Book Ticket (for you)\n";
        cout << "4. Undo Last Booking\n";
        cout << "5. View Sorted Booked Seats\n";
        cout << "6. Exit Booking\n";
        cout << "Choice: ";
        choice = readInt();

        switch (choice) {
        case 1:
            cout << "\n--- Movies Now Showing ---\n";
            for (int i = 0; i < (int)movies.size(); i++) {
                cout << "  " << i + 1 << ". " << movies[i] << " | " << times[i] << " | PHP " << moviePrices[i] << " | Showing until: " << showingUntil[i];
                if (isVip) cout << " (VIP price: PHP " << moviePrices[i] * 0.90 << ")";
                cout << '\n';
            }
            break;
        case 2: {
            cout << "\n--- Movies Now Showing ---\n";
            for (int i = 0; i < (int)movies.size(); i++) {
                cout << "  " << i + 1 << ". " << movies[i] << " (" << times[i] << ") - PHP " << moviePrices[i] << " | Showing until: " << showingUntil[i] << '\n';
            }
            if (movies.empty()) {
                cout << "No movies available.\n";
                break;
            }
            cout << "Select movie (1-" << movies.size() << "): ";
            int m = readInt();
            if (m < 1 || m > (int)movies.size()) {
                cout << "Invalid selection.\n";
                break;
            }
            viewSeatsByType(m - 1, isVip ? "VIP" : "Regular");
            break;
        }
        case 3: {
            cout << "\n--- Booking for: " << name << " ---\n";
            cout << "\n--- Movies Now Showing ---\n";
            for (int i = 0; i < (int)movies.size(); i++) {
                cout << "  " << i + 1 << ". " << movies[i] << " (" << times[i] << ") - PHP " << moviePrices[i] << " | Showing until: " << showingUntil[i] << '\n';
            }
            if (movies.empty()) {
                cout << "No movies available.\n";
                break;
            }
            cout << "Select movie (1-" << movies.size() << "): ";
            int m = readInt();
            if (m < 1 || m > (int)movies.size()) {
                cout << "Invalid selection.\n";
                break;
            }

            cout << "How many seats to book? ";
            int n = readInt();
            if (n < 1) {
                cout << "Invalid number.\n";
                break;
            }

            int available = 0;
            int vipLimit = vipSeatCounts[m - 1];
            int start = isVip ? 0 : vipLimit;
            int end = isVip ? vipLimit : (int)seats[m - 1].size();
            for (int i = start; i < end; i++) if (!seats[m - 1][i]) available++;

            if (n > available) {
                if (isVip) {
                    int regAvailable = 0;
                    for (int i = vipLimit; i < (int)seats[m - 1].size(); ++i) if (!seats[m - 1][i]) regAvailable++;

                    cout << "Only " << available << " VIP seat(s) available.\n";
                    if (regAvailable >= n) {
                        cout << "VIP seats insufficient, but " << regAvailable << " regular seat(s) are available.\n";
                        cout << "Options:\n  1. Book regular seats instead\n  2. Cancel\nChoice: ";
                        int opt = readInt();
                        if (opt == 1) bookSeats(n, m - 1, "Regular", name);
                        else cout << "Booking cancelled.\n";
                    } else {
                        cout << "VIP seats are full and not enough regular seats available. Booking cancelled.\n";
                    }
                } else {
                    cout << "Only " << available << " seat(s) available for your type. Booking cancelled.\n";
                }
                break;
            }

            bookSeats(n, m - 1, isVip ? "VIP" : "Regular", name);
            break;
        }
        case 4:
            undoBooking();
            break;
        case 5: {
            cout << "\n--- Movies Now Showing ---\n";
            for (int i = 0; i < (int)movies.size(); i++) {
                cout << "  " << i + 1 << ". " << movies[i] << '\n';
            }
            if (movies.empty()) {
                cout << "No movies available.\n";
                break;
            }
            cout << "Select movie (1-" << movies.size() << "): ";
            int m = readInt();
            if (m < 1 || m > (int)movies.size()) {
                cout << "Invalid selection.\n";
                break;
            }
            sortBooked(m - 1);
            break;
        }
        case 6:
            cout << "Exiting booking menu...\n";
            currentServedUser = {"", false, 0};
            return false;
        default:
            cout << "Invalid choice. Please enter 1, 2, 3, 4, 5, or 6.\n";
        }
    } while (true);
}

bool regularCustomerSession(const string& name, bool isVip) {
    int choice;
    do {
        cout << "\n----- Customer Menu -----\n";
        cout << "Customer: " << name << '\n';
        cout << "1. View Movies & Prices\n";
        cout << "2. Back to Main Menu\n";
        cout << "3. Exit\n";
        cout << "Choice: ";
        choice = readInt();

        switch (choice) {
        case 1:
            cout << "\n--- Movies Now Showing ---\n";
            for (int i = 0; i < (int)movies.size(); i++) {
                cout << "  " << i + 1 << ". " << movies[i] << " | " << times[i] << " | PHP " << moviePrices[i] << " | Showing until: " << showingUntil[i];
                if (isVip) cout << " (VIP price: PHP " << moviePrices[i] * 0.90 << ")";
                cout << '\n';
            }
            break;
        case 2:
            cout << "Returning to main menu...\n";
            return false;
        case 3:
            cout << "\nThank you for using CineMate! Goodbye!\n";
            return true;
        default:
            cout << "Invalid choice. Please enter 1-3.\n";
        }
    } while (true);
}

bool customerSession() {
    cout << "\n========== CUSTOMER MODE ==========\n";

    string name;
    cout << "Enter customer name: ";
    name = readString();

    if (isReservedAdminName(name)) {
        cout << "That name is reserved for admin. Please use another name.\n";
        return false;
    }

    // If this matches the currently served user, skip the "used name" rejection
    bool isServedCustomer = !currentServedUser.name.empty() &&
        normalizeName(currentServedUser.name) == normalizeName(name);

    // Prevent reuse of customer names (including from previous bookings), unless currently being served
    if (!isServedCustomer && usedNames.find(normalizeName(name)) != usedNames.end()) {
        cout << "That name has already been used. Please enter a different name.\n";
        return false;
    }

    string type;
    bool validType = false;
    while (!validType) {
        cout << "Customer type (VIP / Regular): ";
        type = readString();
        for (char& c : type) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
        if (type == "VIP" || type == "REGULAR") validType = true;
        else cout << "Invalid type. Please enter 'VIP' or 'Regular'.\n";
    }

    bool isVip = (type == "VIP");

    if (isServedCustomer) {
        servedCustomerSession(name, isVip);
        return false;
    }

    enqueueCustomer(name, isVip);
    regularCustomerSession(name, isVip);
    return false;
}

bool adminSession() {
    cout << "\n========== ADMIN LOGIN ==========\n";

    string adminName;
    cout << "Enter admin name: ";
    adminName = readString();

    if (!isReservedAdminName(adminName)) {
        cout << "Invalid admin name. Use " << ADMIN_NAME << ".\n";
        return false;
    }

    if (!authenticateAdmin()) {
        cout << "Admin login failed. Returning to main menu.\n";
        return false;
    }

    currentServedUser = {"", false, 0};
    cout << "Welcome, " << ADMIN_NAME << "!\n";

    int choice;
    do {
        cout << "\n========== ADMIN MENU ==========\n";
        cout << "Admin: " << ADMIN_NAME << '\n';
        cout << "Serving Customer: " << (currentServedUser.name.empty() ? "None" : currentServedUser.name) << '\n';
        cout << "1. View Movies & Prices\n";
        cout << "2. View Available Seats\n";
        cout << "3. View Sorted Booked Seats\n";
        cout << "4. Serve Next User\n";
        cout << "5. Admin Dashboard\n";
        cout << "6. Manage Movies (Add/Edit/Delete)\n";
        cout << "7. Add Seats to Movie\n";
        cout << "8. Back to Main Menu\n";
        cout << "9. Exit\n";
        cout << "================================\n";
        cout << "Choice: ";
        choice = readInt();

        switch (choice) {
        case 1:
            cout << "\n--- Movies Now Showing ---\n";
            for (int i = 0; i < (int)movies.size(); i++) {
                cout << "  " << i + 1 << ". " << movies[i] << " | " << times[i] << " | PHP " << moviePrices[i] << " | Showing until: " << showingUntil[i] << '\n';
            }
            break;
        case 2: {
            cout << "\n--- Movies Now Showing ---\n";
            for (int i = 0; i < (int)movies.size(); i++) {
                cout << "  " << i + 1 << ". " << movies[i] << " (" << times[i] << ") - PHP " << moviePrices[i] << '\n';
            }
            if (movies.empty()) {
                cout << "No movies available.\n";
                break;
            }
            cout << "Select movie (1-" << movies.size() << "): ";
            int m = readInt();
            if (m < 1 || m > (int)movies.size()) {
                cout << "Invalid selection.\n";
                break;
            }
            viewVipSeats(m - 1);
            viewRegularSeats(m - 1);
            break;
        }
        case 3: {
            cout << "\n--- Movies Now Showing ---\n";
            for (int i = 0; i < (int)movies.size(); i++) {
                cout << "  " << i + 1 << ". " << movies[i] << '\n';
            }
            if (movies.empty()) {
                cout << "No movies available.\n";
                break;
            }
            cout << "Select movie (1-" << movies.size() << "): ";
            int m = readInt();
            if (m < 1 || m > (int)movies.size()) {
                cout << "Invalid selection.\n";
                break;
            }
            sortBooked(m - 1);
            break;
        }
        case 4:
            serveUser();
            break;
        case 5:
            adminDashboard();
            break;
        case 6:
            manageMovies();
            break;
        case 7:
            addSeatsToMovie();
            break;
        case 8:
            cout << "Returning to main menu...\n";
            return false;
        case 9:
            cout << "\nThank you for using CineMate! Goodbye!\n";
            return true;
        default:
            cout << "Invalid choice. Please enter 1-9.\n";
        }
    } while (true);
}

int main() {

    cout << "============================================\n";
    cout << "     Welcome to CineMate Ticketing System  \n";
    cout << "============================================\n";

    while (true) {
        cout << "\n========== MAIN MENU ==========\n";
        cout << "1. Customer\n";
        cout << "2. Admin\n";
        cout << "3. Exit\n";
        cout << "================================\n";
        cout << "Choice: ";

        int choice = readInt();
        if (choice == 1) {
            if (customerSession()) break;
        } else if (choice == 2) {
            if (adminSession()) break;
        } else if (choice == 3) {
            cout << "\nThank you for using CineMate! Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice. Please enter 1-3.\n";
        }
    }

    return 0;
}
