#include <fstream>
#include <iostream>
#include <string>
#include <string.h> 

using namespace std;

struct Host {
	int ID;
	int parentID;
	Host* next;
};

struct Base {
	int ID;
	int parentID;
	Host* connectedHost; // points to a LL of MHs connected to the Base
	Base* connectedBase; // points to a LL of sub-Bases
	Base* next; // points to next Base on the same level, connected to same parent
};

struct List {
	Base* head;
	void create();
};

List sys;

void List::create() {
	List::head = NULL;
}

Base* getBase(int ID, Base* target) {
	Base* temp = NULL;
	if (target != NULL) {
		if (ID == target->ID) {
			return target;
		}
		temp = getBase(ID, target->connectedBase);
		if (temp != NULL) {
			return temp;
		}
		else {
			temp = getBase(ID, target->next);
		}
	}

	return temp;
	// add non-existing Base ID condition
	// currently returns NULL if no Base exists
}

void addBase(int ID, int parentID) {
	Base* newbase = new Base();
	newbase->ID = ID;
	newbase->parentID = parentID;
	newbase->next = NULL;
	newbase->connectedHost = NULL;
	newbase->connectedBase = NULL;
	if (ID == 0 && parentID == -1) { // add CC to sys
		sys.head = newbase; // make CC head
		return;
	}
	Base* parentBase = getBase(parentID, sys.head);
	if (parentBase->connectedBase == NULL) {
		parentBase->connectedBase = newbase;
	}
	else {
		Base* traverse = parentBase->connectedBase;
		while (traverse->next != NULL) {
			traverse = traverse->next;
		}
		traverse->next = newbase;
	}
}

void addHost(int ID, int parentID) {
	Host* newhost = new Host();
	newhost->ID = ID;
	newhost->parentID = parentID;
	newhost->next = NULL;
	Base* parentBase = getBase(parentID, sys.head);
	if (parentBase->connectedHost == NULL) {
		parentBase->connectedHost = newhost;
	}
	else {
		Host* traverse = parentBase->connectedHost;
		while (traverse->next != NULL) {
			traverse = traverse->next;
		}
		traverse->next = newhost;
	}
}

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
		char temp[4];
		int ID;
		int parentID;
		netFile.getline(nodeType, 3, ' ');
		netFile.getline(temp, 4, ' ');
		ID = atoi(temp);
		netFile.getline(temp, 4, '\n');
		parentID = atoi(temp);
		// TODO: Reading part needs to account for the extra newline inputs
		if (strcmp(nodeType, "BS") == 0) {
			addBase(ID, parentID); // add Base Stations
		}
		if (strcmp(nodeType, "MH") == 0) {
			addHost(ID, parentID); // add Mobile Host
		}
	}
	netFile.close();
	return true;
}

void traverse(Base* target) {
	if (target != NULL) {
		cout << target->ID << endl;
		if (target->connectedHost != NULL) {
			cout << " Hosts:";
			Host* traHost = target->connectedHost;
			while (traHost->next != NULL) {
				cout << ", " << traHost->ID;
				traHost = traHost->next;
			}
			cout << ", " << traHost->ID << endl;
		}
		traverse(target->connectedBase);
		traverse(target->next);
	}
}

int main(int argc, char* argv[]) {
	sys.create();
	addBase(0, -1); // add head base
	/*
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
	}*/
	char filename[255] = "Network.txt";
	processNetworks(filename);
	traverse(sys.head);

}
