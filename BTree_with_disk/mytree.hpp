
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
            file_size(DEFAULT_PAGE_SIZE),
            field_max(DEFAULT_FIELD_MAX),
            value_max(DEFAULT_VALUE_MAX),
            btree_node_buffer_len(BTREE_NODE_BUFFER_LEN){};
        TableOption(const string& table_name, int file_size, int field_max, int value_max, int btree_node_buffer_len):
            table_name(table_name),
            file_size(file_size),
            field_max(field_max),
            value_max(value_max),
            btree_node_buffer_len(btree_node_buffer_len){};
        ~TableOption() {};
        string table_name;
        int file_size;
        int field_max;
        int value_max;
        int btree_node_buffer_len;
        vector< tuple<string, string, int> > field_info; // 欄位名稱, 資料型態, 欄位值 size
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
        TableOption* table_option;
    private:
        string pk;
        map<string, Btree*> IndexMap;
        shared_ptr <DataPageMgr> data_page_mgr;
};

class Btree {
    public:
        Btree(const string& index_name, int degree, int key_field_len, shared_ptr <DataPageMgr> data_page_mgr, TableOption* table_option);
        Btree(const string& index_name, shared_ptr <DataPageMgr> data_page_mgr, TableOption* table_option);
        ~Btree();
        TableOption* table_option;
    private:
        struct meta_data {
            bool is_pk{false};
            long count{0};
            long root_id{0};
            int degree;
            int key_field_len;
        } header;
        string index_name;
        shared_ptr <BtreePageMgr> btree_page_mgr;
        shared_ptr <DataPageMgr> data_page_mgr;
        queue<BtreeNode*> node_buffer;
        map<long, BtreeNode*> NodeMap;
        BtreeNode* root;
};

struct BtreeKey {
    char* data;
    int _id;
};

class BtreeNode {
    public:
        BtreeNode(long id, int degree, int key_field_len, bool is_root, bool is_leaf);
        ~BtreeNode();

    private:
        struct meta_data {
            long traversal_id;
            int degree;
            int key_field_len;
            bool is_root;
            bool is_leaf;
            long right;
            int key_count;
        } header;

        //BtreeKey key[2*degree-1];
        //long children[2*degree];
        vector<BtreeKey> keys;
        vector<long> children;
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
        void save_node(const long &n, btree_node &node);

        template <class btree_node>
        bool get_node(const long &n, btree_node &node);

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

        template <class btree_node>
        void save_node(const long &n, btree_node &node);

        template <class btree_node>
        bool get_node(const long &n, btree_node &node);

    private:
        bool empty;
        string filename;
        long header_prefix{0};
};

#endif

