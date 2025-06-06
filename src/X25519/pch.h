#pragma once

#pragma warning(disable : 4996)
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>
#include <cinttypes>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "openssl/applink.c"
#include "openssl/evp.h"
#include "openssl/pem.h"
#include "openssl/err.h"


