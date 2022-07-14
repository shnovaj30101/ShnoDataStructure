
#include <iostream>
#include <filesystem>
#include<memory>
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

    ifstream data_file(file_name);

    if (!data_file.is_open()) {
        throw runtime_error(format("file {} open fail", file_name));
    }

    string line;
    while (getline(data_file, line)) {
        cout << line << endl;
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

    this->fill_field_info(DEFAULT_PK, "int");

    while (regex_search(field_content.begin() + start_pos, field_content.end(), match, field_name_type_regex)) {
        this->fill_field_info(match.str(1), match.str(2));
        start_pos = match[0].second - field_content.begin();
    }

    if (field_content.size() > start_pos) {
        throw runtime_error(format("Field_content parsing error: {}\n", field_content.substr(start_pos)));
    }

    fs::create_directory(table_dirname);

    const string& data_fn = format("./{}/{}/data_file", ROOT_DIRNAME, this->table_option->table_name);

    this->data_page_mgr = make_shared<DataPageMgr>(data_fn);

    this->create_primary_index(pk);
    this->write_table_info();
}

void Table::insert_data(const json &json_data) {

}

void Table::create_primary_index(const string& pk) {
    this->pk = pk;
    int degree = (DEFAULT_PAGE_SIZE - 100) / (sizeof(int) + sizeof(long)) / 2 ;
    this->IndexMap[pk] = new Btree(pk, degree, 0, this->data_page_mgr, this->table_option);
}

void Table::create_index(const string& index_name) {
    for (auto& item : this->table_option->field_info) {
        if (index_name == get<0>(item)) {
            int key_field_len = get<2>(item);
            int degree = (DEFAULT_PAGE_SIZE - 100) / (sizeof(int) + key_field_len + sizeof(long)) / 2;
            this->IndexMap[pk] = new Btree(index_name, degree, key_field_len, this->data_page_mgr, this->table_option);
            break;
        }
    }
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
        this->table_option->field_info.push_back(tuple<string, string, int>(field_name, "int", field_size));
    } else if (startsWith(field_type, "char")) {
        field_size = atoi(match.str(2).c_str());
        this->table_option->field_info.push_back(tuple<string, string, int>(field_name, "char", field_size));
    }
}

void Table::write_table_info() {
    // todo:
    // write table option
    const string& table_info_fn = format("./{}/{}/meta_info", ROOT_DIRNAME, this->table_option->table_name);

    fstream f(table_info_fn, ios::out);

    if (f.is_open()) {

        for (auto& item : this->table_option->field_info) {
            f << get<0>(item) << " " << get<1>(item) << " " << get<2>(item) << endl;
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

    fs::path data_fn(format("./{}/{}/data_file", ROOT_DIRNAME, table_name).c_str());

    if (!fs::exists(data_fn)) {
        throw runtime_error(format("table {} data fn not exist", table_name));
    }

    this->table_option = new TableOption(table_name);

    this->data_page_mgr = make_shared<DataPageMgr>(data_fn.string());

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
                int degree = (DEFAULT_PAGE_SIZE - 100) / (sizeof(int) + sizeof(long)) / 2 ;
                this->IndexMap[this->pk] = new Btree(this->pk, degree, 0, this->data_page_mgr, this->table_option);
                continue;
            }
            else if (regex_match(line, m, field_name_type_regex)) {
                this->table_option->field_info.push_back(tuple<string, string, int>(m.str(1), m.str(2), atoi(m.str(3).c_str())));
                continue;
            }
            else if (regex_match(line, m, index_regex)) {
                string index_name = m.str(1);

                for (auto& item : this->table_option->field_info) {
                    if (index_name == get<0>(item)) {
                        int key_field_len = get<2>(item);
                        int degree = (DEFAULT_PAGE_SIZE - 100) / (sizeof(int) + key_field_len + sizeof(long)) / 2;
                        this->IndexMap[index_name] = new Btree(index_name, degree, key_field_len, this->data_page_mgr, this->table_option);
                        break;
                    }
                }

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

Btree::Btree(const string& index_name, int degree, int key_field_len, shared_ptr <DataPageMgr> data_page_mgr, TableOption* table_option) {
    const string& index_dirname = format("./{}/{}/{}_index", ROOT_DIRNAME, table_option->table_name, index_name);
    const string& btree_fn = format("./{}/{}/{}_index/btree_file", ROOT_DIRNAME, table_option->table_name, index_name);

    if (fs::is_directory(index_dirname)) {
        throw runtime_error(format("index {} exist", index_name));
    }
    fs::create_directory(index_dirname);
    this->index_name = index_name;
    if (key_field_len == 0) {
        this->header.is_pk = true;
    }
    this->header.degree = degree;
    this->header.key_field_len = key_field_len;
    this->data_page_mgr = data_page_mgr;
    this->table_option = table_option;

    this->btree_page_mgr = make_shared<BtreePageMgr>(btree_fn);

    this->root = new BtreeNode(header.root_id, degree, key_field_len, true, true);
    this->header.count++;

    this->btree_page_mgr->save_header(this->header);
    this->btree_page_mgr->save_node(header.root_id, this->root);
}

Btree::Btree(const string& index_name, shared_ptr <DataPageMgr> data_page_mgr, TableOption* table_option) {
    const string& index_dirname = format("./{}/{}/{}_index", ROOT_DIRNAME, table_option->table_name, index_name);
    const string& btree_fn = format("./{}/{}/{}_index/btree_file", ROOT_DIRNAME, table_option->table_name, index_name);

    if (!fs::is_directory(index_dirname)) {
        throw runtime_error(format("index {} not exist", index_name));
    }
    this->index_name = index_name;
    this->data_page_mgr = data_page_mgr;
    this->table_option = table_option;
    this->btree_page_mgr = make_shared<BtreePageMgr>(btree_fn);

    this->btree_page_mgr->get_header(this->header);
    this->btree_page_mgr->get_node(this->header.root_id, this->root);
}

Btree::~Btree() {
    for (const auto& [key, value] : this->NodeMap) {
        delete value;
    }
    if (this->root) {
        delete this->root;
    }
}

BtreeNode::BtreeNode(long id, int degree, int key_field_len, bool is_root, bool is_leaf) {
    this->header.traversal_id = id;
    this->header.degree = degree;
    this->header.key_field_len = key_field_len;
    this->header.is_root = is_root;
    this->header.is_leaf = is_leaf;
}

BtreeNode::~BtreeNode() {
    for (const auto& key : this->keys) {
        delete key.data;
    }
}

// ======================================================

BtreePageMgr::BtreePageMgr(const string& filename, bool trunc) : fstream(filename.data(), ios::in | ios::out | ios::binary) {
    this->empty = false;
    this->filename = filename;
    if (!this->good() || trunc) {
        this->empty = true;
        this->open(filename.data(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    }
}

BtreePageMgr::~BtreePageMgr() {
    this->close();
}

template <class header_data>
void BtreePageMgr::save_header(header_data &header) {
    this->clear();
    this->seekp(0, ios::beg);
    this->write(reinterpret_cast<char *>(&header), sizeof(header));
    this->header_prefix = sizeof(header_data);
}

template <class header_data>
bool BtreePageMgr::get_header(header_data &header) {
    this->clear();
    this->seekg(0, ios::beg);
    this->read(reinterpret_cast<char *>(&header), sizeof(header));
    return this->gcount() > 0;
}

template <class btree_node>
void BtreePageMgr::save_node(const long &n, btree_node &node) {
    this->clear();
    this->seekp(this->header_prefix + n * sizeof(btree_node), ios::beg);
    this->write(reinterpret_cast<char *>(&node), sizeof(node));
}

template <class btree_node>
bool BtreePageMgr::get_node(const long &n, btree_node &node) {
    this->clear();
    this->seekg(this->header_prefix + n * sizeof(btree_node), ios::beg);
    this->read(reinterpret_cast<char *>(&node), sizeof(node));
    return this->gcount() > 0;
}

// ======================================================

DataPageMgr::DataPageMgr(const string& filename, bool trunc) : fstream(filename.data(), ios::in | ios::out | ios::binary) {
    this->empty = false;
    this->filename = filename;
    if (!this->good() || trunc) {
        this->empty = true;
        this->open(filename.data(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    }
}

DataPageMgr::~DataPageMgr() {
    this->close();
}

template <class header_data>
void DataPageMgr::save_header(header_data &header) {
    this->clear();
    this->seekp(0, ios::beg);
    this->write(reinterpret_cast<char *>(&header), sizeof(header));
    this->header_prefix = sizeof(header_data);
}

template <class header_data>
bool DataPageMgr::get_header(header_data &header) {
    this->clear();
    this->seekg(0, ios::beg);
    this->read(reinterpret_cast<char *>(&header), sizeof(header));
    return this->gcount() > 0;
}

template <class btree_node>
void DataPageMgr::save_node(const long &n, btree_node &node) {
    this->clear();
    this->seekp(this->header_prefix + n * sizeof(btree_node), ios::beg);
    this->write(reinterpret_cast<char *>(&node.header), sizeof(node.header));

    int i;
    for (i = 0 ; i < node.header.key_count ; i++) {
        if (!node.header.is_leaf) {
            this->write(reinterpret_cast<char *>(&node.children[i]), sizeof(long));
        } else {
            this->seekp(sizeof(long), ios::cur);
        }
        this->write(reinterpret_cast<char *>(&node.keys[i].data), node.header.key_field_len * sizeof(char));
        this->write(reinterpret_cast<char *>(&node.keys[i]._id), sizeof(int));
    }

    if (!node.header.is_leaf) {
        this->write(reinterpret_cast<char *>(&node.children[i]), sizeof(long));
    }
}

template <class btree_node>
bool DataPageMgr::get_node(const long &n, btree_node &node) {
    this->clear();
    this->seekg(this->header_prefix + n * sizeof(btree_node), ios::beg);
    this->read(reinterpret_cast<char *>(&node.header), sizeof(node.header));

    int i;
    long children_tmp;
    for (i = 0 ; i < node.header.key_count ; i++) {
        if (!node.header.is_leaf) {
            this->read(reinterpret_cast<char *>(&children_tmp), sizeof(long));
            node.children.push_back(children_tmp);
        } else {
            this->seekg(sizeof(long), ios::cur);
        }
        struct BtreeKey key;
        key.data = (char*)calloc(node.header.key_field_len, sizeof(char));
        this->read(reinterpret_cast<char *>(&key.data), node.header.key_field_len * sizeof(char));
        this->read(reinterpret_cast<char *>(&key._id), sizeof(int));

        node.keys.push_back(key);
    }

    if (!node.header.is_leaf) {
        this->read(reinterpret_cast<char *>(&children_tmp), sizeof(long));
        node.children.push_back(children_tmp);
    }

    return this->gcount() > 0;
}



