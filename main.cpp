#include <bits/stdc++.h>
#include "header.h"
using namespace std;

int main(int argc, char* argv[]) {
    vector<string> commands;
    for (int i = 1; i < argc; i++)
    {
        commands.push_back(argv[i]);
    }

    if (commands[0] == "init") {
        if (argc != 2) {
            cout << "useage: mygit init\n";
            return 1;
        }
        init();
    }
    else if (commands[0] == "hash-object") {
        if (argc != 3 && argc != 4) {
            cout << "usage: mygit hash-object [-w] <file-name>\n";
            return 1;
        }

        hashObject(commands);
    }
    else if (commands[0] == "cat-file") {
        if (argc != 4) {
            cout << "usage: mygit cat-file <flag> <file-sha>\n";
            return 1;
        }

        catFile(commands);
    }
    else if (commands[0] == "write-tree") {
        if (argc != 2) {
            cout << "usage: mygit write-tree\n";
            return 1;
        }

        writeTree();
    }
    else if (commands[0] == "ls-tree") {
        if (argc != 3 && argc != 4) {
            cout << "usage: mygit ls-tree [--name-only] <file-sha>\n";
            return 1;
        }

        lsTree(commands);
    }
    else if (commands[0] == "add") {
        add(commands);
    }
    else if (commands[0] == "commit") {
        if (argc != 2 && argc != 4) {
            cout << "Enter valid command and arguments\n";
            return 1;
        }

    }
    else if (commands[0] == "log") {
        if (argc != 2) {
            cout << "Enter valid command and arguments\n";
            return 1;
        }

    }
    else if (commands[0] == "checkout") {
        if (argc != 3) {
            cout << "Enter valid command and arguments\n";
            return 1;
        }

    }
    else {
        cout << "git: '" << commands[0] << "' is not a git command.\n";
        return 1;
    }

    return 0;
}