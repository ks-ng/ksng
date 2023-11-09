#include "ksng/q.cpp"

int main() {
	Ket qs(256);
	Bra state(128);
	cout << (state | qs) << endl;
}