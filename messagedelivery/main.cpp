#include <fstream>
#include <iostream>
#include <string>

using namespace std;

bool processNetworks(char filename[]) {
	ifstream netFile;
	netFile.open(filename);
	if (!netFile) {
		cout << "Unable to open " << filename << endl;
		return false;
	}

	// read line by line and parse

	while (netFile.peek() != EOF) {
		char nodeType[3];
		char temp[3];
		int ID;
		int parentID;
		netFile.getline(nodeType, 3, ' ');
		netFile.getline(temp, 3, ' ');
		ID = atoi(temp);
		netFile.getline(temp, 3, '\n');
		parentID = atoi(temp);
		// TODO: Reading part needs to account for the extra newline inputs
	}

	netFile.close();
	return true;
}

int main(int argc, char* argv[]) {
	if (argc == 2) {
		// argv[1] network_file_name
		processNetworks(argv[1]);
		// argv[2] messages_file_name
	}
	else {
		char filename[255] = "Network.txt";
		if (!processNetworks(filename)) {
			cout << "ERROR: Too many or too less arguments" << endl;
			cout << "Execute the program using:" << endl;
			cout << "./executable network_file_name messages_file_name" << endl;
		}
	}
}