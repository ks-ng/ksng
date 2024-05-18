#include "../quantum/qstd.h"

int main() {
    qstd::QV x = {qcore::AMPLITUDE(0.5, 0), qcore::AMPLITUDE(0.5, 0), qcore::AMPLITUDE(0.1, 0)};
    x.normalize();
    cout << x.binStates() << endl;
    for (int i = 0; i < 1000; i++) {
        cout << x.collapse();
    }
    cout << endl;
}