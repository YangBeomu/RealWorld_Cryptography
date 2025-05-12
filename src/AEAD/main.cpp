#include "pch.h"

using namespace std;

//문제인건가
struct GCM_Ctx {
	static constexpr int IV_SIZE = 16;
	static constexpr int KEY_SIZE = 32;
	static constexpr int TAG_SIZE = 16;
	static constexpr int DATA_SIZE = 1024;

	uint8_t aesKey_[KEY_SIZE];
	uint8_t gcmIV_[IV_SIZE];
	//requires
	//shared_ptr<uint8_t[]> dataPtr_{};
	uint8_t* dataPtr_;
	size_t dataSize_;
	//shared_ptr<uint8_t[]> aad_{};
	uint8_t* aadPtr_;
	size_t aadSize_{};
	uint8_t tag_[TAG_SIZE]{};

	bool isEmpty() {
		if (this->dataPtr_ == NULL) return true;
		else return false;
	}
	
	GCM_Ctx* operator=(const GCM_Ctx& ctx) {
		memcpy_s(this, sizeof(GCM_Ctx), &ctx, sizeof(GCM_Ctx));

		dataPtr_ = new uint8_t[dataSize_];
		memcpy_s(this->dataPtr_, dataSize_, ctx.dataPtr_, dataSize_);

		aadPtr_ = new uint8_t[aadSize_];
		memcpy_s(this->aadPtr_, aadSize_, ctx.aadPtr_, aadSize_);

		return this;
	}

	void operator delete(void* ctx) {
		delete[](reinterpret_cast<GCM_Ctx*>(ctx)->dataPtr_);
		delete[](reinterpret_cast<GCM_Ctx*>(ctx)->aadPtr_);

		//delete[](ctx);
	}
};
void usage() {
	string ret = " syntax AESD.exe <plane text> \n sample : AESD.exe HelloWorld";
	cout << ret << endl;
}

int parse(int argc, char* argv[]) {
	if (argc != 2) {
		usage();
		return false;
	}

	return true;
}

GCM_Ctx AES_GCM_Encrypt(uint8_t* data, size_t data_size, uint8_t* aad, size_t aad_size) {
	GCM_Ctx gcmCtx{};

	gcmCtx.aadPtr_ = aad;
	gcmCtx.aadSize_ = aad_size;
	
	OSSL_LIB_CTX* osslLibCtx = NULL;
	EVP_CIPHER_CTX* eCipherCtx = NULL;
	EVP_CIPHER* cipher = NULL;

	try {
		cout << "=== AES GCM Encrypt === " << endl;
		cout << "[PLANE_TEXT] "; BIO_dump_fp(stdout, data, data_size);

		osslLibCtx = OSSL_LIB_CTX_new();
		if (osslLibCtx == NULL) throw runtime_error("Failed to call OSSL_LIB_CTX_new");

		eCipherCtx = EVP_CIPHER_CTX_new();
		if (eCipherCtx == NULL) throw runtime_error("Failed to call EVP_CIPHER_CTX_new");

		static const char* props = NULL;

		cipher = EVP_CIPHER_fetch(osslLibCtx, "AES-256-GCM", props);
		if (cipher == NULL) throw runtime_error("Failed to call EVP_CIPHER_fetch");

		if (!RAND_bytes_ex(osslLibCtx, gcmCtx.gcmIV_, GCM_Ctx::IV_SIZE, 1)) throw runtime_error("Failed to call RAND_bytes_ex");

		size_t gcmIVSize = sizeof(gcmCtx.gcmIV_);
		OSSL_PARAM params[] = { OSSL_PARAM_construct_size_t(OSSL_CIPHER_PARAM_AEAD_IVLEN, &gcmIVSize), OSSL_PARAM_END };

		if (!RAND_bytes_ex(osslLibCtx, gcmCtx.aesKey_, GCM_Ctx::KEY_SIZE, 1)) throw runtime_error("Failed to call RAND_bytes_ex");

		if (!EVP_EncryptInit_ex2(eCipherCtx, cipher, gcmCtx.aesKey_, gcmCtx.gcmIV_, params)) throw runtime_error("Failed to call EVP_EncryptInit_ex2");

		gcmCtx.dataPtr_ = new uint8_t[data_size];

		if (!EVP_EncryptUpdate(eCipherCtx, NULL, reinterpret_cast<int*>(&gcmCtx.dataSize_), gcmCtx.aadPtr_, gcmCtx.aadSize_)) throw runtime_error("Failed to call EVP_EncryptUpdate");
		if (!EVP_EncryptUpdate(eCipherCtx, gcmCtx.dataPtr_, reinterpret_cast<int*>(&gcmCtx.dataSize_), data, data_size)) throw runtime_error("Failed to call EVP_EncryptUpdate");

		cout << "[ENCRYPTED] ";
		BIO_dump_fp(stdout, gcmCtx.dataPtr_, gcmCtx.dataSize_);

		uint8_t out[1024]{};
		int outLen = 0;

		if (!EVP_EncryptFinal_ex(eCipherCtx, out, &outLen)) throw runtime_error("Failed to call EVP_EncryptFinal_ex");


		params[0] = OSSL_PARAM_construct_octet_string(OSSL_CIPHER_PARAM_AEAD_TAG, gcmCtx.tag_, GCM_Ctx::TAG_SIZE);

		if (!EVP_CIPHER_CTX_get_params(eCipherCtx, params)) throw runtime_error("Failed to call EVP_CIPHER_CTX_get_params");



		cout << "[TAG] ";
		BIO_dump_fp(stdout, gcmCtx.tag_, GCM_Ctx::TAG_SIZE);
	}
	catch (const exception& e) {
		cerr << "[MAIN] " << e.what() << endl;
		cerr << "ERROR : "; ERR_print_errors_fp(stderr);
	}

	EVP_CIPHER_free(cipher);
	EVP_CIPHER_CTX_free(eCipherCtx);
	OSSL_LIB_CTX_free(osslLibCtx);

	return gcmCtx;
}

//shared_ptr<uint8_t[]> AES_GCM_Decrypt(GCM_Ctx& gcmCtx) {
GCM_Ctx AES_GCM_Decrypt(GCM_Ctx& rGcmCtx) {
	GCM_Ctx gcmCtx{};
	gcmCtx = rGcmCtx;

	OSSL_LIB_CTX* osslLibCtx = NULL;
	EVP_CIPHER_CTX* eCipherCtx = NULL;
	EVP_CIPHER* cipher = NULL;

	uint8_t* out = new uint8_t[gcmCtx.dataSize_];
	int outLen{};

	try {
		//encrypt
		cout << "=== AES GCM Decrypt === " << endl;
		cout << "[CRYPTED_TEXT] "; BIO_dump_fp(stdout, gcmCtx.dataPtr_, gcmCtx.dataSize_);

		osslLibCtx = OSSL_LIB_CTX_new();
		if (osslLibCtx == NULL) throw runtime_error("Failed to call OSSL_LIB_CTX_new");

		eCipherCtx = EVP_CIPHER_CTX_new();
		if (eCipherCtx == NULL) throw runtime_error("Failed to call EVP_CIPHER_CTX_new");

		const char* props = NULL;

		cipher = EVP_CIPHER_fetch(osslLibCtx, "AES-256-GCM", props);
		if (cipher == NULL) throw runtime_error("Failed to call EVP_CIPHER_fetch");

		size_t gcmIVSize = sizeof(gcmCtx.gcmIV_);
		OSSL_PARAM params[] = { OSSL_PARAM_construct_size_t(OSSL_CIPHER_PARAM_AEAD_IVLEN, &gcmIVSize), OSSL_PARAM_END };

		if (!EVP_DecryptInit_ex2(eCipherCtx, cipher, gcmCtx.aesKey_, gcmCtx.gcmIV_, params)) throw runtime_error("Failed to call EVP_DecryptInit_ex2");

		if (!EVP_DecryptUpdate(eCipherCtx, NULL, &outLen, gcmCtx.aadPtr_, gcmCtx.aadSize_)) throw runtime_error("Failed to call EVP_DecryptUpdate");
		if (!EVP_DecryptUpdate(eCipherCtx, gcmCtx.dataPtr_, reinterpret_cast<int*>(&gcmCtx.dataSize_), gcmCtx.dataPtr_, gcmCtx.dataSize_)) throw runtime_error("Failed to call EVP_DecryptUpdate");

		cout << "[DECRYPTED] ";
		BIO_dump_fp(stdout, gcmCtx.dataPtr_, gcmCtx.dataSize_);

		params[0] = OSSL_PARAM_construct_octet_string(OSSL_CIPHER_PARAM_AEAD_TAG, gcmCtx.tag_, GCM_Ctx::TAG_SIZE);
		if (!EVP_CIPHER_CTX_set_params(eCipherCtx, params)) throw runtime_error("Failed to call EVP_CIPHER_CTX_set_params");

		int ret = EVP_DecryptFinal_ex(eCipherCtx, out, &outLen);

		if (ret) cout << "[TAG] Verify Successful" << endl;
		else cout << "[TAG] Verify Failed" << endl;

		delete[](out);
	}
	catch (const exception& e) {
		cerr << "[MAIN] " << e.what() << endl;
		cerr << "ERROR : "; ERR_print_errors_fp(stderr);
	}

	EVP_CIPHER_free(cipher);
	EVP_CIPHER_CTX_free(eCipherCtx);
	OSSL_LIB_CTX_free(osslLibCtx);

	return gcmCtx;
}

int main(int argc, char* argv[]) {
	if (!parse(argc, argv)) return -1;

	uint8_t* planeText = reinterpret_cast<uint8_t*>(argv[1]);
	size_t planeSize = strlen(reinterpret_cast<const char*>(planeText));

	uint8_t add[]  = "some associated data";

	try {
		GCM_Ctx ctx = AES_GCM_Encrypt(planeText, planeSize, add, sizeof(add));
		if (ctx.isEmpty()) throw runtime_error("Failed to call AES_GCM_Encrypt");

		ctx = AES_GCM_Decrypt(ctx);
		if (ctx.isEmpty()) throw runtime_error("Failed to call AES_GCM_Decrypt");

		delete(&ctx);
	}
	catch (const exception& e) {
		cerr << "[MAIN] " << e.what() << endl;
		return -1;
	}
}