#include "../net/dns/dns.h"

int main() {
    cout << dns::hostInDNSTable("www.google.com");
    cout << dns::hostInDNSTable("hello.world");
    cout << dns::hostInDNSTable("not.real") << endl;
}