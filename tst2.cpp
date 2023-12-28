#include "ksng/hazmat/hazmat.h"

using namespace std;

int main() {
	data::Bytes d = hazmat::read("fn1");
	cout << d.hex() << endl;
}