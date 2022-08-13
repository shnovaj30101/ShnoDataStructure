cmake_minimum_required(VERSION 3.10)
project(ShnoDatabase)

find_package(fmt)
find_package(nlohmann_json)

add_executable(main main.cpp myrandom.cpp myrandom.hpp mytree.cpp mytree.hpp command.cpp command.hpp)
target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main fmt::fmt)
target_link_libraries(main nlohmann_json::nlohmann_json)

add_executable(read_json read_json_test.cpp)
target_compile_features(read_json PUBLIC cxx_std_17)
target_link_libraries(read_json fmt::fmt)
target_link_libraries(read_json nlohmann_json::nlohmann_json)

add_executable(unittest_main unittest_main.cpp myrandom.cpp myrandom.hpp mytree.cpp mytree.hpp command.cpp command.hpp)
target_compile_features(unittest_main PUBLIC cxx_std_17)
target_link_libraries(unittest_main gtest)
target_link_libraries(unittest_main pthread)
target_link_libraries(unittest_main fmt::fmt)
target_link_libraries(unittest_main nlohmann_json::nlohmann_json)

add_executable(integratest_main integratest_main.cpp myrandom.cpp myrandom.hpp mytree.cpp mytree.hpp command.cpp command.hpp)
target_compile_features(integratest_main PUBLIC cxx_std_17)
target_link_libraries(integratest_main fmt::fmt)
target_link_libraries(integratest_main nlohmann_json::nlohmann_json)

