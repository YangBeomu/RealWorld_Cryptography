#include "pch.h"

using namespace std;

bool MakeKeyPair(EVP_PKEY** pKey) {
	EVP_PKEY_CTX* pCtx = nullptr;

	try {
		if ((pCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, NULL)) == NULL) throw runtime_error("Failed to call EVP_PKEY_CTX_new_id");
		if (EVP_PKEY_keygen_init(pCtx) <= 0) throw runtime_error("Failed to call EVP_PKEY_keygen_init");
		if(EVP_PKEY_keygen(pCtx, pKey) <= 0) throw runtime_error("Failed to call EVP_PKEY_keygen");
	}
	catch (const exception& e) {
		cerr << "[MAKE_KEY_PAIR] " << e.what() << endl;
		
		if(pCtx != nullptr) EVP_PKEY_CTX_free(pCtx);

		return false;
	}

	EVP_PKEY_CTX_free(pCtx);
	return true;
}

int main() {
	EVP_PKEY* pKey = NULL;

	if (!MakeKeyPair(&pKey)) return -1;

	PEM_write_PrivateKey(stdout, pKey, NULL, NULL, 0, NULL, NULL);

	uint8_t privateKey[1024]{};
	size_t keyLen = sizeof(privateKey);

	EVP_PKEY_get_raw_private_key(pKey, privateKey, &keyLen);

	cout << "===PRIVATE_KEY===" << endl;

	for (int i = 0; i < keyLen; i++) {
		printf("%02x", privateKey[i]);
	}
	cout << endl;
	cout << "=================" << endl;
}