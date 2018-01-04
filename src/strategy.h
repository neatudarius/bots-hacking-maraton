#include "util.h"
#include <map>

using namespace std;

void strategy_bot_1036(conn_t &conn) {
  headers_t req_headers;
  std::vector<uint8_t> req_body;
  std::string request;

  for (int i = 0; i < 2; ++i) {
    write_response(conn, 200, {{"BotCommand", "xxx"}}, nullptr, 0);

    request = read_request(conn, req_headers, req_body);
    print_headers(req_headers);
    print_body(req_body);
  }

  write_response(conn, 200, {{"BotCommand", "show"}}, nullptr, 0);

  request = read_request(conn, req_headers, req_body);
  print_headers(req_headers);
  print_body(req_body);

  int n = 1, cnt = 10000;
  while (1) {
    string cmd = "move";
    int x = rand() % 9 + 1;
    int y = rand() % 9 + 1;
    // string line;
    // getline(cin, line);
    // istringstream is(line);
    // string x, y;

    // string cmd;
    // is >> cmd;

    cout << "----------------------------------------------\n";
    cout << "(" << cmd << " " << x << " " << y << " "
         << ")\n";

    headers_t req_headers;
    req_headers["BotCommand"] = cmd;
    req_headers["row"] = to_string(x);
    req_headers["col"] = to_string(y);

    // print_headers(req_headers);
    write_response(conn, 200, req_headers, nullptr, 0);

    if (req_headers.size()) {
      req_headers.clear();
    }

    if (req_body.size()) {
      req_body.clear();
    }
    std::string request = read_request(conn, req_headers, req_body);

    cout << "----------------------------------------------\n";

    if (request == "/valid-move") {
      n += 2;
    } else

        if (request == "/final-request-you-won") {
      n += 1;
      n = n / 2;

      cout << "request_recv: " << request << "\n";
      print_headers(req_headers);
      print_body(req_body);
      break;
    } else

        if (request == "/final-request-I-won") {
      n += 2;
      n = n / 2 + 1;

      cout << "request_recv: " << request << "\n";
      print_headers(req_headers);
      print_body(req_body);
      break;
    }

    write_response(conn, 200, {{"BotCommand", "show"}}, nullptr, 0);
    request = read_request(conn, req_headers, req_body);
    print_headers(req_headers);
    print_body(req_body);

    if (cnt-- == 0) {
      break;
    }
  }

  {
    write_response(conn, 200, {{"BotCommand", "show"}}, nullptr, 0);
    request = read_request(conn, req_headers, req_body);
    print_headers(req_headers);
    print_body(req_body);
  }

  {
    headers_t req_headers;
    std::vector<uint8_t> req_body;

    req_headers["BotCommand"] = "final";
    req_headers["rounds"] = to_string(n);
    cout << "request: " << req_headers["BotCommand"] << "\n";
    print_headers(req_headers);
    print_body(req_body);
    cout << "----------------------------------------------\n";

    write_response(conn, 200, req_headers, nullptr, 0);

    request = read_request(conn, req_headers, req_body);

    cout << "request: " << request << "\n";
    print_headers(req_headers);
    print_body(req_body);
    cout << "----------------------------------------------\n";
  }
}

typedef void (*strategy)(conn_t &conn);
map<int, strategy> id2strategy;

id2strategy[1036] = &strategy_bot_1036;