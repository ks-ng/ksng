#include "pktd.h"

namespace smake {

	pktd::Packet smakeUDPWithEth(string eths, string ethd, string ips, string ipd, unsigned int sport, unsigned int dport, data::Bytes payload) {
		pktd::Packet pkt;
		pkt.eth.etht = 2048;
		pkt.eth.src = eths;
		pkt.eth.dst = ethd;
		pkt.ipv4 = pktd::layers::IPv4(ips, ipd, 0x11);
		pkt.ipv4.ipid = 7777;
		pkt.udp.length = payload.getLength() + 8;
		pkt.udp.srcp = sport;
		pkt.udp.dstp = dport;
		pkt.payload = payload;
		pkt.fixUDP();
		pkt.fixIPv4Length();
		return pkt;
	}

	pktd::Packet smakeUDP(string ips, string ipd, unsigned int sport, unsigned int dport, data::Bytes payload) {
		pktd::Packet pkt;
		pkt.ipv4 = pktd::layers::IPv4(ips, ipd, 0x11);
		pkt.ipv4.ipid = 7777;
		pkt.udp.length = payload.getLength() + 8;
		pkt.udp.srcp = sport;
		pkt.udp.dstp = dport;
		pkt.udp.chk = 0;
		pkt.payload = payload;
		pkt.fixUDP();
		pkt.fixIPv4Length();
		return pkt;
	}

	pktd::Packet smakeTCP(string ips, string ipd, unsigned int sport, unsigned int dport, unsigned char flags, data::Bytes payload) {
		pktd::Packet pkt; 
		pkt.ipv4 = pktd::layers::IPv4(ips, ipd, 0x11);
		pkt.ipv4.ipid = 7777;
		pkt.tcp.srcp;
		pkt.tcp.dstp;
		pkt.tcp.seq;
		pkt.tcp.ackn;
		pkt.tcp.dataoff;
		pkt.tcp.flags;
		pkt.tcp.window;
		pkt.tcp.chk;
		pkt.tcp.urgptr;
		return pkt;
	}

};