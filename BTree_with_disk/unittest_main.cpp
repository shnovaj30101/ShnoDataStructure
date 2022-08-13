
#include <gtest/gtest.h>
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

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
