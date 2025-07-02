#include <bits/stdc++.h>
#include <filesystem>
#include <fstream>
#include "header.h"
using namespace std;
namespace fs = filesystem;

string recursiveUpdate(string root_path, string current_path) {
    string index_content = "";

    for (auto& entry: fs::directory_iterator(current_path)) {
        string path = entry.path();
        fs::path rel_path = fs::relative(path, root_path);
        
        string entry_name = entry.path().filename().string();
        if (!entry_name.empty() && entry_name[0] == '.') {
            continue;
        }

        auto perms = entry.status().permissions();
        unsigned perm_val = static_cast<unsigned>(perms);
        string mode;
        if (entry.is_directory()) {
            index_content += recursiveUpdate(root_path, path);
            continue;
        } 
        else if ((perms & fs::perms::owner_exec) != fs::perms::none) {
            mode = "100755";
        } 
        else {
            mode = "100644";
        }

        string file_sha = getFileSha(path);
        string file_sha_prefix = file_sha.substr(0, 2);
        string file_sha_suffix = file_sha.substr(2);

        fs::path blob_folder = fs::path(".mygit/objects") / file_sha_prefix;
        fs::create_directory(blob_folder);

        string header = "blob " + to_string(fs::file_size(path)) + "$";
        string binary_file_path = (string)blob_folder + "/" + file_sha_suffix;
        compress(path, binary_file_path, header);
        
        index_content += (mode + " blob " + file_sha + " " + (string)rel_path + "\n");
    }

    return index_content;
}

void checkIndex() {
    ifstream index_file(".mygit/index");

    if (index_file.is_open()) {
        map<string, string> map;

        string line;
        while (getline(index_file, line)) {
            istringstream iss(line);
            string mode, type, sha, rel_path;
            iss >> mode >> type >> sha;
            getline(iss, rel_path);
            if (!rel_path.empty() && rel_path[0] == ' ') {
                rel_path = rel_path.substr(1);
            }

            fs::path file_path = rel_path;
            if (fs::exists(file_path)) {
                map[rel_path] = line;
            }
        }
        index_file.close();
        
        ofstream out_index_file(".mygit/index", ios::trunc);
        if (!out_index_file.is_open()) {
            cout << "fatal: not a git repository (or any of the parent directories): .mygit\n";
            return;
        }

        for (auto& [sha, line]: map) {
            out_index_file << line << "\n";
        }
        out_index_file.close();
    }
    else {
        cout << "fatal: not a git repository (or any of the parent directories): .mygit\n";
        return;
    }
}

void checkIndexFiles(unordered_set<string> files) {
    ifstream index_file(".mygit/index");

    if (index_file.is_open()) { 
        map<string, string> map;

        string line;
        while (getline(index_file, line)) {
            istringstream iss(line);
            string mode, type, sha, rel_path;
            iss >> mode >> type >> sha;
            getline(iss, rel_path);
            if (!rel_path.empty() && rel_path[0] == ' ') {
                rel_path = rel_path.substr(1);
            }

            if (files.find(rel_path) == files.end()) {
                map[rel_path] = line;
                continue;
            }

            fs::path file_path = rel_path;
            if (fs::exists(file_path)) {
                map[rel_path] = line;
            }
        }
        index_file.close();
        
        ofstream out_index_file(".mygit/index", ios::trunc);
        if (!out_index_file.is_open()) {
            cout << "fatal: not a git repository (or any of the parent directories): .mygit\n";
            return;
        }

        for (auto& [sha, line]: map) {
            out_index_file << line << "\n";
        }
        out_index_file.close();
    }
    else {
        cout << "fatal: not a git repository (or any of the parent directories): .mygit\n";
        return;
    }
}

void add(vector<string> commands) {
    if (commands[1] == ".") {
        fs::path current_path = fs::current_path();
        string index_content = recursiveUpdate(current_path, current_path);
        
        ofstream out_index_file(".mygit/index", ios::app);
        if (!out_index_file.is_open()) {
            cout << "fatal: not a git repository (or any of the parent directories): .mygit\n";
            return;
        }

        out_index_file << index_content;
        out_index_file.close();

        checkIndex();
    }
    else {
        int n = commands.size();
        for (int i=1; i<n; i++) {
            fs::path file_path = commands[i];
            if (fs::exists(file_path) == false) {
                cout << "fatal: pathspec '" << file_path << "' did not match any files\n";
                return;
            }
        }

        fs::path root_path = fs::current_path();
        string index_content = "";
        for (int i=1; i<n; i++) {
            string path = commands[i];
            if (fs::is_directory(path)) {
                index_content += recursiveUpdate(root_path, path);
                continue;
            }

            string file_sha = getFileSha(path);
            string file_sha_prefix = file_sha.substr(0, 2);
            string file_sha_suffix = file_sha.substr(2);
            
            fs::path blob_folder = fs::path(".mygit/objects") / file_sha_prefix;
            fs::create_directory(blob_folder);
            
            fs::path file_path = commands[i];
            string header = "blob " + to_string(fs::file_size(file_path)) + "$";
            string binary_file_path = (string)blob_folder + "/" + file_sha_suffix;
            compress(path, binary_file_path, header);
            
            auto perms = fs::status(file_path).permissions();
            unsigned perm_val = static_cast<unsigned>(perms);
            string mode;
            if ((perms & fs::perms::owner_exec) != fs::perms::none) {
                mode = "100755";
            } 
            else {
                mode = "100644";
            }

            fs::path rel_path = fs::relative(file_path, root_path);
            index_content += (mode + " blob " + file_sha + " " + rel_path.string() + "\n");
        }

        ofstream out_index_file(".mygit/index", ios::app);
        if (!out_index_file.is_open()) {
            cout << "fatal: not a git repository (or any of the parent directories): .mygit\n";
            return;
        }

        out_index_file << index_content;
        out_index_file.close();

        unordered_set<string> set(commands.begin()+1, commands.end());
        checkIndexFiles(set);
    }
}