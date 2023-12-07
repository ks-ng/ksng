using namespace std;

class KingslayerError: public exception {};

int panic(string message) {
	cout << endl << endl << "Kingslayer: fatal error: " << message << endl << endl;
	throw KingslayerError();
	return -1;
}