
#include<iostream>
#include <filesystem>
#include <regex>
#include "basic.hpp"
#include "mytree.hpp"
#include <map>
#include "command.hpp"
#include "nlohmann/json.hpp"
#include "fmt/format.h"

namespace fs = std::filesystem;
using namespace std;
using namespace fmt;
using json = nlohmann::json;

map<string, regex> cmd_regex_map = {
    {"create_table_default", regex("^\\s*create\\s+table\\s+(\\w+?)\\s*$")},
    //{"create_table", regex("^\\s*create\\s+table\\s+(\\w+?)\\s+(\\(.+?\\))\\s*$")},
    {"use_table", regex("^\\s*use\\s+(\\w+?)\\s*$")},
    {"clearb_table", regex("^\\s*clearb\\s+table\\s+(\\w+?)\\s*$")},
    {"delete_table", regex("^\\s*delete\\s+table\\s+(\\w+?)\\s*$")},
    {"create_index", regex("^\\s*create\\s+index\\s+(\\w+?)\\s*$")},
    {"insert_file", regex("^\\s*insert\\s+(\\S+?)\\s*$")},
    {"remove_data", regex("^\\s*remove\\s+(.+?)\\s*$")},
    {"query_data", regex("^\\s*find\\s+(.+?)\\s*$")},
    {"quit", regex("^\\s*quit\\s*$")},
};

void parse_command(const string& command) {
    smatch m;
    if (regex_match(command, m, cmd_regex_map["create_table_default"])) { // 創建一個 table
        create_table_default(m);
    //} else if (regex_match(command, m, cmd_regex_map["create_table"])) { // 創建一個 table 2
        //create_table(m);
    } else if (regex_match(command, m, cmd_regex_map["use_table"])) { // 進入一個 table
        use_table(m);
    } else if (regex_match(command, m, cmd_regex_map["clearb_table"])) { // 進入一個 table
        clearb_table(m);
    } else if (regex_match(command, m, cmd_regex_map["delete_table"])) { // 進入一個 table
        delete_table(m);
    } else if (regex_match(command, m, cmd_regex_map["create_index"])) { // 創建一個 index
        create_index(m);
    } else if (regex_match(command, m, cmd_regex_map["insert_file"])) { // 清除一個 table
        insert_file(m);
    } else if (regex_match(command, m, cmd_regex_map["remove_data"])) { // 插入一個檔案內部的 json 資料
        remove_data(m);
    } else if (regex_match(command, m, cmd_regex_map["query_data"])) { // 刪除一筆資料
        query_data(m);
    } else if (regex_match(command, m, cmd_regex_map["quit"])) {
        throw CmdQuit();
    } else if (command.length() == 0) {
        print("");
    } else {
        print("invalid cmd\n");
    }
}

void create_table_default(const smatch& m) {
    string field_str = DEFAULT_FIELD;

    string table_name = m.str(1);

    try {
        db_system_ptr->create_table(table_name, field_str);
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

void create_table(const smatch& m) {

}

void use_table(const smatch& m) {
    string table_name = m.str(1);

    try {
        db_system_ptr->use_table(table_name);
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

void clearb_table(const smatch& m) {
    string table_name = m.str(1);

    try {
        db_system_ptr->clearb_table(table_name);
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

void delete_table(const smatch& m) {
    string table_name = m.str(1);

    try {
        db_system_ptr->delete_table(table_name);
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

void create_index(const smatch& m) {
    string index_name = m.str(1);

    try {
        db_system_ptr->create_index(index_name);
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

void insert_file(const smatch& m) {
    string file_name = m.str(1);

    try {
        db_system_ptr->insert_file(file_name);
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

void remove_data(const smatch& m) {
}

void query_data(const smatch& m) {
}

