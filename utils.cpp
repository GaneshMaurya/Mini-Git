#include <bits/stdc++.h>
#include <fstream>
#include <filesystem>
#include <openssl/sha.h>
#include <zlib.h>
#include "header.h"
using namespace std;
namespace fs = filesystem;

int BUFFER_SIZE = 8*1024;

string getFileSha(string file_path) {
    ifstream file(file_path, ios::binary);
    if (!file.is_open()) {
        return "fatal: could not open '" + file_path + "' for reading: No such file or directory\n";
    }

    file.seekg(0, ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, ios::beg);
    
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX sha_context;
    SHA1_Init(&sha_context);

    string header = "blob " + to_string(file_size) + "\0";
    SHA1_Update(&sha_context, header.c_str(), header.size());

    char buffer[BUFFER_SIZE];
    while (file) {
        file.read(buffer, BUFFER_SIZE);
        size_t bytes_read = file.gcount();
        
        if (bytes_read > 0) {
            SHA1_Update(&sha_context, buffer, bytes_read);
        }
    }

    SHA1_Final(hash, &sha_context);
    file.close();

    stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }

    return ss.str();
}

void compress(string input, string output, string metadata)
{
    ifstream ipFile(input, ios::binary);
    if (!ipFile.is_open()) {
        cout << "Error opening input file.\n";
        return;
    }

    ofstream opFile(output, ios::binary | ios::trunc);
    if (!opFile.is_open()) {
        cout << "Error opening output file.\n";
        return;
    }

    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;

    if (deflateInit(&zs, Z_DEFAULT_COMPRESSION) != Z_OK) {
        cout << "Error in initialization of compression\n";
        return;
    }

    char inputBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];

    // Compress metadata first
    zs.avail_in = static_cast<uInt>(metadata.size());
    zs.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(metadata.c_str()));

    do {
        zs.avail_out = sizeof(outputBuffer);
        zs.next_out = reinterpret_cast<Bytef *>(outputBuffer);
        deflate(&zs, Z_NO_FLUSH);
        int bytesToWrite = sizeof(outputBuffer) - zs.avail_out;
        opFile.write(outputBuffer, bytesToWrite);
    } while (zs.avail_out == 0);

    // Compress file contents
    while (ipFile) {
        ipFile.read(inputBuffer, sizeof(inputBuffer));
        streamsize bytesRead = ipFile.gcount();
        if (bytesRead <= 0) break;

        zs.avail_in = static_cast<uInt>(bytesRead);
        zs.next_in = reinterpret_cast<Bytef *>(inputBuffer);

        do {
            zs.avail_out = sizeof(outputBuffer);
            zs.next_out = reinterpret_cast<Bytef *>(outputBuffer);

            if (deflate(&zs, Z_NO_FLUSH) == Z_STREAM_ERROR) {
                cout << "Error during compression\n";
                deflateEnd(&zs);
                return;
            }

            int bytesToWrite = sizeof(outputBuffer) - zs.avail_out;
            opFile.write(outputBuffer, bytesToWrite);
        } while (zs.avail_out == 0);
    }

    // Finish compression
    int deflate_ret;
    do {
        zs.avail_out = sizeof(outputBuffer);
        zs.next_out = reinterpret_cast<Bytef *>(outputBuffer);

        deflate_ret = deflate(&zs, Z_FINISH);
        if (deflate_ret == Z_STREAM_ERROR) {
            cout << "Error during finishing compression\n";
            deflateEnd(&zs);
            return;
        }

        int bytesToWrite = sizeof(outputBuffer) - zs.avail_out;
        opFile.write(outputBuffer, bytesToWrite);
    } while (zs.avail_out == 0);

    deflateEnd(&zs);
    ipFile.close();
    opFile.close();
}

vector<string> decompress(string file_path) {
    ifstream ipFile(file_path, ios::binary);
    if (!ipFile.is_open()) {
        cout << "Error opening compressed file.\n";
        return {};
    }

    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.avail_in = 0;
    zs.next_in = Z_NULL;

    if (inflateInit(&zs) != Z_OK) {
        cout << "Error initializing decompression\n";
        return {};
    }

    char inBuffer[BUFFER_SIZE];
    char outBuffer[BUFFER_SIZE];
    string decompressed;
    int ret;

    do {
        ipFile.read(inBuffer, sizeof(inBuffer));
        zs.avail_in = ipFile.gcount();
        if (zs.avail_in == 0) break;
        zs.next_in = reinterpret_cast<Bytef *>(inBuffer);

        do {
            zs.avail_out = sizeof(outBuffer);
            zs.next_out = reinterpret_cast<Bytef *>(outBuffer);

            ret = inflate(&zs, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
                cout << "Error during decompression\n";
                inflateEnd(&zs);
                return {};
            }

            int have = sizeof(outBuffer) - zs.avail_out;
            decompressed.append(outBuffer, have);
        } while (zs.avail_out == 0);

    } while (ret != Z_STREAM_END);

    inflateEnd(&zs);
    ipFile.close();

    // Parse header: "type size$"
    size_t header_end = decompressed.find('$');
    if (header_end == string::npos) {
        cout << "Invalid decompressed data: no header found\n";
        return {};
    }

    string header = decompressed.substr(0, header_end);
    string content = decompressed.substr(header_end + 1);

    // Split header into type and size
    size_t space_pos = header.find(' ');
    if (space_pos == string::npos) {
        cout << "Invalid header format\n";
        return {};
    }

    string type = header.substr(0, space_pos);
    string size = header.substr(space_pos + 1);

    return {type, size, content};
}