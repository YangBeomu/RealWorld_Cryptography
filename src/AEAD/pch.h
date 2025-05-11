#pragma once

#pragma warning(disable : 4996)
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

#include <iostream>
#include <string>
#include <cinttypes>
#include <Windows.h>

#include "openssl/applink.c"
#include "openssl/rand.h"
#include "openssl/aes.h"
#include "openssl/core_names.h"
#include "openssl/bio.h"
#include "openssl/err.h"


