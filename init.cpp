#include <bits/stdc++.h>
#include <fstream>
#include <filesystem>
using namespace std;
namespace fs = filesystem;

void init() {
    // create .mygit folder
    fs::path git_folder = ".mygit";
    fs::create_directory(git_folder);

    // create HEAD file
    ofstream(git_folder / "HEAD") << "refs: refs/heads/main\n";

    // create objects folder
    fs::create_directory(git_folder / "objects");

    // create refs folder
    fs::create_directory(git_folder / "refs");
    fs::create_directory(git_folder / "refs" / "heads");

    cout << "Initialized empty Git structure in .mygit/\n";
}