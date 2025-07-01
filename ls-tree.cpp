#include <bits/stdc++.h>
#include "header.h"
using namespace std;

void lsTree(vector<string> commands) {
    if (commands.size() == 3 && commands[1] != "--name-only") {
        cout << "error: unknown switch '" << commands[1] << "'\n";
        return;
    }

    if (commands[1] == "--name-only") {
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

        vector<string> result = decompress(file_path);
        string content = result[2];

        istringstream iss(content);
        string line;
        while (getline(iss, line)) {
            if (line.empty()) {
                continue;
            }

            istringstream line_ss(line);
            string mode, type, sha, name;
            line_ss >> mode >> type >> sha >> name;
            
            if (type == "tree") {
                cout << name << "/\n";
            }
            else {
                cout << name << "\n";
            }
        }
    }
    else {
        string file_sha = commands[1];
        string sha_prefix = file_sha.substr(0, 2);
        string sha_suffix = file_sha.substr(2);

        string file_path = ".mygit/objects/" + sha_prefix + "/" + sha_suffix;
        
        ifstream file(file_path, ios::binary);
        if (!file.is_open()) {
            cout << "fatal: Not a valid object name " << file_sha << "\n";
            return;
        }
        file.close();

        vector<string> result = decompress(file_path);
        cout << result[2] << "\n";
    }
}