#pragma once
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <vector>
#include <string>

struct Reminder {
    int id = 0;
    std::string title;
    std::string created_at;
    std::string finished_at;
    std::string description;
};

class Database {
public:
    Database(const std::string& path) : m_db_path(path), m_db(nullptr) {
        initDB();
    }

    bool open();
    void close();
    bool initDB();


    bool execute(const std::string& sql) {
        char* errMsg = nullptr;
        if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "SQL error: " << (errMsg ? errMsg : "unknown") << "\n";
            if (errMsg) sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

    bool newRemider(const char* title, const char* description);
    std::vector<Reminder> getReminders();
    bool removeReminder(int id);

private:
    std::string m_db_path;
    sqlite3* m_db;

    static inline const char *db_schema[] = {
    // Initial scheme

    "CREATE TABLE IF NOT EXISTS Reminders (\n"
    "    id INTEGER PRIMARY KEY ASC,\n"
    "    title TEXT NOT NULL,\n"
    "    created_at DATETIME NOT NULL DEFAULT (DATE('now')),\n"
    "    description TEXT DEFAULT NULL\n"
    ");\n",

    "CREATE TABLE IF NOT EXISTS Reminders_old (\n"
    "    id INTEGER PRIMARY KEY ASC,\n"
    "    title TEXT NOT NULL,\n"
    "    created_at DATETIME DEFAULT NULL,\n"
    "    description TEXT DEFAULT NULL,\n"
    "    finished_at DATETIME NOT NULL DEFAULT (DATE('now'))\n"
    ");\n"
};

};