#include <fstream>
#include <iostream>
#include <string>
#include <string.h> 

using namespace std;

struct Host { // reffered to Mobile Host as Host
	int ID; // ID of Host
	int parentID; // ID of connected Base Station
	Host* next; // points to next Host connected to same Base Station (used to traverse Hosts connected to same Base Station)
};

struct Base { // referred to Base Station as Base
	int ID; // ID of Base Station
	int parentID; // ID of connected Base Station
	Host* connectedHost; // points to a LL of MHs connected to the Base
	Base* connectedBase; // points to a LL of sub-Bases
	Base* next; // points to next Base on the same level, connected to same parent (used to traverse connectedBases)
};

struct List {
	Base* head; // points to Central Controller
	void create();
	void close(Base* target);
};

List sys;

void List::create() {
	List::head = NULL;
}

void List::close(Base* target) {
	if (target != NULL) {
		close(target->connectedBase);
		close(target->next);
		if (target->connectedHost != NULL) {
			Host* traHost = target->connectedHost;
			Host* temp;
			while (traHost != NULL) {
				temp = traHost;
				traHost = traHost->next;
				//cout << "Deleted Host: " << temp->ID << " which was connected to Base: " << temp->parentID << endl;
				delete temp;
			}
		}
		//cout << "Deleted Base: " << target->ID << " which was connected to Base: " << target->parentID << endl;
		delete target;
	}
}

Base* getBase(int ID, Base* target) {
	//recursive inorder traversal to get Base
	Base* temp = NULL;
	if (target != NULL) {
		if (ID == target->ID) {
			return target;
		}
		temp = getBase(ID, target->connectedBase);
		if (temp != NULL) {
			return temp; // return the Base if it was found in a connected Base
		}
		else {
			temp = getBase(ID, target->next); // otherwise check the next Base connected to the same parent Base
		}
	}

	return temp;
	// returns NULL if Base can't be found
}

void addBase(int ID, int parentID) {
	Base* newbase = new Base();
	newbase->ID = ID;
	newbase->parentID = parentID;
	newbase->next = NULL;
	newbase->connectedHost = NULL;
	newbase->connectedBase = NULL;
	if (ID == 0 && parentID == -1) { // add Central Controller to sys
		sys.head = newbase; // make CC head
		return;
	}
	Base* parentBase = getBase(parentID, sys.head);
	if (parentBase->connectedBase == NULL) { // if there are no connected Bases
		parentBase->connectedBase = newbase; // add as the first connected Base
	}
	else {
		Base* traverse = parentBase->connectedBase;
		while (traverse->next != NULL) {
			traverse = traverse->next;
		}
		traverse->next = newbase; // add as the last connected Base
	}
}

void addHost(int ID, int parentID) {
	Host* newhost = new Host();
	newhost->ID = ID;
	newhost->parentID = parentID;
	newhost->next = NULL;
	Base* parentBase = getBase(parentID, sys.head);
	if (parentBase->connectedHost == NULL) { // if there are no connected Hosts to that Base
		parentBase->connectedHost = newhost; // add as the first connected Host
	}
	else {
		Host* traverse = parentBase->connectedHost;
		while (traverse->next != NULL) {
			traverse = traverse->next;
		}
		traverse->next = newhost; // add as the last connected Host
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
			addBase(ID, parentID); // add as Base Station
		}
		if (strcmp(nodeType, "MH") == 0) {
			addHost(ID, parentID); // add as Mobile Host
		}
	}
	netFile.close();

	return true;
}

Host* findHost(Base* target, int ID) {
	Host* search = NULL;
	if (target != NULL) {
		cout << target->ID << " ";
		Host* traHost = target->connectedHost;
		while (traHost != NULL) {
			if (traHost->ID == ID) {
				return traHost;
			}
			traHost = traHost->next;
		}
		search = findHost(target->connectedBase, ID);
		if (search != NULL) {
			return search;
		}
		else {
			search = findHost(target->next, ID);
		}
	}
	return search;
}

void printMessage(char message[], int ID) {
	cout << "Traversing:"; // print visited Base Stations
	Host* target = findHost(sys.head, ID); // get the desired Host
	cout << endl;
	if (target != NULL) {
		// TODO: needs improvement
		Base* parentBase = getBase(target->parentID, sys.head);
		int loc[255]; // store direct location in reverse
		int i = 0;
		loc[i] = parentBase->ID;
		while (parentBase->parentID != -1) {
			parentBase = getBase(parentBase->parentID, sys.head);
			loc[++i] = parentBase->ID;
		}
		cout << "Message:" << message << " To:"; // print the message
		for (int k = i; k >= 0; k--) {
			cout << loc[k] << " "; // print direct location
		}
		cout << "mh_" << ID << endl; // print delivered mobile host ID
	}
	else {
		cout << "Can not be reached the mobile host mh_" << ID << " at the moment" << endl; // if MH can not be found
	}
}

bool processMessages(char filename[]) {
	ifstream msgFile;
	msgFile.open(filename);

	if (!msgFile) {
		cout << "Could not open " << filename << endl;
		return false;
	}

	while (msgFile.peek() != EOF) {
		char message[51];
		char temp[10];
		int ID;
		msgFile.getline(message, 51, '>');
		msgFile.getline(temp, 10, '\n');
		if (strlen(temp) != 0) { // inorder to ignore extra newlines at the end of file
			ID = atoi(temp);
			printMessage(message, ID);
		}
	}

	msgFile.close();
	return true;
}

int main(int argc, char* argv[]) {
	sys.create(); // initiate the list
	addBase(0, -1); // add Central Controller to the list
	/*
	if (argc == 3) {
		processNetworks(argv[1]); // argv[1] network_file_name
		processMessages(argv[2]); // argv[2] messages_file_name
	}
	else {
		char netfilename[255] = "Network.txt";
		if (!processNetworks(netfilename)) {
			cout << "Too many or too less arguments" << endl;
			cout << "Execute the program using:" << endl;
			cout << "./executable network_file_name messages_file_name" << endl;
			return 1;
		}
		char msgfilename[255] = "Messages.txt";
		if (!processMessages(msgfilename)) {
			cout << "Too many or too less arguments" << endl;
			cout << "Execute the program using:" << endl;
			cout << "./executable network_file_name messages_file_name" << endl;
			return 1;
		}
	}*/
	char netfilename[255] = "Network.txt";
	processNetworks(netfilename);
	char msgfilename[255] = "Messages.txt";
	processMessages(msgfilename);
	sys.close(sys.head);
}
