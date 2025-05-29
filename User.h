/*
 * User.h
 *
 * This file declares the User base class and its derived classes:
 * Student, Faculty, and Librarian.
 *
 * The User class represents a library user with an associated Account.
 * It declares virtual functions such as borrowBook() and returnBook() that
 * are overridden in derived classes to enforce role-specific borrowing rules.
 *
 * The constructor accepts a flag (isAlreadyHashed) to ensure that raw passwords
 * are hashed only once.
 */

#ifndef USER_H
#define USER_H

#include "Account.h"
#include <string>
#include <vector>
using namespace std;

class Book; 
class User {
public:
    User(int id, const string &n, const string &uname, const string &pwd, bool isAlreadyHashed);
    virtual ~User();

    int getUserId() const;
    string getName() const;
    string getUsername() const;
    string getHashedPassword() const;
    Account &getAccount();

    void setPasswordRaw(const string &rawPwd);
    bool authenticate(const string &uname, const string &enteredPwd) const;
    void updateProfile();

    virtual int getMaxBooks() const = 0;
    virtual int getBorrowPeriod() const = 0;
    virtual int getFinePerDay() const = 0;
    virtual string getRole() const = 0;

    virtual bool canBorrow(long long currentTime, const vector<Book*> &libraryBooks);
    virtual bool additionalBorrowCheck(long long currentTime, const vector<Book*> &libraryBooks);
    virtual bool borrowBook(Book *book, long long currentTime, const vector<Book*> &libraryBooks);
    virtual void returnBook(Book *book, long long returnTime);
    virtual void printDetails() const;

protected:
    int userId;
    string name;
    string username;
    string password;
    Account account;
};

class Student : public User {
public:
    Student(int id, const string &n, const string &uname, const string &pwd, bool isAlreadyHashed);
    virtual int getMaxBooks() const override;
    virtual int getBorrowPeriod() const override;
    virtual int getFinePerDay() const override;
    virtual string getRole() const override;
};

class Faculty : public User {
public:
    Faculty(int id, const string &n, const string &uname, const string &pwd, bool isAlreadyHashed);
    virtual int getMaxBooks() const override;
    virtual int getBorrowPeriod() const override;
    virtual int getFinePerDay() const override;
    virtual string getRole() const override;
    virtual bool additionalBorrowCheck(long long currentTime, const vector<Book*> &libraryBooks) override;
    virtual bool borrowBook(Book *book, long long currentTime, const vector<Book*> &libraryBooks) override;
};

class Librarian : public User {
public:
    Librarian(int id, const string &n, const string &uname, const string &pwd, bool isAlreadyHashed);
    virtual int getMaxBooks() const override;
    virtual int getBorrowPeriod() const override;
    virtual int getFinePerDay() const override;
    virtual string getRole() const override;
    virtual bool borrowBook(Book *book, long long currentTime, const vector<Book*> &libraryBooks) override;
};

#endif