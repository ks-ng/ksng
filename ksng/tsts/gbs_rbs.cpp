#include "../util/notif.h"

int main() {
	cout << getBeforeSymbol("a/b/c", "/") << endl;
	cout << removeBeforeSymbol("a/b/c", "/") << endl;
}