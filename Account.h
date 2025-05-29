/*
 * Account.h
 *
 * This file declares the Account class along with two supporting structures:
 * - BorrowedBook: Represents a currently borrowed book and its borrow time.
 * - BorrowHistory: Represents a record of a borrow-return transaction.
 *
 * The Account class manages a user’s borrowing activity, including:
 * - Adding a borrowed book.
 * - Renewing and returning books (with overdue fine calculations).
 * - Storing a history of transactions.
 * - Tracking the current outstanding fine.
 */

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <vector>
using namespace std;

struct BorrowedBook {
    int bookId;
    long long borrowTime; 
};

struct BorrowHistory {
    int bookId;
    long long borrowTime;
    long long returnTime;
    int overdueDays;
    double fineCharged;
};

class Account {
public:
    Account();
    
    void addBorrowedBook(int bookId, long long borrowTime);
    bool renewBorrowedBook(int bookId, long long newBorrowTime);
    bool returnBook(int bookId, long long returnTime, int borrowPeriodDays, int finePerDay);
    
    int getBorrowedCount() const;
    const vector<BorrowedBook> &getBorrowedBooks() const;
    
    double getFine() const;
    void payFine();
    void setFine(double f);
    
    const vector<BorrowHistory> &getHistory() const;
    void addHistoryRecord(const BorrowHistory &record);

private:
    vector<BorrowedBook> borrowedBooks;
    vector<BorrowHistory> history;
    double fine;
};

#endif 