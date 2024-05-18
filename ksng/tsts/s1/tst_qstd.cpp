#include "../quantum/qstd.h"

int main() {
    qstd::Qubits qr = {qstd::Qubit(0), qstd::Qubit(0), qstd::Qubit(0)};
    cout << "quantum register initial state: " << qr.binStates() << endl;
    cout << "0 * 1 * 2 (combined state): " << ((qr.get(0) * qr.get(1)) * qr.get(2)).binStates() << endl;
    qr.applyOperator(0, qstd::PauliX());
    cout << "quantum register state after inverting qubit 0 (paulix @ 0): " << qr.binStates() << endl;
}