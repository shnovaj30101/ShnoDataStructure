
#ifndef COMMAND
#define COMMAND

#include<iostream>
#include <regex>
#include "nlohmann/json.hpp"
#include "fmt/format.h"

using namespace std;

void create_table(const smatch& m);
void create_table2(const smatch& m);
void use_table(const smatch& m);
void create_index(const smatch& m);
void insert_file(const smatch& m);
void remove_data(const smatch& m);
void query_data(const smatch& m);

#endif

