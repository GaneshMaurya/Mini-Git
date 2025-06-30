#include <bits/stdc++.h>
using namespace std;

void init();

void hashObject(vector<string> commands);

void catFile(vector<string> commands);

string getFileSha(string path);

void compress(string input, string output, string metadata);

vector<string> decompress(string file_path);