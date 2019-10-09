#include "socket.h"

// constructors
std::socket::socket(int family, int type)
: s_family(family), s_type(type) {

	WSADATA wsa_data;
	if(WSAStartup(MAKEWORD(1, 1), &wsa_data) != 0) {
		#ifdef DEBUG_MODE
		std::cerr << "WSAStartup: failed" << std::endl;
		#endif
		exit(1);
	}
}

std::socket::socket() : socket(AF_INET, SOCK_STREAM) {
	socket(AF_INET, SOCK_STREAM);
}

addrinfo *std::socket::ret_res(const char *ip, const char *port) {
	addrinfo hints, *res;

	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = s_family;
	hints.ai_socktype = s_type;

	if(getaddrinfo(ip, port, &hints, &res) != 0) {
		#ifdef DEBUG_MODE
		std::cerr << "getaddrinfo: failed" << std::endl;
		#endif
		return nullptr;
	}

	return res;
}

// connect functions

int std::socket::connect(const char *ip, const char *port) {
	addrinfo *res = ret_res(ip, port);

	addrinfo *rp;
	for(rp = res; rp != nullptr; rp = res->ai_next) {
		sockfd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(sockfd == -1)
			continue;
		if(::connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;

		::shutdown(sockfd, 2);
	}

	if(rp == nullptr)
		return -1;

	this->self_addr = *(reinterpret_cast<sockaddr_in*>(&(rp->ai_addr)));
	this->addr_size = rp->ai_addrlen;

	freeaddrinfo(res);
	freeaddrinfo(rp);

	return 0;
}

int std::socket::send(const char *buff) {
	return ::send(sockfd, buff, ::strlen(buff), 0);
}

int std::socket::send(const char *buff, size_t len) {
	return ::send(sockfd, buff, len, 0);
}

int std::socket::send(char *buff, size_t len) {
	return ::send(sockfd, buff, len, 0);
}

int std::socket::send_f(char *buff, size_t len) {
	size_t total = 0;
	size_t bytesleft = len;
	size_t n;

	while(total < len) {
		n = ::send(sockfd, buff+total, bytesleft, 0);
		if(n == -1) break;
		total += n;
		bytesleft -= n;
	}

	return n == -1? -1 : 0;
}

int std::socket::recv(char *buff, size_t len) {
	return ::recv(sockfd, buff, len, 0);
}

int std::socket::close() {
	return ::closesocket(sockfd);
}

std::socket::~socket() {
	WSACleanup();
	::closesocket(sockfd);
}
