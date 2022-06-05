
#ifndef MYTREE
#define MYTREE

#include<vector>
#include<map>
#include<utility>
#include "nlohmann/json.hpp"
#include<fstream>
#include<queue>

using namespace std;

class Table {
    public:
    private:
        string name;
        vector<Btree*> btree_list;
}

class RecData {
    public:
        read();
        write();
    private:
        vector< tuple<string, string, int> > field_info; // 欄位名稱, 資料型態, 欄位值 size
        json data;

}

class ChildData {
    public:
        read();
        write();
    private:
        string child_file_name;
}

class Btree {
    public:
    private:
        int file_size;
        int field_name_max_len;
        int field_value_max_len;
        int degree;
        string name;
        fstream root_file_handle;
        string root_dirname;
        queue<BtreeNode*> node_buffer;
}

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
}


//typedef struct node_linked_list {
    //struct btree* node;
    //struct node_linked_list* next;
//} NodeLinkedList;

//typedef struct btree {
    //struct btree** child_list;
    //int* key_list;
    //struct btree* parent;
    //int parent_child_id;
    //int degree;
    //int key_num;
    //int child_num;
    //int is_root;
    //int is_leaf;
    //int traversal_id;
    //int traversal_height;
//} BTree;

//void insert(BTree**, BTree*, int);
//int _btree_binary_search(int [], int, int, int);
//void _split_child(BTree**, BTree*);
//void _merge_child(BTree**, BTree*, BTree*);
//void traversal(BTree*);
//void tree_init(BTree**, int, int);
//void tree_free(BTree*, int);
//void check_valid(BTree*, int);
//void _check_valid(BTree*, int, int*);
//void print_node_info(BTree*);
//int delete_node(BTree**, BTree*, int);
//void _handle_key_shortage(BTree**, BTree*);
//BTree* _get_left_most_descendant(BTree*);
//BTree* _get_right_most_descendant(BTree*);

#endif

