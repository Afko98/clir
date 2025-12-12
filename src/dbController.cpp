#include "dbController.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>  

bool Database::open() {
    if (sqlite3_open(m_db_path.c_str(), &m_db) != SQLITE_OK) {
        std::cerr << "Cannot open DB: " << sqlite3_errmsg(m_db) << m_db_path.c_str()<< "\n";
        return false;
    }
    return true;
}

void Database::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }  
}

bool Database::initDB() {
    std::string folder = std::string(getenv("HOME")) + "/sql_db";
    mkdir(folder.c_str(), 0755);
    if (open())
    {
        for (int i = 0; i < (sizeof(db_schema)/sizeof(db_schema[0])); i++)
        {
            execute(db_schema[i]);
        }
    }
    else 
    {
        std::cout << "fail to open db";
        return false;
    }

    close();
    return true;
}

bool Database::newRemider(const char* title, const char* description) {
    sqlite3_stmt* stmt = nullptr;
    open();
    
    if (sqlite3_prepare_v2(m_db, "INSERT INTO Reminders (title, description) VALUES (?, ?)", -1, &stmt, NULL) != SQLITE_OK)
    {
        close();
        return false;
    }

    sqlite3_bind_text(stmt, 1, title ? title : "", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, description ? description : "", -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        std::cerr << "Insert failed: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    } 
    else 
    {
        std::cout << "Reminder added successfully!\n";
        return true;
    }
}
bool Database::removeReminder(int id) {
    sqlite3_stmt* stmt = nullptr;
    open();

    const char* sql_insert_old =
        "INSERT INTO Reminders_old (title, description, created_at) "
        "SELECT title, description, created_at FROM Reminders WHERE id = ?;";

    if (sqlite3_prepare_v2(m_db, sql_insert_old, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (insert into old): " << sqlite3_errmsg(m_db) << "\n";
        close();
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Insert into old failed: " << sqlite3_errmsg(m_db) << "\n";
        sqlite3_finalize(stmt);
        close();
        return false;
    }

    sqlite3_finalize(stmt);

    const char* sql_delete = "DELETE FROM Reminders WHERE id = ?;";

    if (sqlite3_prepare_v2(m_db, sql_delete, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (delete): " << sqlite3_errmsg(m_db) << "\n";
        close();
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Delete failed: " << sqlite3_errmsg(m_db) << "\n";
        sqlite3_finalize(stmt);
        close();
        return false;
    }

    sqlite3_finalize(stmt);
    close();

    return true;
}

std::vector<Reminder> Database::getReminders() {
    sqlite3_stmt* stmt = nullptr;
    std::vector<Reminder> return_reminders;

    if (!m_db && !open()) {
        std::cerr << "Database not open.\n";
        return return_reminders;
    }

    const char* sql_all =
        "SELECT id, title, created_at, description "
        "FROM Reminders ORDER BY created_at;";


    if (sqlite3_prepare_v2(m_db, sql_all, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(m_db) << "\n";
        return return_reminders;
    }
   

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int col = 0;
        Reminder r;
        r.id = sqlite3_column_int(stmt, col++);
        r.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col++));
        r.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col++));
        r.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col++));

        return_reminders.push_back(std::move(r));
    }

    sqlite3_finalize(stmt);
    close();

    return return_reminders;
}