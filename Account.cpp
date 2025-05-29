/*
 * Account.cpp
 *
 * This file implements the functions declared in Account.h.
 * It handles the addition of borrowed books, renewing and returning books
 * (including calculating fines for overdue returns), and maintaining the borrowing history.
 */

#include "Account.h"
#include <cmath>
#include <iostream>
using namespace std;

Account::Account() : fine(0.0) {}

void Account::addBorrowedBook(int bookId, long long borrowTime) {
    borrowedBooks.push_back({bookId, borrowTime});
}

bool Account::renewBorrowedBook(int bookId, long long newBorrowTime) {
    for (auto &bb : borrowedBooks) {
        if (bb.bookId == bookId) {
            bb.borrowTime = newBorrowTime;
            return true;
        }
    }
    return false;
}

bool Account::returnBook(int bookId, long long returnTime, int borrowPeriodDays, int finePerDay) {
    long long allowedTime = static_cast<long long>(borrowPeriodDays) * 24 * 60;
    for (auto it = borrowedBooks.begin(); it != borrowedBooks.end(); ++it) {
        if (it->bookId == bookId) {
            long long overdue = returnTime - it->borrowTime - allowedTime;
            int overdueDays = 0;
            double fineCharged = 0.0;
            if (overdue > 0) {
                overdueDays = static_cast<int>(ceil(overdue / (24.0 * 60)));
                fineCharged = overdueDays * finePerDay;
                fine += fineCharged;
            }
            history.push_back({bookId, it->borrowTime, returnTime, overdueDays, fineCharged});
            borrowedBooks.erase(it);
            if (overdueDays > 0)
                cout << "Book was overdue by " << overdueDays << " days. Fine incurred: " << fineCharged << endl;
            return true;
        }
    }
    cout << "No record found for this book in your borrowed list." << endl;
    return false;
}

int Account::getBorrowedCount() const {
    return borrowedBooks.size();
}

const vector<BorrowedBook> &Account::getBorrowedBooks() const {
    return borrowedBooks;
}

double Account::getFine() const {
    return fine;
}

void Account::payFine() {
    fine = 0.0;
}

void Account::setFine(double f) {
    fine = f;
}

const vector<BorrowHistory> &Account::getHistory() const {
    return history;
}

void Account::addHistoryRecord(const BorrowHistory &record) {
    history.push_back(record);
}
