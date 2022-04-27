
#ifndef MYTREE
#define MYTREE

struct btree;

typedef struct node_linked_list {
    struct btree* node;
    struct node_linked_list* next;
} NodeLinkedList;

typedef struct btree {
    struct btree** child_list;
    int* key_list;
    struct btree* parent;
    int parent_child_id;
    int degree;
    int key_num;
    int child_num;
    int is_root;
    int is_leaf;
    int traversal_id;
    int traversal_height;
} BTree;

void insert(BTree**, BTree*, int);
int _btree_binary_search(int [], int, int, int);
void _split_child(BTree**, BTree*);
void traversal(BTree*);
void tree_init(BTree**, int, int);
void tree_free(BTree*);
void check_valid(BTree*, int);
void _check_valid(BTree*, int, int*);
void print_node_info(BTree*);
int delete_node(BTree**, BTree*, int);

#endif

