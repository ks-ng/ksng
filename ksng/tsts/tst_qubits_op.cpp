#include "../quantum/qcomp.h"

using namespace qcore;
using namespace qcomp;

int main() {
    Qubits qr = {Qubit(AMP1, AMP0), Qubit(AMP1, AMP0), Qubit(AMP0, AMP1)};
    QO qo = {{AMP0, AMP1}, {AMP1, AMP0}};
    cout << qr.binStates() << endl;
    qr.applyOperator(1, qo);
    cout << qr.binStates() << endl;
}