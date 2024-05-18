#include "../crypto/hashes/sha.h"

int main() {
    sha::SHA_512 sha;
    data::Bytes pt = {0, 1, 2, 3, 4, 5, 6, 7};
    cout << sha.hash(pt).hex() << endl;
}