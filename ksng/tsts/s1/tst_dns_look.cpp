#include "../net/dns/dns.h"

int main() {
    sll::SLL<string> ips = dns::lookupDNS("www.google.com");
    cout << ips.repr() << endl;
}