#include "../quantum/qcore.h"

int main() {
    qcore::QuantumVector qv = {AMP1, AMP0};
    cout << qv.repr() << endl;
}