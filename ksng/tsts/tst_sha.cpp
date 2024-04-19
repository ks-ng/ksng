#include "../crypto/hashes/sha.h"

int main() {
    sha::SHA_256 sha256;
    data::Bytes pt = {0, 1, 2, 3, 4, 5, 6, 7};
    cout << sha256.hash(pt).hex() << endl;
}