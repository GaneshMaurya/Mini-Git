#include <bits/stdc++.h>
#include <fstream>
#include <filesystem>
#include "header.h"
using namespace std;
namespace fs = filesystem;

void hashObject(vector<string> commands) {
    if (commands.size() == 3 && commands[1] != "-w") {
        cout << "error: unknown switch '" << commands[1] << "'\n";
        return;
    }
    
    if (commands[1] == "-w") {
        string file_path = commands[2];
        string file_sha = getFileSha(file_path);

        string sha_prefix = file_sha.substr(0, 2);
        string sha_suffix = file_sha.substr(2);

        fs::path blob_folder = fs::path(".mygit/objects") / sha_prefix;
        fs::create_directory(blob_folder);
        
        ifstream file(file_path, ios::binary);
        if (!file.is_open()) {
            cout << "fatal: could not open '" << file_path << "' for reading: No such file or directory\n";
            return;
        }

        file.seekg(0, ios::end);
        size_t file_size = file.tellg();
        file.seekg(0, ios::beg);

        string header = "blob " + to_string(file_size) + "$";
        file.close();

        string binary_file_path = (string)blob_folder + "/" + sha_suffix;
        compress(file_path, binary_file_path, header);
        cout << file_sha << "\n";
    }
    else {
        string file_path = commands[1];
        string file_sha = getFileSha(file_path);
        cout << file_sha << "\n";
    }
}