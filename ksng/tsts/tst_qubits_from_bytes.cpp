#include "../quantum/qstd.h"

int main() {
    data::Bytes d = {0xff, 0xaa};
    qstd::Qubits qr = qstd::Qubits::classicalToQuantum(d);
    cout << qr.binStates() << endl;
}