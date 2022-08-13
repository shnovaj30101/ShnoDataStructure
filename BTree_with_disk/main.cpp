#include "myrandom.hpp"
#include "mytree.hpp"
#include "command.hpp"
#include "basic.hpp"
#include <string.h>
#include <iostream>
#include "nlohmann/json.hpp"
#include "fmt/format.h"

using namespace fmt;
using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    string command;

    db_system_ptr = new DbSystem();
    db_system_ptr->init();

    cout << "ShnoDatabase$ ";
    while (getline(cin, command)) {
        try {
            parse_command(command);
        } catch (CmdQuit &e) {
            break;
        }
        cout << "ShnoDatabase$ ";
    }

    delete db_system_ptr;
}



