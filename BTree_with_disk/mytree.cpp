
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

namespace fs = std::filesystem;
using namespace std;
using namespace fmt;
using json = nlohmann::json;

DbSystem db_system = DbSystem();

DbSystem::DbSystem() {}

DbSystem::~DbSystem() {
    for (const auto& [key, value] : TableMap) {
        print(key);
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
    try {
        TableMap[table_name] = new Table(table_name, field_str);
    } catch (exception &e) {
    }

}

void DbSystem::use_table(const string& table_name) {

}

Table::Table(const string& table_name, const string& field_str) {
    string table_dirname = format("./{}/{}", ROOT_DIRNAME, table_name);

    if (fs::is_directory(table_dirname)) {
        print("table {} exist.\n", table_name);
        return;
    } else {
        fs::create_directory(table_dirname);
    }
}


Table::Table(const string& table_name) {

}

Table::~Table() {}



