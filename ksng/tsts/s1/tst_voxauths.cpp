#include "../crypto/ciphers/vox.h"

int main() {
	cout << voxauth::VOX_AUTH_CALL.astext() << " == " << voxauth::VOX_AUTH_CALL.hex() << endl;
	cout << voxauth::VOX_AUTH_RESP.astext() << " == " << voxauth::VOX_AUTH_RESP.hex() << endl;
}