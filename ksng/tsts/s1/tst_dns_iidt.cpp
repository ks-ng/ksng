#include "../net/dns/dns.h"

int main() {
    cout << dns::ipInDNSTable("1.1.1.1");
    cout << dns::ipInDNSTable("2.2.2.2");
    cout << dns::ipInDNSTable("3.3.3.3") << endl;
}