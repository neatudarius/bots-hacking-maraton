#include "util.h"
#include <stdlib.h>
#include <time.h>
using namespace std;

string dont_look_here =
    "http://theaigames.com/competitions/ultimate-tic-tac-toe/getting-started";

void default_handler(conn_t &conn);
void strategy_bot_121(conn_t &conn);
void strategy_bot_123(conn_t &conn);
void strategy_bot_1036(conn_t &conn);
void strategy_bot_1620(conn_t &conn);
void strategy_bot_1848(conn_t &conn);
void strategy_bot_1(conn_t &conn);

typedef void (*strategy)(conn_t &conn);
map<int, strategy> id2strategy = {
    {1036, &strategy_bot_1036}, {121, &strategy_bot_121},
    {123, &strategy_bot_123},   {1620, &strategy_bot_1620},
    {1848, &strategy_bot_1848}, {1, &strategy_bot_1}};

int main(int argc, char *argv[]) {
  srand(time(NULL));
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

    if (id2strategy.find(bot_version) != id2strategy.end()) {
      auto f = id2strategy[bot_version];
      f(conn);
    } else {
      cout << "nu stiu boss pe " << bot_version << "\n";
      default_handler(conn);
    }

    // strategy_bot_1036(conn);
  });

  return 0;
}

void default_handler(conn_t &conn) {
  headers_t req_headers;
  std::vector<uint8_t> req_body;
  std::string request;

  while (1) {
    string line;
    getline(cin, line);
    istringstream is(line);

    string cmd;
    // is >> cmd;
    cmd = line;

    cout << "(" << cmd << ")\n";
    write_response(conn, 200, {{"BotCommand", cmd}}, nullptr, 0);

    std::string request = read_request(conn, req_headers, req_body);
    cout << "request: " << request << "\n";
    print_headers(req_headers);
    print_body(req_body);
  }
}

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

void strategy_bot_121(conn_t &conn) {
  string gigel;
  {
    headers_t req_headers;
    std::vector<uint8_t> req_body;
    std::string request;

    write_response(conn, 200, {{"BotCommand", "uninstall"}}, nullptr, 0);

    request = read_request(conn, req_headers, req_body);
    cout << "request: " << request << "\n";
    print_headers(req_headers);
    print_body(req_body);

    gigel = req_headers["Remember1"];
  }

  {
    headers_t req_headers;
    std::vector<uint8_t> req_body;
    std::string request;
    write_response(conn, 200, {{"BotCommand", gigel}}, nullptr, 0);
    print_headers(req_headers);
    print_body(req_body);
  }
}

void strategy_bot_123(conn_t &conn) {
  unsigned int lower = 1000, upper = (1 << 17);

  while (lower <= upper) {
    unsigned int mid = (lower + upper) / 2;

    {
      headers_t req_headers;
      std::vector<uint8_t> req_body;
      std::string request;

      req_headers["BotCommand"] = to_string(mid);
      write_response(conn, 200, req_headers, nullptr, 0);
      print_headers(req_headers);
      print_body(req_body);

      if (req_headers.size()) {
        req_headers.clear();
      }

      request = read_request(conn, req_headers, req_body);
      cout << "request: " << request << "\n";
      print_headers(req_headers);
      print_body(req_body);

      istringstream is(req_headers["Compare"]);
      int x;
      is >> x;

      if (x == 0) {
        return;
      }

      if (x == -1) {
        lower = mid + 1;
        continue;
      }

      if (x == 1) {
        upper = mid - 1;
        continue;
      }
    }
  }
}

void strategy_bot_1620(conn_t &conn) {
  vector<string> tokens = {"P0", "P1", "P2", "P3", "y"};
  for (auto t : tokens) {
    headers_t req_headers;
    std::vector<uint8_t> req_body;
    std::string request;

    write_response(conn, 200, {{"BotCommand", t}}, nullptr, 0);
    cout << t << "\n";

    request = read_request(conn, req_headers, req_body);
    cout << "request: " << request << "\n";
    print_headers(req_headers);
    print_body(req_body);
  }
}

void strategy_bot_1848(conn_t &conn) {
  vector<string> tokens = {"start", "get_input", "check"};
  for (auto t : tokens) {
    headers_t req_headers;
    std::vector<uint8_t> req_body;
    std::string request;

    write_response(conn, 200, {{"BotCommand", t}}, nullptr, 0);
    cout << t << "\n";

    request = read_request(conn, req_headers, req_body);
    cout << "request: " << request << "\n";
    print_headers(req_headers);
    print_body(req_body);
  }
}

void strategy_bot_1(conn_t &conn) {
  headers_t req_headers;
  std::vector<uint8_t> req_body;
  std::string request;

  string cmd = " hello ";
  cout << "(" << cmd << ")\n";
  write_response(conn, 200, {{"BotCommand", cmd}}, nullptr, 0);

  request = read_request(conn, req_headers, req_body);
  cout << "request: " << request << "\n";
  print_headers(req_headers);
  print_body(req_body);

  // cmd = " hello ";
  // cout << "(" << cmd << ")\n";
  // write_response(conn, 200, {{"BotCommand", cmd}}, nullptr, 0);

  //  request = read_request(conn, req_headers, req_body);
  // cout << "request: " << request << "\n";
  // print_headers(req_headers);
  // print_body(req_body);

  int n = 43;
  while (1) {
    // string line;
    // getline(cin, line);
    // istringstream is(line);

    // string cmd;
    // is >> cmd;
    // cmd = line;

    cmd = " number ";

    cout << "(" << cmd << ")\n";
    write_response(conn, 200, {{"BotCommand", cmd}}, nullptr, 0);

    std::string request = read_request(conn, req_headers, req_body);
    cout << "request: " << request << "\n";
    print_headers(req_headers);
    print_body(req_body);
    cout << n << "\n";
    n = 2 * n + 1;

    if (n == -1) {
      break;
    }
  }

  n = 43;
  while (1) {
    // string line;
    // getline(cin, line);
    // istringstream is(line);

    // string cmd;
    // is >> cmd;
    // cmd = line;

    string cmd = "" + to_string(n) + "";

    cout << "(" << cmd << ")\n";
    write_response(conn, 200, {{"BotCommand", cmd}}, nullptr, 0);

    std::string request = read_request(conn, req_headers, req_body);
    cout << "request: " << request << "\n";
    print_headers(req_headers);
    print_body(req_body);
    n = 2 * n + 1;

    if (n == -1) {
      break;
    }
  }

  {
    headers_t req_headers;
    std::vector<uint8_t> req_body;
    std::string request;

    while (1) {
      string line;
      getline(cin, line);
      istringstream is(line);

      string cmd;
      // is >> cmd;
      cmd = line;

      cout << "(" << cmd << ")\n";
      write_response(conn, 200, {{"BotCommand", cmd}}, nullptr, 0);

      std::string request = read_request(conn, req_headers, req_body);
      cout << "request: " << request << "\n";
      print_headers(req_headers);
      print_body(req_body);
    }
  }
}