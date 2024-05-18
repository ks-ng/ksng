#include "../util/xln.h"

int main() {
    xln::XLN n = {1, 1, 1, 0, 0, 0, 1, 0, 0, 0};
    cout << n.num() << endl;
    cout << n.truncate(2).num() << endl;
}