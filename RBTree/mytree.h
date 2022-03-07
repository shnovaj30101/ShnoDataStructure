
#ifndef MYTREE
#define MYTREE

typedef struct rbtree {
    struct rbtree* left;
    struct rbtree* right;
    struct rbtree* parent;
    int black_num;
    int data;
    int is_root;
} RBTree;

void insert(RBTree**, RBTree*, int);
void traversal(RBTree*);
void tree_init(RBTree**, int);
void tree_free(RBTree*);
void _rotation(RBTree**, RBTree*, RBTree*);
void _solve_red_collision(RBTree**, RBTree*);
int delete_node(RBTree**, RBTree*, int);
void _delete_node_handle(RBTree**, RBTree*);
void _BBB_delete_node_handle(RBTree**, RBTree*);
void _BRB_delete_node_handle(RBTree**, RBTree*);
void _BBR_delete_node_handle(RBTree**, RBTree*);
RBTree* _get_sibling_node(RBTree*);
RBTree* _get_sibling_node(RBTree*);
RBTree* _get_remote_nephew_node(RBTree*);
RBTree* _get_near_nephew_node(RBTree*);

#endif

