#include "util.h"
#include "tictactoe3d.h"

#define BOT_ID 1036

#define SHOW "show"
#define WHOAMI "whoami"
#define MOVE "move"
#define FINAL "final"

string dont_look_here0 = "http://theaigames.com/competitions/ultimate-tic-tac-toe/getting-started";

struct Client {
    conn_t conn;
    TicTacToe3D game;
    int expected = 0;
    int rounds = 0;
    bool done = false;
    string initial_msg = "486f77206d7563682074696d652064696420776520706c617920544f4745544845523f2028696e2068656164657220706c6561736520696e7365727420726f756e64733a2076616c756529";

    Client(char *server, int port) {
        conn = create_connection(server, port);
    }

    inline void get_version() {
        printf("get version\n");
        write_request(conn, "/", {{"BotVersion", to_string(BOT_ID)}});
    }

    inline  void whoami() {
        cout << WHOAMI << "\n";
       
        std::vector<uint8_t> req_body = { '2' };
        write_request(conn, "/whoami", {}, req_body.data(), (int)req_body.size());
    }

    inline void show() {
        // cout << SHOW << "\n";
           
        std::vector<uint8_t> req_body;
        string s = game.to_string();
        write_request(conn, "/show", {}, s.c_str(), s.size());
    }

    inline  void find_my_move() {
        for (int i = 1; i <= 9; ++i) {
            for (int j = 1; j <= 9; ++j) {
                if (game.check_is_valid(i, j)) {
                    // cout << MOVE << ' ' << i << ' ' << j << "\n";

                    game.move(CLIENT, i, j);
                    return;
                }
            }
        }
    }

    inline  void move(int i_big, int j_big) {
        // cout << MOVE << ' ' << i_big << ' ' << j_big << "\n";

        // cout << "SERVER:\n";        
        if (game.check_is_valid(i_big, j_big)) {
            // cout << "valid\n";
            game.move(SERVER, i_big, j_big);
            rounds++;
            
            if (game.is_done()) {
                expected = rounds / 2;
                done = true;


                std::vector<uint8_t> req_body;
                write_request(conn, "/final-request-you-won", {}, initial_msg.c_str(), initial_msg.size());
            } else {
                // cout << "CLIENT:\n";
                find_my_move();
                rounds++;


                if (game.is_done()) {
                    expected = rounds / 2 + 1;
                    done = true;

                    std::vector<uint8_t> req_body;
                    write_request(conn, "/final-request-I-won", {}, initial_msg.c_str(), initial_msg.size());
                } else {
                    std::vector<uint8_t> req_body;
                    write_request(conn, "/valid-move", {}, req_body.data(), (int)req_body.size());
                }
            }
        } else {
            // cout << "invalid\n";

            std::vector<uint8_t> req_body;
            write_request(conn, "/invalid-move", {}, req_body.data(), (int)req_body.size());
        }
    }
};

int main(int argc, char *argv[])
{
    net_init();

    char *server = argv[1];
    int port = atoi(argv[2]);
    
    Client client = Client(server, port);

    write_request(client.conn, "/", {});

    int no_message = true;
    int is_playing = false;

    int cnt = 100;
    while (true) {
        headers_t resp_headers;
        std::vector<uint8_t> resp_body;
        read_response(client.conn, resp_headers, resp_body);
        auto command = resp_headers["BotCommand"];
        // cout << "recv: " << command << "\n";

        if (command == "get_version") {
            client.get_version();
        } else if (command == SHOW && is_playing) {
            client.show();
        } else if (command == WHOAMI && is_playing) {
            client.whoami();
        } else if (command == MOVE && is_playing) {
            int x = atoi(resp_headers["row"].c_str());
            int y = atoi(resp_headers["col"].c_str());

            client.move(x, y);
            // cout << "end move\n";
        } else  if (command == FINAL && is_playing && client.done) {
            int n =  atoi(resp_headers["rounds"].c_str());
            int ret = (n == client.expected ? 0 : -1);

            string gigel = "GIGEL e pe val! Pup! Pa! GJ!";
            write_request(client.conn, "/CONGRATS", {}, gigel.c_str(), gigel.size());

            return ret;
        } else {
            if (no_message == true) {
                no_message = false;
                write_request(client.conn, "/default_action_fuck_you", {});
            } else {
                is_playing = true;
                cout << "start\n";

                std::vector<uint8_t> req_body;
                headers_t headers;
                headers["what are you doing?"] = "playing tic-tac-toe";
                headers[SHOW] = "show matrix: \"show\"";
                headers[WHOAMI] = "show your symbol: \"whoami\"";
                headers[MOVE] = "try to move at position x y: \"move x y\"";

                write_request(client.conn, "/lets_play_tictactoe", headers, req_body.data(), (int)req_body.size());
            }
        }
    }

    return 0;
}

