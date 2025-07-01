#include <bits/stdc++.h>
using namespace std;

void init();

void hashObject(vector<string> commands);

void catFile(vector<string> commands);

void writeTree();

void lsTree(vector<string> commands);

string getFileSha(string path);

string getStringSha(string path, string header);

void compress(string input, string output, string metadata);

void compressTree(string content, string output, string metadata);

vector<string> decompress(string file_path);