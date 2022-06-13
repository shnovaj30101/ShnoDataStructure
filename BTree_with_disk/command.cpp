
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

void create_table(smatch *mp) {
    string field_str = DEFAULT_FIELD;

    smatch m = *mp;
    string table_name = m.str(1);

    db_system.create_table(table_name, field_str);
}

void create_table2(smatch *mp) {

}

void use_table(smatch *mp) {
}

void insert_file(smatch *mp) {
}

void remove_data(smatch *mp) {
}

void query_data(smatch *mp) {
}

