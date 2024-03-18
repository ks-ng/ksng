#include "../util/data.h"

int main() {
    data::Bytes B = {0xff, 0xaa, 0x11, 0x00};
    cout << B.hex() << endl;
    data::Bits b = data::bytesToBits(B);
    cout << b.bin() << endl;
    cout << data::bitsToBytes(b).hex() << endl;
}