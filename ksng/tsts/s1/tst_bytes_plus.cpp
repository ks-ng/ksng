#include "../util/data.h"

int main() {
    data::Bytes d1 = {1, 2, 3};
    data::Bytes d2 = {4, 5, 6};
    data::Bytes d3 = d1 + d2;
    cout << d3.hex() << endl;
}