/*
 * User.cpp
 *
 * This file implements the User class methods and those of its derived classes
 * (Student, Faculty, and Librarian).
 *
 * It defines methods for user authentication, profile updates, borrowing and
 * returning books, and printing user details.
 * The derived classes override the virtual methods to provide role-specific behavior.
 */

#include "User.h"
#include "Utility.h"
#include "Book.h"
#include <iostream>
#include <algorithm>
using namespace std;

User::User(int id, const string &n, const string &uname, const string &pwd, bool isAlreadyHashed)
    : userId(id), name(n), username(uname) {
    if (isAlreadyHashed)
        password = pwd;
    else
        password = hashPassword(pwd);
}

User::~User() {}

int User::getUserId() const { return userId; }
string User::getName() const { return name; }
string User::getUsername() const { return username; }
string User::getHashedPassword() const { return password; }
Account &User::getAccount() { return account; }

void User::setPasswordRaw(const string &rawPwd) {
    password = hashPassword(rawPwd);
}

bool User::authenticate(const string &uname, const string &enteredPwd) const {
    return (username == uname && password == hashPassword(enteredPwd));
}

void User::updateProfile() {
    cin.ignore();
    cout << "Enter new name (or press enter to keep current name (" << name << ")): " << endl;
    string newName;
    getline(cin, newName);
    if (!newName.empty())
        name = newName;
    cout << "Enter new password (or press enter to keep current password): " << endl;
    string newPwd;
    getline(cin, newPwd);
    if (!newPwd.empty())
        setPasswordRaw(newPwd);
    cout << "Profile updated successfully." << endl;
    logTransaction(userId, "Updated profile");
}

bool User::canBorrow(long long currentTime, const vector<Book*> &libraryBooks) {
    if (account.getFine() > 0) {
        cout << "Outstanding fine: " << account.getFine() << ". Please clear your fine before borrowing." << endl;
        return false;
    }
    if (account.getBorrowedCount() >= getMaxBooks()) {
        cout << "Reached maximum borrowing limit." << endl;
        return false;
    }
    return true;
}

bool User::additionalBorrowCheck(long long currentTime, const vector<Book*> &libraryBooks) {
    return true;
}

bool User::borrowBook(Book *book, long long currentTime, const vector<Book*> &libraryBooks) {
    if (!canBorrow(currentTime, libraryBooks))
        return false;
    account.addBorrowedBook(book->getBookId(), currentTime);
    book->setStatus(BORROWED);
    if (book->getReservedBy() == userId)
        book->setReservedBy(0);
    book->incrementBorrowCount();
    cout << name << " borrowed book: " << book->getTitle() << endl;
    return true;
}

void User::returnBook(Book *book, long long returnTime) {
    bool success = account.returnBook(book->getBookId(), returnTime, getBorrowPeriod(), getFinePerDay());
    if (success) {
        book->setStatus(AVAILABLE);
        if (book->getReservedBy() != 0)
            book->setStatus(RESERVED);
        cout << name << " returned book: " << book->getTitle() << endl;
    }
}

void User::printDetails() const {
    cout << "User ID: " << userId << endl
         << "Name: " << name << endl
         << "Role: " << getRole() << endl;
}

Student::Student(int id, const string &n, const string &uname, const string &pwd, bool isAlreadyHashed)
    : User(id, n, uname, pwd, isAlreadyHashed) {}

int Student::getMaxBooks() const { return 3; }
int Student::getBorrowPeriod() const { return 15; }
int Student::getFinePerDay() const { return 10; }
string Student::getRole() const { return "Student"; }

Faculty::Faculty(int id, const string &n, const string &uname, const string &pwd, bool isAlreadyHashed)
    : User(id, n, uname, pwd, isAlreadyHashed) {}

int Faculty::getMaxBooks() const { return 5; }
int Faculty::getBorrowPeriod() const { return 30; }
int Faculty::getFinePerDay() const { return 0; }
string Faculty::getRole() const { return "Faculty"; }
bool Faculty::additionalBorrowCheck(long long currentTime, const vector<Book*> &libraryBooks) {
    const vector<BorrowedBook> &books = account.getBorrowedBooks();
    for (auto &bb : books) {
        long long limit = 60LL * 24 * 60;
        if (currentTime - bb.borrowTime > limit) {
            cout << "Cannot borrow more books as one of your books is overdue by more than 60 days." << endl;
            return false;
        }
    }
    return true;
}
bool Faculty::borrowBook(Book *book, long long currentTime, const vector<Book*> &libraryBooks) {
    if (!canBorrow(currentTime, libraryBooks) || !additionalBorrowCheck(currentTime, libraryBooks))
        return false;
    account.addBorrowedBook(book->getBookId(), currentTime);
    book->setStatus(BORROWED);
    book->incrementBorrowCount();
    cout << name << " borrowed book: " << book->getTitle() << endl;
    return true;
}

Librarian::Librarian(int id, const string &n, const string &uname, const string &pwd, bool isAlreadyHashed)
    : User(id, n, uname, pwd, isAlreadyHashed) {}

int Librarian::getMaxBooks() const { return 0; }
int Librarian::getBorrowPeriod() const { return 0; }
int Librarian::getFinePerDay() const { return 0; }
string Librarian::getRole() const { return "Librarian"; }
bool Librarian::borrowBook(Book *book, long long currentTime, const vector<Book*> &libraryBooks) {
    cout << "Librarians are not allowed to borrow books." << endl;
    return false;
}
 