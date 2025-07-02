#include <bits/stdc++.h>
#include <fstream>
#include <filesystem>
#include "header.h"
using namespace std;
namespace fs = filesystem;

void init() {
    // create .mygit folder
    fs::path git_folder = ".mygit";
    bool is_fresh = fs::create_directory(git_folder);

    // Always ensure these subfolders exist
    fs::create_directories(git_folder / "objects");
    fs::create_directories(git_folder / "refs" / "heads");

    // Create HEAD file only if it doesn't exist
    fs::path head_file = git_folder / "HEAD";
    if (!fs::exists(head_file)) {
        ofstream(head_file) << "refs: refs/heads/main\n";
    }
    else {
        ofstream(head_file) << "refs: refs/heads/main\n";
    }

    // Create index file only if it doesn't exist
    fs::path index_file = git_folder / "index";
    if (!fs::exists(index_file)) {
        ofstream(index_file) << "";
    }
    else {
        ofstream(index_file) << "";
    }

    // Create main file only if it doesn't exist
    fs::path main_file = git_folder / "refs" / "heads" / "main";
    if (!fs::exists(main_file)) {
        ofstream(main_file) << "";
    }
    else {
        ofstream(main_file) << "";
    }

    if (is_fresh) {
        cout << "Initialized empty Git structure in .mygit/\n";
    } else {
        cout << "Reinitialized existing Git repository in .mygit/\n";
    }
}