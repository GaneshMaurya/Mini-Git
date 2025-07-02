#include <bits/stdc++.h>
#include <filesystem>
#include <fstream>
#include "header.h"
using namespace std;
namespace fs = filesystem;

string getDir(string path) {
    size_t pos = path.find('/');
    if (pos == string::npos) {
        return "";
    } 
    return path.substr(0, pos);
}

string removePrefixFromLine(const string& dir, const string& line) {
    istringstream iss(line);
    string mode, type, sha, rel_path;
    iss >> mode >> type >> sha;
    getline(iss, rel_path);
    if (!rel_path.empty() && rel_path[0] == ' ') {
        rel_path = rel_path.substr(1);
    } 

    if (rel_path.find(dir + "/") == 0) {
        rel_path = rel_path.substr(dir.length() + 1);
    }

    return mode + " " + type + " " + sha + " " + rel_path;
}

string buildTree(string content) {
    vector<pair<string, string>> entries;
    
    istringstream iss_stream(content);
    string line;
    while (getline(iss_stream, line)) {
        istringstream iss(line);
        string mode, type, sha, rel_path;
        iss >> mode >> type >> sha;
        getline(iss, rel_path);
        if (!rel_path.empty() && rel_path[0] == ' ') {
            rel_path = rel_path.substr(1);
        }

        entries.push_back({rel_path, line});
    }
    
    sort(entries.begin(), entries.end());
    
    string final_content = "";
    map<string, string> subtree_lines;
    
    for (auto& [rel_path, line]: entries) {
        size_t slash_pos = rel_path.find('/');
        if (slash_pos == string::npos) {
            final_content += (line + "\n");
        }
        else {
            string folder = rel_path.substr(0, slash_pos);
            subtree_lines[folder] += removePrefixFromLine(folder, line) + "\n";
        }
    }

    for (auto& [dir, lines]: subtree_lines) {
        string tree_content = buildTree(lines);
        string header = "tree " + to_string(tree_content.size()) + "$";
        string tree_sha = getStringSha(tree_content, header);
        string tree_sha_prefix = tree_sha.substr(0, 2);
        string tree_sha_suffix = tree_sha.substr(2);

        fs::path blob_folder = fs::path(".mygit/objects") / tree_sha_prefix;
        fs::create_directory(blob_folder);

        string tree_file_path = (string)blob_folder + "/" + tree_sha_suffix;
        compressTree(tree_content, tree_file_path, header);

        final_content += "040000 tree " + tree_sha + " " + dir + "\n";
    }

    return final_content;
}

void commit(vector<string> commands) {
    if (commands.size() == 3 && commands[1] != "-m") {
        cout << "error: unknown switch '" << commands[2] << "'\n";
        return;
    }

    string commit_message = "default commit message";
    if (commands.size() == 3 && commands[1] == "-m") {
        string message = commands[2];
        commit_message = message;
    }

    ifstream index_file(".mygit/index");
    
    if (index_file.is_open()) {
        string index_content = "";

        string line;
        while (getline(index_file, line)) {
            istringstream iss(line);
            string mode, type, sha, rel_path;
            iss >> mode >> type >> sha;
            getline(iss, rel_path);
            if (!rel_path.empty() && rel_path[0] == ' ') {
                rel_path = rel_path.substr(1);
            }

            index_content += (line + "\n");
        }
        
        index_file.close();
        if (index_content.empty()) {
            cout << "nothing added to commit\n";
            return;
        }

        string commit_tree_content = buildTree(index_content);
        string commit_tree_header = "tree " + to_string(commit_tree_content.size()) + "$";
        string commit_tree_sha = getStringSha(commit_tree_content, commit_tree_header);

        string commit_tree_sha_prefix = commit_tree_sha.substr(0, 2);
        string commit_tree_sha_suffix = commit_tree_sha.substr(2);

        fs::path blob_folder = fs::path(".mygit/objects") / commit_tree_sha_prefix;
        fs::create_directory(blob_folder);

        string commit_tree_file_path = (string)blob_folder + "/" + commit_tree_sha_suffix;
        compressTree(commit_tree_content, commit_tree_file_path, commit_tree_header);

        string commit_object_content = "";
        commit_object_content += ("tree " + commit_tree_sha + "\n");
        
        string parent_commit_sha = "";
        ifstream ref(".mygit/refs/heads/main");
        if (ref.is_open()) {
            getline(ref, parent_commit_sha);
            commit_object_content += ("parent " + parent_commit_sha + "\n");
            ref.close();
        }

        auto now = chrono::system_clock::now();
        time_t timestamp = chrono::system_clock::to_time_t(now);

        commit_object_content += ("author Ganesh @gmail.com " + to_string(timestamp) + " +0530\n");
        commit_object_content += ("committer Ganesh @gmail.com " + to_string(timestamp) + " +0530\n\n");
        commit_object_content += ("\t" + commit_message + "\n");

        string commit_object_header = "commit " + to_string(commit_object_content.size()) + "$";
        string commit_object_sha = getStringSha(commit_object_content, commit_object_header);

        string commit_object_sha_prefix = commit_object_sha.substr(0, 2);
        string commit_object_sha_suffix = commit_object_sha.substr(2);

        fs::path commit_folder = fs::path(".mygit/objects") / commit_object_sha_prefix;
        fs::create_directory(commit_folder);

        string commit_object_file_path = (string)commit_folder + "/" + commit_object_sha_suffix;
        compressTree(commit_object_content, commit_object_file_path, commit_object_header);

        ofstream ref_out(".mygit/refs/heads/main");
        ref_out << commit_object_sha << "\n";
        cout << commit_object_sha << "\n";

        ofstream index_file(".mygit/index");
        index_file << "";
    }
    else {
        cout << "fatal: not a git repository (or any of the parent directories): .mygit\n";
        return;
    }
}