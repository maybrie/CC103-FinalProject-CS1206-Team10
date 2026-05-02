#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <limits>
using namespace std;  

// --------------- STRUCT ---------------
struct Booking {
int movieIndex;
int seat;
int ticketID;
};

// ---------------- DATA STRUCTURES ----------------
stack<Booking> bookingHistory;
queue<string> regularQueue;
queue<string> vipQueue;
// ---------------- MOVIES + SEATS ----------------
vector<string> movies = {"Avengers", "Batman", "Spider-Man"};
vector<string> times = {"10:00 AM", "1:00 PM", "4:00 PM"};
vector<int> moviePrices = {500, 450, 400};

// Seats per movie (3 movies, 10 seats each)
vector<vector<bool>> seats(3, vector<bool>(10, false));

//----------------- TICKET ID ----------------
int ticketID = 1000;

// ---------------- INPUT HELPERS ----------------
int readInt() {
    int val;
    while (!(cin >> val)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Try again: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return val;
}

char readchar() {
char c;
cin.get(c);
return c;
}

string readString() {
    string s;
    cin >> s;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return s;
}

// ---------------- DECLARATIONS ----------------
void viewVipSeats(int m);
void viewRegularSeats(int m);

// ---------------- VIEW SEATS ----------------
void viewSeats(int m) {
    cout << "\nSeats for " << movies[m] << " (" << times[m] << ")\n";
    for (int i = 0; i < 10; i++) {
        cout << "  Seat " << i + 1 << ": "
             << (seats[m][i] ? "[BOOKED]" : "[AVAILABLE]") << "\n";
    }
}

void viewVipSeats(int m) {
    cout << "\nVIP Seats (1-3) for " << movies[m] << ":\n";
    for (int i = 0; i < 3; i++) {
        cout << "  Seat " << i + 1 << ": "
             << (seats[m][i] ? "[BOOKED]" : "[AVAILABLE]") << "\n";
    }
}

void viewRegularSeats(int m) {
    cout << "\nRegular Seats (4-10) for " << movies[m] << ":\n";
    for (int i = 3; i < 10; i++) {
        cout << "  Seat " << i + 1 << ": "
             << (seats[m][i] ? "[BOOKED]" : "[AVAILABLE]") << "\n";
    }
}

void viewSeatsByType(int m, const string& type) {
    if (type == "VIP")
        viewVipSeats(m);
    else
        viewRegularSeats(m);
}

void viewAvailable(int m) {
    cout << "\nAvailable Seats:\n";
    for (int i = 0; i < 10; i++) {
        if (!seats[m][i])
            cout << "  Seat " << i + 1 << "\n";
    }
}

void viewBooked(int m) {
    cout << "\nBooked Seats:\n";
    bool any = false;
    for (int i = 0; i < 10; i++) {
        if (seats[m][i]) {
            cout << "  Seat " << i + 1 << "\n";
            any = true;
        }
    }
    if (!any) cout << "  None.\n";
}

// ---------------- SORT ----------------
void sortBooked(int m) {
    vector<int> booked;
    for (int i = 0; i < 10; i++) {
        if (seats[m][i])
            booked.push_back(i + 1);
    }

    if (booked.empty()) {
        cout << "No booked seats for " << movies[m] << " yet.\n";
        return;
    }

    sort(booked.begin(), booked.end());

    cout << "Sorted booked seats for " << movies[m] << ": ";
    for (int s : booked) cout << s << " ";
    cout << "\n";
}

// ---------------- BOOK SEATS (RECURSIVE) ----------------
void bookSeats(int count, int m, const string& type, const string& customerName) {
    if (count == 0) return;

    viewSeatsByType(m, type);

    int seat;
    cout << "\nEnter seat number to book: ";
    seat = readInt();

    if (seat < 1 || seat > 10) {
        cout << "Invalid seat number. Must be 1-10.\n";
        bookSeats(count, m, type, customerName);
        return;
    }

    if (type == "VIP" && seat > 3) {
        cout << "VIP customers can only book seats 1-3.\n";
        bookSeats(count, m, type, customerName);
        return;
    }

    if (type == "Regular" && seat <= 3) {
        cout << "Seats 1-3 are reserved for VIP customers.\n";
        bookSeats(count, m, type, customerName);
        return;
    }

    if (seats[m][seat - 1]) {
        cout << "Seat " << seat << " is already booked. Please choose another.\n";
        bookSeats(count, m, type, customerName);
        return;
    }

    // ---------------- PRICING ----------------
    int basePrice = moviePrices[m];
    double finalPrice = basePrice;
    if (type == "VIP")
        finalPrice = basePrice * 0.90; // 10% VIP discount

    cout << "\n========== BOOKING SUMMARY ==========\n";
    cout << " Customer    : " << customerName << "\n";
    cout << "  Movie      : " << movies[m] << "\n";
    cout << "  Showtime   : " << times[m]  << "\n";
    cout << "  Seat       : " << seat      << "\n";
    cout << "  Type       : " << type      << "\n";
    cout << "  Base Price : PHP " << basePrice << "\n";
    if (type == "VIP")
        cout << "  VIP Disc.  : 10%\n";
    cout << "  Final Price: PHP " << finalPrice << "\n";
    cout << "=====================================\n";

// ---------------- VIEW SEATS ----------------
void viewSeats(int m) {
    cout << "\nSeats for " << movies[m] << ":\n";
    for (int i = 0; i < 10; i++) {
        cout << "Seat " << i + 1 << ": "
             << (seats[m][i] ? "[BOOKED]" : "[AVAILABLE]") << "\n";
    }
}
// ---------------- PAYMENT ----------------
cout << "S\Select Payment Method:\n";
cout << " 1. GCash\n";
cout << " 2. Card\n";
cout << "Choice: ";
int pay = readInt();

if (pay < 1 || pay > 2) {
    cout << "Invalid payment method. Booking cancelled.\n"
    return;
}
cout << "Processing payment";
for(int i=0; i<3; i++) {
cout << "."; 
cout.flush(); }
cout << "\n";

if (pay == 1) cout << "Payment method: GCash\n";
    else cout << "Payment method: Card\n"

// ---------------- CONFIRM ---------------
cout << "\nConfirm booking? (Y/N): ";
char confirm = readChar();
    if (confirm != 'Y' && confirm != 'y') {
    cout << "Booking cancelled.\n";
    return;
}
    
// ---------------- RECEIPT ----------------
    cout << "\n========== TICKET RECEIPT ==========\n";
    cout << "  Ticket ID  : " << ticketID    << "\n";
    cout << "  Customer   : " << customerName << "\n";
    cout << "  Movie      : " << movies[m]   << "\n";
    cout << "  Showtime   : " << times[m]    << "\n";
    cout << "  Seat       : " << seat        << "\n";
    cout << "  Type       : " << type        << "\n";
    cout << "  Amount Paid: PHP " << finalPrice << "\n";
    cout << "=====================================\n";
    cout << "Booking successful!\n";

    bookSeats(count - 1, m, type, customerName);
}

// ---------------- UNDO ----------------
void undoBooking() {
    if (bookingHistory.empty()) {
        cout << "Nothing to undo.\n";
        return;
    }

    Booking last = bookingHistory.top();
    bookingHistory.pop();

    // Release the seat
    seats[last.movieIndex][last.seat - 1] = false;

   ifstream fin("bookings.txt");
    if (!fin) {
        cout << "Undo successful but could not update records.\n";
        return;
    }

    string line;
    string targetID = "Ticket ID  : " + to_string(last.ticketID);
    bool patched = false;

    vector<string> lines;
    while (getline(fin, line)) lines.push_back(line);
    fin.close();


    int patchLine = -1;
    for (int i = (int)lines.size() - 1; i >= 0; i--) {
        if (lines[i] == targetID) { patchLine = i; break; }
    }

    if (patchLine != -1) {
       
        for (int i = patchLine; i < (int)lines.size(); i++) {
            if (lines[i].find("Status     :") != string::npos) {
                lines[i] = "Status     : CANCELLED";
                patched = true;
                break;
            }
            if (lines[i].find("------") != string::npos) break; 
        }
    }

    ofstream fout("bookings.txt");
    for (const string& l : lines) fout << l << "\n";
    fout.close();

    cout << "Undo successful: Seat " << last.seat
         << " for " << movies[last.movieIndex] << " has been released.\n";
    if (patched)
        cout << "Ticket #" << last.ticketID << " marked as CANCELLED in the dashboard.\n";
}

//----------------- MAIN FUNCTION ----------------
int main() {
    cout << "============================================\n";
    cout << "     Welcome to CineMate Ticketing System  \n";
    cout << "============================================\n";

    string name, type;

    cout << "Enter your name: ";
    name = readString();

    cout << "Customer type (VIP / Regular): ";
    type = readString();

    for (char& c : type) c = toupper(c);

    if (type == "VIP") {
        vipQueue.push(name);
        cout << "Welcome, VIP customer " << name << "! You enjoy a 10% seat discount.\n";
    } else {
        type = "Regular";
        regularQueue.push(name);
        cout << "Welcome, " << name << "!\n";
    }

    int choice;

    do {
        cout << "\n========== MAIN MENU ==========\n";
        cout << "1. View Movies & Prices\n";
        cout << "2. View Available Seats\n";
        cout << "3. Book Ticket\n";
        cout << "4. Undo Last Booking\n";
        cout << "5. View Sorted Booked Seats\n";
        cout << "6. Serve Next User\n";
        cout << "7. Admin Dashboard\n";
        cout << "8. Exit\n";
        cout << "================================\n";
        cout << "Choice: ";
        choice = readInt();

        switch (choice) {
            case 1: cout << "[Feature coming soon]\n"; break;
            case 2: cout << "[Feature coming soon]\n"; break;
            case 3: bookTicket(); break;   
            case 4: undoBooking(); break; 
            case 5: cout << "[Feature coming soon]\n"; break;
            case 6: cout << "[Feature coming soon]\n"; break;
            case 7: cout << "[Feature coming soon]\n"; break;
            case 8:
                cout << "\nThank you for using CineMate! Goodbye, " << name << "!\n";
                break;
            default:
                cout << "Invalid choice. Please enter 1-8.\n";
        }

    } while (choice != 8);

    return 0;
}
