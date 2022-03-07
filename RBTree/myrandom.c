
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int random_list_gen(int** random_list_ptr, int length, int rand_min, int rand_max) {

    srand( time(NULL) );
    *random_list_ptr = (int*)malloc(length * sizeof(int));

    for (int i = 0; i < length ; i++) {
        int tmp_rand = rand();
        (*random_list_ptr)[i] = tmp_rand % (rand_max - rand_min) + rand_min;
    }

    return length;
}
