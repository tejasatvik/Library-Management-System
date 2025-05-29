/*
 * Utility.h
 *
 * This file declares helper functions used throughout the project:
 * - trim(): Removes leading and trailing whitespace from a string.
 * - hashPassword(): Hashes a password using std::hash.
 * - getCurrentTimeInMinutes(): Returns the current time in minutes since the epoch.
 * - logTransaction(): Logs transactions to a file.
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <string>
using namespace std;

string trim(const string &s);
string hashPassword(const string &pwd);
long long getCurrentTimeInMinutes();
void logTransaction(int userId, const string &message);

#endif
