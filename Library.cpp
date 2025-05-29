/*
 * Library.cpp
 *
 * This file implements the functions declared in Library.h.
 * It manages the overall system by handling book and user management,
 * processing borrow/return operations, performing searches, and managing
 * data persistence through file I/O operations.
 *
 * It also includes functions for librarian-specific operations, such as adding
 * or removing users, and now reservation expiration logic.
 */

 #include "Library.h"
 #include "Utility.h"
 #include "User.h"
 #include "Book.h"
 #include <iostream>
 #include <fstream>
 #include <sstream>
 #include <algorithm>
 using namespace std;
 
 Library::Library() : nextBookId(1), nextUserId(1) { }
 
 Library::~Library() {
     for (auto b : books)
         delete b;
     for (auto u : users)
         delete u;
 }
 
 void Library::addUser(User *user) {
     users.push_back(user);
     cout << "Added " << user->getRole() << ": " << user->getName() << endl;
 }
 
 int Library::getBooksCount() const { return books.size(); }
 int Library::getUsersCount() const { return users.size(); }
 
 void Library::checkOverdueNotifications(User *user, long long currentTime) {
     const vector<BorrowedBook> &borrows = user->getAccount().getBorrowedBooks();
     if (borrows.empty()) return;
     cout << "\nOverdue Notifications:" << endl;
     bool anyNotification = false;
     for (const auto &bb : borrows) {
         Book *book = findBook(bb.bookId);
         if (!book) continue;
         int allowedPeriod = user->getBorrowPeriod();
         long long allowedTime = static_cast<long long>(allowedPeriod) * 24 * 60;
         long long elapsed = currentTime - bb.borrowTime;
         if (elapsed >= allowedTime) {
             cout << "Book \"" << book->getTitle() << "\" is OVERDUE." << endl;
             anyNotification = true;
         } else if (elapsed >= allowedTime * 0.8) {
             cout << "Book \"" << book->getTitle() << "\" is nearing its due date." << endl;
             anyNotification = true;
         }
     }
     if (!anyNotification)
         cout << "No overdue or near-due notifications." << endl;
 }
 
 void Library::renewBook(User *user, int bookId, long long currentTime) {
     Book *book = findBook(bookId);
     if (!book) {
         cout << "Book not found." << endl;
         return;
     }
     if (book->getReservedBy() != 0) {
         cout << "Book is reserved by another user. Renewal denied." << endl;
         return;
     }
     bool success = user->getAccount().renewBorrowedBook(bookId, currentTime);
     if (success) {
         cout << "Book \"" << book->getTitle() << "\" renewed successfully." << endl;
         logTransaction(user->getUserId(), "Renewed book " + to_string(bookId));
     } else {
         cout << "You have not borrowed this book." << endl;
     }
 }
 
 void Library::cancelReservation(User *user, int bookId) {
     Book *book = findBook(bookId);
     if (!book) {
         cout << "Book not found." << endl;
         return;
     }
     if (book->getReservedBy() != user->getUserId()) {
         cout << "You did not reserve this book." << endl;
         return;
     }
     book->setReservedBy(0);
     book->setReserveTime(0);
     if (book->getStatus() == RESERVED)
         book->setStatus(AVAILABLE);
     cout << "Reservation for book \"" << book->getTitle() << "\" cancelled." << endl;
     logTransaction(user->getUserId(), "Cancelled reservation for book " + to_string(bookId));
 }
 
 bool isReservationExpired(Book *book, long long currentTime) {
     const long long reservationPeriod = 7LL * 24 * 60; 
     return (currentTime - book->getReserveTime()) > reservationPeriod;
 }
 
 void Library::advancedSearchBooks() {
     cin.ignore();
     cout << "Enter search term (or press enter to skip): " << endl;
     string term;
     getline(cin, term);
     cout << "Enter publication year to filter (or 0 to skip): " << endl;
     int yearFilter;
     cin >> yearFilter;
     cout << "Filter by availability? (1: Available, 2: Borrowed, 3: Reserved, 0: skip): " << endl;
     int availFilter;
     cin >> availFilter;
     cout << "Sort results (1: Popularity, 2: Recency, 0: none): " << endl;
     int sortOption;
     cin >> sortOption;
     vector<Book *> results;
     for (auto b : books) {
         bool match = true;
         if (!term.empty()) {
             string title = b->getTitle(), author = b->getAuthor();
             transform(title.begin(), title.end(), title.begin(), ::tolower);
             transform(author.begin(), author.end(), author.begin(), ::tolower);
             string termLower = term;
             transform(termLower.begin(), termLower.end(), termLower.begin(), ::tolower);
             if (title.find(termLower) == string::npos && author.find(termLower) == string::npos)
                 match = false;
         }
         if (yearFilter != 0 && b->getYear() != yearFilter)
             match = false;
         if (availFilter != 0) {
             BookStatus status = b->getStatus();
             if (availFilter == 1 && status != AVAILABLE)
                 match = false;
             if (availFilter == 2 && status != BORROWED)
                 match = false;
             if (availFilter == 3 && status != RESERVED)
                 match = false;
         }
         if (match)
             results.push_back(b);
     }
     if (sortOption == 1) {
         sort(results.begin(), results.end(), [](Book *a, Book *b) {
             return a->getBorrowCount() > b->getBorrowCount();
         });
     } else if (sortOption == 2) {
         sort(results.begin(), results.end(), [](Book *a, Book *b) {
             return a->getBookId() > b->getBookId();
         });
     }
     if (results.empty()) {
         cout << "No matching books found." << endl;
     } else {
         cout << "Advanced Search Results:" << endl;
         for (auto b : results) {
             b->printDetails();
             cout << "---------------------" << endl;
         }
     }
 }
 
 void Library::loadData() {
     ifstream bookFile("books.csv");
     if (bookFile.is_open()) {
         string line;
         int maxBookIdLocal = 0;
         while (getline(bookFile, line)) {
             line = trim(line);
             if (line.empty()) continue;
             stringstream ss(line);
             vector<string> tokens;
             string token;
             while (getline(ss, token, ',')) {
                 tokens.push_back(token);
             }
             if (tokens.size() < 8) {
                 cerr << "Skipping malformed line in books.csv: " << line << endl;
                 continue;
             }
             try {
                 int bookId = stoi(tokens[0]);
                 string title = tokens[1];
                 string author = tokens[2];
                 string publisher = tokens[3];
                 int year = stoi(tokens[4]);
                 string isbn = tokens[5];
                 int statusInt = stoi(tokens[6]);
                 int reservedBy = stoi(tokens[7]);
                 BookStatus status = static_cast<BookStatus>(statusInt);
                 Book *book = new Book(bookId, title, author, publisher, year, isbn);
                 book->setStatus(status);
                 book->setReservedBy(reservedBy);
                 // If reservedBy is non-zero, the reserveTime should have been stored.
                 // (For simplicity, if not found, it remains 0.)
                 books.push_back(book);
                 if (bookId > maxBookIdLocal)
                     maxBookIdLocal = bookId;
             } catch (const exception &e) {
                 cerr << "Skipping malformed line in books.csv: " << line << endl;
                 continue;
             }
         }
         nextBookId = maxBookIdLocal + 1;
         bookFile.close();
     }
 
     ifstream userFile("users.csv");
     if (userFile.is_open()) {
         string line;
         int maxUserIdLocal = 0;
         while (getline(userFile, line)) {
             line = trim(line);
             if (line.empty()) continue;
             stringstream ss(line);
             vector<string> tokens;
             string token;
             while (getline(ss, token, ',')) {
                 tokens.push_back(token);
             }
             if (tokens.size() < 6) {
                 cerr << "Skipping malformed line in users.csv: " << line << endl;
                 continue;
             }
             try {
                 int userId = stoi(tokens[0]);
                 string name = tokens[1];
                 string role = tokens[2];
                 string uname = tokens[3];
                 string hashedPwd = tokens[4];
                 double fine = stod(tokens[5]);
                 User *user = nullptr;
                 if (role == "Student")
                     user = new Student(userId, name, uname, hashedPwd, true);
                 else if (role == "Faculty")
                     user = new Faculty(userId, name, uname, hashedPwd, true);
                 else if (role == "Librarian")
                     user = new Librarian(userId, name, uname, hashedPwd, true);
                 if (user) {
                     user->getAccount().setFine(fine);
                     users.push_back(user);
                     if (userId > maxUserIdLocal)
                         maxUserIdLocal = userId;
                 }
             } catch (const exception &e) {
                 cerr << "Skipping malformed line in users.csv: " << line << endl;
                 continue;
             }
         }
         nextUserId = maxUserIdLocal + 1;
         userFile.close();
     }
 
     ifstream borrowedFile("borrowed.csv");
     if (borrowedFile.is_open()) {
         string line;
         while (getline(borrowedFile, line)) {
             line = trim(line);
             if (line.empty()) continue;
             stringstream ss(line);
             vector<string> tokens;
             string token;
             while (getline(ss, token, ',')) {
                 tokens.push_back(token);
             }
             if (tokens.size() < 3) {
                 cerr << "Skipping malformed line in borrowed.csv: " << line << endl;
                 continue;
             }
             try {
                 int userId = stoi(tokens[0]);
                 int bookId = stoi(tokens[1]);
                 long long bTime = stoll(tokens[2]);
                 User *user = findUser(userId);
                 if (user)
                     user->getAccount().addBorrowedBook(bookId, bTime);
             } catch (const exception &e) {
                 cerr << "Skipping malformed line in borrowed.csv: " << line << endl;
                 continue;
             }
         }
         borrowedFile.close();
     }
 
     ifstream historyFile("history.csv");
     if (historyFile.is_open()) {
         string line;
         while (getline(historyFile, line)) {
             line = trim(line);
             if (line.empty()) continue;
             stringstream ss(line);
             vector<string> tokens;
             string token;
             while (getline(ss, token, ',')) {
                 tokens.push_back(token);
             }
             if (tokens.size() < 6) {
                 cerr << "Skipping malformed line in history.csv: " << line << endl;
                 continue;
             }
             try {
                 int userId = stoi(tokens[0]);
                 int bookId = stoi(tokens[1]);
                 long long bTime = stoll(tokens[2]);
                 long long rTime = stoll(tokens[3]);
                 int overdueDays = stoi(tokens[4]);
                 double fineCharged = stod(tokens[5]);
                 User *user = findUser(userId);
                 if (user) {
                     BorrowHistory record = {bookId, bTime, rTime, overdueDays, fineCharged};
                     user->getAccount().addHistoryRecord(record);
                 }
             } catch (const exception &e) {
                 cerr << "Skipping malformed line in history.csv: " << line << endl;
                 continue;
             }
         }
         historyFile.close();
     }
 }
 
 void Library::saveData() {
     ofstream bookFile("books.csv");
     if (bookFile.is_open()) {
         for (auto b : books) {
             bookFile << b->getBookId() << "," << b->getTitle() << ","
                      << b->getAuthor() << "," << b->getPublisher() << ","
                      << b->getYear() << "," << b->getISBN() << ","
                      << static_cast<int>(b->getStatus()) << ","
                      << b->getReservedBy() << "\n";
         }
         bookFile.close();
     } else {
         cerr << "Error saving books.csv" << endl;
     }
     ofstream userFile("users.csv");
     if (userFile.is_open()) {
         for (auto u : users) {
             userFile << u->getUserId() << "," << u->getName() << ","
                      << u->getRole() << "," << u->getUsername() << ","
                      << u->getHashedPassword() << ","
                      << u->getAccount().getFine() << "\n";
         }
         userFile.close();
     } else {
         cerr << "Error saving users.csv" << endl;
     }
     ofstream borrowedFile("borrowed.csv");
     if (borrowedFile.is_open()) {
         for (auto u : users) {
             const vector<BorrowedBook> &borrowed = u->getAccount().getBorrowedBooks();
             for (auto bb : borrowed) {
                 borrowedFile << u->getUserId() << "," << bb.bookId << ","
                              << bb.borrowTime << "\n";
             }
         }
         borrowedFile.close();
     } else {
         cerr << "Error saving borrowed.csv" << endl;
     }
     ofstream historyFile("history.csv");
     if (historyFile.is_open()) {
         for (auto u : users) {
             const vector<BorrowHistory> &hist = u->getAccount().getHistory();
             for (auto h : hist) {
                 historyFile << u->getUserId() << "," << h.bookId << ","
                             << h.borrowTime << "," << h.returnTime << ","
                             << h.overdueDays << "," << h.fineCharged << "\n";
             }
         }
         historyFile.close();
     } else {
         cerr << "Error saving history.csv" << endl;
     }
 }
 
 void Library::addBook(const string &title, const string &author,
                       const string &publisher, int year, const string &isbn) {
     Book *book = new Book(nextBookId++, title, author, publisher, year, isbn);
     books.push_back(book);
     cout << "Added book: " << title << endl;
     logTransaction(0, "Added book " + to_string(book->getBookId()) + ": " + title);
 }
 
 void Library::removeBook(int bookId) {
     auto it = remove_if(books.begin(), books.end(), [bookId](Book *b) {
         return b->getBookId() == bookId;
     });
     if (it != books.end()) {
         cout << "Removed book with ID: " << bookId << endl;
         delete *it;
         books.erase(it);
         logTransaction(0, "Removed book " + to_string(bookId));
     } else {
         cout << "Book with ID " << bookId << " not found." << endl;
     }
 }
 
 void Library::updateBookDetails(int bookId) {
     Book *book = findBook(bookId);
     if (!book) {
         cout << "Book not found." << endl;
         return;
     }
     string newTitle, newAuthor, newPublisher, newISBN;
     int newYear;
     cin.ignore();
     cout << "Enter new title (or press enter to keep \"" << book->getTitle() << "\"):" << endl;
     getline(cin, newTitle);
     if (newTitle.empty()) newTitle = book->getTitle();
     cout << "Enter new author (or press enter to keep \"" << book->getAuthor() << "\"):" << endl;
     getline(cin, newAuthor);
     if (newAuthor.empty()) newAuthor = book->getAuthor();
     cout << "Enter new publisher (or press enter to keep \"" << book->getPublisher() << "\"):" << endl;
     getline(cin, newPublisher);
     if (newPublisher.empty()) newPublisher = book->getPublisher();
     cout << "Enter new year (or 0 to keep \"" << book->getYear() << "\"):" << endl;
     cin >> newYear;
     if (newYear == 0) newYear = book->getYear();
     cin.ignore();
     cout << "Enter new ISBN (or press enter to keep \"" << book->getISBN() << "\"):" << endl;
     getline(cin, newISBN);
     if (newISBN.empty()) newISBN = book->getISBN();
     book->updateDetails(newTitle, newAuthor, newPublisher, newYear, newISBN);
     cout << "Book details updated." << endl;
     logTransaction(0, "Updated details for book " + to_string(bookId));
 }
 
 void Library::reserveBook(User *user, int bookId) {
     Book *book = findBook(bookId);
     if (!book) {
         cout << "Book not found." << endl;
         return;
     }
     if (book->getStatus() == AVAILABLE) {
         cout << "Book is available. You can borrow it instead of reserving." << endl;
         return;
     }
     if (book->getReservedBy() != 0) {
         cout << "Book is already reserved." << endl;
         return;
     }
     book->setReservedBy(user->getUserId());
     book->setReserveTime(getCurrentTimeInMinutes());
     cout << "Book \"" << book->getTitle() << "\" reserved successfully." << endl;
     logTransaction(user->getUserId(), "Reserved book " + to_string(bookId));
 }
 
 void Library::borrowReservedBook(User *user, int bookId, long long currentTime) {
     Book *book = findBook(bookId);
     if (!book) {
         cout << "Book not found." << endl;
         return;
     }
     if (isReservationExpired(book, currentTime)) {
         book->setReservedBy(0);
         book->setReserveTime(0);
         book->setStatus(AVAILABLE);
         cout << "Reservation expired. The book is now available." << endl;
         return;
     }
     if (book->getStatus() != RESERVED || book->getReservedBy() != user->getUserId()) {
         cout << "No reserved book found for you with this ID." << endl;
         return;
     }
     user->borrowBook(book, currentTime, books);
     logTransaction(user->getUserId(), "Borrowed reserved book " + to_string(bookId));
 }
 
 void Library::searchBooks(const string &term) {
     cout << endl << "Search results for \"" << term << "\":" << endl;
     bool found = false;
     for (auto b : books) {
         string title = b->getTitle(), author = b->getAuthor();
         transform(title.begin(), title.end(), title.begin(), ::tolower);
         transform(author.begin(), author.end(), author.begin(), ::tolower);
         string termLower = term;
         transform(termLower.begin(), termLower.end(), termLower.begin(), ::tolower);
         if (title.find(termLower) != string::npos || author.find(termLower) != string::npos) {
             b->printDetails();
             cout << "---------------------" << endl;
             found = true;
         }
     }
     if (!found)
         cout << "No matching books found." << endl;
 }
 
 User *Library::findUser(int userId) {
     for (auto u : users) {
         if (u->getUserId() == userId)
             return u;
     }
     return nullptr;
 }
 
 Book *Library::findBook(int bookId) {
     for (auto b : books) {
         if (b->getBookId() == bookId)
             return b;
     }
     return nullptr;
 }
 
 User *Library::login(const string &uname, const string &pwd) {
     for (auto u : users) {
         if (u->authenticate(uname, pwd))
             return u;
     }
     return nullptr;
 }
 
 void Library::displayBooks() {
     for (auto b : books) {
         b->printDetails();
         cout << "---------------------" << endl;
     }
 }
 
 void Library::borrowBook(User *user, int bookId, long long currentTime) {
     if (!user) {
         cout << "Invalid user." << endl;
         return;
     }
     Book *book = findBook(bookId);
     if (!book) {
         cout << "Book not found." << endl;
         return;
     }
     if (book->getStatus() != AVAILABLE) {
         cout << "Book is not available for borrowing." << endl;
         return;
     }
     if (user->borrowBook(book, currentTime, books))
         logTransaction(user->getUserId(), "Borrowed book " + to_string(bookId));
 }
 
 void Library::returnBook(User *user, int bookId, long long returnTime) {
     if (!user) {
         cout << "Invalid user." << endl;
         return;
     }
     Book *book = findBook(bookId);
     if (!book) {
         cout << "Book not found." << endl;
         return;
     }
     user->returnBook(book, returnTime);
     logTransaction(user->getUserId(), "Returned book " + to_string(bookId));
 }
 
 void Library::payFine(User *user) {
     if (!user) {
         cout << "Invalid user." << endl;
         return;
     }
     double before = user->getAccount().getFine();
     cout << "Fine before payment: " << before << endl;
     user->getAccount().payFine();
     cout << "Fine paid. Current fine: " << user->getAccount().getFine() << endl;
     logTransaction(user->getUserId(), "Paid fine of " + to_string(before));
 }
 
 void Library::displayUsers() {
     for (auto u : users) {
         u->printDetails();
         cout << "Current borrowed books: " << u->getAccount().getBorrowedCount() << endl;
         cout << "Outstanding fine: " << u->getAccount().getFine() << endl;
         cout << "---------------------" << endl;
     }
 }
 
 void Library::displayFullBorrowHistory(User *user) {
     const auto &currentBorrows = user->getAccount().getBorrowedBooks();
     if (!currentBorrows.empty()) {
         cout << endl << "Currently Borrowed Books:" << endl;
         for (const auto &bb : currentBorrows) {
             Book *book = findBook(bb.bookId);
             if (book) {
                 cout << "Book ID: " << bb.bookId << " - " << book->getTitle() 
                      << " | Borrowed at: " << bb.borrowTime << endl;
             }
         }
     } else {
         cout << endl << "No books are currently borrowed." << endl;
     }
     const auto &hist = user->getAccount().getHistory();
     if (!hist.empty()) {
         cout << endl << "Past Borrowing History:" << endl;
         for (const auto &h : hist) {
             Book *book = findBook(h.bookId);
             if (book) {
                 cout << "Book ID: " << h.bookId << " - " << book->getTitle() 
                      << " | Borrowed at: " << h.borrowTime 
                      << " | Returned at: " << h.returnTime 
                      << " | Overdue Days: " << h.overdueDays 
                      << " | Fine Charged: " << h.fineCharged << endl;
             }
         }
     } else {
         cout << endl << "No past borrowing history found." << endl;
     }
 }
 
 void Library::addNewUser() {
     cin.ignore();
     cout << "Enter user type (Student, Faculty, Librarian): " << endl;
     string type;
     getline(cin, type);
 
     cout << "Enter name: " << endl;
     string name;
     getline(cin, name);
 
     cout << "Enter username: " << endl;
     string uname;
     getline(cin, uname);
 
     cout << "Enter password: " << endl;
     string pwd;
     getline(cin, pwd);
 
     User *newUser = nullptr;
     if (type == "Student" || type == "student") {
         newUser = new Student(nextUserId, name, uname, pwd, false);
     } else if (type == "Faculty" || type == "faculty") {
         newUser = new Faculty(nextUserId, name, uname, pwd, false);
     } else if (type == "Librarian" || type == "librarian") {
         newUser = new Librarian(nextUserId, name, uname, pwd, false);
     } else {
         cout << "Invalid user type." << endl;
         return;
     }
     addUser(newUser);
     nextUserId++;
     logTransaction(newUser->getUserId(), "Added new user (" + newUser->getRole() + ")");
 }
 
 void Library::removeUser(int userId) {
     for (auto it = users.begin(); it != users.end(); ++it) {
         if ((*it)->getUserId() == userId) {
             cout << "Removing user: " << (*it)->getName() << endl;
             logTransaction(userId, "Removed user (" + (*it)->getRole() + ")");
             delete *it;
             users.erase(it);
             return;
         }
     }
     cout << "User with ID " << userId << " not found." << endl;
 }
 