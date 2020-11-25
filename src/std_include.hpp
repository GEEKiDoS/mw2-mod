#pragma once

#define BINARY_PAYLOAD_SIZE 0x0A000000
#define TLS_PAYLOAD_SIZE 0x2000

#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(disable: 4458)
#pragma warning(disable: 4702)
#pragma warning(disable: 5054)
#pragma warning(disable: 6297)
#pragma warning(disable: 6385)
#pragma warning(disable: 6386)
#pragma warning(disable: 26451)
#pragma warning(disable: 26444)
#pragma warning(disable: 26451)
#pragma warning(disable: 26495)
#pragma warning(disable: 28020)

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <MsHTML.h>
#include <MsHtmHst.h>
#include <ExDisp.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <corecrt_io.h>
#include <fcntl.h>
#include <shellapi.h>
#include <csetjmp>
#include <ShlObj.h>
#include <winternl.h>
#include <VersionHelpers.h>
#include <Psapi.h>
#include <urlmon.h>
#include <atlbase.h>
#include <iphlpapi.h>
#include <DbgHelp.h>
#include <TlHelp32.h>
#include <winnt.h>
#include <d3d9.h>

// min and max is required by gdi, therefore NOMINMAX won't work
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <map>
#include <atomic>
#include <vector>
#include <mutex>
#include <queue>
#include <regex>
#include <chrono>
#include <thread>
#include <fstream>
#include <utility>
#include <filesystem>

#include <zlib.h>
#include <zstd.h>
#include <tomcrypt.h>

#include <gsl/gsl>

#include <rapidjson/document.h>

#include <udis86.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "dbghelp.lib")

#pragma warning(pop)
#pragma warning(disable: 4100)

#include "resource.hpp"

using namespace std::literals;

extern __declspec(thread) char tls_data[TLS_PAYLOAD_SIZE];