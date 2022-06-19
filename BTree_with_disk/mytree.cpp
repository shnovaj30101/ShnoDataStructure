
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <exception>
#include <stdlib.h>
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

bool startsWith(const std::string& str, const std::string prefix) {
    return (str.rfind(prefix, 0) == 0);
}

bool endsWith(const std::string& str, const std::string suffix) {
    if (suffix.length() > str.length()) { return false; }

    return (str.rfind(suffix) == (str.length() - suffix.length()));
}

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

void DbSystem::create_table(const string& table_name, const string& field_str, const string& pk) {
    TableMap[table_name] = new Table(table_name, field_str, pk);
}

void DbSystem::use_table(const string& table_name) {
    if (TableMap.find(table_name) == TableMap.end()) {
        TableMap[table_name] = new Table(table_name);
    }
    now_table = TableMap[table_name];
}

void DbSystem::create_index(const string& index_name) {
}

void DbSystem::insert_file(const string& file_name) {
    fs::path file_path(file_name.c_str());

    if (!fs::exists(file_path)) {
        throw runtime_error(format("file {} not exist", file_name));
    }

}

Table::Table(const string& _table_name, const string& _field_str, const string& _pk) {
    string table_dirname = format("./{}/{}", ROOT_DIRNAME, _table_name);

    if (fs::is_directory(table_dirname)) {
        throw runtime_error(format("table {} exist", _table_name));
    }

    table_name = _table_name;

    const string field_content_pat = "^\\s*\\((.+?)\\)\\s*$";
    regex field_content_regex(field_content_pat);

    const string field_name_type_pat = "^\\s*(\\w+?)\\s+(.+?)\\s*(,|$)";
    regex field_name_type_regex(field_name_type_pat);

    smatch m;

    if (!regex_match(_field_str, m, field_content_regex)) {
        throw runtime_error(format("invalid field format for pat {}", field_content_pat));
    }

    const string field_content = m.str(1);

    int start_pos = 0;
    smatch match;

    while (regex_search(field_content.begin() + start_pos, field_content.end(), match, field_name_type_regex)) {
        fill_field_info(match.str(1), match.str(2));
        start_pos = match[0].second - field_content.begin();
    }

    if (field_content.size() > start_pos) {
        throw runtime_error(format("Field_content parsing error: {}\n", field_content.substr(start_pos)));
    }

    fs::create_directory(table_dirname);

    pk = _pk;

    create_primary_index();
    write_table_info();
}

void Table::create_primary_index() {

}

void Table::fill_field_info(const string& field_name, const string& raw_field_type) {
    const string field_name_pat = "^\\w+$";
    regex field_name_regex(field_name_pat);

    const string field_type_pat = "^(int|char\\((\\d+?)\\))$";
    regex field_type_regex(field_type_pat);
    string field_type;
    int field_size;

    smatch match;

    if (!regex_match(field_name, match, field_name_regex)) {
        throw runtime_error(format("Field_name parsing error: {}\n", field_name));
    }

    if (!regex_match(raw_field_type, match, field_type_regex)) {
        throw runtime_error(format("Raw_field_type parsing error: {}\n", raw_field_type));
    }

    field_type = match.str(1);

    if (field_type == "int") {
        field_size = 4;
        field_info.push_back(tuple<string, string, int>(field_name, "int", field_size));
    } else if (startsWith(field_type, "char")) {
        field_size = atoi(match.str(2).c_str());
        field_info.push_back(tuple<string, string, int>(field_name, "char", field_size));
    }
}

void Table::write_table_info() {
    const string& table_info_fn = format("./{}/{}/meta_info", ROOT_DIRNAME, table_name);

    fstream f(table_info_fn, ios::out);

    if (f.is_open()) {
        vector< tuple<string, string, int> >::iterator it;

        for (it = field_info.begin() ; it < field_info.end() ; it++) {
            f << get<0>(*it) << " " << get<1>(*it) << " " << get<2>(*it) << endl;
        }

        f << "@PRIMARY_KEY " << pk << endl;

        f.close();
    }
}

Table::Table(const string& _table_name) {
    string table_dirname = format("./{}/{}", ROOT_DIRNAME, _table_name);

    if (!fs::is_directory(table_dirname)) {
        throw runtime_error(format("table {} not exist", _table_name));
    }

    fs::path table_info_fn(format("./{}/{}/meta_info", ROOT_DIRNAME, _table_name).c_str());

    if (!fs::exists(table_info_fn)) {
        throw runtime_error(format("table {} info fn not exist", _table_name));
    }

    table_name = _table_name;

    read_table_info();
}

void Table::read_table_info() {
    const string& table_info_fn = format("./{}/{}/meta_info", ROOT_DIRNAME, table_name);
    const string field_name_type_pat = "^\\s*(\\w+?)\\s+(\\w+?)\\s*(\\d+?)\\s*$";
    regex field_name_type_regex(field_name_type_pat);
    const string primary_key_pat = "^\\s*@PRIMARY_KEY\\s+(\\w+?)\\s*$";
    regex primary_key_regex(primary_key_pat);
    smatch m;

    fstream f(table_info_fn, ios::in);

    if (f.is_open()) {
        string line;
        while (getline(f, line)) {
            if (regex_match(line, m, primary_key_regex)) {
                pk = m.str(1);
                continue;
            }
            else if (!regex_match(line, m, field_name_type_regex)) {
                throw runtime_error(format("Invaild line {} in {}", line, table_info_fn));
            }
            field_info.push_back(tuple<string, string, int>(m.str(1), m.str(2), atoi(m.str(3).c_str())));
        }
        f.close();
    }
}

Table::~Table() {
    for (const auto& [key, value] : IndexMap) {
        delete value;
    }
}



