#include <bits/stdc++.h>
using namespace std;

void init();

void hashObject(vector<string> commands);

void catFile(vector<string> commands);

void writeTree();

void lsTree(vector<string> commands);

void add(vector<string> commands);

void commit(vector<string> commands);

void log();

void checkout(vector<string> commands);

string getFileSha(string path);

string getStringSha(string content, string header);

void compress(string input, string output, string metadata);

void compressTree(string content, string output, string metadata);

vector<string> decompress(string file_path);