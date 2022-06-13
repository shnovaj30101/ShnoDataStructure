
#ifndef COMMAND
#define COMMAND

#include<iostream>
#include <regex>
#include "nlohmann/json.hpp"
#include "fmt/format.h"

using namespace std;

void create_table(smatch *mp);
void create_table2(smatch *mp);
void use_table(smatch *mp);
void insert_file(smatch *mp);
void remove_data(smatch *mp);
void query_data(smatch *mp);

#endif

