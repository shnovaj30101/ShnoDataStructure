
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

using json = nlohmann::json;
namespace fs = std::filesystem;
using namespace fmt;
using namespace std;

map<string, regex> cmd_regex_map = {
    {"create_table", regex("create\\s+table\\s+(\\w+)")},
    //{"create_table2", regex("create\\s+table\\s+(\\w+)\\s+(\\(.+?\\))")},
    {"use_table", regex("use\\s+(\\w+)")},
    {"insert_file", regex("insert\\s+(\\w+)")},
    {"remove_data", regex("remove\\s+(\\w+)")},
    {"query_data", regex("find\\s+(\\w+)")},
    {"quit", regex("find\\s+(\\w+)")}, ///
};

void parse_command(const string& command) {
    smatch m;
    if (regex_match(command, m, cmd_regex_map["create_table"])) { // 創建一個 table
        create_table(&m);
    //} else if (regex_match(command, m, cmd_regex_map["create_table2"])) { // 進入一個 table
        //create_table(&m);
    } else if (regex_match(command, m, cmd_regex_map["use_table"])) { // 進入一個 table
        use_table(&m);
    } else if (regex_match(command, m, cmd_regex_map["insert_file"])) { // 清除一個 table
        insert_file(&m);
    } else if (regex_match(command, m, cmd_regex_map["remove_data"])) { // 插入一個檔案內部的 json 資料
        remove_data(&m);
    } else if (regex_match(command, m, cmd_regex_map["query_data"])) { // 刪除一筆資料
        query_data(&m);
    } else if (command.length() == 0) {
        print("");
    } else {
        print("invalid cmd\n");
    }
}

int main(int argc, char* argv[]) {
    string command;

    db_system.init();

    cout << "ShnoDatabase$ ";
    while (getline(cin, command)) {
        parse_command(command);
        cout << "ShnoDatabase$ ";
    }

    cout << endl;
}

