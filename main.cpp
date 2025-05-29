/*
 * main.cpp
 *
 * This is the entry point of the Library Management System.
 * It displays a login prompt, and after successful login, displays a menu tailored to the user role.
 * The program uses the Library class to manage books, users, and transactions.
 * Data is saved on exit.
 */

 #include "Library.h"
 #include "Utility.h"
 #include <iostream>
 using namespace std;

 void printUserHelp();
 void printLibrarianHelp();
 void showUserMenu();
 void showLibrarianMenu();
 
 int main() {
     Library lib;
     lib.loadData();
 
     if (lib.getBooksCount() == 0) {
        lib.addBook("Pattern Recognition and Machine Learning", "Christopher Bishop", "Springer", 2006, "9780387310732");
        lib.addBook("Machine Learning: A Probabilistic Perspective", "Kevin Murphy", "MIT Press", 2012, "9780262018029");
        lib.addBook("Deep Learning", "Ian Goodfellow", "MIT Press", 2016, "9780262035613");
        lib.addBook("The Elements of Statistical Learning", "Trevor Hastie", "Springer", 2009, "9780387848570");
        lib.addBook("Introduction to Machine Learning with Python", "Andreas Müller", "O'Reilly Media", 2016, "9781449369415");
        lib.addBook("Hands-On Machine Learning with Scikit-Learn; Keras; and TensorFlow", "Aurélien Géron", "O'Reilly Media", 2019, "9781492032649");
        lib.addBook("Data Mining: Concepts and Techniques", "Jiawei Han", "Morgan Kaufmann", 2011, "9780123814791");
        lib.addBook("Reinforcement Learning: An Introduction", "Richard S. Sutton", "MIT Press", 2018, "9780262039246");
        lib.addBook("Understanding Machine Learning: From Theory to Algorithms", "Shai Shalev-Shwartz", "Cambridge University Press", 2014, "9781107057135");
        lib.addBook("Artificial Intelligence: A Modern Approach", "Stuart Russell", "Prentice Hall", 2010, "9780136042594");
        
        lib.addUser(new Student(1, "Teja",    "teja",    "teja123",    false));
        lib.addUser(new Student(2, "Obul",      "obul",      "obul123",      false));
        lib.addUser(new Student(3, "Anirudh",  "anirudh",  "anirudh123",  false));
        lib.addUser(new Student(4, "Nikhilesh",    "nikhilesh",    "nikhilesh123",    false));
        lib.addUser(new Student(5, "Satvik",      "satvik",      "satvik123",      false));
        lib.addUser(new Faculty(6, "Prof. Indranil Saha",    "indranil",    "indranil123",    false));
        lib.addUser(new Faculty(7, "Prof. Sandeep Shukla",  "sandeep",  "sandeep123",  false));
        lib.addUser(new Faculty(8, "Prof. Debapriya Basu Roy", "debapriya", "debapriya123", false));
        lib.addUser(new Librarian(9, "Mr. Tirupati", "tirupati", "tirupati123", false));
     }
 
     User *currentUser = nullptr;
     bool exitProgram = false;
     while (!exitProgram) {
         while (!currentUser) {
             cout << endl << "=== Login Page ===" << endl;
             cout << "Username: " << flush;
             string uname;
             cin >> uname;
             cout << "Password: " << flush;
             string pwd;
             cin >> pwd;
             currentUser = lib.login(uname, pwd);
             if (!currentUser)
                 cout << "Invalid credentials, please try again." << endl;
         }
 
         long long currentTime = getCurrentTimeInMinutes();
         if (currentUser->getRole() != "Librarian")
             lib.checkOverdueNotifications(currentUser, currentTime);
 
         bool logout = false;
         while (!logout) {
             currentTime = getCurrentTimeInMinutes();
             if (currentUser->getRole() == "Librarian") {
                 showLibrarianMenu();
                 int choice;
                 cin >> choice;
                 switch (choice) {
                     case 0:
                         printLibrarianHelp();
                         break;
                     case 1:
                         lib.displayBooks();
                         break;
                     case 2: {
                         cout << "Enter search term: " << flush;
                         string term;
                         cin.ignore();
                         getline(cin, term);
                         lib.searchBooks(term);
                         break;
                     }
                     case 3:
                         lib.displayUsers();
                         break;
                     case 4: {
                         string title, author, publisher, isbn;
                         int year;
                         cin.ignore();
                         cout << "Enter book title: " << flush;
                         getline(cin, title);
                         cout << "Enter author: " << flush;
                         getline(cin, author);
                         cout << "Enter publisher: " << flush;
                         getline(cin, publisher);
                         cout << "Enter year: " << flush;
                         cin >> year;
                         cout << "Enter ISBN: " << flush;
                         cin >> isbn;
                         lib.addBook(title, author, publisher, year, isbn);
                         break;
                     }
                     case 5: {
                         int bookId;
                         cout << "Enter book ID to remove: " << flush;
                         cin >> bookId;
                         lib.removeBook(bookId);
                         break;
                     }
                     case 6: {
                         int bookId;
                         cout << "Enter book ID to update: " << flush;
                         cin >> bookId;
                         lib.updateBookDetails(bookId);
                         break;
                     }
                     case 7:
                         lib.addNewUser();
                         break;
                     case 8: {
                         int userId;
                         cout << "Enter user ID to remove: " << flush;
                         cin >> userId;
                         lib.removeUser(userId);
                         break;
                     }
                     case 9:
                         logout = true;
                         currentUser = nullptr;
                         break;
                     case 10:
                         currentUser->updateProfile();
                         break;
                     case 11:
                         lib.advancedSearchBooks();
                         break;
                     default:
                         cout << "Invalid option. Try again." << endl;
                 }
             } else {
                 showUserMenu();
                 int choice;
                 cin >> choice;
                 switch (choice) {
                     case 0:
                         printUserHelp();
                         break;
                     case 1:
                         lib.displayBooks();
                         break;
                     case 2: {
                         cout << "Enter search term: " << flush;
                         string term;
                         cin.ignore();
                         getline(cin, term);
                         lib.searchBooks(term);
                         break;
                     }
                     case 3: {
                         int bookId;
                         cout << "Enter book ID to borrow: " << flush;
                         cin >> bookId;
                         lib.borrowBook(currentUser, bookId, currentTime);
                         break;
                     }
                     case 4: {
                         int bookId;
                         cout << "Enter reserved book ID to borrow: " << flush;
                         cin >> bookId;
                         lib.borrowReservedBook(currentUser, bookId, currentTime);
                         break;
                     }
                     case 5: {
                         int bookId;
                         cout << "Enter book ID to reserve: " << flush;
                         cin >> bookId;
                         lib.reserveBook(currentUser, bookId);
                         break;
                     }
                     case 6: {
                         int bookId;
                         cout << "Enter book ID to return: " << flush;
                         cin >> bookId;
                         lib.returnBook(currentUser, bookId, currentTime);
                         break;
                     }
                     case 7:
                         lib.displayFullBorrowHistory(currentUser);
                         break;
                     case 8:
                         lib.payFine(currentUser);
                         break;
                     case 9:
                         logout = true;
                         currentUser = nullptr;
                         break;
                     case 10:
                         currentUser->updateProfile();
                         break;
                     case 11:
                         lib.advancedSearchBooks();
                         break;
                     case 12: {
                         int bookId;
                         cout << "Enter book ID to renew: " << flush;
                         cin >> bookId;
                         lib.renewBook(currentUser, bookId, currentTime);
                         break;
                     }
                     case 13: {
                         int bookId;
                         cout << "Enter book ID to cancel reservation: " << flush;
                         cin >> bookId;
                         lib.cancelReservation(currentUser, bookId);
                         break;
                     }
                     default:
                         cout << "Invalid option. Try again." << endl;
                 }
             }
         }
         cout << "Do you want to exit the program? (y/n): " << flush;
         char exitChoice;
         cin >> exitChoice;
         if (exitChoice == 'y' || exitChoice == 'Y')
             exitProgram = true;
     }
     lib.saveData();
     return 0;
 }
 
 void printUserHelp() {
     cout << endl
          << "User Help:" << endl
          << "0: Help - Display this help message." << endl
          << "1: Display all books." << endl
          << "2: Search books by title or author." << endl
          << "3: Borrow a book." << endl
          << "4: Borrow a reserved book (if you reserved it)." << endl
          << "5: Reserve a book." << endl
          << "6: Return a book." << endl
          << "7: View your full borrowing history (current & past)." << endl
          << "8: Pay outstanding fine." << endl
          << "9: Logout." << endl
          << "10: Update Profile." << endl
          << "11: Advanced Search." << endl
          << "12: Renew a Book." << endl
          << "13: Cancel Reservation." << endl;
 }
 
 void printLibrarianHelp() {
     cout << endl
          << "Librarian Help:" << endl
          << "0: Help - Display this help message." << endl
          << "1: Display all books." << endl
          << "2: Search books by title or author." << endl
          << "3: Display all users." << endl
          << "4: Add a new book." << endl
          << "5: Remove a book." << endl
          << "6: Update book details." << endl
          << "7: Add a new user." << endl
          << "8: Remove a user." << endl
          << "9: Logout." << endl
          << "10: Update Profile." << endl
          << "11: Advanced Search." << endl;
 }
 
 void showUserMenu() {
     cout << endl
          << "----- User Menu -----" << endl
          << "0. Help" << endl
          << "1. Display all books" << endl
          << "2. Search books" << endl
          << "3. Borrow a book" << endl
          << "4. Borrow a reserved book" << endl
          << "5. Reserve a book" << endl
          << "6. Return a book" << endl
          << "7. View your full borrowing history" << endl
          << "8. Pay fine" << endl
          << "9. Logout" << endl
          << "10. Update Profile" << endl
          << "11. Advanced Search" << endl
          << "12. Renew a Book" << endl
          << "13. Cancel Reservation" << endl
          << "Enter your choice: " << flush;
 }
 
 void showLibrarianMenu() {
     cout << endl
          << "----- Librarian Menu -----" << endl
          << "0. Help" << endl
          << "1. Display all books" << endl
          << "2. Search books" << endl
          << "3. Display all users" << endl
          << "4. Add a new book" << endl
          << "5. Remove a book" << endl
          << "6. Update book details" << endl
          << "7. Add a new user" << endl
          << "8. Remove a user" << endl
          << "9. Logout" << endl
          << "10. Update Profile" << endl
          << "11. Advanced Search" << endl
          << "Enter your choice: " << flush;
 }
 