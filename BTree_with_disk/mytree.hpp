
#ifndef MYTREE
#define MYTREE

#include<vector>
#include<memory>
#include<map>
#include<utility>
#include "nlohmann/json.hpp"
#include "basic.hpp"
#include "fmt/format.h"
#include<fstream>
#include<queue>

#define FieldTypeInfo tuple<string,string,int>

namespace fs = std::filesystem;
using namespace std;
using namespace fmt;
using json = nlohmann::json;

class Btree;
class BtreeNode;
class DbSystem;
class Table;
class BtreePageMgr;
class DataPageMgr;

enum class FieldType {
    int_type = 0,
    char_type = 1,
};

static string FieldType_to_string[] = {
    "int",
    "char",
};

struct BtreeKey {
    int _id;
    char* data;
}; /// todo _id 不一定是 int , data 可以都轉成 char* , 但是一定要記錄型態 , 因為比對要用


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
            page_size(DEFAULT_PAGE_SIZE),
            field_max(DEFAULT_FIELD_MAX),
            value_max(DEFAULT_VALUE_MAX),
            btree_node_buffer_len(BTREE_NODE_BUFFER_LEN){};
        TableOption(const string& table_name, int page_size, int field_max, int value_max, int btree_node_buffer_len):
            table_name(table_name),
            page_size(page_size),
            field_max(field_max),
            value_max(value_max),
            btree_node_buffer_len(btree_node_buffer_len){};
        ~TableOption() {};
        string table_name;
        int page_size;
        int field_max;
        int value_max;
        int btree_node_buffer_len;
        vector< FieldTypeInfo> field_info; // 欄位名稱, 資料型態, 欄位值 size
        /// todo FieldTypeInfo 中間 type 改成 FieldType
};


class Table {
    public:
        Table(const string& table_name, const string& field_str, const string& pk); // create table 時使用
        Table(const string& table_name); // use table 時使用
        ~Table();

        void create_primary_index(const string& pk);
        void create_index(const string& index_name);
        void fill_field_info(const string& field_name, const string& field_type);
        void write_table_info();
        void read_table_info();
        void insert_data(json &json_data);
        void insert_data_validation(const json &json_data); /// todo
        TableOption* table_option;
        struct meta_data {
            long count{0};
        } data_header;
    private:
        string pk;
        map<string, Btree*> IndexMap;
        shared_ptr <DataPageMgr> data_page_mgr;
};

class Btree {
    public:
        Btree(const string& index_name, int degree, FieldType key_field_type, int key_field_len, shared_ptr <DataPageMgr> data_page_mgr, TableOption* table_option);
        Btree(const string& index_name, shared_ptr <DataPageMgr> data_page_mgr, TableOption* table_option);
        ~Btree();
        TableOption* table_option;
        void insert_key(struct BtreeKey &key, long data_page_pos);
        void split_child(BtreeNode *node, vector<pair<long, int>> &traversal_node_record);
        int key_compare(struct BtreeKey &key1, struct BtreeKey &key2);
        struct meta_data {
            bool is_pk{false};
            long count{0};
            long root_id{0};
            int degree;
            int key_field_len;
            FieldType key_field_type;
        } header;
    private:
        string index_name;
        shared_ptr <BtreePageMgr> btree_page_mgr;
        shared_ptr <DataPageMgr> data_page_mgr;
        queue<BtreeNode*> node_buffer;
        map<long, BtreeNode*> NodeMap;
        BtreeNode* root;
};

class BtreeNode {
    public:
        BtreeNode(long id, int degree, FieldType key_field_type, int key_field_len, bool is_root, bool is_leaf);
        BtreeNode();
        ~BtreeNode();
        struct meta_data {
            long traversal_id;
            int degree;
            int key_field_len;
            FieldType key_field_type;
            bool is_root;
            bool is_leaf;
            long right{-1};
            int key_count{0};
        } header;
        vector<struct BtreeKey> keys;
        vector<long> children;
        bool is_full();
        struct BtreeKey key_copy(int key_idx);
    private:
};

class BtreePageMgr : protected fstream {
    public:
        BtreePageMgr(const string& filename, bool trunc = false);
        ~BtreePageMgr();

        template <class header_data>
        void save_header(header_data &header);

        template <class header_data>
        bool get_header(header_data &header);

        template <class btree_node>
        void save_node(const long &n, btree_node &node, TableOption* table_option);

        template <class btree_node>
        bool get_node(const long &n, btree_node &node, TableOption* table_option);

    private:
        bool empty;
        string filename;
        long header_prefix{0};
};

class DataPageMgr : protected fstream {
    public:
        DataPageMgr(const string& filename, bool trunc = false);
        ~DataPageMgr();

        template <class header_data>
        void save_header(header_data &header);

        template <class header_data>
        bool get_header(header_data &header);

        void save_node(const long &n, const json &node, vector<FieldTypeInfo> &field_info);

        bool get_node(const long &n, json &node, vector<FieldTypeInfo> &field_info);

    private:
        bool empty;
        string filename;
        long header_prefix{0};
};

#endif

