
#include "myrandom.hpp"
#include "mytree.hpp"
#include "command.hpp"
#include "basic.hpp"
#include <string.h>
#include <iostream>
#include "nlohmann/json.hpp"
#include "fmt/format.h"

using namespace fmt;
using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

void test1() {
    db_system_ptr = new DbSystem();
    db_system_ptr->init();

    parse_command(string("delete table test"));
    parse_command(string("create table test"));
    parse_command(string("use test"));
    parse_command(string("insert ../test_case/test.json"));
    parse_command(string("create index example_id"));
    parse_command(string("create index example_str"));
    parse_command(string("clearb table test"));

    delete db_system_ptr;
}

void test2() {
    db_system_ptr = new DbSystem();
    db_system_ptr->init();

    parse_command(string("delete table test2"));
    parse_command(string("create table test2"));
    parse_command(string("use test2"));
    parse_command(string("create index example_id"));
    parse_command(string("create index example_str"));
    parse_command(string("insert ../test_case/test2.json"));
    parse_command(string("clearb table test2"));

    delete db_system_ptr;
}

int main(int argc, char **argv) {
    test1();
    //test2();
}
