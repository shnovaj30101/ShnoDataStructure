
#include "mytree.h"
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

        for (int i = 0 ; i < t->degree ; i++) {
            split_node->child_list[i] = t->child_list[t->degree+i];
        }
        split_node->child_num = t->degree;

        t->key_num = t->degree-1;
        t->child_num = t->degree;

        *root = new_root_node;

    } else {
        BTree* split_node;
        tree_init(&split_node, t->degree, 0);

        for (int i = t->parent->key_num-1 ; i >= t->parent_child_id ; i--) {
            t->parent->key_list[i+1] = t->parent->key_list[i];
            t->parent->child_list[i+2] = t->parent->child_list[i+1];
        }
        t->parent->key_list[t->parent_child_id] = t->key_list[t->degree];
        t->parent->child_list[t->parent_child_id+1] = split_node;
        t->parent->key_num += 1;
        t->parent->child_num += 1;

        split_node->parent = t->parent;
        split_node->parent_child_id = t->parent_child_id+1;

        for (int i = 0 ; i < t->degree-1 ; i++) {
            split_node->key_list[i] = t->key_list[t->degree+i];
        }
        split_node->key_num = t->degree-1;

        for (int i = 0 ; i < t->degree ; i++) {
            split_node->child_list[i] = t->child_list[t->degree+i];
        }
        split_node->child_num = t->degree;

        t->key_num = t->degree-1;
        t->child_num = t->degree;

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
        if (t->key_num == 0) {
            t->child_list[0]->parent = NULL;
            t->child_list[0]->parent_child_id = -1;
            free(t);
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
    } else if (t->parent_child_id < t->parent->key_num-1 &&
        t->parent->child_list[t->parent_child_id+1]->key_num > t->degree-1) {
        BTree* right_sibling = t->parent->child_list[t->parent_child_id+1];
        t->key_list[t->key_num] = t->parent->key_list[t->parent_child_id];
        t->key_num += 1;
        t->parent->key_list[t->parent_child_id] = right_sibling->key_list[0];
        for (int i=0 ; i < right_sibling->key_num-1 ; i++) {
            t->key_list[i] = t->key_list[i+1];
        }
        right_sibling->key_num -= 1;
    } else {
        // todo
    }
}

void traversal(BTree* t) {
    int traversal_id = 0;
    NodeLinkedList *head, *tail, *now;
    head = calloc(1, sizeof(NodeLinkedList));
    tail = head;
    now = head;

    head->node = t;
    head->node->traversal_id = traversal_id++;

    while (now) {
        BTree* now_node = now->node;

        for (int i = 0 ; i < now_node->key_num+1 ; i++) {
            NodeLinkedList *new = calloc(1, sizeof(NodeLinkedList));
            new->node = now_node->child_list[i];
            new->node->traversal_id = traversal_id++;
            tail->next = new;
            tail = new;
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
    printf("%d %d %d %d %d\n",
        t->traversal_id,
        t->key_num,
        t->parent ? t->parent->traversal_id : -1,
        t->parent_child_id,
        t->degree
    );
    printf("%d %d \n",
        t->is_root,
        t->is_leaf
    );
    for (int i = 0 ; i < t->key_num ; i++) {
        printf("%d ", t->key_list[i]);
    }
    printf("\n");
    for (int i = 0 ; i < t->key_num+1 ; i++) {
        if (t->child_list[i]) {
            printf("%d ", t->child_list[i]->traversal_id);
        } else {
            printf("%d ", -1);
        }
    }
    printf("\n");
    printf("=====================");
}

void check_valid(BTree* t, int is_root) {
    int tree_height = -1;

    int traversal_id = 0;
    NodeLinkedList *head, *tail, *now;
    head = calloc(1, sizeof(NodeLinkedList));
    tail = head;
    now = head;

    head->node = t;
    head->node->traversal_id = traversal_id++;
    head->node->traversal_height = 1;

    while (now) {
        BTree* now_node = now->node;

        for (int i = 0 ; i < now_node->key_num+1 ; i++) {
            NodeLinkedList *new = calloc(1, sizeof(NodeLinkedList));
            new->node = now_node->child_list[i];
            new->node->traversal_id = traversal_id++;
            new->node->traversal_height = now_node->traversal_height+1;
            tail->next = new;
            tail = new;
        }
        _check_valid(t, is_root, &tree_height);
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

void _check_valid(BTree* t, int is_root, int* tree_height) {
    if (!t) {
        return;
    }
    if (is_root) {
        if (!t->is_root) {
            printf("root is_root != 1");
            print_node_info(t);
            exit(-1);
        }
        if (t->key_num < 1) {
            printf("root key_num < 1");
            print_node_info(t);
            exit(-1);
        }

        if (t->child_num < 2) {
            printf("root child_num < 2");
            print_node_info(t);
            exit(-1);
        }
    } else {
        if (t->is_root) {
            printf("is_root == 1");
            print_node_info(t);
            exit(-1);
        }
        if (t->key_num < t->degree-1) {
            printf("key_num < degree-1");
            print_node_info(t);
            exit(-1);
        }

        if (t->child_num < t->degree) {
            printf("child_num < degree");
            print_node_info(t);
            exit(-1);
        }
    }

    if (t->key_num + 1 != t->child_num) {
        printf("key_num < degree");
        print_node_info(t);
        exit(-1);
    }

    if (t->is_leaf) {
        if (t->child_num > 0) {
            printf("key_num < degree");
            print_node_info(t);
            exit(-1);
        }
    } else {
        for (int i = 0 ; i < t->child_num ; i++) {
            if (!t->child_list[i]) {
                printf("child_list[%d] is NULL", i);
                print_node_info(t);
                exit(-1);
            }
            if (t->child_list[i]->parent != t) {
                if (!t->child_list[i]->parent) {
                    printf("child_list[%d](%d)->parent is NULL",
                            i,
                            t->child_list[i]->traversal_id
                        );
                } else {
                    printf("child_list[%d](%d)->parent(%d) != t(%d)",
                            i,
                            t->child_list[i]->traversal_id,
                            t->child_list[i]->parent->traversal_id,
                            t->traversal_id
                        );
                }
                print_node_info(t);
                exit(-1);
            }
            if (t->child_list[i]->parent_child_id == i) {
                printf("child_list[%d](%d)->parent_child_id(%d) != i(%d)",
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
            printf("key_list[%d](%d) < key_list[%d](%d)",
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
            printf("root parent is not NULL (%d)",
                    t->parent->traversal_id
                );
            print_node_info(t);
            exit(-1);
        }

        if (t->parent_child_id != -1) {
            printf("root parent_child_id != -1 (%d)",
                    t->parent_child_id
                );
            print_node_info(t);
            exit(-1);
        }
    } else {
        if (!t->parent) {
            printf("parent is NULL");
            print_node_info(t);
            exit(-1);
        }
        if (t->parent_child_id == -1) {
            printf("parent_child_id != -1");
            print_node_info(t);
            exit(-1);
        }
    }

    if (t->parent_child_id > 0 && t->key_list[0] < t->parent->key_list[t->parent_child_id-1]) {
        printf("key_list[0](%d) < parent->key_list[parent_child_id-1(%d)](%d)",
                t->key_list[0],
                t->parent_child_id-1,
                t->parent->key_list[t->parent_child_id-1]
            );
        print_node_info(t);
        exit(-1);
    }

    if (t->key_list[t->key_num-1] > t->parent->key_list[t->parent_child_id]) {
        printf("key_list[key_num-1(%d)](%d) > parent->key_list[parent_child_id(%d)](%d)",
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
            printf("ensure tree_height(%d) in node %d",
                    *tree_height,
                    t->traversal_id
                );
        } else if (*tree_height != t->traversal_height) {
            printf("tree_height(%d) != traversal_height(%d)",
                    *tree_height,
                    t->traversal_height
                );
            print_node_info(t);
            exit(-1);
        }
    }
}

void tree_init(BTree** tp, int degree, int is_root) {
    *tp = malloc(sizeof(BTree));
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
    t->child_list = calloc((2*degree), sizeof(BTree*));
    t->key_list = calloc((2*degree-1), sizeof(int));
}

void tree_free(BTree* t) {
    if (!t) {
        return;
    }

    free(t->key_list);

    for (int i = 0 ; i < 2*t->degree ; i++) {
        tree_free(t->child_list[i]);
    }

    free(t->child_list);

    free(t);
}
