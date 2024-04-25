#include "../net/dns/dns.h"

int main() {
    sll::SLL<string> ips = dns::makeDNSRequest("www.google.com", 4);
    cout << ips.repr() << endl;
}