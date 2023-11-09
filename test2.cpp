#include "ksng/web.cpp"

int main() {
	WebInterface wi;
	Response rsp = wi.get("http://www.google.com", "/");
	cout << rsp.repr() << endl;
}