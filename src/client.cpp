#include <stdio.h>
#include <iterator>
#include <fstream>
#include "util.h"

int main(int argc, char *argv[])
{
    net_init();

    char *server = argv[1];
    int port = atoi(argv[2]);
    conn_t conn = create_connection(server, port);

    write_request(conn, "/", {});

    while (true) {
        headers_t resp_headers;
        std::vector<uint8_t> resp_body;
        read_response(conn, resp_headers, resp_body);
        auto command = resp_headers["BotCommand"];

        if (command == "get_version") {
            printf("get version\n");
            write_request(conn, "/", {{"BotVersion", "1"}});
        } else if (command == "upload_self") {
            printf("upload self\n");
            std::ifstream selff(argv[0], std::ios::binary);
            std::vector<uint8_t> req_body((std::istreambuf_iterator<char>(selff)),
                                           std::istreambuf_iterator<char>());
            write_request(conn, "/", {}, req_body.data(), (int)req_body.size());
        } else if (command == "exit") {
            break;
        } else {
            write_request(conn, "/", {});
        }
    }

    return 0;
}

