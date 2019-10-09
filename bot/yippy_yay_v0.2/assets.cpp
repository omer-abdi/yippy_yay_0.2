#include "assets.h"

#ifndef _CIPHER
#define _CIPHER
AES _aes(256);
unsigned int len;
unsigned char _key[] = {
	0x00, 0x01, 0x02, 0x03,
	0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13,
	0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b,
	0x1c, 0x1d, 0x1e, 0x1f
};
unsigned char _iv[] = {
	0x00, 0x01, 0x02, 0x03,
	0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f
};

#endif // _CIPHER

bool asset::send_file(std::socket &sock, const char *file_path) {
	std::ifstream file(file_path, std::ios::binary);
	if(file.is_open()) {
		asset::size_f file_size = asset::get_file_size(file);
		sock.send(std::to_string(file_size).c_str());

		asset::size_f chunks = file_size / asset::AES_BUFF;
		asset::size_f remaining_bytes = file_size % asset::AES_BUFF;

		char *read_buff = new char[asset::AES_BUFF];
		for(asset::size_f i = 0; i < chunks; ++i) {
			// TODO: encrypt each chunk bytes and send it
			// file.read(read_buff, asset::BUFF_SIZE);
			// sock.send(_cipher(read_buff, encrypt, asset::BUFF_SIZE));
			file.read(read_buff, asset::AES_BUFF);
			sock.send(_cipher(read_buff, encrypt), asset::AES_BUFF);

		}

		// TODO: encrypt and send the left byte
		if(remaining_bytes != 0) {
			// asset::size_f zeros_needed = asset::BUFF_SIZE - remaining_bytes;
			// file.read(read_buff, remaining_bytes);
			// memcpy(read_buff+remaining_bytes, "0", zeros_needed);
			memset(read_buff, 48, asset::AES_BUFF); // 48 == "0"
			file.read(read_buff, remaining_bytes);
			sock.send(_cipher(read_buff, encrypt), asset::AES_BUFF);
		}

		delete [] read_buff;
		return true;

	} else {
		sock.send("-1");
	}
	return false;
}

bool asset::recv_file(std::socket &sock, const char *file_name) {
	char c_file_sz[1024] = {}; // file size we are reciving
	sock.recv(c_file_sz, 1024);
	asset::size_f file_size = atoll(c_file_sz);
	
	if(file_size != -1) {
		asset::size_f chunks = file_size / asset::AES_BUFF;
		asset::size_f remaining_bytes = file_size % asset::AES_BUFF;
		std::ofstream file(file_name, std::ios::binary);

		char *read_buff = new char[AES_BUFF];
		for(asset::size_f i = 0; i < chunks; ++i) {
			sock.recv(read_buff, AES_BUFF);
			file.write(_cipher(read_buff, decrypt), AES_BUFF);
		}

		// TODO: receive, decrypt and write the remaining bytes
		if(remaining_bytes != 0) {
			sock.recv(read_buff, AES_BUFF);
			file.write(_cipher(read_buff, decrypt), remaining_bytes);
		}

		delete [] read_buff;
		
		return true;

	} else {
		return false;
	}
}


asset::size_f asset::get_file_size(std::ifstream &file) {
	// since we are passing file
	// as a reference we need to
	// set the current pointer back
	asset::size_f cur = file.tellg();

	// getting the file size
	file.seekg(0, std::ios::end);
	asset::size_f _end = file.tellg();

	// set the current pointer back to it's position
	file.seekg(cur);

	return _end;
}

char *asset::encrypt(char *in) {
	return (char*)_aes.EncryptCBC((unsigned char*)in, 16, _key, _iv, len);
}

char *asset::decrypt(char *in) {
	return (char*)_aes.DecryptCBC((unsigned char*)in, 16, _key, _iv, len);
}

char *asset::_cipher(char *input, char*(*mode)(char*)) {
	return mode(input);
}
