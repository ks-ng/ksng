#include "../net/http.h"

int main() {
	data::Bytes d = http::GET("www.google.com", "/");
	cout << d.astext() << endl;
}