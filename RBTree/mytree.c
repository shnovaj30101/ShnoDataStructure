
#include "mytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1
#define int_swap(x, y) { int t; t=x; x=y; y=t; }

void insert(RBTree** root, RBTree* t, int data) {
    if (t->data == -1) {
        t->data = data;
        printf("%d\n", t->data);
        _solve_red_collision(root, t);
    } else if (t->data > data) {
        if (!t->left) {
            tree_init(&(t->left), 0);
            t->left->parent = t;
        }
        insert(root, t->left, data);
    } else {
        if (!t->right) {
            tree_init(&(t->right), 0);
            t->right->parent = t;
        }
        insert(root, t->right, data);
    }
};

int delete_node(RBTree** root, RBTree* t, int data) {
    // find the target node
    RBTree* now = t;
    while (now) {
        if (now->data == data) {
            break;
        } else if (now->data < data) {
            now = now->right;
        } else {
            now = now->left;
        }
    }
    if (!now) {
        return FALSE;
    }
    RBTree* target = now;
    RBTree* replace = NULL;

    // find the replace node
    if (now->left) {
        now = now->left;
        while (now->right) {
            now = now->right;
        }
        replace = now;
    } else {
        replace = now;
    }

    // swap the replace node and target node
    int_swap(target->data, replace->data);

    if (replace->black_num == 0) {
        // 1. replace node is red

        if (replace->parent && replace->parent->left == replace) {
            replace->parent->left = NULL;
        } else if (replace->parent && replace->parent->right == replace) {
            replace->parent->right = NULL;
        }
        free(replace);
        return TRUE;

    } else if (replace->right || replace->left) {
        // 2. replace node have child
        RBTree* child;
        if (replace->right) {
            child = replace->right;
        } else {
            child = replace->left;
        }

        if (replace->parent && replace->parent->left == replace) {
            replace->parent->left = child;
        } else if (replace->parent && replace->parent->right == replace) {
            replace->parent->right = child;
        }

        child->black_num = 1;
        child->parent = replace->parent;
        free(replace);
        return TRUE;

    } else {
        if (!replace->parent) {
            // 2.5. replace is only root node
            replace->data = -1;
            return TRUE;
        }
        // 3. BBB
        // 4. BRB
        // 5. BBR
        _delete_node_handle(root, replace);

        if (replace->parent && replace->parent->left == replace) {
            replace->parent->left = NULL;
        } else if (replace->parent && replace->parent->right == replace) {
            replace->parent->right = NULL;
        }

        free(replace);
        return TRUE;
    }
}

void _delete_node_handle(RBTree** root, RBTree* t) {
    RBTree* parent = t->parent;

    if (!parent) {
        return;
    }

    RBTree* sibling = _get_sibling_node(t);

    if (sibling->black_num == 1 && parent->black_num == 1) {
        // 3. BBB
        _BBB_delete_node_handle(root, t);
    } else if (sibling->black_num == 1 && parent->black_num == 0) {
        // 4. BRB
        _BRB_delete_node_handle(root, t);
    } else if (sibling->black_num == 0 && parent->black_num == 1) {
        // 5. BBR
        _BBR_delete_node_handle(root, t);
    }

}

void _BBB_delete_node_handle(RBTree** root, RBTree* t) {
    RBTree* parent = t->parent;
    RBTree* sibling = _get_sibling_node(t);
    RBTree* remote_nephew = _get_remote_nephew_node(t);
    RBTree* near_nephew = _get_near_nephew_node(t);

    if (remote_nephew && remote_nephew->black_num == 0) {
        _rotation(root, sibling, parent);
        remote_nephew->black_num = 1;

    } else if (near_nephew && near_nephew->black_num == 0) {
        _rotation(root, near_nephew, sibling);
        near_nephew->black_num = 1;
        sibling->black_num = 0;
        _rotation(root, near_nephew, parent);
        sibling->black_num = 1;
    } else {
        sibling->black_num = 0;
        _delete_node_handle(root, parent);
    }
}

void _BRB_delete_node_handle(RBTree** root, RBTree* t) {
    RBTree* parent = t->parent;
    RBTree* sibling = _get_sibling_node(t);
    RBTree* remote_nephew = _get_remote_nephew_node(t);
    RBTree* near_nephew = _get_near_nephew_node(t);

    if (remote_nephew && remote_nephew->black_num == 0) {
        _rotation(root, sibling, parent);
        parent->black_num = 1;
        sibling->black_num = 0;
        remote_nephew->black_num = 1;

    } else if (near_nephew && near_nephew->black_num == 0) {
        _rotation(root, near_nephew, sibling);
        near_nephew->black_num = 1;
        sibling->black_num = 0;
        _rotation(root, near_nephew, parent);
        parent->black_num = 1;
        near_nephew->black_num = 0;
        sibling->black_num = 1;
    } else {
        parent->black_num = 1;
        sibling->black_num = 0;
    }
}

void _BBR_delete_node_handle(RBTree** root, RBTree* t) {
    RBTree* parent = t->parent;
    RBTree* sibling = _get_sibling_node(t);
    _rotation(root, sibling, parent);
    parent->black_num = 0;
    sibling->black_num = 1;
    _BRB_delete_node_handle(root, t);
}

RBTree* _get_sibling_node(RBTree* t) {
    if (!t->parent) {
        return NULL;
    }

    if (t->parent->left == t) {
        return t->parent->right;
    } else {
        return t->parent->left;
    }
}

RBTree* _get_remote_nephew_node(RBTree* t) {
    RBTree* sibling = _get_sibling_node(t);
    if (!sibling) {
        return NULL;
    }
    if (sibling->data > t->data) {
        return sibling->right;
    } else {
        return sibling->left;
    }
}

RBTree* _get_near_nephew_node(RBTree* t) {
    RBTree* sibling = _get_sibling_node(t);
    if (!sibling) {
        return NULL;
    }
    if (sibling->data > t->data) {
        return sibling->left;
    } else {
        return sibling->right;
    }
}

void _solve_red_collision(RBTree** root, RBTree* t) {
    if (t->is_root) {
        t->black_num = 1;
        return;
    }
    RBTree* parent = t->parent;
    if (t->black_num || parent->black_num) {
        return;
    }
    RBTree* grand_parent = parent->parent;
    if (t == parent->left && parent == grand_parent->left) {
        if (grand_parent->right && !grand_parent->right->black_num) { // R/R/B\R
            parent->black_num += 1;
            grand_parent->black_num = 0;
            grand_parent->right->black_num += 1;
            _solve_red_collision(root, grand_parent);
        } else { // R/R/B\B
            _rotation(root, parent, grand_parent);
            parent->black_num += 1;
            grand_parent->black_num = 0;
        }
    } else if (t == parent->left && parent == grand_parent->right) {
        if (grand_parent->left && !grand_parent->left->black_num) { // R/R\B/R
            parent->black_num += 1;
            grand_parent->black_num = 0;
            grand_parent->left->black_num += 1;
            _solve_red_collision(root, grand_parent);
        } else { // R/R\B/B
            _rotation(root, t, parent);
            _rotation(root, t, grand_parent);
            t->black_num += 1;
            grand_parent->black_num = 0;
        }
    } else if (t == parent->right && parent == grand_parent->left) {
        if (grand_parent->right && !grand_parent->right->black_num) { // R\R/B\R
            parent->black_num += 1;
            grand_parent->black_num = 0;
            grand_parent->right->black_num += 1;
            _solve_red_collision(root, grand_parent);
        } else { // R\R/B\B
            _rotation(root, t, parent);
            _rotation(root, t, grand_parent);
            t->black_num += 1;
            grand_parent->black_num = 0;
        }
    } else if (t == parent->right && parent == grand_parent->right) {
        if (grand_parent->left && !grand_parent->left->black_num) { // R\R\B/R
            parent->black_num += 1;
            grand_parent->black_num = 0;
            grand_parent->left->black_num += 1;
            _solve_red_collision(root, grand_parent);
        } else { // R\R\B/B
            _rotation(root, parent, grand_parent);
            parent->black_num += 1;
            grand_parent->black_num = 0;
        }
    }
}

void _rotation(RBTree** root, RBTree* up, RBTree* down) {
    /*printf("%d %d\n", up->data, down->data);*/
    if (up == down->left) {
        up->parent = down->parent;
        if (!up->parent) {
        } else if (up->parent->left == down) {
            up->parent->left = up;
        } else {
            up->parent->right = up;
        }
        down->parent = up;
        down->left = up->right;
        if (up->right) {
            up->right->parent = down;
        }
        up->right = down;
        up->is_root = down->is_root;
        if (up->is_root) {
            (*root) = up;
        }
        down->is_root = 0;
    } else if (up == down->right) {
        up->parent = down->parent;
        if (!up->parent) {
        } else if (up->parent->left == down) {
            up->parent->left = up;
        } else {
            up->parent->right = up;
        }
        down->parent = up;
        down->right = up->left;
        if (up->left) {
            up->left->parent = down;
        }
        up->left = down;
        up->is_root = down->is_root;
        if (up->is_root) {
            (*root) = up;
        }
        down->is_root = 0;
    } else {
        printf("lalala\n");
        exit(-1);
    }
};

void traversal(RBTree* t) {
    if (!t) {
        return;
    }
    traversal(t->left);
    printf("%d %d %d %d %d\n",
        t->parent ? t->parent->data : -1,
        t->data,
        t->black_num,
        t->left ? t->left->data : -1,
        t->right ? t->right->data : -1
    );
    traversal(t->right);
};

void tree_init(RBTree** tp, int is_root) {
    *tp = malloc(sizeof(RBTree));
    RBTree* t = *tp;
    t->left = t->right = t->parent = NULL;
    t->is_root = is_root;
    t->data = -1;
    if (is_root) {
        t->black_num = 1;
    }
};

void tree_free(RBTree* t) {
    if (!t) {
        return;
    }
    tree_free(t->left);
    tree_free(t->right);
    free(t);
};
