using namespace std;

class CryptographicError: public exception {

	public:

		char* what() {
			return "Kingslayer: fatal error: cryptographic error";
		}

};

class AccessDeniedError: public exception {

	public:

		char* what() {
			return "Kingslayer: fatal error: access denied";
		}

};