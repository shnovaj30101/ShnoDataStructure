
#ifndef MYTREE
#define MYTREE

#include<vector>
#include<map>
#include<utility>
#include "nlohmann/json.hpp"
#include "basic.hpp"
#include "fmt/format.h"
#include<fstream>
#include<queue>

namespace fs = std::filesystem;
using namespace std;
using namespace fmt;
using json = nlohmann::json;

class Btree;
class BtreeNode;
class DbSystem;
class Table;

extern DbSystem* db_system_ptr;

class DbSystem {
    public:
        DbSystem();
        ~DbSystem();
        void init();
        void create_table(const string& table_name, const string& field_str, const string& pk = DEFAULT_PK);
        void use_table(const string& table_name);
        void create_index(const string& index_name);
        void insert_file(const string& file_name);
    private:
        map<string, Table*> TableMap;
        Table* now_table;
};

class TableOption {
    public:
        TableOption(const string& table_name):
            table_name(table_name),
            file_size(DEFAULT_FILE_SIZE),
            field_max(DEFAULT_FIELD_MAX),
            value_max(DEFAULT_VALUE_MAX),
            btree_node_buffer_len(BTREE_NODE_BUFFER_LEN){};
        TableOption(const string& table_name, int file_size, int field_max, int value_max, btree_node_buffer_len):
            table_name(table_name),
            file_size(file_size),
            field_max(field_max),
            value_max(value_max),
            btree_node_buffer_len(btree_node_buffer_len){};
        ~TableOption();
        string table_name;
        int file_size;
        int field_max;
        int value_max;
        int btree_node_buffer_len;
}

class Table {
    public:
        Table(const string& table_name, const string& field_str, const string& pk); // create table 時使用
        Table(const string& table_name); // use table 時使用
        ~Table();

        void create_primary_index(const string& pk);
        void fill_field_info(const string& field_name, const string& field_type);
        void write_table_info();
        void read_table_info();
        TableOption* table_option;
    private:
        string pk;
        map<string, Btree*> IndexMap;
        vector< tuple<string, string, int> > field_info; // 欄位名稱, 資料型態, 欄位值 size
};

class RecData {
    public:
    private:
        vector< tuple<string, string, int> > field_info; // 欄位名稱, 資料型態, 欄位值 size
        json data;
};

class ChildData {
    public:
    private:
        string child_file_name;
};

class Btree {
    public:
        Btree(const string& index_name, bool init, TableOption* table_option);
        ~Btree();
        TableOption* table_option;
    private:
        int file_size;
        int field_name_max_len;
        int field_value_max_len;
        int node_buffer_len;
        int degree;
        string index_name;
        fstream root_file_handle;
        string root_dirname;
        queue<BtreeNode*> node_buffer;
        map<string, BtreeNode*> NodeMap;
};

class BtreeNode {
    public:
    private:
        vector<RecData> rec_list;
        vector<ChildData> child_list;
        fstream file_handle;
        bool is_root;
        bool is_leaf;
        int traversal_id;
        int traversal_height;
        int degree;
        int key_num;
        int child_num;
        BtreeNode* parent;
        int parent_child_id;
};

#endif

