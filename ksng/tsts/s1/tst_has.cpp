#include "../util/data.h"

using namespace std;

int main() {
	data::Bytes x(3);
	x.set(0, 1);
	x.set(1, 2);
	x.set(2, 3);
	cout << x.has(0) << x.has(1) << x.has(2) << x.has(3) << x.has(4) << endl;
}
