
#include "myrandom.h"
#include "mytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 10000
#define DEFAULT_LENGTH 16
#define DEFAULT_RANGE 1000
#define DEFAULT_DEGREE 2

int main(int argc, char* argv[]) {
    int* random_list;
    int length = DEFAULT_LENGTH;
    int range = DEFAULT_RANGE;
    int degree = DEFAULT_DEGREE;

#ifdef DEBUG1
    length = 10;
#elif defined DEBUG2
    length = 22;
#elif defined DEBUG3
    length = 256;
#elif defined DEBUG4
    length = 20;
#else
    for (int i = 1; i < argc ; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            length = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-r") == 0) {
            range = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-d") == 0) {
            degree = atoi(argv[++i]);
        }
    }
#endif

#ifdef DEBUG1
    int tmp[10] = {1,5,2,8,9,7,6,3,4,10};
    int true_length = length;
    random_list = (int*)malloc(length * sizeof(int));
    for (int i = 0 ; i < length ; i++) {
        random_list[i] = tmp[i];
    }
#elif defined DEBUG2
    int tmp[22] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
    int true_length = length;
    random_list = (int*)malloc(length * sizeof(int));
    for (int i = 0 ; i < length ; i++) {
        random_list[i] = tmp[i];
    }
#elif defined DEBUG3
    int true_length = length;
    random_list = (int*)malloc(length * sizeof(int));
    for (int i = 0 ; i < length ; i++) {
        random_list[i] = i+1;
    }

#elif defined DEBUG4
    int tmp[20] = {760, 912, 807, 434, 495, 257, 990, 337, 521, 674, 613, 673, 378, 456, 897, 865, 162, 304, 493, 142};
    int true_length = length;
    random_list = (int*)malloc(length * sizeof(int));
    for (int i = 0 ; i < length ; i++) {
        random_list[i] = tmp[i];
    }

#else
    int true_length = random_list_gen(&random_list, (length > MAX_LENGTH) ? MAX_LENGTH : length, 0, range);

#ifdef CHECK
    FILE * fp;

    fp = fopen ("test.log", "w");
    for (int i = 0; i < true_length; i++) {
        fprintf(fp, "%d ", random_list[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);
#endif

#endif
    BTree* t;
    tree_init(&t, degree, 1);

    for (int i = 0; i < true_length; i++) {
        insert(&t, t, random_list[i]);

#ifdef CHECK
        check_valid(t, 1);
#endif

    }

#ifdef DEBUG1
    /*delete_node(&t, t, 8); // delete cond 4*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 5); // delete cond 1*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 4); // delete cond 1*/
    /*check_valid(t, 1);*/
#elif defined DEBUG2
    /*delete_node(&t, t, 5); // delete cond 4*/
    /*check_valid(t, 1);*/
#endif

    free(random_list);
    tree_free(t, 1);
}


