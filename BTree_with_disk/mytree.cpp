
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
    for (const auto& [key, value] : this->TableMap) {
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
    this->TableMap[table_name] = new Table(table_name, field_str, pk);
}

void DbSystem::use_table(const string& table_name) {
    if (this->TableMap.find(table_name) == this->TableMap.end()) {
        this->TableMap[table_name] = new Table(table_name);
    }
    this->now_table = this->TableMap[table_name];
}

void DbSystem::create_index(const string& index_name) {
}

void DbSystem::insert_file(const string& file_name) {
    fs::path file_path(file_name.c_str());

    if (!fs::exists(file_path)) {
        throw runtime_error(format("file {} not exist", file_name));
    }

}

Table::Table(const string& table_name, const string& field_str, const string& pk) {
    string table_dirname = format("./{}/{}", ROOT_DIRNAME, table_name);

    if (fs::is_directory(table_dirname)) {
        throw runtime_error(format("table {} exist", table_name));
    }

    this->table_option = new TableOption(table_name);

    const string field_content_pat = "^\\s*\\((.+?)\\)\\s*$";
    regex field_content_regex(field_content_pat);

    const string field_name_type_pat = "^\\s*(\\w+?)\\s+(.+?)\\s*(,|$)";
    regex field_name_type_regex(field_name_type_pat);

    smatch m;

    if (!regex_match(field_str, m, field_content_regex)) {
        throw runtime_error(format("invalid field format for pat {}", field_content_pat));
    }

    const string field_content = m.str(1);

    int start_pos = 0;
    smatch match;

    while (regex_search(field_content.begin() + start_pos, field_content.end(), match, field_name_type_regex)) {
        this->fill_field_info(match.str(1), match.str(2));
        start_pos = match[0].second - field_content.begin();
    }

    if (field_content.size() > start_pos) {
        throw runtime_error(format("Field_content parsing error: {}\n", field_content.substr(start_pos)));
    }

    fs::create_directory(table_dirname);

    this->create_primary_index(pk);
    this->write_table_info();
}

void Table::create_primary_index(const string& pk) {
    this->pk = pk;
    this->IndexMap[pk] = new Btree(this->table_option->table_name, pk, true);

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
        this->field_info.push_back(tuple<string, string, int>(field_name, "int", field_size));
    } else if (startsWith(field_type, "char")) {
        field_size = atoi(match.str(2).c_str());
        this->field_info.push_back(tuple<string, string, int>(field_name, "char", field_size));
    }
}

void Table::write_table_info() {
    // todo:
    // write table option
    const string& table_info_fn = format("./{}/{}/meta_info", ROOT_DIRNAME, this->table_option->table_name);

    fstream f(table_info_fn, ios::out);

    if (f.is_open()) {
        //vector< tuple<string, string, int> >::iterator it;

        //for (it = this->field_info.begin() ; it < this->field_info.end() ; it++) {
            //f << get<0>(*it) << " " << get<1>(*it) << " " << get<2>(*it) << endl;
        //}

        for (auto& item : this->field_info) {
            f << get<0>(it) << " " << get<1>(it) << " " << get<2>(it) << endl;
        }

        f << "@PRIMARY_KEY " << this->pk << endl;

        for (auto& item : this->IndexMap) {
            f << "@INDEX " << item.first << endl;
        }

        f.close();
    }
}

Table::Table(const string& table_name) {
    string table_dirname = format("./{}/{}", ROOT_DIRNAME, table_name);

    if (!fs::is_directory(table_dirname)) {
        throw runtime_error(format("table {} not exist", table_name));
    }

    fs::path table_info_fn(format("./{}/{}/meta_info", ROOT_DIRNAME, table_name).c_str());

    if (!fs::exists(table_info_fn)) {
        throw runtime_error(format("table {} info fn not exist", table_name));
    }

    this->table_option = new TableOption(table_name);

    this->read_table_info();
}

void Table::read_table_info() {
    // todo:
    // read table option
    const string& table_info_fn = format("./{}/{}/meta_info", ROOT_DIRNAME, this->table_option->table_name);
    const string field_name_type_pat = "^\\s*(\\w+?)\\s+(\\w+?)\\s*(\\d+?)\\s*$";
    regex field_name_type_regex(field_name_type_pat);
    const string primary_key_pat = "^\\s*@PRIMARY_KEY\\s+(\\w+?)\\s*$";
    regex primary_key_regex(primary_key_pat);
    const string index_pat = "^\\s*@INDEX\\s+(\\w+?)\\s*$";
    regex index_regex(index_pat);
    smatch m;

    fstream f(table_info_fn, ios::in);

    if (f.is_open()) {
        string line;
        while (getline(f, line)) {
            if (regex_match(line, m, primary_key_regex)) {
                this->pk = m.str(1);
                continue;
            }
            else if (regex_match(line, m, field_name_type_regex)) {
                this->field_info.push_back(tuple<string, string, int>(m.str(1), m.str(2), atoi(m.str(3).c_str())));
                continue;
            }
            else if (regex_match(line, m, index_regex)) {
                string index_name = m.str(1);
                this->IndexMap[index_name] = Btree(this->table_option->table_name, index_name, false);
                continue;
            }
            throw runtime_error(format("Invaild line {} in {}", line, table_info_fn));
        }
        f.close();
    }
}

Table::~Table() {
    for (const auto& [key, value] : this->IndexMap) {
        delete value;
    }
    delete this->table_option;
}

Btree::Btree(const string& index_name, bool init, TableOption* table_option) {
    const string& index_dirname = format("./{}/{}/{}", ROOT_DIRNAME, table_option->table_name, index_name);

    if (init) {
        if (fs::is_directory(index_dirname)) {
            throw runtime_error(format("index {} exist", index_name));
        }
        fs::create_directory(index_dirname);
        this->table_option = table_option;
        this->index_name = index_name;

    } else {
        if (!fs::is_directory(index_dirname)) {
            throw runtime_error(format("index {} not exist", index_name));
        }
        this->table_option = table_option;
        this->index_name = index_name;
    }
}

Btree::~Btree() {

}

