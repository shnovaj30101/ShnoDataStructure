
#include "myrandom.hpp"
#include "mytree.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <regex>
#include <iostream>
#include "nlohmann/json.hpp"
#include "fmt/format.h"

using json = nlohmann::json;
using namespace fmt;
using namespace std;

#define DEFAULT_FILE_SIZE 4096
#define DEFAULT_FIELD_MAX 128
#define DEFAULT_VALUE_MAX 128

void parse_command(string command) {
    regex create_table_regex();
    regex use_table_regex();
    regex clear_table_regex();
    regex insert_file_regex();
    regex remove_data_regex();
    regex query_data_regex();

    if () { // 創建一個 table
    } else if () { // 進入一個 table
    } else if () { // 清除一個 table
    } else if () { // 插入一個檔案內部的 json 資料
    } else if () { // 刪除一筆資料
    } else if () { // 查詢一筆資料
    }
}

int main(int argc, char* argv[]) {
    string command;

    cout << "ShnoDatabase$ ";
    while (getline(cin, command)) {
        parse_command(command);
        cout << "ShnoDatabase$ ";
    }

    cout << endl;
}

/*#define MAX_LENGTH 10000*/
/*#define DEFAULT_LENGTH 16*/
/*#define DEFAULT_RANGE 1000*/
/*#define DEFAULT_DEGREE 2*/
/*#define int_swap(x, y) { int t; t=x; x=y; y=t; }*/


/*int main(int argc, char* argv[]) {*/
    /*int* random_list;*/
    /*int length = DEFAULT_LENGTH;*/
    /*int range = DEFAULT_RANGE;*/
    /*int degree = DEFAULT_DEGREE;*/

/*#ifdef DEBUG1*/
    /*length = 10;*/
/*#elif defined DEBUG2*/
    /*length = 22;*/
/*#elif defined DEBUG3*/
    /*for (int i = 1; i < argc ; i++) {*/
        /*if (strcmp(argv[i], "-n") == 0) {*/
            /*length = atoi(argv[++i]);*/
        /*} else if (strcmp(argv[i], "-d") == 0) {*/
            /*degree = atoi(argv[++i]);*/
        /*}*/
    /*}*/
/*#elif defined DEBUG4*/
    /*length = 20;*/
/*#elif defined DEBUG5*/
    /*length = 7;*/
/*#else*/
    /*for (int i = 1; i < argc ; i++) {*/
        /*if (strcmp(argv[i], "-n") == 0) {*/
            /*length = atoi(argv[++i]);*/
        /*} else if (strcmp(argv[i], "-r") == 0) {*/
            /*range = atoi(argv[++i]);*/
        /*} else if (strcmp(argv[i], "-d") == 0) {*/
            /*degree = atoi(argv[++i]);*/
        /*}*/
    /*}*/
/*#endif*/

/*#ifdef DEBUG1*/
    /*int tmp[10] = {1,5,2,8,9,7,6,3,4,10};*/
    /*int true_length = length;*/
    /*random_list = (int*)malloc(length * sizeof(int));*/
    /*for (int i = 0 ; i < length ; i++) {*/
        /*random_list[i] = tmp[i];*/
    /*}*/
/*#elif defined DEBUG2*/
    /*int tmp[22] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};*/
    /*int true_length = length;*/
    /*random_list = (int*)malloc(length * sizeof(int));*/
    /*for (int i = 0 ; i < length ; i++) {*/
        /*random_list[i] = tmp[i];*/
    /*}*/
/*#elif defined DEBUG3*/
    /*srand( time(NULL) );*/
    /*int true_length = length;*/
    /*random_list = (int*)malloc(length * sizeof(int));*/
    /*for (int i = 0 ; i < length ; i++) {*/
        /*random_list[i] = i+1;*/
    /*}*/
    /*for (int i = 0 ; i < length ; i++) {*/
        /*int tmp_rand = rand() % (length-i);*/
        /*int_swap(random_list[length-i-1], random_list[tmp_rand]);*/
    /*}*/

/*#elif defined DEBUG4*/
    /*int tmp[20] = {760, 912, 807, 434, 495, 257, 990, 337, 521, 674, 613, 673, 378, 456, 897, 865, 162, 304, 493, 142};*/
    /*int true_length = length;*/
    /*random_list = (int*)malloc(length * sizeof(int));*/
    /*for (int i = 0 ; i < length ; i++) {*/
        /*random_list[i] = tmp[i];*/
    /*}*/
/*#elif defined DEBUG5*/
    /*int tmp[7] = {1,2,3,4,5,6,7};*/
    /*int true_length = length;*/
    /*random_list = (int*)malloc(length * sizeof(int));*/
    /*for (int i = 0 ; i < length ; i++) {*/
        /*random_list[i] = tmp[i];*/
    /*}*/

/*#else*/
    /*int true_length = random_list_gen(&random_list, (length > MAX_LENGTH) ? MAX_LENGTH : length, 0, range);*/

/*#ifdef CHECK*/
    /*FILE * fp;*/

    /*fp = fopen ("test.log", "w");*/
    /*for (int i = 0; i < true_length; i++) {*/
        /*fprintf(fp, "%d ", random_list[i]);*/
    /*}*/
    /*fprintf(fp, "\n");*/
    /*fclose(fp);*/
/*#endif*/

/*#endif*/
    /*BTree* t;*/
    /*tree_init(&t, degree, 1);*/

    /*for (int i = 0; i < true_length; i++) {*/
        /*insert(&t, t, random_list[i]);*/

/*#ifdef CHECK*/
        /*check_valid(t, 1);*/
/*#endif*/

    /*}*/

/*#ifdef DEBUG1*/
    /*delete_node(&t, t, 8);*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 5);*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 4);*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 2);*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 3);*/
    /*check_valid(t, 1);*/
/*#elif defined DEBUG2*/
    /*delete_node(&t, t, 5);*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 12);*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 3);*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 18);*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 2);*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 20);*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 14);*/
    /*check_valid(t, 1);*/
    /*delete_node(&t, t, 8);*/
    /*check_valid(t, 1);*/
/*#elif defined DEBUG3*/
    /*srand( time(NULL) );*/

    /*int* delete_random_list;*/
    /*delete_random_list = (int*)malloc(length * sizeof(int));*/
    /*for (int i = 0 ; i < length ; i++) {*/
        /*delete_random_list[i] = i+1;*/
    /*}*/
    /*for (int i = 0 ; i < length ; i++) {*/
        /*int tmp_rand = rand() % (length-i);*/
        /*int_swap(delete_random_list[length-i-1], delete_random_list[tmp_rand]);*/
    /*}*/
    /*[>for (int i = 0 ; i < length ; i++) {<]*/
        /*[>printf("%d ", delete_random_list[i]);<]*/
    /*[>}<]*/
    /*[>printf("\n");<]*/

    /*[>traversal(t);<]*/
    /*for (int i = 0 ; i < length ; i++) {*/
        /*[>printf("%d\n", delete_random_list[i]);<]*/
        /*delete_node(&t, t, delete_random_list[i]);*/
        /*check_valid(t, 1);*/
    /*}*/
    /*free(delete_random_list);*/
/*#elif defined DEBUG5*/
    /*delete_node(&t, t, 6);*/
    /*check_valid(t, 1);*/
/*#endif*/

    /*free(random_list);*/
    /*tree_free(t, 1);*/
/*}*/


