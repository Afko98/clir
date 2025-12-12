#include <iostream>
#include "dbController.h"
#include <sys/stat.h>
#include <sys/types.h>
#include  <cstring>
#include <errno.h>
#include <iomanip> 

std::string folder = std::string(getenv("HOME")) + "/sql_db";
std::string db_path = folder + "/clir.db";
Database db(db_path);



struct Command {
    const char *name;
    const char *description;
    bool (*run)(Command *command, int &argc, char **argv);
};


const char* shift(char** &argv, int &argc) {
    if (argc <= 0) return nullptr;
    const char* first = argv[0];
    argv++;   
    argc--;   
    return first;
}

bool help(Command *command, int &argc, char **argv);
bool newReminder(Command *command, int &argc, char **argv);
bool deleteReminder(Command *command, int &argc, char **argv);
bool getReminders(Command *command, int &argc, char **argv) ;

Command commands[]{
    {"--help", "Show help\n", &help},
    {"new", "Create new reminder\nExample 1: clir new title1 \"This is reminder description\"\nExample 2: clir new \"title w space\" \"This is reminder description\"", &newReminder},
    {"-l", "List all active reminders", &getReminders},
    {"rm", "Remove reminder.\nExample: clir rm <reminder ID>", &deleteReminder}
};

bool help(Command *command, int &argc, char **argv) {
    size_t maxLen = 0;
    for (int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) {
        size_t len = std::strlen(commands[i].name);
        if (len > maxLen) maxLen = len;
    }
    const int spacing = 2; 

    for (int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) {
        std::string desc = commands[i].description;
        size_t pos = 0;
        bool firstLine = true;

        while (true) {
            size_t next = desc.find('\n', pos);
            std::string line = desc.substr(pos, next - pos);

            if (firstLine) {
                std::cout << std::left
                          << std::setw(maxLen + spacing)
                          << (std::string(commands[i].name) + ":")
                          << line << "\n";
                firstLine = false;
            } else {
                std::cout << std::setw(maxLen + spacing) << " " << line << "\n";
            }

            if (next == std::string::npos) break;
            pos = next + 1;
        }
        std::cout<<std::endl;
    }
    return true;
}
bool newReminder(Command *command, int &argc, char **argv) {
    if (argc != 2)
    {
        std::cout << "\nNot enough parameters. Use --help to see all options.\n";
        return false;
    }

    const char* title = shift(argv, argc);
    const char* descriotion = shift(argv, argc);

    return db.newRemider(title, descriotion);
}

bool deleteReminder(Command *command, int &argc, char **argv)
{
    if (argc != 1)
    {
        std::cout << "\nWrong parameters. Use --help to see all options.\n";
        return false;
    }

    const char* id_str = shift(argv, argc);
    if (!id_str) {
        std::cerr << "No ID provided\n";
        return false;
    }

    int id = std::stoi(id_str); 

    if (db.removeReminder(id)) 
    {
        std::cout << "Reminder removed successfully\n";
            return true;
    } 
    else 
    {
        std::cerr << "Failed to remove reminder\n";
        return false;
    }
}

bool getReminders(Command *command, int &argc, char **argv) {
    if (argc > 0)
    {
        std::cout << "\nExtra parameters. Use --help to see all options.\n";
        return false;
    }

    std::vector<Reminder> reminders = db.getReminders();

for (const auto& r : reminders) {

    std::cout << r.id << "  "
              << r.title << "  "
              << r.created_at << "\n";

    const std::string& desc = r.description;
    size_t start = 0;
    size_t width = 40;
    while (start < desc.size()) {
        std::string line = desc.substr(start, width);
        std::cout << "   " << line << "\n";
        start += width;
    }

    std::cout << "\n";
}
    return true;
}



int main(int argc, char *argv[]) {
    const char *command = NULL;
    if (argc < 2) 
    {
        std::cout << "\nUse --help to display all options.\n";
        return 1;
    }
    else 
    {
        shift(argv, argc); //discard appname;
        command = shift(argv, argc);
    }
    
    for (int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) 
    {
        if (strcmp(commands[i].name, command) == 0) 
        {
            commands[i].run(&commands[i], argc, argv);
            return 0;
        }
    }

    std::cerr << "Unknown command: " << argv[1] << "\n";
    return 1;
}
