#include "pch.h"
#include "NetworkController.h"

using namespace std;

NetworkController::NetworkController(const std::string& ip, const WORD port)
	: ip_(ip), port_(port) {
	if (!init()) throw runtime_error(message("Failed to call init"));
}

NetworkController::~NetworkController() {
	WSACleanup();
}

string NetworkController::message(const std::string& msg) {
	return "[NetworkController] " + msg;
}

void NetworkController::ErrorHandling(const exception& e, const std::string& funcName) {
	cerr << string("[NetworkController:" + funcName + "] ") << e.what() << endl;
	cerr << "ERROR : " << WSAGetLastError() << endl;
}

bool NetworkController::init() {
	WSADATA wsaData{};

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	try {
		if ((socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == NULL)
			throw runtime_error("Failed to call socket");

		return true;
	}
	catch (const exception& e) {
		cerr << "[INIT] " << e.what() << endl;
	}

	return false;
}

bool NetworkController::Open() {
	sockaddr_in sockAddr{};


	try {
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(port_);

		if (inet_pton(AF_INET, ip_.c_str(), &sockAddr.sin_addr) == -1)
			throw runtime_error("Failed to call inet_pton");

		if (bind(socket_, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr)) == SOCKET_ERROR)
			throw runtime_error("Failed to call bind");

		if (listen(socket_, 0));

		sockaddr_in clientInfo{};
		int addrLen = sizeof(clientInfo);

		if (accept(socket_, reinterpret_cast<sockaddr*>(&clientInfo), &addrLen) == SOCKET_ERROR)
			throw runtime_error("Failed to call accept");

		return true;
	}
	catch (const exception& e) {
		ErrorHandling(e, "Open");
	}

	return false;
}

bool NetworkController::Connect() {
	sockaddr_in sockAddr{};

	try {
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(port_);

		if (inet_pton(AF_INET, ip_.c_str(), &sockAddr.sin_addr) == -1)
			throw runtime_error("Failed to call inet_pton");

		if (connect(socket_, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr)) == INVALID_SOCKET)
			throw runtime_error("Failed to call connect");

		return true;
	}
	catch (const exception& e) {
		ErrorHandling(e, "Connect");
	}

	return false;
}

NetworkController::KeyData NetworkController::Recv() {
	KeyData kd{};
	try {
		if (recv(socket_, reinterpret_cast<char*>(&kd.dataLen_), sizeof(kd.dataLen_), 0) <= 0)
			throw runtime_error("Failed call recv");
		if (recv(socket_, reinterpret_cast<char*>(kd.dataPtr_), kd.dataLen_, 0) <= 0)
			throw runtime_error("Failed call recv");
	}
	catch (const exception& e) {
		ErrorHandling(e, "Recv");
	}

	return kd;
}

bool NetworkController::Send(NetworkController::KeyData& kd) {
	try {
		/*if (send(socket_, reinterpret_cast<char*>(&kd.dataLen_), sizeof(kd.dataLen_), 0) <= 0)
			throw runtime_error("Failed call send");
		if (send(socket_, reinterpret_cast<char*>(kd.dataPtr_), kd.dataLen_, 0) <= 0)
			throw runtime_error("Failed call send");*/

		uint8_t test[1024]{};

		memcpy(test, reinterpret_cast<char*>(&kd.dataLen_), sizeof(kd.dataLen_));
		memcpy(test + sizeof(kd.dataLen_), reinterpret_cast<char*>(kd.dataPtr_), kd.dataLen_);

		if(send(socket_, reinterpret_cast<char*>(test), 1024, 0) <= 0)
			throw runtime_error("Failed call send");

		return true;
	}
	catch (const exception& e) {
		ErrorHandling(e, "Send");
	}

	return false;
}