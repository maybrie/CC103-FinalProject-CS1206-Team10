#include <iostream>
#include <queue>
#include <string>
#include <limits>
using namespace std;

// ---------------- DATA STRUCTURES ----------------
queue<string> regularQueue;
queue<string> vipQueue;


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

string readString() {
    string s;
    cin >> s;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return s;
}

// ---------------- MAIN ----------------
int main() {
    cout << "============================================\n";
    cout << "     Welcome to CineBook Ticketing System  \n";
    cout << "============================================\n";

    string name, type;

    cout << "Enter your name: ";
    name = readString();

    cout << "Customer type (VIP / Regular): ";
    type = readString();

    // Normalize
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
        cout << "2. View My Available Seats\n";
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
            case 3: cout << "[Feature coming soon]\n"; break;
            case 4: cout << "[Feature coming soon]\n"; break;
            case 5: cout << "[Feature coming soon]\n"; break;
            case 6: cout << "[Feature coming soon]\n"; break;
            case 7: cout << "[Feature coming soon]\n"; break;
            case 8: cout << "\nThank you for using CineBook! Goodbye, " << name << "!\n"; break;
            default: cout << "Invalid choice. Please enter 1-8.\n";
        }

    } while (choice != 8);

    return 0;
}