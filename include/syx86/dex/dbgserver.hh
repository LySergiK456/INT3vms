// #define WIN32_LEAN_AND_MEAN
// #include <winsock2.h>
// #include <ws2tcpip.h>
// #include <windows.h>
// #include <memory>

// #ifndef KRACK_DEBUG_SERVER_HH
// #define KRACK_DEBUG_SERVER_HH
// #include "../syx86.hh"
// #include "../../conf.hh"
// extern "C" {
// 	#include "./dbg.h"
// };

// class DbgTcp {
// 	WSADATA				wsa;
// 	SOCKET				server;
// 	sockaddr_in			addr{};
// 	__u16				port{};
// 	__s8				ok{0};
// 	std::shared_ptr<SlitYaX86::Razor> r;

// public:
// 	DbgTcp(std::shared_ptr<SlitYaX86::Razor> _r, __u16 port);
// 	~DbgTcp();
// 	int Run();
// };
// #endif
