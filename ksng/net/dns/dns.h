#pragma once

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../../util/dstruct/sll.h"
#include "../../util/fileops.h"

namespace dns {

    const string dnsTableFilename = "ksng/net/dns/dnstable.kingslayer.dat";

    void wipeDNSTable() {
        fileops::writeFile(dnsTableFilename, data::Bytes());
    }

    sll::SLL<string> lookupDNS(string hostname) {
        sll::SLL<string> result;
        ifstream tf(dnsTableFilename);
        string line;
        string word;
        stringstream ss;
        int i;
        while(getline(tf, line)) {
            ss = stringstream("");
            for (i = 0; i < line.length(); i++) {
                if (line[i] == ' ') {
                    break;
                } else {
                    ss << line[i];
                }
            }
            if (ss.str() == hostname) {
                ss = stringstream("");
                for (i++; i < line.length(); i++) {
                    ss << line[i];
                }
                result.append(ss.str());
            }
        }
        return result;
    }

    sll::SLL<string> getAllDNSTableIPs(string ip) {
        sll::SLL<string> result;
        ifstream tf(dnsTableFilename);
        string line;
        string word;
        stringstream ss;
        int i;
        while(getline(tf, line)) {
            for (i = 0; i < line.length(); i++) {
                if (line[i] == ' ') {
                    break;
                }
            }
            ss = stringstream("");
            for (i++; i < line.length(); i++) {
                ss << line[i];
            }
            result.append(ss.str());
        }
        return result;
    }

    bool ipInDNSTable(string ip) {
        ifstream tf(dnsTableFilename);
        string line;
        string word;
        stringstream ss;
        int i;
        while(getline(tf, line)) {
            for (i = 0; i < line.length(); i++) {
                if (line[i] == ' ') {
                    break;
                }
            }
            ss = stringstream("");
            for (i++; i < line.length(); i++) {
                ss << line[i];
            }
            if (ss.str() == ip) { return true; }
        }
        return false;
    }

    bool hostInDNSTable(string hostname) {
        sll::SLL<string> ips = lookupDNS(hostname);
        return (ips.getLength() != 0);
    }

    sll::SLL<string> makeDNSRequest(const string &host_name, int ipv=4) {
		sll::SLL<string> output;
	
		struct addrinfo hints, *res, *p;
		int status, ai_family;
		char ip_address[INET6_ADDRSTRLEN];
		ai_family = ipv==6 ? AF_INET6 : AF_INET;
		ai_family = ipv==0 ? AF_UNSPEC : ai_family;
		memset(&hints, 0, sizeof hints);
		hints.ai_family = ai_family; 
		hints.ai_socktype = SOCK_STREAM;
	
		if ((status = getaddrinfo(host_name.c_str(), NULL, &hints, &res)) != 0) {
			return output;
		}
		
		for(p = res;p != NULL; p = p->ai_next) {
			void *addr;
			if (p->ai_family == AF_INET) {
				struct sockaddr_in *ipv4 = (struct sockaddr_in*)p->ai_addr;
				addr = &(ipv4->sin_addr);
			} else {
				struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)p->ai_addr;
				addr = &(ipv6->sin6_addr);
			}
	
			inet_ntop(p->ai_family, addr, ip_address, sizeof ip_address);
			output.append(ip_address);
		}
	
		freeaddrinfo(res);
	
		return output;
	}

};