#include <iostream>

int read_message(string s) {
	if (s == "gigel") {
		return 1;
	}

	return 0;
}

int main() {
	string line;
	getline(cin, line);

	int n = 0;
	if (read_message("vasile")) {
		n = 1;
	}

	return 0;
}