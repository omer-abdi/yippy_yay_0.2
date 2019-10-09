#pragma once

#include "mode.h"

// std libs
#include <fstream>
#include <string>

// own libs
#include "socket.h"

// downloaded libs
#include "aes.h"

// dev libs
#ifdef DEBUG_MODE
#include <iostream>
#define raw_out(x) std::cout << "'" << x << "'" << std::endl
#endif // DEBUG_MODE


namespace asset {
	// constants
	using size_f = long long;

	const int BUFF_SIZE = 1024;
	const int AES_BUFF = 16;


	bool send_file(std::socket &, const char *);
	bool recv_file(std::socket &, const char *);

	size_f get_file_size(std::ifstream &file);

	char *encrypt(char *);
	char *decrypt(char *);

	char *_cipher(char *, char*(*)(char*));
}
