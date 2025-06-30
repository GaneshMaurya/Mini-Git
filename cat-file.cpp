#include <bits/stdc++.h>
#include <fstream>
#include <filesystem>
#include "header.h"
using namespace std;

void catFile(vector<string> commands) {
    if (commands[1] != "-p" && commands[1] != "-s" && commands[1] != "-t") {
        cout << "error: unknown switch '" << commands[1] << "'\n";
        return;
    }

    string flag = commands[1];
    string file_sha = commands[2];
    string sha_prefix = file_sha.substr(0, 2);
    string sha_suffix = file_sha.substr(2);

    string file_path = ".mygit/objects/" + sha_prefix + "/" + sha_suffix;
    ifstream file(file_path, ios::binary);
    if (!file.is_open()) {
        cout << "fatal: Not a valid object name " << file_sha << "\n";
        return;
    }

    file.close();

    vector<string> results = decompress(file_path);
    if (results.size() < 3) {
        return;
    }

    if (flag == "-t") {
        cout << results[0] << "\n";
    }
    else if (flag == "-s") {
        cout << results[1] << "\n";
    }
    else if (flag == "-p") {
        cout << results[2] << "\n";
    }
}