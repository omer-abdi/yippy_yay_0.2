#pragma once

#include "mode.h"

// is needed for sockets on windows
#define _WIN32_WINNT 0x501

#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef DEBUG_MODE
#include <iostream>
#endif

namespace std {
	class socket {
	private:
		int sockfd;
		int s_family, s_type;

		sockaddr_in self_addr;
		socklen_t addr_size;

		addrinfo *ret_res(const char *, const char *);
	public:
		// constructor
		socket();
		socket(int, int);

		// connect functions
		int connect(const char *, const char *);

		// exchange functions
		int send(const char *);
		int send(const char *, size_t);
		int send_f(char *, size_t);
		int send(char *, size_t);
		int recv(char *, size_t);

		// cleanup
		int close();
		~socket();

	};
}
