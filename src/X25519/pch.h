#pragma once

#pragma warning(disable : 4996)
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

#include <iostream>
#include <string>
#include <cinttypes>
#include <Windows.h>

#include "openssl/applink.c"
#include "openssl/evp.h"
#include "openssl/pem.h"


