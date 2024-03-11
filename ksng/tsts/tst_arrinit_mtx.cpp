#include "../util/dstruct/sda.h"

int main() {
    sda::SDM<int> matrix = {{1, 2}, {3, 4}};
    cout << matrix.repr() << endl;
}