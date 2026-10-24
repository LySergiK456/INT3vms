#!/usr/bin/python3
import sys
import socket
import ctypes

p = print

class X86Regs(ctypes.Structure):
	_fields_ = [
	    ( "rax", ctypes.c_uint64 ),
	    ( "rbx", ctypes.c_uint64 ),
	    ( "rcx", ctypes.c_uint64 ),
	    ( "rdx", ctypes.c_uint64 ),
	    ( "rdi", ctypes.c_uint64 ),
	    ( "rsi", ctypes.c_uint64 ),
	    ( "rsp", ctypes.c_uint64 ),
	    ( "rbp", ctypes.c_uint64 ),
	    ( "r8", ctypes.c_uint64 ),
	    ( "r9", ctypes.c_uint64 ),
	    ( "r10", ctypes.c_uint64 ),
	    ( "r11", ctypes.c_uint64 ),
	    ( "r12", ctypes.c_uint64 ),
	    ( "r13", ctypes.c_uint64 ),
	    ( "r14", ctypes.c_uint64 ),
	    ( "r15", ctypes.c_uint64 ),
	    ( "rip", ctypes.c_uint64 ),
	    ( "gs", ctypes.c_uint64 ),
	    ( "fs", ctypes.c_uint64 ),
	    ( "rflags", ctypes.c_uint64 )
    ]

class DbgHdr(ctypes.Structure):
	_fields_ = [
		( "filename", ctypes.c_ubyte * 0x100 ),
		( "regs", X86Regs ),
		( "num_of_maps", ctypes.c_uint32 )
	]
PORT = 8473
class DbgTcpClient:
	def __init__(self, port):
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.connect(("localhost", port))

	def recvHdr(self) -> DbgHdr:
		sz = ctypes.sizeof(DbgHdr)
		buf = bytearray()
		while len(buf) < sz:
			chunk = self.sock.recv(sz - len(buf))
			if not chunk:
				raise ConnectionError("Server Disconnected")

			buf.extend(chunk)

		return DbgHdr.from_buffer_copy(buf)


def main():
	c = DbgTcpClient(PORT)
	dbg_hdr = c.recvHdr()
	p(bytes(dbg_hdr.filename).decode("utf-8"))


if __name__ == "__main__":
	sys.exit(main())