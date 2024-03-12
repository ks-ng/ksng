#include "../util/xln.h"

int main() {
    xln::XLN n = {1, 1, 0, 0, 1, 1};
    cout << n.num() << endl << n.withLeadingZeros(2).num() << endl;
}