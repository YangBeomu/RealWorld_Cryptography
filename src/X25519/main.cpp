#include "pch.h"

using namespace std;

enum KeyType {
	PRIVATE_KEY,
	PUBLIC_KEY
};

void handleErrors(void) {
	ERR_print_errors_fp(stderr);
	abort();
}

bool MakeKeyPair(EVP_PKEY** pKey) {
	EVP_PKEY_CTX* pCtx = nullptr;

	try {
		if ((pCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, NULL)) == NULL) throw runtime_error("Failed to call EVP_PKEY_CTX_new_id");
		if (EVP_PKEY_keygen_init(pCtx) <= 0) throw runtime_error("Failed to call EVP_PKEY_keygen_init");
		if (EVP_PKEY_keygen(pCtx, pKey) <= 0) throw runtime_error("Failed to call EVP_PKEY_keygen");

		EVP_PKEY_CTX_free(pCtx);
		return true;
	}
	catch (const exception& e) {
		cerr << "[MAKE_KEY_PAIR] " << e.what() << endl;
		cerr << "ERROR : ";	handleErrors();
	}

	if (pCtx != nullptr) EVP_PKEY_CTX_free(pCtx);

	return false;
}

vector<uint8_t> GetRawKey(const EVP_PKEY* pKey,const int keyLen, const KeyType type) {
	vector<uint8_t> key(keyLen);
	size_t keySize = keyLen;

	try {
		switch (type) {
		case PRIVATE_KEY:
			if (EVP_PKEY_get_raw_private_key(pKey, key.data(), &keySize) != 1) throw runtime_error("Failed to call EVP_PKEY_get_raw_private_key");
			break;
		case PUBLIC_KEY:
			if(EVP_PKEY_get_raw_public_key(pKey, key.data(), &keySize) != 1) throw runtime_error("Failed to call EVP_PKEY_get_raw_public_key");
			break;
		default:
			break;
		}
	}
	catch (const exception& e) {
		cerr << "[GetRawKey] " << e.what() << endl;
		cerr << "ERROR : "; handleErrors();
	}

	return key;
}

string GetKey(EVP_PKEY* pKey, const KeyType type) {
	BIO* mem = BIO_new(BIO_s_mem());
	string ret{};

	try {
		switch (type) {
		case PUBLIC_KEY:
			if (PEM_write_bio_PUBKEY(mem, pKey) != 1) throw runtime_error("Failed to call PEM_write_bio_PUBKEY");
			break;
		case PRIVATE_KEY:
			//not save encrypt algorithm, encrypt passwd ...
			if (PEM_write_bio_PrivateKey(mem, pKey, nullptr, nullptr, 0, nullptr, nullptr) != 1) throw runtime_error("Failed to call PEM_write_bio_PUBKEY");
			break;
		default:
			break;
		}

		BUF_MEM* memPtr{};
		BIO_get_mem_ptr(mem, &memPtr);

		ret = string(memPtr->data, memPtr->length);
	}
	catch (const exception& e) {
		cerr << "[GetKey] " << e.what() << endl;
		cerr << "ERROR : "; handleErrors();
	}

	BIO_free(mem);

	return ret;
}

int main() {
	EVP_PKEY* pKey = NULL;

	if (!MakeKeyPair(&pKey)) return -1;

	cout << GetKey(pKey, KeyType::PRIVATE_KEY) << endl;
	cout << GetKey(pKey, KeyType::PUBLIC_KEY) << endl;

	int keyLen = EVP_PKEY_size(pKey);

	auto privateRawKey = GetRawKey(pKey, keyLen, KeyType::PRIVATE_KEY);
	auto publicRawKey = GetRawKey(pKey, keyLen, KeyType::PUBLIC_KEY);
	
	//pem 형식으로 변환 필요
	cout << "===Private Raw Key===" << endl;
	//cout << std::string(privateRawKey.begin(), privateRawKey.end()) << endl;
	for (const auto& elem : privateRawKey) printf("%02x", elem);
	cout << endl; cout << "=====================" << endl;

	cout << "===Public Raw Key===" << endl;
	//cout << std::string(publicRawKey.begin(), publicRawKey.end()) << endl;
	for (const auto& elem : publicRawKey) printf("%02x", elem);
	cout << endl; cout << "=====================" << endl;
}