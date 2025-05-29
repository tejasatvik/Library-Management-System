/*
 * Utility.cpp
 *
 * This file implements the helper functions declared in Utility.h.
 * These functions handle string trimming, password hashing, obtaining the current time,
 * and logging transactions to a file for audit or debugging purposes.
 */

#include "Utility.h"
#include <string>
#include <chrono>
#include <fstream>
#include <functional>
#include <ctime>
#include <iostream>
using namespace std;

string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

string hashPassword(const string &pwd) {
    return to_string(std::hash<string>{}(pwd));
}

long long getCurrentTimeInMinutes() {
    return chrono::duration_cast<chrono::minutes>(
               chrono::system_clock::now().time_since_epoch())
        .count();
}

void logTransaction(int userId, const string &message) {
    ofstream logfile("transactions.log", ios::app);
    if (logfile.is_open()) {
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        string timeStr = ctime(&now_time);
        if (!timeStr.empty() && timeStr[timeStr.size() - 1] == '\n')
            timeStr.erase(timeStr.size() - 1);
        logfile << "User " << userId << " at " << timeStr << ": " << message << endl;
        logfile.close();
    }
}
