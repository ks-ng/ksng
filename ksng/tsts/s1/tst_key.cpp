#include "../crypto/core/key.h"

int main() {
	key::Key key = key::Key::random(256);
	key.reveal();
	cout << key.getByte(0) << endl;
	key.store("mykey.key");
}