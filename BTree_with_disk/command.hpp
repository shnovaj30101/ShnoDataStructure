
#ifndef COMMAND
#define COMMAND

#include<iostream>
#include <regex>
#include <map>
#include "nlohmann/json.hpp"
#include "fmt/format.h"

using namespace std;

class CmdQuit:public exception {
    public:
        const char * what() const throw() {
            return "Command line quit";
        }
};

extern map<string, regex> cmd_regex_map;

void create_table_default(const smatch& m);
void create_table(const smatch& m);
void use_table(const smatch& m);
void clearb_table(const smatch& m);
void delete_table(const smatch& m);
void create_index(const smatch& m);
void insert_file(const smatch& m);
void remove_data(const smatch& m);
void query_data(const smatch& m);
void parse_command(const string& command);

#endif

