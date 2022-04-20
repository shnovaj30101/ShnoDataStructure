
#include "mytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1
#define int_swap(x, y) { int t; t=x; x=y; y=t; }

void insert(BTree** root, BTree* t, int data) {
}

int delete_node(BTree** root, BTree* t, int data) {
}

void traversal(BTree* t) {
    int traversal_id = 0;
    NodeLinkedList *head, *tail, *now;
    head = calloc(1, sizeof(NodeLinkedList));
    tail = head;
    now = head;

    head->node = t;
    head->node->traversal_id = traversal_id++;

    _traversal(now, &tail, traversal_id);

    now = head;
    while (now) {
        NodeLinkedList* to_delete = now;
        now = now->next;
        free(to_delete);
    }
}

void _traversal(NodeLinkedList* now, NodeLinkedList** tail, int traversal_id) {
    if (!now) {
        return;
    }

    BTree* now_node = now->node;

    for (int i = 0 ; i < now_node->key_num+1 ; i++) {
        NodeLinkedList *new = calloc(1, sizeof(NodeLinkedList));
        new->node = now_node->child_list[i];
        new->node->traversal_id = traversal_id++;
        (*tail)->next = new;
        *tail = new;
    }

    printf("%d %d %d %d %d\n",
        now_node->traversal_id,
        now_node->key_num,
        now_node->parent ? now_node->parent->traversal_id : -1,
        now_node->parent_child_id,
        now_node->degree,
    );
    printf("%d %d \n",
        now_node->is_root,
        now_node->is_leaf,
    );
    for (int i = 0 ; i < now_node->key_num ; i++) {
        printf("%d ", now_node->key_list[i]);
    }
    printf("\n");
    for (int i = 0 ; i < now_node->key_num+1 ; i++) {
        printf("%d ", now_node->child_list[i]->traversal_id);
    }
    printf("\n");
    printf("=====================");

    now = now->next;
    _traversal(now, tail, traversal_id);
}

void check_valid(BTree* t) {
    
}

void tree_init(BTree** tp, int degree, int is_root) {
    *tp = malloc(sizeof(BTree));
    BTree* t = *tp;
    t->degree = degree;
    t->key_num = 0;
    t->is_root = is_root;
    t->is_leaf = TRUE;
    t->parent_child_id = -1;
    t->traversal_id = -1;
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
