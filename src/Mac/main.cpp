#include "pch.h"

using namespace std;

void usage() {
	cout << "syntax : Mac.exe <data> <key>" << endl;
	cout << "sample : Mac.exe Hello Test" << endl;
}

bool parse(int argc, char* argv[]) {
	if (argc < 3) {
		usage();
		return false;
	}

	return true;
}

int main(int argc, char* argv[]) {
	if (!parse(argc, argv)) return -1;

	string data = argv[1];
	string key = argv[2];
	
	uint8_t* hmac = HMAC(EVP_sha3_256(), key.c_str(), key.size(), reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), NULL, NULL);
	if (hmac == nullptr) {
		cerr << "Failed to make hamc \n";
		return -1;
	}

	cout << "---HMAC---" << endl;
	for (int i = 0; i < 32; i++)
		printf("%02x", hmac[i]);
	cout<<"\n----------" << endl;

}