#include "../quantum/qcore.h"

int main() {
    qcore::QuantumVector qv1 = {AMP1, AMP0};
    qcore::QuantumVector qv2 = {AMP0, AMP1};
    qcore::QuantumOperator qo = {{AMP0, AMP1}, 
                                 {AMP1, AMP0}};
    cout << "quantum vector 1: " << qv1.repr() << endl;
    cout << "negated quantum vector 1: " << (qo | qv1).repr() << endl;
    cout << "quantum vector 2: " << qv2.repr() << endl;
    cout << "negated quantum vector 2: " << (qo | qv2).repr() << endl;
    cout << "combined quantum vector: " << (qv1 * qv2).repr() << endl;
}