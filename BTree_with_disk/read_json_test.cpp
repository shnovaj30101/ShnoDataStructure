
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include "fmt/format.h"

using namespace std;
using namespace fmt;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    ifstream data_file(argv[1]);

    string line;
    while (getline(data_file, line)) {
        json line_json = json::parse(line);
        cout << line_json["example_id"].get<int>() << endl;
        cout << line_json["example_str"] << endl;
        string s = "example_str2";
        cout << line_json[s].get<string>() << endl;

        for (auto &item : line_json.items()) {
            cout << item.key() << " " << item.value() << endl;
        }
    }
}
