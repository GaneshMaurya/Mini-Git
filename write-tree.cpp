#include <bits/stdc++.h>
#include <filesystem>
#include <fstream>
#include "header.h"
using namespace std;
namespace fs = std::filesystem;

string recursiveWriteTree(fs::path current_path) {
    string tree_content = "";

    for (auto& entry: fs::directory_iterator(current_path)) {
        string path = entry.path();
        fs::path rel_path = fs::relative(path, current_path);
        
        string entry_name = entry.path().filename().string();
        if (!entry_name.empty() && entry_name[0] == '.') {
            continue;
        }

        auto perms = entry.status().permissions();
        unsigned perm_val = static_cast<unsigned>(perms);
        string mode;
        if (entry.is_directory()) {
            mode = "040000";
        } else if ((perms & fs::perms::owner_exec) != fs::perms::none) {
            mode = "100755";
        } else {
            mode = "100644";
        }

        if (entry.is_directory()) {
            string sub_tree_content = recursiveWriteTree(path);
            string header = "tree " + to_string(sub_tree_content.size()) + "$";
            
            string sub_tree_sha = getStringSha(sub_tree_content, header);
            string sub_tree_sha_prefix = sub_tree_sha.substr(0, 2);
            string sub_tree_sha_suffix = sub_tree_sha.substr(2);

            fs::path blob_folder = fs::path(".mygit/objects") / sub_tree_sha_prefix;
            fs::create_directory(blob_folder);


            string binary_file_path = (string)blob_folder + "/" + sub_tree_sha_suffix;
            compressTree(sub_tree_content, binary_file_path, header);

            tree_content += (mode + " tree " + sub_tree_sha + " " + (string)rel_path + "\n");
        }
        else {
            string file_sha = getFileSha(path);
            tree_content += (mode + " blob " + file_sha + " " + (string)rel_path + "\n");
        }
    }

    return tree_content;
}

void writeTree() {
    fs::path current_path = fs::current_path();

    string tree_content = recursiveWriteTree(current_path);
    string header = "tree " + to_string(tree_content.size()) + "$";

    string tree_sha = getStringSha(tree_content, header);
    string tree_sha_prefix = tree_sha.substr(0, 2);
    string tree_sha_suffix = tree_sha.substr(2);

    fs::path blob_folder = fs::path(".mygit/objects") / tree_sha_prefix;
    fs::create_directory(blob_folder);

    string binary_file_path = (string)blob_folder + "/" + tree_sha_suffix;
    compressTree(tree_content, binary_file_path, header);

    cout << tree_sha << "\n";
}