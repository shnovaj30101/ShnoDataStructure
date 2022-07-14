#include "myrandom.hpp"
#include "mytree.hpp"
#include "command.hpp"
#include "basic.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <regex>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <map>
#include "nlohmann/json.hpp"
#include "fmt/format.h"

using namespace fmt;
using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

class CmdQuit:public exception {
    public:
        const char * what() const throw() {
            return "Command line quit";
        }
};

map<string, regex> cmd_regex_map = {
    {"create_table_default", regex("^\\s*create\\s+table\\s+(\\w+?)\\s*$")},
    //{"create_table", regex("^\\s*create\\s+table\\s+(\\w+?)\\s+(\\(.+?\\))\\s*$")},
    {"use_table", regex("^\\s*use\\s+(\\w+?)\\s*$")},
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

int main(int argc, char* argv[]) {
    string command;

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

// test cmd
//
// create table test
// use table test
// insert file test.json
// create index example_id
// create index example_str
// quit
//
// create table test2
// use table test2
// create index example_id
// create index example_str
// insert file test.json
// quit


