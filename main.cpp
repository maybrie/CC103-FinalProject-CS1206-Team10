#include <iostream>
#include <cctype>
#include <deque>
#include <stack>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_set>
#include <algorithm>
#include <limits>
using namespace std;  

struct Booking {
    int movieIndex;
    int seat;
    int ticketID;
};

struct User {
    string name;
    bool vip;
    int seq;
};

stack<Booking> bookingHistory;
deque<User> userQueue;
unordered_set<string> usedNames;
User currentServedUser = {"", false, 0};  
int userSeq = 0;
int ticketID = 1000;

const string ADMIN_NAME = "Maria";
const string ADMIN_PASSWORD = "Admin@123";

vector<string> movies = {"Avengers", "Batman", "Spider-Man"};
vector<string> times  = {"10:00 AM - 12:40 PM", "1:00 PM - 2:30 PM",  "4:00 PM - 5:45 PM"};
vector<int> moviePrices = {500, 450, 400};
vector<vector<bool>> seats(3,vector<bool>(10,false));

string trimCopy(const string& text) {
    size_t start = text.find_last_not_of("\t\r\n");
    if(start == string::npos) return""; size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}


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


string trimCopy(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

int findMovieIndex(const string& movieName) {
    for (int i = 0; i < (int)movies.size(); ++i) {
        if (movies[i] == movieName)
            return i;
    }
    return -1;
}

void loadBookingsFromFile() {
    ifstream file("bookings.txt");
    if (!file) return;

    for (auto& movieSeats : seats) {
        fill(movieSeats.begin(), movieSeats.end(), false);
    }

    string line;
    int currentTicketId = -1;
    int currentMovieIndex = -1;
    int currentSeat = -1;
    string currentStatus;

    auto finalizeRecord = [&]() {
        if (currentTicketId > ticketID)
            ticketID = currentTicketId;

        if (currentStatus == "CONFIRMED" && currentMovieIndex >= 0 && currentSeat >= 1 && currentSeat <= 10) {
            seats[currentMovieIndex][currentSeat - 1] = true;
        }

        currentTicketId = -1;
        currentMovieIndex = -1;
        currentSeat = -1;
        currentStatus.clear();
    };

    while (getline(file, line)) {
        if (line.rfind("Ticket ID  :", 0) == 0) {
            currentTicketId = stoi(trimCopy(line.substr(line.find(':') + 1)));
        } else if (line.rfind("Movie      :", 0) == 0) {
            currentMovieIndex = findMovieIndex(trimCopy(line.substr(line.find(':') + 1)));
        } else if (line.rfind("Seat       :", 0) == 0) {
            currentSeat = stoi(trimCopy(line.substr(line.find(':') + 1)));
        } else if (line.rfind("Status     :", 0) == 0) {
            currentStatus = trimCopy(line.substr(line.find(':') + 1));
        } else if (line.find("--------------------------------------") != string::npos) {
            finalizeRecord();
        }
    }

    finalizeRecord();

    if (ticketID < 1000)
        ticketID = 1000;
}

// ---------------- DECLARATIONS ----------------
void viewVipSeats(int m);
void viewRegularSeats(int m);
void addUserPrompt();

// ---------------- VIEW SEATS FUNCTIONS ----------------
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
    for (int i = 0; i < 3; i++) { cout << "."; cout.flush(); }
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

 // ---------------- SAVE ----------------
    ticketID++; // 

    seats[m][seat - 1] = true;
    bookingHistory.push({m, seat, ticketID}); // 

    ofstream file("bookings.txt", ios::app);
    file << "Ticket ID  : " << ticketID << "\n";
    file << "Customer   : " << customerName << "\n";
    file << "Movie      : " << movies[m] << "\n";
    file << "Showtime   : " << times[m]  << "\n";
    file << "Seat       : " << seat      << "\n";
    file << "Type       : " << type      << "\n";
    file << "Final Price: PHP " << finalPrice << "\n";
    file << "Status     : CONFIRMED\n";
    file << "--------------------------------------\n";
    file.close();
    
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

// ---------------- ADMIN DASHBOARD ----------------
void adminDashboard() {
    ifstream file("bookings.txt");

    if (!file || file.peek() == EOF) {
        cout << "No booking records found.\n";
        return;
    }

    cout << "\n============ ADMIN DASHBOARD ============\n";
    string line;
    while (getline(file, line)) {
        cout << line << "\n";
    }
    file.close();
    cout << "=========================================\n";
}

// ---------------- SERVE USERS ----------------
void serveUser() {
    if (!userQueue.empty()) {
        int vipIndex = -1;
        for (size_t i = 0; i < userQueue.size(); ++i) {
            if (userQueue[i].vip) { vipIndex = (int)i; break; }
        }

        if (vipIndex != -1) {
            User u = userQueue[vipIndex];
            currentServedUser = u;  // Store served user
            cout << "Now serving VIP: " << u.name << "\n";
            userQueue.erase(userQueue.begin() + vipIndex);
        } else {
            User u = userQueue.front();
            currentServedUser = u;  // Store served user
            cout << "Now serving Regular: " << u.name << "\n";
            userQueue.pop_front();
        }
    } else {
        cout << "No users in queue." << "\n";
    }
}

//----------------- MAIN ----------------
int main() {
    loadBookingsFromFile();
    
    cout << "============================================\n";
    cout << "     Welcome to CineMate Ticketing System  \n";
    cout << "============================================\n";

    string name, type;

    cout << "Enter your name: ";
    name = readString();

    bool validType = false;
    while (!validType) {
        cout << "Customer type (VIP / Regular): ";
        type = readString();
        
    for (char& c : type) c = toupper(c);

    if (type == "VIP" || type == "REGULAR") {
            validType = true;
        } else {
            cout << "Invalid type. Please enter 'VIP' or 'Regular'.\n";
        }
    }

    if (type == "VIP") {
       userSeq++;
        userQueue.push_back({name, true, userSeq});
        cout << "Welcome, VIP customer " << name << "! You enjoy a 10% seat discount.\n";
    } else {
        type = "Regular";
        userSeq++;
        userQueue.push_back({name, false, userSeq});
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
        cout << "6. Add User to Queue\n";
        cout << "7. Serve Next User\n";
        cout << "8. Admin Dashboard\n";
        cout << "9. Exit\n";
        cout << "================================\n";
        cout << "Choice: ";
        choice = readInt();

        switch (choice) {
            
            case 1: 
                    if (currentServedUser.name.empty()) {
                cout << "No customer currently being served. Use option 7 first.\n";
                break;
            }
            cout << "\n--- Movies Now Showing ---\n";
            {
                User currentUser = currentServedUser;
                for (int i = 0; i < (int)movies.size(); i++) {
                    cout << "  " << i + 1 << ". " << movies[i]
                         << " | " << times[i]
                         << " | PHP " << moviePrices[i];
                    if (currentUser.vip)
                        cout << " (You pay: PHP " << moviePrices[i] * 0.90 << " with 10% VIP discount)";
                    cout << "\n";
                }
            }
            break;
            
            case 2: {
                 if (currentServedUser.name.empty()) {
                cout << "No customer currently being served. Use option 7 first.\n";
                break;
            }
                
                    cout << "\n--- Movies Now Showing ---\n"; 
                    for (int i = 0; i < (int)movies.size(); i++)
                        cout << "  " << i + 1 << ". " << movies[i]
                             << " (" << times[i] << ")"
                             << " - PHP " << moviePrices[i] << "\n";
                    cout << "Select movie (1-3): ";
                    int m = readInt();
                
                    User currentUser = currentServedUser;
                    string userType = currentUser.vip ? "VIP" : "Regular";
                    viewSeatsByType(m - 1, userType);
                    viewSeatsByType(m - 1, type);     
                    break;
            }
        
            case 3: {
                     // Get currently served user
                if (currentServedUser.name.empty()) {
                    cout << "No customer currently being served. Use option 7 first.\n";
                    break;
                }

                User currentUser = currentServedUser;
                string userType = currentUser.vip ? "VIP" : "Regular";

                    cout << "\n--- Serving: " << currentUser.name << " (" << userType << ") ---\n";
                    cout << "\n--- Movies Now Showing ---\n";
                    for (int i = 0; i < (int)movies.size(); i++) {
                        cout << "  " << i + 1 << ". " << movies[i]
                             << " (" << times[i] << ")"
                             << " - PHP " << moviePrices[i] << "\n";
                    }
                    cout << "Select movie (1-3): ";
                    int m = readInt();
                    if (m < 1 || m > 3) { cout << "Invalid selection.\n"; break; }
        
                    cout << "How many seats to book? ";
                    int n = readInt();
                    if (n < 1) { cout << "Invalid number.\n"; break; }
        
                    // Check enough seats available for this type
                    int available = 0;
                    int start = (type == "VIP") ? 0 : 3;
                    int end   = (type == "VIP") ? 3 : 10;
                    for (int i = start; i < end; i++)
                        if (!seats[m - 1][i]) available++;
        
                    if (n > available) {
                        if (type == "VIP") {
                            int regAvailable = 0;
                            for (int i = 3; i < 10; ++i)
                                if (!seats[m - 1][i]) regAvailable++;
        
                            cout << "Only " << available << " VIP seat(s) available.\n";
        
                            if (regAvailable >= n) {
                                cout << "VIP seats insufficient, but " << regAvailable << " regular seat(s) are available.\n";
                                cout << "Options:\n  1. Book regular seats instead\n  2. Cancel\nChoice: ";
                                int opt = readInt();
                                if (opt == 1) {
                                    bookSeats(n, m - 1, "Regular", name);
                                } else {
                                    cout << "Booking cancelled.\n";
                                }
                            } else {
                                cout << "VIP seats are full and not enough regular seats available. Booking cancelled.\n";
                            }
                        } else {
                            cout << "Only " << available << " seat(s) available for " << type << " customers.\n";
                        }
                        break;
                    }
        
                    bookSeats(n, m - 1, userType, currentUser.name);
                    break;
            }
            
            case 4: undoBooking(); 
                break; 
            
            case 5: cout << "\n--- Movies Now Showing ---\n"; 
                    for (int i = 0; i < (int)movies.size(); i++)
                        cout << "  " << i + 1 << ". " << movies[i] << "\n";
                    cout << "Select movie (1-3): ";
                    int m = readInt();
                    if (m < 1 || m > 3) { cout << "Invalid selection.\n"; break; }
                    sortBooked(m - 1);
                break;
            }

            case 6: addUserPrompt();
                break;
    
            case 7: serveUser(); 
                break;
    
            case 8: adminDashboard(); 
                break;
    
            case 9:
                cout << "\nThank you for using CineMate! Goodbye, " << name << "!\n";
                break;
    
            default:
                cout << "Invalid choice. Please enter 1-8.\n";
            }

        } while (choice != 9);
    
        return 0;
    }

// ---------------- ADD USER (helper) ----------------
void addUserPrompt() {
    cout << "Enter name: ";
    string name = readString();

    string utype;
    bool validType = false;
    while (!validType) {
        cout << "VIP or Regular? ";
        utype = readString();
        for (char& c : utype) c = toupper(c);
        
        if (utype == "VIP" || utype == "REGULAR") {
            validType = true;
        } else {
            cout << "Invalid type. Please enter 'VIP' or 'Regular'.\n";
        }
    }

    bool isVip = (utype == "VIP");
    userSeq++;
    userQueue.push_back({name, isVip, userSeq});

    if (isVip) cout << name << " added to VIP queue.\n";
    else       cout << name << " added to Regular queue.\n";
}
