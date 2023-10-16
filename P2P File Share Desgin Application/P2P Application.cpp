#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

struct SharedFile {
    string filename;
    string filepath;
};

struct Peer {
    string name;
    string ip;
    int port;
    SSL* ssl;
};

mutex mtx;

vector<SharedFile> sharedFiles;
vector<Peer> connectedPeers;

void initSSL() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

void shareFileWithPeer(Peer peer, string filename) {
    SSL_write(peer.ssl, filename.c_str(), filename.length());
}

void searchFiles(string query) {
    vector<SharedFile> searchResults;

    mtx.lock();
    for (SharedFile file : sharedFiles) {
        if (file.filename.find(query) != string::npos) {
            searchResults.push_back(file);
        }
    }
    mtx.unlock();

    if (!searchResults.empty()) {
        cout << "Search Results:" << endl;
        for (SharedFile file : searchResults) {
            cout << "Filename: " << file.filename << " Path: " << file.filepath << endl;
        }
    } else {
        cout << "No matching files found." << endl;
    }
}

int main() {
    initSSL();

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    while (true) {
        cout << "1. Share a file" << endl;
        cout << "2. Search for files" << endl;
        cout << "3. Exit" << endl;
        cout << "Choose an option: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            string filename;
            cout << "Enter the filename to share: ";
            cin >> filename;

            if (filename.find('/') != string::npos || filename.find('\\') != string::npos) {
                cout << "Invalid filename. Do not use '/' or '\\' characters." << endl;
                continue;
            }

            mtx.lock();
            sharedFiles.push_back({filename, "path_to_file"});
            mtx.unlock();

            for (Peer peer : connectedPeers) {
                shareFileWithPeer(peer, filename);
            }
        } else if (choice == 2) {
            string query;
            cout << "Enter a search query: ";
            cin >> query;

            if (query.empty()) {
                cout << "Invalid query. Please provide a search term." << endl;
                continue;
            }

            searchFiles(query);
        } else if (choice == 3) {
            break;
        } else {
            cout << "Invalid choice. Try again." << endl;
        }
    }

    SSL_CTX_free(ctx);
    ERR_free_strings();
    EVP_cleanup();
    return 0;
}