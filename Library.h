/*
 * Library.h
 *
 * This file declares the Library class, which serves as the central component
 * integrating books, users, and file operations.
 *
 * The Library class provides methods to:
 * - Add, remove, and update books.
 * - Manage user accounts (including adding and removing users, for librarians).
 * - Process borrowing and returning transactions.
 * - Perform advanced searches.
 * - Load data from and save data to CSV files for data persistence.
 */

#ifndef LIBRARY_H
#define LIBRARY_H

#include "Book.h"
#include "User.h"
#include <vector>
#include <string>
using namespace std;

class Library {
public:
    Library();
    ~Library();

    void addUser(User *user);
    int getBooksCount() const;
    int getUsersCount() const;
    void checkOverdueNotifications(User *user, long long currentTime);
    void renewBook(User *user, int bookId, long long currentTime);
    void cancelReservation(User *user, int bookId);
    void advancedSearchBooks();
    void loadData();
    void saveData();
    void addBook(const string &title, const string &author,
                 const string &publisher, int year, const string &isbn);
    void removeBook(int bookId);
    void updateBookDetails(int bookId);
    void reserveBook(User *user, int bookId);
    void borrowReservedBook(User *user, int bookId, long long currentTime);
    void searchBooks(const string &term);
    User *findUser(int userId);
    Book *findBook(int bookId);
    User *login(const string &uname, const string &pwd);
    void displayBooks();
    void borrowBook(User *user, int bookId, long long currentTime);
    void returnBook(User *user, int bookId, long long returnTime);
    void payFine(User *user);
    void displayUsers();
    void displayFullBorrowHistory(User *user);
    void addNewUser();     
    void removeUser(int userId); 

private:
    vector<Book *> books;
    vector<User *> users;
    int nextBookId;
    int nextUserId;
};

#endif