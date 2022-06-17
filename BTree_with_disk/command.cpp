
#include<iostream>
#include <filesystem>
#include <regex>
#include "basic.hpp"
#include "mytree.hpp"
#include "command.hpp"
#include "nlohmann/json.hpp"
#include "fmt/format.h"

namespace fs = std::filesystem;
using namespace std;
using namespace fmt;
using json = nlohmann::json;

void create_table(const smatch& m) {
    string field_str = DEFAULT_FIELD;

    string table_name = m.str(1);

    try {
        db_system_ptr->create_table(table_name, field_str);
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

void create_table2(const smatch& m) {

}

void use_table(const smatch& m) {
    string table_name = m.str(1);

    try {
        db_system_ptr->use_table(table_name);
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

void insert_file(const smatch& m) {
}

void remove_data(const smatch& m) {
}

void query_data(const smatch& m) {
}

