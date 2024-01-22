#include "../net/nicr.h"

using namespace std;

int main() {
	nicr::NICR nr("eth0");
	cout << "awaiting input, ping this machine to complete test" << endl;
	cout << nr.receiveData().hex() << endl;
}
