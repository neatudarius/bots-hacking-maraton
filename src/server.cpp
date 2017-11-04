#include <stdio.h>
#include "util.h"

int main(int argc, char *argv[])
{
    net_init();
    int port = atoi(argv[1]);

    run_server(port, [](conn_t conn) {
        headers_t req_headers;
        std::vector<uint8_t> req_body;
        std::string request = read_request(conn, req_headers, req_body);
        write_response(conn, 200, {{"BotCommand", "get_version"}}, nullptr, 0);

        request = read_request(conn, req_headers, req_body);
        int bot_version = atoi(req_headers["BotVersion"].c_str());
        printf("bot version %d\n", bot_version);

        write_response(conn, 200, {{"BotCommand", "upload_self"}}, nullptr, 0);
        request = read_request(conn, req_headers, req_body);
        // bot content is in req_body
        printf("bot size %d\n", int(req_body.size()));

        write_response(conn, 200, {{"BotCommand", "exit"}}, nullptr, 0);
    });

    return 0;
}

