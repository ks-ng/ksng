#include "../../net/nicr.h"

int main() {
	nicr::NICR nr("eth0");
	cout << nr.receiveData().hex() << endl;
}