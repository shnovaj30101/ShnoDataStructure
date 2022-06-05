
#include "mytree.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1
#define int_swap(x, y) { int t; t=x; x=y; y=t; }

int _btree_binary_search(int arr[], int start_id, int end_id, int data) {

    if (start_id >= end_id) {
        if (arr[start_id] > data) {
            return start_id;
        } else {
            return start_id+1;
        }
    }

    int middle_id = (start_id+end_id)/2;

    if (arr[middle_id] > data) {
        return _btree_binary_search(arr, start_id, middle_id-1, data);
    }
    else {
        return _btree_binary_search(arr, middle_id+1, end_id, data);
    }
}

void insert(BTree** root, BTree* t, int data) {
    if (t->is_root && t->key_num == 0) {
        t->key_list[t->key_num] = data;
        t->key_num += 1;
        return;
    }

    int insert_id = _btree_binary_search(t->key_list, 0, t->key_num-1, data);

    if (t->is_leaf) {
        for (int i = t->key_num-1 ; i >= insert_id ; i--) {
            t->key_list[i+1] = t->key_list[i];
        }
        t->key_list[insert_id] = data;
        t->key_num += 1;

        if (t->key_num == 2*t->degree-1) {
            _split_child(root, t);
        }
    } else {
        insert(root, t->child_list[insert_id], data);
    }
}

void _split_child(BTree** root, BTree* t) {
    if (t->is_root) {
        BTree* new_root_node;
        tree_init(&new_root_node, t->degree, 1);
        new_root_node->is_leaf = FALSE;
        BTree* split_node;
        tree_init(&split_node, t->degree, 0);

        insert(root, new_root_node, t->key_list[t->degree-1]);

        new_root_node->child_list[0] = t;
        t->is_root = FALSE;
        t->parent = new_root_node;
        t->parent_child_id = 0;
        new_root_node->child_list[1] = split_node;
        split_node->parent = new_root_node;
        split_node->parent_child_id = 1;
        new_root_node->child_num = 2;

        for (int i = 0 ; i < t->degree-1 ; i++) {
            split_node->key_list[i] = t->key_list[t->degree+i];
        }
        split_node->key_num = t->degree-1;
        t->key_num = t->degree-1;

        if (!t->is_leaf) {
            split_node->is_leaf = FALSE;
            for (int i = 0 ; i < t->degree ; i++) {
                split_node->child_list[i] = t->child_list[t->degree+i];
                split_node->child_list[i]->parent_child_id = i;
                split_node->child_list[i]->parent = split_node;
            }
            split_node->child_num = t->degree;
            t->child_num = t->degree;
        }

        *root = new_root_node;

    } else {
        BTree* split_node;
        tree_init(&split_node, t->degree, 0);

        for (int i = t->parent->key_num-1 ; i >= t->parent_child_id ; i--) {
            t->parent->key_list[i+1] = t->parent->key_list[i];
            t->parent->child_list[i+2] = t->parent->child_list[i+1];
            t->parent->child_list[i+2]->parent_child_id = i+2;
        }
        t->parent->key_list[t->parent_child_id] = t->key_list[t->degree-1];
        t->parent->child_list[t->parent_child_id+1] = split_node;
        t->parent->key_num += 1;
        t->parent->child_num += 1;

        split_node->parent = t->parent;
        split_node->parent_child_id = t->parent_child_id+1;

        for (int i = 0 ; i < t->degree-1 ; i++) {
            split_node->key_list[i] = t->key_list[t->degree+i];
        }
        split_node->key_num = t->degree-1;
        t->key_num = t->degree-1;

        if (!t->is_leaf) {
            split_node->is_leaf = FALSE;
            for (int i = 0 ; i < t->degree ; i++) {
                split_node->child_list[i] = t->child_list[t->degree+i];
                split_node->child_list[i]->parent_child_id = i;
                split_node->child_list[i]->parent = split_node;
            }
            split_node->child_num = t->degree;
            t->child_num = t->degree;
        }

        if (t->parent->key_num == 2*t->degree-1) {
            _split_child(root, t->parent);
        }
    }
}

int delete_node(BTree** root, BTree* t, int data) {
    int target_id = _btree_binary_search(t->key_list, 0, t->key_num-1, data);
    target_id -= 1;

    if (t->key_list[target_id] == data) {
        if (!t->is_leaf) {
            BTree* left_right_most = _get_right_most_descendant(t->child_list[target_id]);

            t->key_list[target_id] = left_right_most->key_list[left_right_most->key_num-1];
            left_right_most->key_num -= 1;

            if (left_right_most->key_num < left_right_most->degree-1) {
                _handle_key_shortage(root, left_right_most);
            }
        } else {
            for (int i = target_id ; i < t->key_num-1 ; i++) {
                t->key_list[i] = t->key_list[i+1];
            }
            t->key_num -= 1;

            if (t->key_num < t->degree-1) {
                _handle_key_shortage(root, t);
            }
        }

        return TRUE;
    } else {
        if (t->is_leaf) {
            return FALSE;
        }
        return delete_node(root, t->child_list[target_id+1], data);
    }
}

BTree* _get_left_most_descendant(BTree* t) {
    BTree* output = t;

    while (!output->is_leaf) {
        output = output->child_list[0];
    }

    return output;
}

BTree* _get_right_most_descendant(BTree* t) {
    BTree* output = t;

    while (!output->is_leaf) {
        output = output->child_list[output->child_num-1];
    }

    return output;
}

void _handle_key_shortage(BTree** root, BTree* t) {
    if (t->is_root) {
        if (t->child_num == 1) {
            t->child_list[0]->parent = NULL;
            t->child_list[0]->parent_child_id = -1;
            t->child_list[0]->is_root = TRUE;
            *root = t->child_list[0];
            tree_free(t, 0);
        }
        return;
    }

    if (t->parent_child_id > 0 &&
        t->parent->child_list[t->parent_child_id-1]->key_num > t->degree-1) {
        BTree* left_sibling = t->parent->child_list[t->parent_child_id-1];
        for (int i=t->key_num-1 ; i>=0 ; i--) {
            t->key_list[i+1] = t->key_list[i];
        }
        t->key_num += 1;
        t->key_list[0] = t->parent->key_list[t->parent_child_id-1];
        t->parent->key_list[t->parent_child_id-1] = left_sibling->key_list[left_sibling->key_num-1];
        left_sibling->key_num -= 1;

        if (!t->is_leaf) {
            for (int i=t->child_num-1; i >= 0  ; i--) {
                t->child_list[i]->parent_child_id = i+1;
                t->child_list[i+1] = t->child_list[i];
            }
            t->child_num += 1;

            left_sibling->child_list[left_sibling->child_num-1]->parent = t;
            left_sibling->child_list[left_sibling->child_num-1]->parent_child_id = 0;
            t->child_list[0] = left_sibling->child_list[left_sibling->child_num-1];
            left_sibling->child_num -= 1;
        }
    } else if (t->parent_child_id < t->parent->child_num-1 &&
        t->parent->child_list[t->parent_child_id+1]->key_num > t->degree-1) {
        BTree* right_sibling = t->parent->child_list[t->parent_child_id+1];
        t->key_list[t->key_num++] = t->parent->key_list[t->parent_child_id];
        t->parent->key_list[t->parent_child_id] = right_sibling->key_list[0];
        for (int i=0 ; i < right_sibling->key_num-1 ; i++) {
            right_sibling->key_list[i] = right_sibling->key_list[i+1];
        }
        right_sibling->key_num -= 1;

        if (!t->is_leaf) {
            right_sibling->child_list[0]->parent = t;
            right_sibling->child_list[0]->parent_child_id = t->child_num;
            t->child_list[t->child_num++] = right_sibling->child_list[0];

            for (int i=0 ; i < right_sibling->child_num-1 ; i++) {
                right_sibling->child_list[i+1]->parent_child_id = i;
                right_sibling->child_list[i] = right_sibling->child_list[i+1];
            }
            right_sibling->child_num -= 1;
        }
    } else {
        BTree *left, *right;
        if (t->parent_child_id > 0) {
            BTree* left_sibling = t->parent->child_list[t->parent_child_id-1];
            left = left_sibling;
            right = t;
        } else {
            BTree* right_sibling = t->parent->child_list[t->parent_child_id+1];
            left = t;
            right = right_sibling;
        }

        _merge_child(root, left, right);
        if (left->parent && left->parent->key_num < left->degree-1) {
            _handle_key_shortage(root, left->parent);
        }
    }
}

void _merge_child(BTree** root, BTree* left, BTree* right) {
    left->key_list[left->key_num++] = left->parent->key_list[left->parent_child_id];

    for (int i = 0 ; i < right->key_num ; i++) {
        left->key_list[left->key_num++] = right->key_list[i];
    }

    if (!left->is_leaf) {
        for (int i = 0 ; i < right->child_num ; i++) {
            right->child_list[i]->parent = left;
            right->child_list[i]->parent_child_id = left->child_num;
            left->child_list[left->child_num++] = right->child_list[i];
        }
    }

    tree_free(right, 0);

    for (int i = left->parent_child_id + 1 ; i < left->parent->key_num ; i++ ) {
        left->parent->key_list[i-1] = left->parent->key_list[i];
    }
    left->parent->key_num -= 1;

    for (int i = left->parent_child_id + 2 ; i < left->parent->child_num ; i++ ) {
        left->parent->child_list[i]->parent_child_id = i-1;
        left->parent->child_list[i-1] = left->parent->child_list[i];
    }
    left->parent->child_num -= 1;

    if (left->parent->key_num < left->parent->degree-1) {
        _handle_key_shortage(root, left->parent);
    }
}

void traversal(BTree* t) {
    int traversal_id = 0;
    NodeLinkedList *head, *tail, *now;
    head = (NodeLinkedList*)calloc(1, sizeof(NodeLinkedList));
    tail = head;
    now = head;

    head->node = t;
    head->node->traversal_id = traversal_id++;

    while (now) {
        BTree* now_node = now->node;

        for (int i = 0 ; i < now_node->child_num ; i++) {
            NodeLinkedList *_new = (NodeLinkedList*)calloc(1, sizeof(NodeLinkedList));
            _new->node = now_node->child_list[i];
            _new->node->traversal_id = traversal_id++;
            tail->next = _new;
            tail = _new;
        }
        print_node_info(now_node);

        now = now->next;
    }

    now = head;
    while (now) {
        NodeLinkedList* to_delete = now;
        now = now->next;
        free(to_delete);
    }
}

void print_node_info(BTree* t) {
    printf("traversal_id: %d, key_num: %d, child_num: %d, parent_traversal_id: %d, parent_child_id: %d, degree: %d\n",
        t->traversal_id,
        t->key_num,
        t->child_num,
        t->parent ? t->parent->traversal_id : -1,
        t->parent_child_id,
        t->degree
    );
    printf("is_root: %d, is_leaf: %d \n",
        t->is_root,
        t->is_leaf
    );
    printf("key_list:\n");
    for (int i = 0 ; i < t->key_num ; i++) {
        printf("%d ", t->key_list[i]);
    }
    printf("\n");
    printf("child_list:\n");
    for (int i = 0 ; i < t->child_num ; i++) {
        if (t->child_list[i]) {
            printf("%d ", t->child_list[i]->traversal_id);
        } else {
            printf("%d ", -1);
        }
    }
    printf("\n");
    printf("=====================\n");
}

void check_valid(BTree* t, int is_root) {
    int tree_height = -1;

    int traversal_id = 0;
    NodeLinkedList *head, *tail, *now;
    head = (NodeLinkedList*)calloc(1, sizeof(NodeLinkedList));
    tail = head;
    now = head;

    head->node = t;
    head->node->traversal_id = traversal_id++;
    head->node->traversal_height = 1;

    while (now) {
        BTree* now_node = now->node;

        for (int i = 0 ; i < now_node->child_num ; i++) {
            NodeLinkedList *_new = (NodeLinkedList*)calloc(1, sizeof(NodeLinkedList));
            _new->node = now_node->child_list[i];
            _new->node->traversal_id = traversal_id++;
            _new->node->traversal_height = now_node->traversal_height+1;
            tail->next = _new;
            tail = _new;
        }
        if (now == head) {
            _check_valid(now_node, is_root, &tree_height);
        } else {
            _check_valid(now_node, 0, &tree_height);
        }
        /*print_node_info(now_node);*/

        now = now->next;
    }
    /*printf("==============================\n");*/

    now = head;
    while (now) {
        NodeLinkedList* to_delete = now;
        now = now->next;
        free(to_delete);
    }
}

void _check_valid(BTree* t, int is_root, int* tree_height) {
    if (!t) {
        return;
    }
    if (is_root) {
        if (!t->is_root) {
            printf("root is_root != 1\n");
            print_node_info(t);
            exit(-1);
        }
        if (t->key_num < 1 && t->child_num > 0) {
            printf("root key_num < 1\n");
            print_node_info(t);
            exit(-1);
        }

        if (!t->is_leaf) {
            if (t->child_num < 2) {
                printf("root child_num < 2\n");
                print_node_info(t);
                exit(-1);
            }
        } else {
            if (t->child_num != 0) {
                printf("root child_num(%d) != 0\n", t->child_num);
                print_node_info(t);
                exit(-1);
            }
        }
    } else {
        if (t->is_root) {
            printf("is_root == 1\n");
            print_node_info(t);
            exit(-1);
        }
        if (t->key_num < t->degree-1) {
            printf("key_num(%d) < degree-1(%d)\n", t->key_num, t->degree-1);
            print_node_info(t);
            exit(-1);
        }

        if (!t->is_leaf) {
            if (t->child_num < t->degree) {
                printf("child_num(%d) < degree(%d)\n", t->child_num, t->degree);
                print_node_info(t);
                exit(-1);
            }
        } else {
            if (t->child_num != 0) {
                printf("root child_num(%d) != 0\n", t->child_num);
                print_node_info(t);
                exit(-1);
            }
        }
    }

    if (!t->is_leaf) {
        if (t->key_num + 1 != t->child_num) {
            printf("t->key_num + 1(%d) != t->child_num(%d)\n", t->key_num + 1, t->child_num);
            print_node_info(t);
            exit(-1);
        }
    }

    if (t->is_leaf) {
        if (t->child_num > 0) {
            printf("t->child_num(%d) > 0\n", t->child_num);
            print_node_info(t);
            exit(-1);
        }
    } else {
        for (int i = 0 ; i < t->child_num ; i++) {
            if (!t->child_list[i]) {
                printf("child_list[%d] is NULL\n", i);
                print_node_info(t);
                exit(-1);
            }
            if (t->child_list[i]->parent != t) {
                if (!t->child_list[i]->parent) {
                    printf("child_list[%d](%d)->parent is NULL\n",
                            i,
                            t->child_list[i]->traversal_id
                        );
                } else {
                    printf("child_list[%d](%d)->parent(%d) != t(%d)\n",
                            i,
                            t->child_list[i]->traversal_id,
                            t->child_list[i]->parent->traversal_id,
                            t->traversal_id
                        );
                }
                print_node_info(t);
                exit(-1);
            }
            if (t->child_list[i]->parent_child_id != i) {
                printf("child_list[%d](%d)->parent_child_id(%d) != i(%d)\n",
                        i,
                        t->child_list[i]->traversal_id,
                        t->child_list[i]->parent_child_id,
                        i
                    );
                print_node_info(t);
                exit(-1);
            }
        }
    }

    for (int i = 1 ; i < t->key_num ; i++) {
        if (t->key_list[i] < t->key_list[i-1]) {
            printf("key_list[%d](%d) < key_list[%d](%d)\n",
                    i,
                    t->key_list[i],
                    i-1,
                    t->key_list[i-1]
              );
            print_node_info(t);
            exit(-1);
        }
    }

    if (t->is_root) {
        if (t->parent) {
            printf("root parent is not NULL (%d)\n",
                    t->parent->traversal_id
                );
            print_node_info(t);
            exit(-1);
        }

        if (t->parent_child_id != -1) {
            printf("root parent_child_id != -1 (%d)\n",
                    t->parent_child_id
                );
            print_node_info(t);
            exit(-1);
        }
    } else {
        if (!t->parent) {
            printf("parent is NULL\n");
            print_node_info(t);
            exit(-1);
        }
        if (t->parent_child_id == -1) {
            printf("parent_child_id != -1\n");
            print_node_info(t);
            exit(-1);
        }
    }

    if (t->parent && t->parent_child_id > 0 && t->key_list[0] < t->parent->key_list[t->parent_child_id-1]) {
        printf("key_list[0](%d) < parent->key_list[parent_child_id-1(%d)](%d)\n",
                t->key_list[0],
                t->parent_child_id-1,
                t->parent->key_list[t->parent_child_id-1]
            );
        print_node_info(t);
        exit(-1);
    }

    if (t->parent && t->parent_child_id < t->parent->key_num && t->key_list[t->key_num-1] > t->parent->key_list[t->parent_child_id]) {
        printf("key_list[key_num-1(%d)](%d) > parent->key_list[parent_child_id(%d)](%d)\n",
                t->key_num-1,
                t->key_list[t->key_num-1],
                t->parent_child_id,
                t->parent->key_list[t->parent_child_id]
            );
        print_node_info(t);
        exit(-1);
    }

    if (t->is_leaf) {
        if (*tree_height == -1) {
            *tree_height = t->traversal_height;
            /*printf("ensure tree_height(%d) in node %d\n",*/
                    /**tree_height,*/
                    /*t->traversal_id*/
                /*);*/
        } else if (*tree_height != t->traversal_height) {
            printf("tree_height(%d) != traversal_height(%d)\n",
                    *tree_height,
                    t->traversal_height
                );
            print_node_info(t);
            exit(-1);
        }
    }
}

void tree_init(BTree** tp, int degree, int is_root) {
    *tp = (BTree*)malloc(sizeof(BTree));
    BTree* t = *tp;
    t->degree = degree;
    t->key_num = 0;
    t->child_num = 0;
    t->is_root = is_root;
    t->is_leaf = TRUE;
    t->parent_child_id = -1;
    t->traversal_id = -1;
    t->traversal_height = -1;
    t->parent = NULL;
    t->child_list = (BTree**)calloc((2*degree), sizeof(BTree*));
    t->key_list = (int*)calloc((2*degree-1), sizeof(int));
}

void tree_free(BTree* t, int free_child) {
    if (!t) {
        return;
    }

    free(t->key_list);

    if (free_child) {
        for (int i = 0 ; i < t->child_num; i++) {
            tree_free(t->child_list[i], 1);
        }
    }

    free(t->child_list);

    free(t);
}
