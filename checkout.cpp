#include <bits/stdc++.h>
#include <fstream>
#include <filesystem>
#include "header.h"
using namespace std;
namespace fs = filesystem;

void createFile(string dir, string path, string sha) {
    string sha_prefix = sha.substr(0, 2);
    string sha_suffix = sha.substr(2);

    string object_path = ".mygit/objects/" + sha_prefix + "/" + sha_suffix;
    vector<string> result = decompress(object_path);
    if (result.size() != 3) {
        return;
    }

    string content = result[2];
    fs::path output_path = fs::path(dir) / path;
    fs::create_directories(output_path.parent_path());
    
    ofstream out(output_path, ios::binary | ios::trunc);
    if (!out.is_open()) {
        return;
    }
    out.write(content.data(), content.size());
    out.close();
}

void createTree(string dir, string sha) {
    string sha_prefix = sha.substr(0, 2);
    string sha_suffix = sha.substr(2);

    string object_path = ".mygit/objects/" + sha_prefix + "/" + sha_suffix;
    vector<string> result = decompress(object_path);
    if (result.size() != 3) {
        return;
    }

    string tree_content = result[2];
    istringstream commit_stream(tree_content);
    string commit_line;

    while (getline(commit_stream, commit_line)) {
        istringstream line_stream(commit_line);
        string mode, type, sha, path;
        line_stream >> mode >> type >> sha;
        getline(line_stream, path);

        if (!path.empty() && path[0] == ' ') {
            path = path.substr(1);
        }

        if (type == "blob") {
            createFile(dir, path, sha);
        }
        else {
            fs::path new_dir = fs::path(dir) / path;
            fs::create_directories(new_dir);
            createTree(new_dir.string(), sha);
        }
    }
}

void checkout(vector<string> commands) {
    string commit_sha = commands[1];
    string commit_sha_prefix = commit_sha.substr(0, 2);
    string commit_sha_suffix = commit_sha.substr(2);

    string commit_path = ".mygit/objects/" + commit_sha_prefix + "/" + commit_sha_suffix;
        
    ifstream commit_file(commit_path, ios::binary);
    if (!commit_file.is_open()) {
        cout << "fatal: Not a valid object name " << commit_sha << "\n";
        return;
    }
    commit_file.close();
    
    vector<string> commit_results = decompress(commit_path);
    if (commit_results.size() < 3) {
        return;
    }
    
    string commit_content = commit_results[2];
    
    string tree_sha = "";
    istringstream iss(commit_content);
    string line;
    if (getline(iss, line)) {
        if (line.rfind("tree ", 0) == 0) {
            tree_sha = line.substr(5);
        }
    }

    if (tree_sha.empty()) {
        cout << "fatal: commit " << commit_sha << " does not contain a tree\n";
        return;
    }

    string dir = commit_sha.substr(0, 7);
    fs::create_directories(dir);
    createTree(dir, tree_sha);
}