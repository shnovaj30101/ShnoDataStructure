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

add_executable(test1_main main.cpp myrandom.cpp myrandom.hpp mytree.cpp mytree.hpp command.cpp command.hpp)
target_compile_definitions(test1_main PUBLIC -DTEST1)
target_compile_features(test1_main PUBLIC cxx_std_17)
target_link_libraries(test1_main fmt::fmt)
target_link_libraries(test1_main nlohmann_json::nlohmann_json)

add_executable(test2_main main.cpp myrandom.cpp myrandom.hpp mytree.cpp mytree.hpp command.cpp command.hpp)
target_compile_definitions(test2_main PUBLIC -DTEST2)
target_compile_features(test2_main PUBLIC cxx_std_17)
target_link_libraries(test2_main fmt::fmt)
target_link_libraries(test2_main nlohmann_json::nlohmann_json)

