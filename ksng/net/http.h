#include "dns/dns.h"
#include "../util/data.h"
#include "conn.h"

namespace http {

	data::Bytes GET(string url, string page="/", string lang="en-us") {
		stringstream ss;
		ss << "GET " << page << " HTTP/1.1\r\n";
		ss << "Host: " << url << "\r\n";
		ss << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.8) Gecko/20091102 Firefox/3.5.5\r\n";
		ss << "Accept: text/html,application/xhtml+xml,\r\n";
		ss << "Accept-Language: en-us\r\n";
		ss << "Accept-Encoding: gzip,deflate\r\n";
		ss << "Accept-Charset: ISO-8859-1,utf-8\r\n";
		ss << "Cache-Control: no-cache\r\n";
		ss << "Connection: close\r\n\r\n";
		data::Bytes request(ss.str());
		string ip = dns::lookupDNS(url).get(0);
		conn::TCPConnection tcp(ip.c_str(), 80);
		tcp.establish();
		tcp.transmit(request);
		sll::SLL<data::Bytes> results;
		data::Bytes r;
		data::Bytes l;
		while (true) {
			l = r;
			r = tcp.receive();
			cout << "Received packet: " << r.getLength() << endl;
			if (r.getLength() != 0 && !(r == l)) {
				results.append(r);
			} else { results.append(tcp.receive()); break; }
		}
		r = results.get(0);
		for (int i = results.getLength() - 1; i > 0; i--) {
			r = r + results.get(i);
		}
		return r;
	}

};