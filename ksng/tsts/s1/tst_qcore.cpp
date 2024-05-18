#include "../quantum/qcore.h"

int main() {
    qcore::QuantumVector qv1 = {AMP1, AMP0};
    qcore::QuantumVector qv2 = {AMP0, AMP1};

    qcore::QuantumOperator qo = {{AMP0, AMP1}, 
                                 {AMP1, AMP0}};
                                 
    cout << "quantum vector 1: " << qv1.binStates() << endl;
    cout << "negated quantum vector 1: " << (qo | qv1).binStates() << endl;
    cout << "quantum vector 2: " << qv2.binStates() << endl;
    cout << "negated quantum vector 2: " << (qo | qv2).binStates() << endl;
    cout << "combined quantum vector: " << (qv1 * qv2).binStates() << endl;
    sda::SDA<qcore::QV> decomp = (qv1 * qv2).extractQuantumData();
    cout << "extracted quantum data: " << endl << decomp.get(0).binStates() << endl << decomp.get(1).binStates() << endl;
}