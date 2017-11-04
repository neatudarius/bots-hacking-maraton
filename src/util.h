#include <stdlib.h>
#include <stdint.h>
#include <map>
#include <string>
#include <vector>
#include <functional>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
using conn_t = UINT_PTR;
#else
using conn_t = int;
#endif

using headers_t = std::map<std::string, std::string>;

void net_init();
void error(const char *msg="");
conn_t create_connection(const char *server, int port);
void close_connection(conn_t conn);
void run_server(int port, std::function<void(conn_t)> client_handler);

void write_request(conn_t conn, const char *path, headers_t headers, const void *body=nullptr, int body_size=0);
void write_response(conn_t conn, int status, headers_t headers, const void *body=nullptr, int body_size=0);
std::string read_request(conn_t conn, headers_t &headers, std::vector<uint8_t> &body);
int read_response(conn_t conn, headers_t& headers, std::vector<uint8_t> &body);

