
#include <iostream>
#include <filesystem>
#include <regex>
#include <exception>
#include "basic.hpp"
#include "mytree.hpp"
#include "command.hpp"
#include "mytree.hpp"
#include "nlohmann/json.hpp"
#include "fmt/format.h"

using namespace std;
using namespace fmt;
using json = nlohmann::json;
namespace fs = std::filesystem;

DbSystem* db_system_ptr = new DbSystem();

DbSystem::DbSystem() {}

DbSystem::~DbSystem() {
    for (const auto& [key, value] : TableMap) {
        delete value;
    }
}

void DbSystem::init() {
    string root_dirname = format("./{}", ROOT_DIRNAME);

    if (!fs::is_directory(root_dirname)) {
        fs::create_directory(root_dirname);
    }
}

void DbSystem::create_table(const string& table_name, const string& field_str) {
    TableMap[table_name] = new Table(table_name, field_str);
}

void DbSystem::use_table(const string& table_name) {

}

Table::Table(const string& _table_name, const string& _field_str) {
    string table_dirname = format("./{}/{}", ROOT_DIRNAME, _table_name);

    if (fs::is_directory(table_dirname)) {
        throw runtime_error(format("table {} exist", _table_name));
    } else {
        fs::create_directory(table_dirname);
    }

    table_name = _table_name;

    string field_content_pat = "^\\s*\\((.+?)\\)\\s*$";
    regex field_content_regex(field_content_pat);

    string field_name_type_pat = "^\\s*(\\w+)\\s+(\\w+)\\s*(,|$)";
    regex field_name_type_regex(field_name_type_pat);

    smatch m;

    if (!regex_match(_field_str, m, field_content_regex)) {
        throw runtime_error(format("invalid field format for pat {}", field_content_pat));
    }

    string field_content = m.str(1);

}


Table::Table(const string& table_name) {

}

Table::~Table() {
    for (const auto& [key, value] : IndexMap) {
        delete value;
    }
}



