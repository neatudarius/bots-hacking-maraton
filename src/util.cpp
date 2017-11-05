#include "util.h"
#include <sstream>
#include <string.h>
#include <set>
#include <mutex>
#include <thread>
#include <algorithm>
#include <exception>

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "unistd.h"

#define INVALID_SOCKET -1
#endif

void net_init()
{
#ifdef _WIN32
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0) {
        errno = err;
        error();
    }
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif
}

void error(const char *msg)
{
    throw std::runtime_error(std::string(msg) + strerror(errno));
}

conn_t create_connection(const char *server, int port)
{
    conn_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET)
        error();

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server);
    server_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
        error();

    return sockfd;
}

static void send_bytes(conn_t conn, const void *data, int size)
{
    const char *p = (const char*) data;
    while (size > 0) {
        int n = send(conn, p, size, 0);
        if (n == -1)
            error();
        size -= n;
        p += n;
    }
}

void write_request(conn_t conn, const char *path, headers_t headers, const void *body, int body_size)
{
    headers["Content-Length"] = std::to_string(body_size);

    char request[4096];
    char *p = request;
    p += sprintf(p, "GET %s HTTP/1.1\r\n", path);
    for (auto &it : headers) {
        p += sprintf(p, "%s: %s\r\n", it.first.c_str(), it.second.c_str());
    }
    p += sprintf(p, "\r\n");

    send_bytes(conn, request, (int)(p - request));
    send_bytes(conn, body, body_size);
}

void write_response(conn_t conn, int status, headers_t headers, const void *body, int body_size)
{
    // cout << "write_response\n";
    
    headers["Content-Length"] = std::to_string(body_size);

    char response[4096];
    char *p = response;
    p += sprintf(p, "HTTP/1.1 %d\r\n", status);
    for (auto &it : headers) {
        p += sprintf(p, "%s:%s\r\n", it.first.c_str(), it.second.c_str());
    }
    p += sprintf(p, "\r\n");

    send_bytes(conn, response, (int)(p - response));
    send_bytes(conn, body, body_size);
}


std::string read_message(conn_t conn, headers_t &headers, std::vector<uint8_t> &body)
{
    headers.clear();
    body.clear();

    std::stringstream data;
    std::string start_line;

    while (true) {
        // try reading a line
        std::string line;
        std::getline(data, line);

        if (data.eof()) {
            // partial line
            data.clear();
            data << line;

            char buff[4096];
            int n = recv(conn, buff, sizeof(buff), 0);
            if (n == -1)
                error();

            data.write(buff, n);
        }

        if (start_line.empty()) {
            start_line = line;
            continue;
        }

        if (line == "\r") {
            break;
        }

        auto sep = line.find(':');
        std::string name = line.substr(0, sep);
        std::string value = line.substr(sep + 1, line.length() - sep - 2);
        headers[name] = value;
    }

    if (headers.find("Content-Length") != headers.end()) {
        int content_length = atoi(headers["Content-Length"].c_str());

        auto body_part = data.str().substr(data.tellg());
        body.insert(body.end(), body_part.c_str(), body_part.c_str() + body_part.length());

        while ((int)(body.size()) < content_length) {
            char buff[4096];
            int n = recv(conn, buff, sizeof(buff), 0);
            if (n == -1)
                error();

            body.insert(body.end(), buff, buff + n);
        }
    }

    return start_line;
}

std::string read_request(conn_t conn, headers_t &headers, std::vector<uint8_t> &body)
{
    auto start_line = read_message(conn, headers, body);
    auto method_end = start_line.find(' ');
    auto url_end = start_line.rfind(' ');
    return start_line.substr(method_end + 1, url_end - method_end - 1);
}

int read_response(conn_t conn, headers_t &headers, std::vector<uint8_t> &body)
{
    // cout << "read_response\n";
    auto start_line = read_message(conn, headers, body);
    int status;
    sscanf(start_line.c_str(), "HTTP/1.1 %d", &status);
    return status;
}

void run_server(int port, std::function<void(conn_t)> client_handler)
{
    conn_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET)
        error();

    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int)) < 0)
        error();
	
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
        error();

    if (listen(sockfd, 10) != 0)
        error();

    std::vector<std::pair<std::thread, bool>> client_list;
    std::mutex client_lock;

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);
        conn_t client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
        if (client_fd < 0)
            error();

        std::lock_guard<std::mutex> lock(client_lock);
        client_list.emplace_back([client_fd, &client_list, &client_handler, &client_lock]() {
            try {
                client_handler(client_fd);
            } catch (std::runtime_error) { }
#ifdef _WIN32
            closesocket(client_fd);
#else
            close(client_fd);
#endif

            std::lock_guard<std::mutex> lock(client_lock);
            for (auto &it : client_list) {
                if (it.first.get_id() == std::this_thread::get_id())
                    it.second = true;
            }
        }, false);

        for (auto &it : client_list) {
            if (it.second)
                it.first.join();
        }
        client_list.erase(
            std::remove_if(client_list.begin(), client_list.end(),
                           [](const auto &it) { return it.second; }),
            client_list.end());
     }
}

void print_headers(const headers_t& headers) {
    cout << "headers: \n";
    for (auto h : headers) {
        cout << h.first << " : (" << h.second << ")\n"; 
    }
}

void print_body(const body_t& body) {
    // cout << "body(" << body.size() <<  ") : \n";
    // for (int i = 0; i < body.size(); ++i) {
    //     if (body[i] == ' ') continue;

    //     printf("%4d", i / 2);
    // }
    // cout << "\n";

    for (auto b : body) {
        cout << b;
        // if (b == ' ') continue;
        // printf("%4d", b - '0');
    }
    cout << '\n';


    for (auto b : body) {
        cout << "(" << ( (int) (char) b) << ")" ;
    }
    cout << '\n';
}
