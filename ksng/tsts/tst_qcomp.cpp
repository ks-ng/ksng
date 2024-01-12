#include "../quantum/qcomp.h"

using namespace std;

int main() {
	qcomp::QR qr(4); // quantum nybble (qunybble?)
	for (int i = 0; i < 4; i++) {
		qr.get(i).initialize(1, PI * i / 4);
	}
	cout << qr.qubitRepr() << endl;
}