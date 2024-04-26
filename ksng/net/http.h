#include "dns/dns.h"
#include "../util/data.h"
#include "conn.h"

namespace http {

	data::Bytes GET(string url, string page="/") {
		stringstream ss;
		ss << "GET " << page << " HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
		data::Bytes request(ss.str());
		string ip = dns::lookupDNS(url).get(0);
		conn::TCPConnection tcp(ip.c_str(), 80);
		tcp.establish();
		tcp.transmit(request);
		data::Bytes result = tcp.receive();
		return result;
	}

};