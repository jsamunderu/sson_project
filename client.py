import socket

class SSONClient(object):
	def __init__(self, host, port):
		self._host = host
		self._port = port
		self._skt = None

	def __del__(self):
		if self._skt is not None:
			self._skt.close()
	def connect(self):
		self._skt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self._skt.connect((self._host, self._port))

	def recvAll(self):
		data = b''
		while True:
			buf = self._skt.recv(64)
			if not buf:
				return data
			data += buf
		return data

if __name__ == '__main__':
	client = SSONClient("127.0.0.1", 8181)
	client.connect()
	print repr(client.recvAll())
