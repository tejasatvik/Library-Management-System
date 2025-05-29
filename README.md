## Library Management System
### Overview

This Library Management System (LMS) is built in C++ using Object-Oriented Programming (OOP) concepts. It simulates a modern library where users can borrow, return, and reserve books while tracking borrowing history and fines. The system supports three types of users—Students, Faculty, and Librarians—with role-based functionalities.

### Features

#### User Roles

- **Student:**
  - Can borrow up to 3 books for a maximum of 15 days.
  - Overdue returns incur a fine of 10 rupees per day.

- **Faculty:**
  - Can borrow up to 5 books for 30 days.
  - No fines, but if any book is overdue by more than 60 days, borrowing is restricted.

- **Librarian:**
  - Can manage the library. This includes adding, removing, and updating book records and managing user accounts.
  - Librarians cannot borrow books.

#### Book Management

- Each book includes details such as title, author, publisher, publication year, and ISBN.
- Books have statuses: Available, Borrowed, or Reserved.
- Only available books can be borrowed.

#### Account Management

- Users can view their borrowing history and outstanding fines.
- Fine payment resets fines and restores borrowing privileges.

#### Data Persistence

- The system stores all data (books, users, current borrows, and borrowing history) in CSV files so that your data persists between sessions.

### Getting Started

#### Compilation

Ensure you have a C++ compiler (e.g., g++) installed. From the project directory, compile all source files with:

```bash
g++ main.cpp Book.cpp Account.cpp Utility.cpp User.cpp Library.cpp -o main
```

#### Running the Program

- **On Linux/macOS:**
  ```bash
  ./main
  ```
- **On Windows:**
  ```bash
  main OR main.exe
  ```

### Logging In

The system starts with a login prompt. Use the sample credentials below or your own if you have added new users.

#### Sample Credentials

- **Student:**
  - Username: `teja`
  - Password: `teja123`
- **Faculty:**
  - Username: `indranil`
  - Password: `indranil123`
- **Librarian:**
  - Username: `tirupati`
  - Password: `tirupati123`

### Using the System

After logging in, the system displays a menu tailored to your user role:

#### Students/Faculty

- Borrow available books.
- Reserve and return books.
- View borrowing history and update profile.
- Pay outstanding fines.

#### Librarians

- Manage books (add, remove, update).
- Manage user accounts (add and remove users).
- Search for books and display user information.
- Update profile.
- **IMPORTANT NOTE:**  
When updating or entering attribute values (such as book titles, authors, or any other field) in this Library Management System, please refrain from using commas. Since the data is stored in CSV (Comma Separated Values) files, using commas within fields can lead to parsing errors. Instead, use an alternative delimiter—such as a semicolon (;)—to separate multiple values (for example, when listing multiple authors or if the title itself contains a comma). This practice ensures data integrity and prevents potential issues during file read/write operations.

### Data Persistence

Any changes made (like borrowing or returning books, updating profiles, etc.) are saved automatically to CSV files when you exit the program. These files are used to reload your data the next time the program runs.
