
#include <iostream>
#include <vector>
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

DbSystem* db_system_ptr = NULL;

DbSystem::DbSystem() {}

DbSystem::~DbSystem() {
    map<string, Table*>::iterator it;
    for (it = this->TableMap.begin() ; it != this->TableMap.end() ; ) {
        delete it->second;
        this->TableMap.erase(it++);
    }
}

void DbSystem::init() {
    string root_dirname = format("./{}", ROOT_DIRNAME);

    if (!fs::is_directory(root_dirname)) {
        fs::create_directory(root_dirname);
    }
}

json DbSystem::get_btree_node_info(const string& table_name, const string& index_name, const long& n) {
    Table* table;
    Btree* btree;
    BtreeNode btree_node = BtreeNode();

    if (!this->table_exist(table_name)) {
        throw runtime_error(format("table {} not exist", table_name));
    }
    if (this->TableMap.find(table_name) != this->TableMap.end()) {
        table = this->TableMap[table_name];
    } else {
        table = new Table(table_name);
    }

    if (table->IndexMap.find(index_name) != table->IndexMap.end()) {
        btree = table->IndexMap[index_name];
    } else {
        throw runtime_error(format("index {} not exist in table {}", index_name, table_name));
    }

    if (btree->header.count <= n) {
        throw runtime_error(format("node {} not exist in inde {} table {}", n, index_name, table_name));
    }

    btree->btree_page_mgr->get_node(n, btree_node, btree->table_option);

    json output_json;

    output_json["traversal_id"] = btree_node.header.traversal_id;
    output_json["is_root"] = btree_node.header.is_root;
    output_json["is_leaf"] = btree_node.header.is_leaf;
    output_json["right"] = btree_node.header.right;
    output_json["key_count"] = btree_node.header.key_count;
    output_json["keys"] = json::array();
    output_json["children"] = json::array();

    for (int i = 0 ; i < btree_node.keys.size() ; ++i) {
        struct BtreeKey key = btree_node.keys[i];
        string output_data;

        if (key.data == NULL) {
            output_data = "";
        } else {
            output_data = string(key.data);
        }
        output_json["keys"].push_back({key._id, key.data});
    }

    for (int i = 0 ; i < btree_node.children.size() ; ++i) {
        output_json["children"].push_back(btree_node.children[i]);
    }

    delete btree;
    delete table;

    return output_json;
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

void DbSystem::clearb_table(const string& table_name) {
    if (!this->table_exist(table_name)) {
        throw runtime_error(format("table {} not exist", table_name));
    }
    if (this->TableMap.find(table_name) != this->TableMap.end()) {
        delete this->TableMap[table_name];
        this->TableMap.erase(table_name);
    }
}

void DbSystem::delete_table(const string& table_name) {
    if (!this->table_exist(table_name)) {
        throw runtime_error(format("table {} not exist", table_name));
    }
    this->clearb_table(table_name);

    string table_dirname = format("./{}/{}", ROOT_DIRNAME, table_name);
    fs::remove_all(table_dirname); /// todo dangerous
}

bool DbSystem::table_exist(const string& table_name) {
    string table_dirname = format("./{}/{}", ROOT_DIRNAME, table_name);
    return fs::is_directory(table_dirname);
}

void DbSystem::create_index(const string& index_name) {
    if (this->now_table == NULL) {
        throw runtime_error("now_table not set");
    } else {
        this->now_table->create_index(index_name);
    }
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
        try {
            json line_json = json::parse(line);
            this->now_table->insert_data(line_json);
        } catch (exception &e) {
            cout << e.what() << endl;
        }
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

void Table::insert_data(json &json_data) { /// todo 目前都預設 pk 一定是 _id 且是 int

    insert_data_validation(json_data);

    if (this->pk == DEFAULT_PK) {
        json_data[this->pk] = this->data_header.count;
    }

    long pk = json_data[this->pk];

    ++this->data_header.count;
    this->data_page_mgr->save_header(this->data_header);
    this->data_page_mgr->save_node(this->data_header.count-1, json_data, this->table_option->field_info);

    for (auto &elem : json_data.items()) { /// todo 萬一出現 exception, data_header 要有 rollback 機制
        string json_key = elem.key();

        if (json_key == this->pk) {
            struct BtreeKey btree_key{ pk , NULL };
            this->IndexMap[this->pk]->insert_key(btree_key, this->data_header.count-1);
        }
        else if (this->IndexMap.find(json_key) != this->IndexMap.end()) {
            struct BtreeKey btree_key{ pk , NULL };

            int data_size;
            string data_type;
            for (auto& item : this->table_option->field_info) {
                string field_name = get<0>(item);
                string field_type = get<1>(item);
                int field_size = get<2>(item);
                if (json_key == field_name) {
                    data_size = field_size;
                    data_type = field_type;
                    break;
                }
            }

            btree_key.data = new char[data_size]();
            if (data_type == "int") {
                int json_raw_value = elem.value().get<int>();
                strncpy(btree_key.data, reinterpret_cast<char *>(&json_raw_value), data_size);
            } else if (data_type == "char") {
                string json_raw_value = elem.value().get<string>();
                strncpy(btree_key.data, json_raw_value.c_str(), data_size);
            }

            this->IndexMap[json_key]->insert_key(btree_key, this->data_header.count-1);
        }
    }
}

void Table::insert_data_validation(const json &json_data) {
}

void Table::create_primary_index(const string& pk) {
    this->pk = pk;
    int degree = (DEFAULT_PAGE_SIZE - 100) / (sizeof(int) + sizeof(long)) / 2 ;
    this->IndexMap[pk] = new Btree(pk, degree, FieldType::int_type, 0, this->data_page_mgr, this->table_option);
}

void Table::create_index(const string& index_name) {

    for (auto& item : this->IndexMap) {
        if (index_name == item.first) {
            throw runtime_error(format("index {} is already existed\n", index_name));
        }
    }

    bool create_new_index = false;
    int key_data_size;
    string key_data_type;
    for (auto& item : this->table_option->field_info) {
        string field_name = std::get<0>(item);
        string field_type = std::get<1>(item);
        int field_size = std::get<2>(item);

        if (index_name == field_name) {
            int key_field_len = field_size;
            key_data_size = field_size;
            key_data_type = field_type;
            FieldType key_field_type;

            if (field_type == "int") {
                key_field_type = FieldType::int_type;
            } else if (field_type == "char") {
                key_field_type = FieldType::char_type;
            }

            int degree = (DEFAULT_PAGE_SIZE - 100) / (sizeof(int) + key_field_len + sizeof(long)) / 2;
            this->IndexMap[index_name] = new Btree(index_name, degree, key_field_type, key_field_len, this->data_page_mgr, this->table_option);
            create_new_index = true;
            break;
        }
    }

    if (!create_new_index) {
        throw runtime_error(format("index_name {} not match any field\n", index_name));
    }

    for (long i = 0 ; i < this->data_header.count ; ++i) {
        json json_data;
        this->data_page_mgr->get_node(i, json_data, this->table_option->field_info);

        if (!json_data["exist"]) {
            continue;
        }

        json_data.erase("exist");

        struct BtreeKey btree_key{ i , NULL };

        btree_key.data = new char[key_data_size]();
        if (key_data_type == "int") {
            int json_raw_value = json_data[index_name].get<int>();
            strncpy(btree_key.data, reinterpret_cast<char *>(&json_raw_value), key_data_size);
        } else if (key_data_type == "char") {
            string json_raw_value = json_data[index_name].get<string>();
            strncpy(btree_key.data, json_raw_value.c_str(), key_data_size);
        }
        this->IndexMap[index_name]->insert_key(btree_key, i);
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
        this->table_option->field_info.push_back(FieldTypeInfo(field_name, "int", field_size));
    } else if (startsWith(field_type, "char")) {
        field_size = atoi(match.str(2).c_str());
        this->table_option->field_info.push_back(FieldTypeInfo(field_name, "char", field_size));
    }
}

void Table::write_table_info() {
    // todo:
    // write table option
    const string& table_info_fn = format("./{}/{}/meta_info", ROOT_DIRNAME, this->table_option->table_name);

    fstream f(table_info_fn, ios::out);

    if (f.is_open()) {

        for (auto& item : this->table_option->field_info) {
            f << std::get<0>(item) << " " << std::get<1>(item) << " " << std::get<2>(item) << endl;
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
    this->data_page_mgr->get_header(this->data_header);

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
                this->IndexMap[this->pk] = new Btree(this->pk, this->data_page_mgr, this->table_option);
                //this->IndexMap[this->pk] = new Btree(this->pk, degree, FieldType::int_type, 0, this->data_page_mgr, this->table_option);
                continue;
            }
            else if (regex_match(line, m, field_name_type_regex)) {
                this->table_option->field_info.push_back(FieldTypeInfo(m.str(1), m.str(2), atoi(m.str(3).c_str())));
                continue;
            }
            else if (regex_match(line, m, index_regex)) {
                string index_name = m.str(1);

                for (auto& item : this->table_option->field_info) {
                    if (index_name == std::get<0>(item)) {
                        int key_field_len = std::get<2>(item);
                        int degree = (DEFAULT_PAGE_SIZE - 100) / (sizeof(int) + key_field_len + sizeof(long)) / 2;
                        FieldType key_field_type;

                        if (std::get<1>(item) == "int") {
                            key_field_type = FieldType::int_type;
                        } else if (std::get<1>(item) == "char") {
                            key_field_type = FieldType::char_type;
                        }

                        this->IndexMap[index_name] = new Btree(index_name, this->data_page_mgr, this->table_option);
                        //this->IndexMap[index_name] = new Btree(index_name, degree, key_field_type, key_field_len, this->data_page_mgr, this->table_option);
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
    map<string, Btree*>::iterator it;
    for (it = this->IndexMap.begin() ; it != this->IndexMap.end() ; ) {
        delete it->second;
        this->IndexMap.erase(it++);
    }
    delete this->table_option;
}

Btree::Btree(const string& index_name, int degree, FieldType key_field_type, int key_field_len, shared_ptr <DataPageMgr> data_page_mgr, TableOption* table_option) {
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
    this->header.key_field_type = key_field_type;
    this->data_page_mgr = data_page_mgr;
    this->table_option = table_option;

    this->btree_page_mgr = make_shared<BtreePageMgr>(btree_fn);

    this->root = new BtreeNode(header.root_id, degree, key_field_type, key_field_len, true, true);
    this->NodeMap[header.root_id] = this->root;
    this->node_buffer.push(this->root);
    ++this->header.count;

    this->btree_page_mgr->save_header(this->header);
    this->btree_page_mgr->save_node(header.root_id, *this->root, this->table_option);
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
    this->btree_page_mgr->get_node(this->header.root_id, *(this->root), table_option);
}

Btree::~Btree() {
    map<long, BtreeNode*>::iterator it;
    for (it = this->NodeMap.begin() ; it != this->NodeMap.end() ; ) {
        delete it->second;
        this->NodeMap.erase(it++);
    }
}

void Btree::insert_key(struct BtreeKey &key, long data_page_pos) {
    BtreeNode *now_node = this->root;
    vector<pair<long, int>> traversal_node_record;

    while (true) {
        if (now_node->header.is_leaf) {
            bool is_inserted = false;
            vector<struct BtreeKey>::iterator kit;
            vector<long>::iterator cit;
            for (kit = now_node->keys.begin(), cit = now_node->children.begin(); kit != now_node->keys.end() ; ++kit) {
                if (this->key_compare(key, *kit) < 0) {
                    now_node->keys.insert(kit, key);
                    now_node->children.insert(cit, data_page_pos);
                    ++now_node->header.key_count;
                    this->btree_page_mgr->save_node(now_node->header.traversal_id, *now_node, this->table_option);
                    is_inserted = true;
                    break;
                }
            }

            if (!is_inserted) {
                now_node->keys.push_back(key);
                now_node->children.push_back(data_page_pos);
                ++now_node->header.key_count;
                this->btree_page_mgr->save_node(now_node->header.traversal_id, *now_node, this->table_option);
            }

            if (now_node->is_full()) {
                this->split_child(now_node, traversal_node_record);
            }

            break;

        } else {
            vector<struct BtreeKey>::iterator kit;
            vector<long>::iterator cit;

            long insert_child_id = -1;
            int child_idx=0;
            for (kit = now_node->keys.begin(), cit = now_node->children.begin() ;
                    kit != now_node->keys.end() ; ++kit, ++cit, ++child_idx) {
                if (this->key_compare(key, *kit) < 0) {
                    insert_child_id = *cit;
                    break;
                }
            }
            traversal_node_record.push_back(make_pair(now_node->header.traversal_id, child_idx));

            if (insert_child_id > -1) {
                if (this->NodeMap.find(insert_child_id) == this->NodeMap.end()) {
                    now_node = new BtreeNode();
                    this->btree_page_mgr->get_node(insert_child_id, *now_node, this->table_option);
                    this->NodeMap[insert_child_id] = now_node;
                    this->node_buffer.push(now_node);
                } else {
                    now_node = this->NodeMap[insert_child_id];
                }
                continue;
            } else {
                --cit;
                --kit;
                kit->_id = key._id;
                strncpy(kit->data, key.data, this->header.key_field_len);
                insert_child_id = *cit;
                if (this->NodeMap.find(insert_child_id) == this->NodeMap.end()) {
                    now_node = new BtreeNode();
                    this->btree_page_mgr->get_node(insert_child_id, *now_node, this->table_option);
                    this->NodeMap[insert_child_id] = now_node;
                    this->node_buffer.push(now_node);
                } else {
                    now_node = this->NodeMap[insert_child_id];
                }
                continue;
            }

        }
    }
}

int Btree::key_compare(struct BtreeKey &key1, struct BtreeKey &key2) {
    if (key1.data == NULL) {

        if (key1._id > key2._id) {
            return 1;
        } else if (key1._id < key2._id) {
            return -1;
        } else {
            return 0;
        }

    } else if (this->header.key_field_type == FieldType::int_type) {
        int key1_data;
        int key2_data;
        strncpy(reinterpret_cast<char *>(&key1_data), key1.data, this->header.key_field_len);
        strncpy(reinterpret_cast<char *>(&key2_data), key2.data, this->header.key_field_len);

        if (key1_data > key2_data) {
            return 1;
        } else if (key1_data < key2_data) {
            return -1;
        } else if (key1._id > key2._id) {
            return 1;
        } else if (key1._id < key2._id) {
            return -1;
        } else {
            return 0;
        }

    } else if (this->header.key_field_type == FieldType::char_type) {
        int cmp_result = strncmp(key1.data, key2.data, this->header.key_field_len);

        if (cmp_result == 0) {
            if (key1._id > key2._id) {
                return 1;
            } else if (key1._id < key2._id) {
                return -1;
            } else {
                return 0;
            }
        } else {
            return cmp_result;
        }

    } else {
        throw runtime_error(format("Unknown field_type {} to compare", FieldType_to_string[static_cast<int>(this->header.key_field_type)]));
    }
}

void Btree::split_child(BtreeNode *now_node, vector<pair<long, int>> &traversal_node_record) {
    while (true) {
        if (now_node->header.is_root) {
            ++this->header.count;
            BtreeNode *new_root = new BtreeNode(
                    this->header.count-1,
                    this->header.degree,
                    this->header.key_field_type,
                    this->header.key_field_len,
                    true,
                    false
                    );
            this->NodeMap[this->header.count-1] = new_root;
            this->node_buffer.push(new_root);
            ++this->header.count;
            BtreeNode *right = new BtreeNode(
                    this->header.count-1,
                    this->header.degree,
                    this->header.key_field_type,
                    this->header.key_field_len,
                    false,
                    now_node->header.is_leaf
                    );
            this->NodeMap[this->header.count-1] = right;
            this->node_buffer.push(right);

            now_node->header.is_root = false;
            int key_count = now_node->header.key_count;
            new_root->keys.push_back(now_node->key_copy(key_count/2-1));
            new_root->children.push_back(now_node->header.traversal_id);
            new_root->keys.push_back(now_node->key_copy(key_count-1));
            new_root->children.push_back(right->header.traversal_id);
            new_root->header.key_count = 2;

            vector<struct BtreeKey>::iterator kit;
            vector<long>::iterator cit;

            for (kit = now_node->keys.begin() + (key_count/2), cit = now_node->children.begin() + (key_count/2);
                    kit != now_node->keys.end() ; ++kit, ++cit) {
                right->keys.push_back(*kit);
                right->children.push_back(*cit);
            }
            right->header.key_count = key_count/2;
            now_node->header.key_count = key_count/2;

            this->btree_page_mgr->save_header(this->header);
            this->btree_page_mgr->save_node(new_root->header.traversal_id, *new_root, this->table_option);
            this->btree_page_mgr->save_node(right->header.traversal_id, *right, this->table_option);
            this->btree_page_mgr->save_node(now_node->header.traversal_id, *now_node, this->table_option);
        } else {
            BtreeNode *parent;
            BtreeNode *right;

            pair<long, int> parentId_childIdx = traversal_node_record.back();
            traversal_node_record.pop_back();
            long parent_id = parentId_childIdx.first;
            int child_idx = parentId_childIdx.second;
            int key_count = now_node->header.key_count;

            if (this->NodeMap.find(parent_id) != this->NodeMap.end()) {
                parent = this->NodeMap[parent_id];
            } else {
                parent = new BtreeNode();
                this->btree_page_mgr->get_node(parent_id, *parent, this->table_option);
                this->NodeMap[parent_id] = parent;
                this->node_buffer.push(parent);
            }

            ++this->header.count;
            right = new BtreeNode(
                    this->header.count-1,
                    this->header.degree,
                    this->header.key_field_type,
                    this->header.key_field_len,
                    false,
                    now_node->header.is_leaf
                    );
            this->NodeMap[this->header.count-1] = right;
            this->node_buffer.push(right);

            parent->keys[child_idx]._id = now_node->keys[key_count/2-1]._id;
            strncpy(parent->keys[child_idx].data, now_node->keys[key_count/2-1].data, this->header.key_field_len);

            vector<struct BtreeKey>::iterator kit;
            vector<long>::iterator cit;

            for (kit = now_node->keys.begin() + (key_count/2), cit = now_node->children.begin() + (key_count/2);
                    kit != now_node->keys.end() ; ++kit, ++cit) {
                right->keys.push_back(*kit);
                right->children.push_back(*cit);
            }
            right->header.key_count = key_count/2;
            now_node->header.key_count = key_count/2;

            parent->keys.insert(parent->keys.begin() + child_idx + 1, right->key_copy(right->header.key_count-1));
            parent->children.insert(parent->children.begin() + child_idx + 1, right->header.traversal_id);
            parent->header.key_count += 1;

            this->btree_page_mgr->save_header(this->header);
            this->btree_page_mgr->save_node(parent->header.traversal_id, *parent, this->table_option);
            this->btree_page_mgr->save_node(right->header.traversal_id, *right, this->table_option);
            this->btree_page_mgr->save_node(now_node->header.traversal_id, *now_node, this->table_option);

            if (parent->is_full()) {
                now_node = parent;
            }
        }
    }
}

bool BtreeNode::is_full() {
    return this->header.key_count == 2*this->header.degree;
}

struct BtreeKey BtreeNode::key_copy(int key_idx) {
    struct BtreeKey output_key;
    output_key._id = this->keys[key_idx]._id;
    output_key.data = new char[this->header.key_field_len]();
    strncpy(output_key.data, this->keys[key_idx].data, this->header.key_field_len);

    return output_key;
}

BtreeNode::BtreeNode(long id, int degree, FieldType key_field_type, int key_field_len, bool is_root, bool is_leaf) {
    this->header.traversal_id = id;
    this->header.degree = degree;
    this->header.key_field_len = key_field_len;
    this->header.key_field_type = key_field_type;
    this->header.is_root = is_root;
    this->header.is_leaf = is_leaf;
}

BtreeNode::BtreeNode() {
}

BtreeNode::~BtreeNode() {
    for (const auto& key : this->keys) {
        if (key.data != NULL) {
            delete [] key.data;
        } else {
            break;
        }
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
void BtreePageMgr::save_node(const long &n, btree_node &node, TableOption* table_option) {
    this->clear();
    this->seekp(this->header_prefix + n * table_option->page_size, ios::beg);
    this->write(reinterpret_cast<char *>(&(node.header)), sizeof(node.header));

    int i;
    for (i = 0 ; i < node.header.key_count ; i++) {
        this->write(reinterpret_cast<char *>(&node.children[i]), sizeof(long));

        this->write(node.keys[i].data, node.header.key_field_len * sizeof(char));
        this->write(reinterpret_cast<char *>(&node.keys[i]._id), sizeof(int));
    }
}

template <class btree_node>
bool BtreePageMgr::get_node(const long &n, btree_node &node, TableOption* table_option) {
    this->clear();
    this->seekg(this->header_prefix + n * table_option->page_size, ios::beg);
    this->read(reinterpret_cast<char *>(&(node.header)), sizeof(node.header));

    int i;
    long children_tmp;
    for (i = 0 ; i < node.header.key_count ; i++) {
        this->read(reinterpret_cast<char *>(&children_tmp), sizeof(long));
        node.children.push_back(children_tmp);

        struct BtreeKey key;
        key.data = (char*)calloc(node.header.key_field_len, sizeof(char));
        this->read(key.data, node.header.key_field_len * sizeof(char));
        this->read(reinterpret_cast<char *>(&key._id), sizeof(int));

        node.keys.push_back(key);
    }

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

void DataPageMgr::save_node(const long &n, const json &node, vector<FieldTypeInfo> &field_info) {
    this->clear();

    int node_size = 1, accu_size = 0; // node_size 1 is to record if the node is deleted
    for (auto& item : field_info) {
        node_size += std::get<2>(item);
    }

    char* node_write_data;
    node_write_data = new char[node_size]();
    for (auto& item : field_info) {
        string field_name = std::get<0>(item);
        string field_type = std::get<1>(item);
        int field_size = std::get<2>(item);

        if (field_type == "int") { /// todo need to check if correct
            int field_value = node[field_name].get<int>();
            strncpy(node_write_data + accu_size, reinterpret_cast<char *>(&field_value), field_size);
        } else if (field_type == "char") {
            strncpy(node_write_data + accu_size, node[field_name].get<string>().c_str(), field_size);
        } else {
            throw runtime_error(format("Unknown field_type {}", field_type));
        }
        accu_size += field_size;
    }

    this->seekp(this->header_prefix + n * sizeof(char) * node_size, ios::beg);
    bool exist = true;
    this->write(reinterpret_cast<char *>(&exist), sizeof(char) * 1);
    //this->flush(); /// todo
    this->write(node_write_data, sizeof(char) * (node_size-1));
    //this->flush(); /// todo
    delete [] node_write_data;
}

bool DataPageMgr::get_node(const long &n, json &node, vector<FieldTypeInfo> &field_info) {
    this->clear();

    int node_size = 1; // node_size 1 is to record if the node is deleted
    for (auto& item : field_info) {
        node_size += std::get<2>(item);
    }

    this->seekg(this->header_prefix + n * sizeof(char) * node_size, ios::beg);
    bool exist;
    this->read(reinterpret_cast<char *>(&exist), 1 * sizeof(char));
    node["exist"] = exist;

    for (auto& item : field_info) {
        string field_name = std::get<0>(item);
        string field_type = std::get<1>(item);
        int field_size = std::get<2>(item);
        if (field_type == "int") { /// todo need to check if correct
            int read_data;
            this->read(reinterpret_cast<char *>(&read_data), field_size * sizeof(char));
            node[std::get<0>(item)] = read_data;
        } else if (field_type == "char") {
            char* read_data = new char[field_size];
            this->read(read_data, field_size * sizeof(char));
            node[std::get<0>(item)] = string(read_data);
            delete [] read_data;
        } else {
            throw runtime_error(format("Unknown field_type {}", field_type));
        }
    }

    return this->gcount() > 0;
}

