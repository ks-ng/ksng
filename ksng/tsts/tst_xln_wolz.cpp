#include "../util/xln.h"

int main() {
    xln::XLN n = {0, 0, 1, 0, 1, 0, 0, 0, 1, 1};
    cout << n.num() << endl << n.withoutLeadingZeros().num() << endl;
}