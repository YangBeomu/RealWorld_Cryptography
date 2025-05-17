#pragma once

//WINDOW
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include <iostream>

class NetworkController {
public:
	struct KeyData {
		uint8_t* dataPtr_;
		size_t dataLen_;
	};

protected:
	std::string ip_{};
	WORD port_{};
	SOCKET socket_{};
	SOCKET clientSocket_{};

	bool init();
	std::string message(const std::string& msg);
	void ErrorHandling(const std::exception& e, const std::string& funcName);

public:
	NetworkController(const std::string& ip, const WORD port);
	~NetworkController();

	bool Open();
	bool Connect();
	KeyData Recv();
	bool Send(KeyData& kd);
};

