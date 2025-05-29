/*
 * Book.h
 *
 * This file declares the Book class, which represents a single book in the library.
 * It includes attributes such as title, author, publisher, publication year, ISBN,
 * status (Available, Borrowed, or Reserved), reservation information (including the time the
 * reservation was made), and the borrow count.
 *
 * It provides getters and setters for these attributes, as well as methods to update
 * book details, increment the borrow count, and print the book’s information.
 */

 #ifndef BOOK_H
 #define BOOK_H
 
 #include <string>
 using namespace std;
 
 enum BookStatus { AVAILABLE, BORROWED, RESERVED };
 
 string statusToString(BookStatus status);
 
 class Book {
 public:
     Book(int id, const string &t, const string &a, const string &p, int y, const string &i);
     
     int getBookId() const;
     string getTitle() const;
     string getAuthor() const;
     string getPublisher() const;
     int getYear() const;
     string getISBN() const;
     BookStatus getStatus() const;
     void setStatus(BookStatus s);
     
     int getReservedBy() const;
     void setReservedBy(int uid);
     
     void updateDetails(const string &newTitle, const string &newAuthor,
                        const string &newPublisher, int newYear, const string &newISBN);
     void incrementBorrowCount();
     int getBorrowCount() const;
     
     void printDetails() const;
 
     long long getReserveTime() const;
     void setReserveTime(long long t);
 
 private:
     int bookId;
     string title;
     string author;
     string publisher;
     int year;
     string isbn;
     BookStatus status;
     int reservedBy;
     int borrowCount;
     long long reserveTime;  
 };
 
 #endif