#include <bits/stdc++.h>
#include <filesystem>
#include <fstream>
#include "header.h"
using namespace std;
namespace fs = filesystem;

void log() {
    ifstream main_file(".mygit/refs/heads/main");
    string commit_sha = "";
    
    if (!main_file.is_open()) {
        cout << "fatal: not a git repository (or any of the parent directories): .mygit\n";
        return;
    }

    getline(main_file, commit_sha);
    if (commit_sha.empty()) {
        cout << "fatal: your current branch 'main' does not have any commits yet\n";
        return;
    }

    string log_content = "";
    while (!commit_sha.empty()) {
        string commit_sha_prefix = commit_sha.substr(0, 2);
        string commit_sha_suffix = commit_sha.substr(2);
        string file_path = ".mygit/objects/" + commit_sha_prefix + "/" + commit_sha_suffix;

        ifstream file(file_path, ios::binary);
        if (!file.is_open()) {
            cout << "fatal: Not a valid object name " << commit_sha << "\n";
            return;
        }
        file.close();

        vector<string> results = decompress(file_path);
        if (results.size() < 3) {
            return;
        }

        string commit_content = results[2];

        log_content += ("commit " + commit_sha + "\n");

        istringstream iss(commit_content);
        string line, author_line, message = "";
        string parent_commit_sha = "";
        string author_name, email, timestamp_str, timezone;
        time_t timestamp = 0;

        while (getline(iss, line)) {
            if (line.rfind("parent ", 0) == 0) {
                parent_commit_sha = line.substr(7);
            }
            else if (line.rfind("author ", 0) == 0) {
                istringstream auth(line);
                string word;
                auth >> word;
                auth >> author_name >> email >> timestamp_str >> timezone;

                timestamp = stol(timestamp_str);
                author_line = "Author: " + author_name + " <" + email + ">";
            }
            else if (line.empty()) {
                while (getline(iss, line)) {
                    message += line + "\n";
                }
                break;
            }
        }

        char date_str[100];
        tm* timeinfo = localtime(&timestamp);
        strftime(date_str, sizeof(date_str), "%a %b %e %T %Y", timeinfo);

        log_content += (author_line + "\n");
        log_content += ("Date: " + (string)date_str + " " + timezone + "\n\n");
        log_content += message + "\n";
        
        commit_sha = parent_commit_sha;
    }

    cout << log_content << "\n";
}