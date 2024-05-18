#include "../crypto/core/fragment.h"

using namespace std;

int main() {
	data::Bytes d = fileops::readFileBytes("t.dat");
	cout << "initial: " << d.hex() << endl;
	sda::SDA<data::Bytes> s;
	for (int i = 0; i < 10; i++) {
		s = fragment::split(d);
		cout << "final " << s.get(0).hex() << ", " << s.get(1).hex() << " reas to " << fragment::reassemble(s).hex() << endl;
	}
}