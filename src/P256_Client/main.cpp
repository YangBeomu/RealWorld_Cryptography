#include "pch.h"
#include "P256_Client.h"

using namespace std;

enum KeyType {
	PRIVATE_KEY,
	PUBLIC_KEY
};

void usage() {
	printf("syntax P256.exe <ip> <port> \n");
	printf("sample : P256.exe 192.168.x.x xxxx");
}

bool parse(int argc) {
	if (argc != 3) {
		usage();
		return false;
	}

	return true;
}

void handleErrors(void)
{
	ERR_print_errors_fp(stderr);
	abort();
}

EVP_PKEY* GetParseKey(const NetworkController::KeyData kd) {
	BIO* mem{};
	EVP_PKEY* key{};

	try {
		BIO* mem = BIO_new_mem_buf(kd.dataPtr_, kd.dataLen_);
		EVP_PKEY* key = PEM_read_bio_PUBKEY(mem, nullptr, nullptr, nullptr);
	}
	catch (const exception& e) {
		cerr << "[GetParseKey] " << e.what() << endl;
		handleErrors();
	}

	return key;
}

EVP_PKEY* MakeKeyPair() {
	EVP_PKEY_CTX* pCtx{}; //Param Context
	EVP_PKEY_CTX* kCtx{}; //Key Context

	EVP_PKEY* pKey{}; //key
	EVP_PKEY* params{};

	try {
		if ((pCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL)) == NULL) throw runtime_error("Failed to call EVP_PKEY_CTX_new_id");

		if (EVP_PKEY_paramgen_init(pCtx) != 1) throw runtime_error("Failed to call EVP_PKEY_paramgen_init");
		if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pCtx, NID_X9_62_prime256v1) != 1) throw runtime_error("Failed to call EVP_PKEY_CTX_set_ec_paramgen_curve_nid");
		if (!EVP_PKEY_paramgen(pCtx, &params)) throw runtime_error("Failed to call EVP_PKEY_paramgen");

		if ((kCtx = EVP_PKEY_CTX_new(params, NULL)) == NULL) throw runtime_error("Failed to call EVP_PKEY_CTX_new");
		if (EVP_PKEY_keygen_init(kCtx) != 1) throw runtime_error("Failed to call EVP_PKEY_keygen_init");
		if (EVP_PKEY_keygen(kCtx, &pKey) != 1) throw runtime_error("Failed to call EVP_PKEY_keygen");
	}
	catch (const exception& e) {
		cerr << "[MakeKeyPair] " << e.what() << endl;
		cerr << "ERROR : "; handleErrors();
	}

	EVP_PKEY_CTX_free(pCtx);
	EVP_PKEY_CTX_free(kCtx);

	EVP_PKEY_free(params);

	return pKey;
}

void SaveKey(HANDLE publicFile, HANDLE privateFile) {
	EVP_PKEY_CTX* pCtx{}; //Param Context
	EVP_PKEY_CTX* kCtx{}; //Key Context

	EVP_PKEY* pKey{}; //key
	EVP_PKEY* params{};

	try {
		if ((pCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL)) == NULL) throw runtime_error("Failed to call EVP_PKEY_CTX_new_id");

		if (EVP_PKEY_paramgen_init(pCtx) != 1) throw runtime_error("Failed to call EVP_PKEY_paramgen_init");
		if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pCtx, NID_X9_62_prime256v1) != 1) throw runtime_error("Failed to call EVP_PKEY_CTX_set_ec_paramgen_curve_nid");
		if (!EVP_PKEY_paramgen(pCtx, &params)) throw runtime_error("Failed to call EVP_PKEY_paramgen");

		if ((kCtx = EVP_PKEY_CTX_new(params, NULL)) == NULL) throw runtime_error("Failed to call EVP_PKEY_CTX_new");
		if (EVP_PKEY_keygen_init(kCtx) != 1) throw runtime_error("Failed to call EVP_PKEY_keygen_init");
		if (EVP_PKEY_keygen(kCtx, &pKey) != 1) throw runtime_error("Failed to call EVP_PKEY_keygen");

		PEM_write_PUBKEY(reinterpret_cast<FILE*>(publicFile), pKey);
		PEM_write_PrivateKey(reinterpret_cast<FILE*>(privateFile), pKey, NULL, NULL, NULL, NULL, NULL);
	}
	catch (const exception& e) {
		cerr << "[SaveKey] " << e.what() << endl;
		cerr << "ERROR : "; handleErrors();
	}

	EVP_PKEY_CTX_free(pCtx);
	EVP_PKEY_CTX_free(kCtx);

	EVP_PKEY_free(pKey);
	EVP_PKEY_free(params);
}

EVP_PKEY* LoadKey(const HANDLE file, const KeyType type) {
	EVP_PKEY* pKey{};

	switch (type) {
	case PRIVATE_KEY:
		pKey = PEM_read_PrivateKey(reinterpret_cast<FILE*>(file), NULL, NULL, NULL);
		break;
	case PUBLIC_KEY:
		pKey = PEM_read_PUBKEY(reinterpret_cast<FILE*>(file), NULL, NULL, NULL);
		break;
	default:
		break;
	}

	return pKey;
}

uint8_t* ECDH(const NetworkController::KeyData kd, size_t* sKeyLen) {
	EVP_PKEY_CTX* pCtx{}; //Param Context
	EVP_PKEY_CTX* kCtx{}; //Key Context
	EVP_PKEY_CTX* ctx{};
	uint8_t* secret{};

	EVP_PKEY* pKey{}; //key
	EVP_PKEY* peerKey{}; //shared key?
	EVP_PKEY* params{};

	try {
		if ((pCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL)) == NULL) throw runtime_error("Failed to call EVP_PKEY_CTX_new_id");
		
		if (EVP_PKEY_paramgen_init(pCtx) != 1) throw runtime_error("Failed to call EVP_PKEY_paramgen_init");
		if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pCtx, NID_X9_62_prime256v1) != 1) throw runtime_error("Failed to call EVP_PKEY_CTX_set_ec_paramgen_curve_nid");
		if (!EVP_PKEY_paramgen(pCtx, &params)) throw runtime_error("Failed to call EVP_PKEY_paramgen");
		
		if ((kCtx = EVP_PKEY_CTX_new(params, NULL)) == NULL) throw runtime_error("Failed to call EVP_PKEY_CTX_new");
		if (EVP_PKEY_keygen_init(kCtx) != 1) throw runtime_error("Failed to call EVP_PKEY_keygen_init");
		if (EVP_PKEY_keygen(kCtx, &pKey) != 1) throw runtime_error("Failed to call EVP_PKEY_keygen");

		if ((ctx = EVP_PKEY_CTX_new(pKey, NULL)) == NULL) throw runtime_error("Failed to call EVP_PKEY_CTX_new");
		if (EVP_PKEY_derive_init(ctx) != 1) throw runtime_error("Failed to call EVP_PKEY_derive_init");

		peerKey = GetParseKey(kd);

		if (EVP_PKEY_derive_set_peer(ctx, peerKey) != 1) throw runtime_error("Failed to call EVP_PKEY_derive_set_peer");
		if (EVP_PKEY_derive(ctx, NULL, sKeyLen) != 1) throw runtime_error("Failed to call EVP_PKEY_derive");

		if ((secret = reinterpret_cast<uint8_t*>(OPENSSL_malloc(*sKeyLen))) == NULL) throw runtime_error("Failed to call OPENSSL_malloc");
		if (EVP_PKEY_derive(ctx, secret, sKeyLen) != 1) throw runtime_error("Failed to call EVP_PKEY_derive");
	}catch (const exception& e) {
		cerr << "[ECDH] " << e.what() << endl;
		cerr << "ERROR : "; handleErrors();
	}

	EVP_PKEY_CTX_free(pCtx);
	EVP_PKEY_CTX_free(kCtx);
	EVP_PKEY_CTX_free(ctx);

	EVP_PKEY_free(pKey);
	EVP_PKEY_free(peerKey);
	EVP_PKEY_free(params);

	return secret;
}

int main(int argc,char* argv[]) {
	if (!parse(argc)) return -1;

	string ip(argv[1]);

	NetworkController nc(ip, stoi(argv[2]));

	EVP_PKEY* key = MakeKeyPair();

	if (!nc.Connect()) return -1;

	NetworkController::KeyData kd{};

	BIO* mem = BIO_new(BIO_s_mem());

	if (PEM_write_bio_PUBKEY(mem, key) != 1) return -1;

	BUF_MEM* memPtr{};
	BIO_get_mem_ptr(mem, &memPtr);

	
	kd.dataLen_ = memPtr->length;
	kd.dataPtr_ = new uint8_t[memPtr->length];
	memcpy_s(kd.dataPtr_, memPtr->length, memPtr->data, memPtr->length);
	
	nc.Send(kd);

	return 0;
}