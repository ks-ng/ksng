#include "../quantum/qstd.h"

int main() {
    data::Bits b = {0, 0, 0, 1, 1, 1, 0, 0};
    qstd::Qubits qr = qstd::Qubits::classicalToQuantum(b);
    cout << qr.binStates() << endl;
}