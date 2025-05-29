/*
 * Book.cpp
 *
 * This file implements the functions declared in Book.h.
 * It provides the functionality for creating a Book object, updating its details,
 * managing its status and borrow count, and printing its details to the console.
 */

 #include "Book.h"
 #include <iostream>
 #include <algorithm>
 using namespace std;
 
 string statusToString(BookStatus status) {
     if (status == AVAILABLE) return "Available";
     if (status == BORROWED) return "Borrowed";
     if (status == RESERVED) return "Reserved";
     return "Unknown";
 }
 
 Book::Book(int id, const string &t, const string &a, const string &p, int y, const string &i)
     : bookId(id), title(t), author(a), publisher(p), year(y), isbn(i),
       status(AVAILABLE), reservedBy(0), borrowCount(0), reserveTime(0) { }
 
 int Book::getBookId() const { return bookId; }
 string Book::getTitle() const { return title; }
 string Book::getAuthor() const { return author; }
 string Book::getPublisher() const { return publisher; }
 int Book::getYear() const { return year; }
 string Book::getISBN() const { return isbn; }
 BookStatus Book::getStatus() const { return status; }
 void Book::setStatus(BookStatus s) { status = s; }
 
 int Book::getReservedBy() const { return reservedBy; }
 void Book::setReservedBy(int uid) { reservedBy = uid; }
 
 void Book::updateDetails(const string &newTitle, const string &newAuthor,
                            const string &newPublisher, int newYear, const string &newISBN) {
     title = newTitle;
     author = newAuthor;
     publisher = newPublisher;
     year = newYear;
     isbn = newISBN;
 }
 
 void Book::incrementBorrowCount() { borrowCount++; }
 int Book::getBorrowCount() const { return borrowCount; }
 
 void Book::printDetails() const {
     cout << "Book ID: " << bookId << "\n"
          << "Title: " << title << "\n"
          << "Author: " << author << "\n"
          << "Publisher: " << publisher << "\n"
          << "Year: " << year << "\n"
          << "ISBN: " << isbn << "\n"
          << "Status: " << statusToString(status);
     if (status == RESERVED)
         cout << " (Reserved by user " << reservedBy << ")";
     cout << "\nBorrowed " << borrowCount << " times." << "\n";
 }
 
 long long Book::getReserveTime() const {
     return reserveTime;
 }
 
 void Book::setReserveTime(long long t) {
     reserveTime = t;
 }
 