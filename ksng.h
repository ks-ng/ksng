#pragma once
#include <netinet/ether.h>
#include <unistd.h>
#include <bitset>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <gmp.h> // for really really big ints
#include <time.h>
#include <string>
#include <net/if.h>
#include <cstdlib>
#include <sstream>
// Kingslayer-NG
// This file was originally SEVERAL DIFFERENT HEADERS and may not properly function!

// ksng/ksng.h



// ksng/quantum/qcore.h
#define AMP1 complex<double>(1,0)
#define AMP0 complex<double>(0,0)

using namespace std;

namespace qcore {

    bool isPowOf2(int x) {
        int y = 1;
        while (y < x) { y *= 2; }
        return x == y;
    }

    using AMPLITUDE = complex<double>;

    class QuantumVector: public sda::SecureDataArray<AMPLITUDE> {

        public:

            QuantumVector() {}
			QuantumVector(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			QuantumVector(initializer_list<AMPLITUDE> initList) {
				length = initList.size();
				elements = new AMPLITUDE[length];
				locked = false; // By default, not locked when initialized with a list
				securityLevel = ALERT; // Default security level
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

            AMPLITUDE operator|(QuantumVector other) {
                if (other.getLength() != getLength()) {
                    stringstream ss;
                    ss << "incompatible kets: size " << getLength() << " and size " << other.getLength();
                    notif::error(ss.str());
                    return AMP0;
                } else {
                    AMPLITUDE result;
                    for (int i = 0; i < getLength(); i++) {
                        result += get(i) * other.get(i);
                    }
                    return result;
                }
            }

            QuantumVector operator*(QuantumVector b) {
                QuantumVector output(getLength() * b.getLength());
                int index = 0;
                for (int i = 0; i < getLength(); i++) {
                    for (int j = 0; j < b.getLength(); j++) {
                        output.set(index, get(i) * b.get(j));
                        index++;
                    }
                }
                return output;
            }

            AMPLITUDE magnitude() {
                AMPLITUDE result;
                for (int i = 0; i < getLength(); i++) {
                    result += pow(get(i), 2);
                }
                return sqrt(result);
            }

            void normalize() {
                AMPLITUDE mag = magnitude();
                for (int i = 0; i < getLength(); i++) { set(i, get(i) / mag); }
            }

            string intStates() {
                stringstream ss;
                ss << "< ";
                for (int i = 0; i < getLength(); i++) {
                    ss << get(i) << "|" << i << "〉";
                    if (i != getLength() - 1) {
                        ss << "+ ";
                    }
                }
                ss << ">";
                return ss.str();
            }

            string binStates() {
                stringstream ss;
                ss << "< ";
                const int bitlength = ceil(log2(getLength()));
                for (int i = 0; i < getLength(); i++) {
                    ss << get(i) << "|";
                    for (int j = bitlength - 1; j >= 0; j--) {
                        if ((i >> j) % 2 == 1) {
                            ss << "1";
                        } else {
                            ss << "0";
                        }
                    }
                    ss << "〉";
                    if (i != getLength() - 1) {
                        ss << "+ ";
                    }
                }
                ss << ">";
                return ss.str();
            }

            sda::SDA<QuantumVector> extractQuantumData(bool autoNormalize=false) {
				int len = getLength();
				int log2len = log2(len);
				if (!isPowOf2(len)) {
					notif::error("cannot provide binary decomposition of non-binary ket");
					return sda::SDA<QuantumVector>(0);
				}

				sda::SDA<QuantumVector> results(log2len);
				for (int i = 0; i < log2len; i++) { results.get(i).initialize(2); }
				QuantumVector qv;
				
				for (int i = 0; i < len; i++) {
					for (int bitIndex = 0; bitIndex < log2len; bitIndex++) {
						qv = results.get(bitIndex);
						int bit = (i >> bitIndex) % 2;
						qv.set(bit, qv.get(bit) + get(i));
					}
				}

				if (autoNormalize) { for (int i = 0; i < log2len; i++) { results.get(i).normalize(); } }

				return results.reverse();
			}

            AMPLITUDE amp(int x) {
                return get(x);
            }

            double prob(int x) {
                return pow(abs(get(x)), 2);
            }

            int collapse() {
                srand(time(nullptr) + rand());
                double m = 0.0;
                for (int i = 0; i < getLength(); i++) {
                    m += prob(i);
                }
                double n = static_cast<double>(rand()) / static_cast<double>(RAND_MAX/m);
                for (int i = 0; i < getLength(); i++) {
                    n -= prob(i);
                    if (n <= 0) { return i; }
                }
                return 0;
            }

    };

    using QV = QuantumVector;

    class QuantumOperator: public sda::SecureDataMatrix<AMPLITUDE> {

        public:

            QuantumOperator() {}
			QuantumOperator(int rows, int cols, bool locked=false, Severity securityLevel=ALERT) { initialize(rows, cols, locked, securityLevel); }

			QuantumOperator(initializer_list<initializer_list<AMPLITUDE>> initList) {
				initialize(initList.begin()->size(), initList.size(), false, ALERT);
				int i = 0, j = 0;
				for (const auto& row : initList) {
					for (const auto& val : row) {
						elements[i][j] = val;
						j++;
					} 
					i++;
					j = 0;
				}
				cout << endl;
			}

            inline int getInputSize() { return getRows(); }
            inline int getOutputSize() { return getCols(); }

            QuantumVector operator|(QuantumVector vec) {
                if (vec.getLength() != getInputSize()) {
                    stringstream ss;
                    ss << "bad operator size for vector: operator input size " << getRows() << ", vector size " << vec.getLength();
                    notif::fatal(ss.str());
                    return QuantumVector(0);
                } else {
                    QuantumVector output(getOutputSize());
                    for (int i = 0; i < output.getLength(); i++) { output.set(i, AMP0); }
                    for (int i = 0; i < vec.getLength(); i++) {
                        for (int j = 0; j < getOutputSize(); j++) {
                            output.set(j, output.get(j) + vec.get(i) * get(i, j));
                        }
                    }
                    return output;
                }
            }

    };

    using QO = QuantumOperator;

};


// ksng/quantum/qstd.h

namespace qstd {

	complex<double> im(0.0, 1.0);
	double sqrt2 = sqrt(2);
	double invsqrt2 = 1 / sqrt2;
	complex<double> AMPsqrt2(sqrt2, 0);
	complex<double> AMPinvsqrt2(invsqrt2, 0);

	using namespace qcore;
	using namespace qcomp;

	QuantumOperator Erasure(int size=2) {
		QuantumOperator result(size, size);
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				result.set(i, j, AMP0);
			}
		}
		return result;
	}

	QuantumOperator Controlled(QuantumOperator op) {
		QuantumOperator result = Erasure(op.getRows() * 2);
		for (int i = 0; i < op.getRows(); i++) {
			result.set(i, i, AMP1);
		}
		int r = op.getRows();
		int c = op.getCols();
		for (int i = 0; i < r; i++) {
			for (int j = 0; j < c; j++) {
				result.set(i + r, j + c, op.get(i, j));
			}
		}
		return result;
	}

	inline QuantumOperator GlobalPhase(double phase) {
		AMPLITUDE iphase = exp(im * AMPLITUDE(phase));
		QO r = {{iphase, AMP0},
				{AMP0, iphase}};
		return r;
	}

	inline QuantumOperator PauliX() {
		QO r = {{AMP0, AMP1},
				{AMP1, AMP0}};
		return r;
	}

	inline QuantumOperator NOT() { return PauliX(); }

	inline QuantumOperator PauliY() {
		QO r = {{AMP0, -im},
				{im, AMP0}};
		return r;
	}

	inline QuantumOperator PauliZ() {
		QO r = {{AMP1, AMP0},
				{AMP0, -AMP1}};
		return r;
	}

	inline QuantumOperator SPhase() {
		QO r = {{AMP1, AMP0},
				{AMP0, im}};
		return r;
	}

	inline QuantumOperator SqrtZ() { return SPhase(); } 

	inline QuantumOperator SqrtX() {
		QO r = {{AMPLITUDE(1, 1) / AMPLITUDE(2, 0),  AMPLITUDE(1, -1) / AMPLITUDE(2, 0)},
				{AMPLITUDE(1, -1) / AMPLITUDE(2, 0), AMPLITUDE(1, 1) / AMPLITUDE(2, 0)}};
		return r;
	}

	inline QuantumOperator SqrtNOT() { return SqrtX(); }

	inline QuantumOperator Hadamard() {
		QO r = {{AMPinvsqrt2, AMPinvsqrt2},
				{AMPinvsqrt2, -AMPinvsqrt2}};
		return r;
	}

	inline QuantumOperator H() { return Hadamard(); }

}



// ksng/quantum/qcomp.h

namespace qcomp {

    inline qcore::QuantumVector Qubit(qcore::AMPLITUDE alpha, qcore::AMPLITUDE beta) {
        qcore::QV result = {alpha, beta};
        return result;
    }

    inline qcore::QuantumVector Qubit(int value) {
        if (value == 0) {
            qcore::QV result = {AMP1, AMP0};
            return result;
        } else {
            qcore::QV result = {AMP0, AMP1};
            return result;
        }
    }

    class Qubits: public sda::SecureDataArray<qcore::QuantumVector> {

        public:

            Qubits() {}
			Qubits(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			Qubits(initializer_list<qcore::QV> initList) {
				length = initList.size();
				elements = new qcore::QV[length];
				locked = false; // By default, not locked when initialized with a list
				securityLevel = ALERT; // Default security level
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

            string intStates(string delimiter=(string)(" ")) {
                stringstream ss;
                for (int i = 0; i < getLength(); i++) {
                    ss << get(i).intStates();
                    if (i < getLength() - 1) { ss << delimiter; }
                }
                return ss.str();
            }

            string binStates(string delimiter=(string)(" ")) {
                stringstream ss;
                for (int i = 0; i < getLength(); i++) {
                    ss << get(i).binStates();
                    if (i < getLength() - 1) { ss << delimiter; }
                }
                return ss.str();
            }

            void applyOperator(initializer_list<int> targetQubits, qcore::QuantumOperator op) {
                qcore::QV v = {AMP1};
                bool firstDone = false;
                for (const auto& index : targetQubits) {
                    if (firstDone) {
                        v = v * get(index);
                    } else {
                        v = get(index);
                        firstDone = true;
                    }
                }
                v = (op | v);
                sda::SDA<qcore::QV> quantumData = v.extractQuantumData();
                int i = 0;
                for (const auto& index : targetQubits) {
                    set(index, quantumData.get(i));
                    i++;
                }
            }

            void applyOperator(int targetQubit, qcore::QO op) {
                set(targetQubit, op | get(targetQubit));
            }

            static Qubits classicalToQuantum(data::Bits classical) {
                Qubits result(classical.getLength());
                for (int i = 0; i < classical.getLength(); i++) {
                    result.set(i, Qubit(classical.get(i)));
                }
                return result;
            }

            static Qubits classicalToQuantum(data::Bytes classical) { return Qubits::classicalToQuantum(data::bytesToBits(classical)); }

    };

    bool WARN_FOR_INDETERMINACY = true;

    data::Bits qubitsToBits(Qubits qr) {
        if (WARN_FOR_INDETERMINACY) {
            WARNED_FOR_INDETERMINACY = false;
            notif::warning("quantum data is not determinate and may yield different results each time measurements are taken");
        }
        data::Bits result(qr.getLength());
        for (int i = 0; i < qr.getLength(); i++) {
            result.set(i, qr.get(i).collapse());
        }
        return result;
    }

    inline data::Bytes qubitsToBytes(Qubits qr) {
        return data::bitsToBytes(qubitsToBits(qr));
    }

    inline Qubits bitsToQubits(data::Bits classical) { return Qubits::classicalToQuantum(classical); }
    inline Qubits bytesToQubits(data::Bytes classical) { return Qubits::classicalToQuantum(classical); }

};


// ksng/net/conn.h


using namespace std;

namespace conn {

	class Connection {

		protected:

			sockaddr_in serverAddress;

		public:

			virtual ~Connection() {}

			virtual void establish() = 0;
			virtual void transmit(data::Bytes msg) = 0;
			virtual data::Bytes receive() = 0;

	};

	class TCPConnection: public Connection {

		private:

			int sock = socket(AF_INET, SOCK_STREAM, 0);

		public:

			TCPConnection() {}
			TCPConnection(const char* ip, int port) {
				if (sock < 0) {
					notif::fatal("error building socket");
				}

				memset(&serverAddress, 0, sizeof(serverAddress));
				serverAddress.sin_family = AF_INET;
				inet_pton(AF_INET, ip, &serverAddress.sin_addr);
				serverAddress.sin_port = htons(port);
			}

			void establish() override {
				if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { notif::fatal("could not establish connection"); }
			}

			void transmit(data::Bytes msg) override {
				unsigned char data[msg.getLength()];
				for (int i = 0; i < msg.getLength(); i++) { data[i] = msg.get(i); }
				send(sock, data, (size_t)(msg.getLength()), 0);
			}

			data::Bytes receive() override {
				unsigned char* buffer = new unsigned char[1500];
				socklen_t addrLen = sizeof(serverAddress);
				ssize_t bytesRead = recvfrom(sock, buffer, 1500, 0, (struct sockaddr*)(&serverAddress), &addrLen);
				if (bytesRead < 0) {
					notif::error("error receiving data");
					return data::Bytes(0);
				}
				data::Bytes result(1500);
				for (int i = 0; i < 1500; i++) {
					result.set(i, buffer[i]);
				}
				return result;
			}

	};

	class UDPConnection: public Connection {

		private:

			int sock = socket(AF_INET, SOCK_DGRAM, 0);

		public:

			UDPConnection() {}
			UDPConnection(const char* ip, int port) {
				if (sock < 0) {
					notif::fatal("error building socket");
				}

				memset(&serverAddress, 0, sizeof(serverAddress));
				serverAddress.sin_family = AF_INET;
				inet_pton(AF_INET, ip, &serverAddress.sin_addr);
				serverAddress.sin_port = htons(port);
			}

			void establish() override {}

			void transmit(data::Bytes msg) override {
				unsigned char data[msg.getLength()];
				for (int i = 0; i < msg.getLength(); i++) { data[i] = msg.get(i); }
				sendto(sock, data, (size_t)(msg.getLength()), 0, (struct sockaddr*)(&serverAddress), sizeof(serverAddress));
			}

			data::Bytes receive() override {
				unsigned char* buffer = new unsigned char[1500];
				socklen_t addrLen = sizeof(serverAddress);
				ssize_t bytesRead = recvfrom(sock, buffer, 1500, 0, (struct sockaddr*)(&serverAddress), &addrLen);
				if (bytesRead < 0) {
					notif::error("error receiving data");
					return data::Bytes(0);
				}
				data::Bytes result(1500);
				for (int i = 0; i < 1500; i++) {
					result.set(i, buffer[i]);
				}
				return result;
			}

	};	

	template <typename ConnectionType, typename CipherType>
	class Encrypted: public Connection {

		// for example:
		//
		//   Encrypted<TCPConnection, VOX> myEncryptedConnection("1.2.3.4");
		//
		// works with any Connection class

		private:

			ConnectionType conn;
			CipherType cipher;
			key::Key k;
			bool keyAcquired;

		public:

			Encrypted() {}
			Encrypted(const char* ip, int port): conn(ConnectionType(ip, port)), keyAcquired(true) {}
			Encrypted(const char* ip, int port, key::Key k): conn(ConnectionType(ip, port)), k(k), keyAcquired(true) {}

			void loadKey(key::Key k_) { k = k_; keyAcquired = true; }
			void loadKey(data::Bytes rawData) { k = key::Key(rawData); keyAcquired = true; }
			void loadKey(string filename) { k = key::Key(filename); keyAcquired = true; }

			void establish() { conn.establish(); }

			void transmitRaw(data::Bytes plaintext) { conn.transmit(plaintext); }
			void transmit(data::Bytes plaintext) override { conn.transmit(cipher.encrypt(plaintext, k)); }
			data::Bytes receiveRaw() { return conn.receive(); }
			data::Bytes receive() override { return cipher.decrypt(conn.receive(), k); }

	};

};



// ksng/net/pktd.h
//catboy zion
namespace pktd {

	class Layer {

		public:

			bool dissected = false;

			virtual data::Bytes dissect(data::Bytes raw) = 0; // return the leftovers
			virtual data::Bytes assemble() = 0; // return the assembled layer
			virtual string repr() = 0;

	};

	namespace layers {

		data::Bytes _ethaddrToBytes(string addr) {
			ether_addr* ethaddr = ether_aton(addr.c_str());
			unsigned char buf[6];
			for (int i = 0; i < 6; i++) { buf[i] = ethaddr->ether_addr_octet[i]; }
			data::Bytes result(6);
			for (int i = 0; i < 6; i++) { result.set(i, buf[i]); }
			return result;
		}

		data::Bytes ethaddrToBytes(string addr) {
			data::Bytes result(6);
			for (int i = 0; i < 6; i++) { result.set(i, stoi(addr.substr(3 * i, 2), 0, 16)); }
			return result;
		}

		string _bytesToEthaddr(data::Bytes raw) {
			unsigned char buf[6];
			for (int i = 0; i < 6; i++) { buf[i] = raw.get(i); }
			ether_addr* ethaddr;
			for (int i = 0; i < 6; i++) { ethaddr->ether_addr_octet[i] = buf[i]; }
			return (string)(ether_ntoa(ethaddr));
		}

		string bytesToEthaddr(data::Bytes raw, string delim=(string)(":")) {
			stringstream ss;
			for (int i = 0; i < 6; i++) {
				ss << HEX_ALPHABET[raw.get(i)];
				if (i != 5) { ss << delim; }
			}
			return ss.str();
		}

		data::Bytes ipv4ToBytes(string addr) {
			data::Bytes result(4);
			stringstream ss;
			int i = 0;
			for (int j = 0; j < addr.length(); j++) {
				if (addr.substr(j, 1) == (string)(".") || j == addr.length() - 1) {
					ss << addr[j];
					result.set(i, stoi(ss.str()));
					i++;
					ss.str(string());
					if (i == 4) { break; }
				} else {
					ss << addr[j];
				}
			}
			return result;
		}

		string bytesToIPv4(data::Bytes raw) {
			stringstream ss;
			for (int i = 0; i < 4; i++) { ss << to_string(raw.get(i)); if (i < 3) { ss << "."; } }
			return ss.str();
		}

		class Ethernet: public Layer {

			public:

				string src;
				string dst;
				short etht;

				Ethernet(string src="00:00:00:00:00:00", string dst="00:00:00:00:00:00", short etht=0): src(src), dst(dst), etht(etht) {}

				data::Bytes dissect(data::Bytes rawData) override {
					dst = bytesToEthaddr(rawData.subbytes(0, 6));
					src = bytesToEthaddr(rawData.subbytes(6, 12));
					etht = (rawData.get(12) * 256) + rawData.get(13);
					dissected = true;
					return rawData.subbytes(14, rawData.getLength());
				}

				data::Bytes assemble() override {
					data::Bytes srcbytes = ethaddrToBytes(src);
					data::Bytes dstbytes = ethaddrToBytes(dst);
					data::Bytes ethtbytes = data::Bytes(2);
					ethtbytes.set(0, etht >> 8);
					ethtbytes.set(1, etht % 256);
					data::Bytes result(14);
					dstbytes.copyTo(result, 0);
					srcbytes.copyTo(result, 6);
					ethtbytes.copyTo(result, 12);
					return result;
				}

				string repr() override {
					stringstream ss;
					ss << "[Ethernet: " << src << " -> " << dst << " (etht " << (int)(etht) << ")]";
					return ss.str();
				}

		};

		class IPv4: public Layer {

			public:

				unsigned char ver;
				unsigned char ihl;
				unsigned char dscp;
				unsigned char ecn;
				unsigned short length;
				unsigned short id;
				unsigned char flags;
				unsigned short fragoff;
				unsigned char ttl;
				unsigned char proto;
				unsigned short chk;
				string src;
				string dst;

				IPv4() {}

				data::Bytes dissect(data::Bytes rawData) override {
					ver = rawData.get(0) >> 4;
					ihl = rawData.get(0) % 16;
					dscp = rawData.get(1) >> 2;
					ecn = rawData.get(1) % 4;
					length = rawData.getShort(2);
					id = rawData.getShort(4);
					flags = rawData.get(6) >> 5;
					fragoff = rawData.getShort(6) % 8192;
					ttl = rawData.get(8);
					proto = rawData.get(9);
					chk = rawData.getShort(10);
					src = bytesToIPv4(rawData.subbytes(12, 16));
					dst = bytesToIPv4(rawData.subbytes(16, 20));
					dissected = true;
					return rawData.subbytes(20, rawData.getLength());
				}

				data::Bytes assemble() override {
					data::Bytes result(20);
					result.set(0, (ver << 4) + ihl);
					result.set(1, (dscp << 2) + ecn);
					result.loadShort(length, 2);
					result.loadShort(id, 4);
					result.loadShort((unsigned short)(flags << 5) + (unsigned short)(fragoff), 6);
					result.set(8, ttl);
					result.set(9, proto);
					result.loadShort(chk, 10);
					ipv4ToBytes(src).copyTo(result, 12);
					ipv4ToBytes(dst).copyTo(result, 16);
					return result;
				}

				string repr() override {
					stringstream ss;
					ss << "[IPv" << (unsigned int)(ver) << ": " << src << " -> " << dst << " (proto " << (unsigned int)(proto) << ")]";
					return ss.str();
				}

		};

		class ICMPv4: public Layer {

			public:

				unsigned char type;
				unsigned char code;
				unsigned short chk;
				data::Bytes roh;

				data::Bytes dissect(data::Bytes rawData) override {
					type = rawData.get(0);
					code = rawData.get(1);
					chk = rawData.getShort(2);
					roh = rawData.subbytes(4, 8);
					dissected = true;
					return rawData.subbytes(8, rawData.getLength());					
				} 

				data::Bytes assemble() override {
					data::Bytes result(8);
					result.set(0, type);
					result.set(1, code);
					result.loadShort(chk, 2);
					roh.copyTo(result, 4);
					return result;
				}

				string repr() override {
					stringstream ss;
					ss << "[ICMPv4: type " << (unsigned int)(type) << ", code " << (unsigned int)(code) << "]";
					return ss.str();
				}

		};

		class TCP: public Layer {

			public:

				unsigned short srcp;
				unsigned short dstp;
				unsigned int seq;
				unsigned int ackn;
				unsigned char dataoff;
				unsigned char flags;
				unsigned short window;
				unsigned short chk;
				unsigned short urgptr;

				TCP() {}

				data::Bytes dissect(data::Bytes rawData) override {
					srcp = rawData.getShort(0);
					dstp = rawData.getShort(2);
					seq = rawData.getInt(4);
					ackn = rawData.getInt(8);
					dataoff = rawData.get(12) >> 4;
					flags = rawData.get(13);
					window = rawData.getShort(14);
					chk = rawData.getShort(16);
					urgptr = rawData.getShort(18);
					dissected = true;
					return rawData.subbytes(20, rawData.getLength());
				}

				data::Bytes assemble() override {
					data::Bytes result(20);
					result.loadShort(srcp, 0);
					result.loadShort(dstp, 2);
					result.loadInt(seq, 4);
					result.loadInt(ackn, 8);
					result.set(12, dataoff << 4);
					result.set(13, flags);
					result.loadShort(window, 14);
					result.loadShort(chk, 16);
					result.loadShort(urgptr, 18);
					return result;
				}

				string repr() override {
					stringstream ss;
					ss << "[TCP: " << (unsigned int)(srcp) << " -> " << (unsigned int)(dstp) << "]";
					return ss.str();
				}

		};

		class UDP: public Layer {

			public:

				unsigned short srcp;
				unsigned short dstp;
				unsigned short length;
				unsigned short chk;

				data::Bytes dissect(data::Bytes rawData) override {
					srcp = rawData.getShort(0);
					dstp = rawData.getShort(2);
					length = rawData.getShort(4);
					chk = rawData.getShort(6);
					dissected = true;
					return rawData.subbytes(8, rawData.getLength() - 1);
				}

				data::Bytes assemble() override {
					data::Bytes result(8);
					result.loadShort(srcp, 0);
					result.loadShort(dstp, 2);
					result.loadShort(length, 4);
					result.loadShort(chk, 6);
					return result;
				}

				string repr() override {
					stringstream ss;
					ss << "[UDP: " << (unsigned int)(srcp) << " -> " << (unsigned int)(dstp) << "]";
					return ss.str();
				}

		};

	};

	class Packet {

		public:

			layers::Ethernet eth;
			layers::IPv4 ipv4;
			layers::ICMPv4 icmpv4;
			layers::TCP tcp;
			layers::UDP udp;
			data::Bytes payload;

			string repr() {
				stringstream ss;
				ss << eth.repr();
				if (eth.etht == 2048) {
					ss << " / " << ipv4.repr();
					if (ipv4.proto == 1) {
						ss << " / " << icmpv4.repr();
					} else if (ipv4.proto == 6) {
						ss << " / " << tcp.repr();
					} else if (ipv4.proto == 17) {
						ss << " / " << udp.repr();
					}
				}
				ss << " / " << payload.getLength() << "-byte payload";
				return ss.str();
			}

	};

	class PacketDissector {

		private:

			nicr::NICR nr;

		public:

			PacketDissector(string interface): nr(nicr::NICR(interface)) {}

			Packet receivePacket() {
				data::Bytes rawData = nr.receiveData();
				Packet result;
				rawData = result.eth.dissect(rawData);
				if (result.eth.etht == 2048) {
					rawData = result.ipv4.dissect(rawData);
					if (result.ipv4.proto == 1) {
						result.payload = result.icmpv4.dissect(rawData);
						return result;
					} else if (result.ipv4.proto == 6) {
						result.payload = result.tcp.dissect(rawData);
						return result;
					} else if (result.ipv4.proto == 17) {
						result.payload = result.udp.dissect(rawData);
						return result;
					}
				}
				result.payload = rawData;
				return result;
			}

	};

};



// ksng/net/nicr.h

using namespace std;

namespace nicr {

	struct RawPacket {
		ssize_t size;
		unsigned char data[1500];
	};

	class NICReader {

		protected:

			int socketDescriptor;
			string interfaceName;
			int interfaceIndex;

		public:

			NICReader() {}

			NICReader(const string& interfaceName): interfaceName(interfaceName) {
				initialize(interfaceName);
			}

			~NICReader() {
				close(socketDescriptor);
			}

			void initialize(const string& interfaceName_) {
				interfaceName = interfaceName_;
				socketDescriptor = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
				if (socketDescriptor == -1) {
					notif::fatal("NIC access: error opening raw socket; possible permissions error");
				}
				notif::success("NIC access: successfully opened raw socket.");
				enablePromiscuousMode();
			}

			void enablePromiscuousMode() {
				struct ifreq ifr;
				memset(&ifr, 0, sizeof(ifr));
				strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);

				if (ioctl(socketDescriptor, SIOCGIFFLAGS, &ifr) == -1) {
					notif::warning("NIC access: error getting interface flags");
					return;
				}

				ifr.ifr_flags |= IFF_PROMISC;

				if (ioctl(socketDescriptor, SIOCSIFFLAGS, &ifr) == -1) {
					notif::warning("Error enabling promiscuous mode");
				}

				notif::success("NIC access: successfully enabled promiscuous mode.");
			}

			// Receive a packet and return it
			RawPacket receiveRawPacket() {
				RawPacket pkt;
				pkt.size = recv(socketDescriptor, pkt.data, sizeof(pkt.data), 0);

				if (pkt.size == -1) {
					notif::fatal("NIC access: error receiving packet");
				}

				return pkt;
			}

			data::Bytes receiveData() {
				RawPacket pkt = receiveRawPacket();
				data::Bytes result(pkt.size);
				for (int i = 0; i < pkt.size; i++) {
					result.set(i, pkt.data[i]);
				}
				return result;
			}

			void sendData(data::Bytes msg) {
				unsigned char rawData[msg.getLength()];
				for (int i = 0; i < msg.getLength(); i++) {
					rawData[i] = msg.get(i);
				}
				send(socketDescriptor, rawData, msg.getLength(), 0);
			}

			int getInterfaceIndex() {
				struct ifreq ifr;
				memset(&ifr, 0, sizeof(ifr));
				strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);

				if (ioctl(socketDescriptor, SIOCGIFINDEX, &ifr) == -1) {
					notif::fatal("NIC access: error getting interface index");
					return -1;
				}

				return ifr.ifr_ifindex;
			}

	};

	using NICR = NICReader;	

};



// ksng/crypto/ciphers/vox.h

using namespace std;

namespace vox {

	// Vector Operation Xnterchange (i know the name is silly)
	class VOX: public cipher::SymmetricCipher {

		public:

			inline bool validateKey(key::Key key) { return (bool)(key.getBitLength() == 2048); }

			data::Bytes encryptBlock(data::Bytes plaintext, key::Key k) {
				if (plaintext.getLength() < 128) {
					data::Bytes n(128);
					plaintext.copyTo(n);
					for (int i = plaintext.getLength(); i < 128; i++) {
						n.set(i, 170);
					}
					plaintext = n;
				} else if (plaintext.getLength() > 128) {
					notif::fatal("VOX can\'t encrypt blocks more than 128 bytes at a time at the moment");
				}
				data::Bytes result(256);
				sda::SDA<data::Bytes> fragments = fragment::split(plaintext);
				fragments.get(0).copyTo(result, 0);
				fragments.get(1).copyTo(result, 128);
				result = permute::permute(result, k.getBytes());
				return result;
			}

			data::Bytes decryptBlock(data::Bytes ciphertext, key::Key k) {
				if (ciphertext.getLength() != 256) {
					notif::fatal("invalid ciphertext block length");
				}
				data::Bytes result = permute::depermute(ciphertext, k.getBytes());
				sda::SDA<data::Bytes> fragments(2);
				fragments.set(0, result.subbytes(0, 128));
				fragments.set(1, result.subbytes(128, 256));
				return fragment::reassemble(fragments);
			}

			data::Bytes encrypt(data::Bytes plaintext, key::Key k) override {
				if (plaintext.getLength() != 128) { notif::warning("VOX padded plaintext; strip trailing 0xAA bytes"); }
				if (plaintext.getLength() <= 128) {
					return encryptBlock(plaintext, k);
				} else {
					int blockCount = ceil((float)(plaintext.getLength()) / 128.0);
					data::Bytes ciphertext(blockCount * 256);
					for (int i = 0; i < blockCount; i++) {
						encryptBlock(plaintext.subbytes(i * 128, (i + 1) * 128), k).copyTo(ciphertext, i * 256);
					}
					return ciphertext;
				}
			};

			data::Bytes decrypt(data::Bytes ciphertext, key::Key k) override {
				if (ciphertext.getLength() % 256 != 0) { notif::fatal("invalid ciphertext"); }
				int blockCount = ciphertext.getLength() / 256;
				data::Bytes plaintext(ciphertext.getLength() / 2);
				for (int i = 0; i < blockCount; i++) {
					decryptBlock(ciphertext.subbytes(i * 256, (i + 1) * 256), k).copyTo(plaintext, i * 128);
				}
				return plaintext;
			};

			key::Key generateKey() override {
				data::Bytes result(256);
				unsigned char r = csprng::bytes(1).get(0);
				for (int i = 0; i < 255; i++) {
					r = csprng::bytes(1).get(0);
					while (result.has(r)) {
						r += 1;
					}
					result.set(i, r);
				}
				return key::Key(result);
			}

	};
	
};


// ksng/crypto/hashes/marsh.h

namespace marsh {

	const unsigned char PERMUTATION[256] = {
		0xB0, 0x83, 0x9A, 0x29, 0x9F, 0x34, 0xFA, 0xD3, 
		0x74, 0x3A, 0xD6, 0x71, 0xC2, 0xEE, 0x25, 0xAC, 
		0x20, 0x02, 0x21, 0x7F, 0x01, 0xA7, 0x43, 0xFF, 
		0x99, 0x52, 0x77, 0x15, 0x49, 0xAD, 0x36, 0x2C, 
		0x04, 0x69, 0x55, 0x78, 0x92, 0xED, 0xE7, 0xC3, 
		0x2B, 0x2A, 0xF6, 0x97, 0xD7, 0x7A, 0x38, 0x14, 
		0x58, 0x03, 0x79, 0x2D, 0x09, 0xE2, 0xF8, 0xB1, 
		0xC8, 0x76, 0x89, 0xC6, 0x06, 0x23, 0x22, 0x59, 
		0x75, 0x56, 0xD8, 0x10, 0x8D, 0x8E, 0x0A, 0x3C, 
		0xDD, 0x9B, 0x1A, 0xFB, 0xA9, 0xCC, 0xE4, 0x7B, 
		0x07, 0x33, 0xF9, 0x82, 0xD9, 0x16, 0xFD, 0x6A, 
		0x05, 0xFE, 0x80, 0x26, 0x42, 0x62, 0xC4, 0x17, 
		0x31, 0xF1, 0x28, 0x11, 0x6B, 0x1D, 0xE5, 0x5B, 
		0x8F, 0xE8, 0xE6, 0x7C, 0x5A, 0xE3, 0xEB, 0xFC, 
		0x4E, 0x4A, 0x2F, 0x94, 0x9C, 0x08, 0x8A, 0x0B, 
		0x7D, 0x0C, 0x0F, 0x7E, 0xC5, 0x24, 0x9D, 0x8B, 
		0xE9, 0xF3, 0x4B, 0x27, 0xA1, 0x0D, 0x2E, 0xA0, 
		0x0E, 0x96, 0xC7, 0x18, 0x12, 0xB9, 0xE0, 0xE1, 
		0xF7, 0x3B, 0x57, 0x19, 0x13, 0x1B, 0x6C, 0x90, 
		0x9E, 0x3D, 0x1C, 0x44, 0x84, 0x81, 0xBD, 0x53, 
		0x85, 0x32, 0xF0, 0x41, 0x46, 0xD4, 0xA8, 0xB2, 
		0xEA, 0x5C, 0x8C, 0xA2, 0x91, 0xCD, 0x86, 0x95, 
		0xA5, 0xC9, 0xAA, 0xCA, 0x87, 0x4C, 0xD5, 0x1E, 
		0xEC, 0xA3, 0x60, 0x98, 0x1F, 0xB3, 0x88, 0x30, 
		0xCE, 0xAE, 0xBC, 0xCF, 0x35, 0x37, 0x93, 0xAB, 
		0x39, 0xDA, 0x3E, 0x61, 0x4D, 0xA4, 0x3F, 0x40, 
		0xBF, 0x45, 0xA6, 0xEF, 0x47, 0x48, 0x4F, 0x50, 
		0x51, 0x54, 0xB4, 0x5D, 0x5E, 0x5F, 0x63, 0xF2, 
		0x64, 0xF4, 0x72, 0xF5, 0x65, 0x66, 0x67, 0x68, 
		0x6D, 0x6E, 0xAF, 0xB5, 0xB6, 0xCB, 0xB7, 0x6F, 
		0xB8, 0x70, 0x73, 0xBA, 0xBB, 0xBE, 0xC0, 0xC1, 
		0xD0, 0xD1, 0xD2, 0xDB, 0xDC, 0xDE, 0xDF, 0x00
	};

	// Mixing and Addition for Really Secure Hashing
	class MARSH: public hashing::HashFunction {

		private:

			data::Bytes p = data::Bytes(256);

		public:

			MARSH() { for (int i = 0; i < 256; i++) { p.set(i, PERMUTATION[i]); } }

			data::Bytes hash(data::Bytes plaintext) override {
				data::Bytes transient(256);
				int i, j;
				for (i = 0; i < plaintext.getLength(); i++) { transient.set(i % 256, plaintext.get(i)); }
				for (j = i % 256; j < 256; j++) { transient.set(j, j * 251); }
				transient = permute::permute(permute::permute(transient, p), transient);
				data::Bytes fp;
				for (int i = 0; i < 17; i++) {
					transient = permute::permute(transient ^ permute::permute(transient, p), p ^ transient);
					transient = transient ^ p;
					for (int i = 1; i < 256; i++) { transient.set(i, transient.get(i) + transient.get(i - 1) + p.get(i)); }
				}
				return transient;
			}

	};

	// MARSH - Keyed
	class MARSH_K: public hashing::KeyedHashFunction {

		private:

			MARSH marsh;

	};

};


// ksng/crypto/core/fragment.h

using namespace sda;
using namespace data;

namespace fragment {

	SecureDataArray<Bytes> fragment(Bytes data, int count=2) {
		int dataLength = data.getLength();
		SDA<Bytes> result(count);

		result.set(0, data);
		for (int i = 1; i < count; i++) {
			result.set(i, csprng::bytes(dataLength));
			result.set(0, result.get(0) ^ result.get(i));
		}

		return result;
	}

	SDA<Bytes> split(Bytes data) {
		return fragment(data, 2);
	}

	Bytes reassemble(SecureDataArray<Bytes> data) {
		Bytes result = data.get(0);

		for (int i = 1; i < data.getLength(); i++) {
			result = result ^ data.get(i);
		}

		return result;
	}

	SecureDataArray<Bits> fragment(Bits data, int count=2) {
		int dataLength = data.getLength();
		SDA<Bits> result(count);

		result.set(0, data);
		for (int i = 1; i < count; i++) {
			result.set(i, csprng::bits(dataLength));
			result.set(0, result.get(0) ^ result.get(i));
		}

		return result;
	}

	SDA<Bits> split(Bits data) {
		return fragment(data, 2);
	}

	Bits reassemble(SecureDataArray<Bits> data) {
		Bits result = data.get(0);

		for (int i = 1; i < data.getLength(); i++) {
			result = result ^ data.get(i);
		}

		return result;
	}

};


// ksng/crypto/core/cipher.h

namespace cipher {

	enum SymmetricCipherMode {
		ECB = 1,
		CBC = 2,
		CFB = 3,
		OFB = 4,
		CTR = 5
	};

	struct Keypair {
		key::Key publicKey;
		key::Key privateKey;
	};

	class Cipher {

		public:

			virtual data::Bytes encrypt(data::Bytes plaintext, key::Key key) = 0;
			virtual data::Bytes decrypt(data::Bytes ciphertext, key::Key key) = 0;

			void encryptFile(string filename, key::Key key) {
				data::Bytes plaintext = fileops::readFileBytes(filename);
				fileops::writeFile(filename, encrypt(plaintext, key));
			}

			void decryptFile(string filename, key::Key key) {
				data::Bytes ciphertext = fileops::readFileBytes(filename);
				fileops::writeFile(filename, decrypt(ciphertext, key));
			}

	};

	class SymmetricCipher: public Cipher {

		public:

			virtual key::Key generateKey() = 0;

	};

	class AsymmetricCipher: Cipher {

		public:

			virtual Keypair generateKeys() = 0;

	};

};


// ksng/crypto/core/permute.h

namespace permute {

	// Please don't permute data arrays of more than 256 items at a time!
	// It makes the bytes sad for being too small to represent more values :(

	data::Bytes permute(data::Bytes data, data::Bytes permutation) {
		data::Bytes result(data.getLength());
		for (int i = 0; i < data.getLength(); i++) {
			result.set(permutation.get(i), data.get(i));
		}
		return result;
	}

	data::Bytes depermute(data::Bytes data, data::Bytes permutation) {
		data::Bytes result(data.getLength());
		for (int i = 0; i < data.getLength(); i++) {
			result.set(i, data.get(permutation.get(i)));
		}
		return result;
	}

	data::Bits permute(data::Bits data, data::Bytes permutation) {
		data::Bits result(data.getLength());
		for (int i = 0; i < data.getLength(); i++) {
			result.set(permutation.get(i), data.get(i));
		}
		return result;
	}

	data::Bits depermute(data::Bits data, data::Bytes permutation) {
		data::Bits result(data.getLength());
		for (int i = 0; i < data.getLength(); i++) {
			result.set(i, data.get(permutation.get(i)));
		}
		return result;
	}

};


// ksng/crypto/core/key.h

using namespace std;

namespace key {

	class Key {

		private:

			data::Bytes keybytes;
			data::Bits keybits;
			bool locked = false;

		public:

			// Constructors & destructors

			Key() {}

			Key(data::Bytes data) {
				keybytes = data;
				keybits = data::bytesToBits(data);

				keybytes.hide();
				keybits.hide();
			}

			Key(string filename) {
				keybytes = fileops::readFileBytes(filename);
				keybits = fileops::readFileBits(filename);

				keybytes.hide();
				keybits.hide();
			}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() {
				if (locked) {
					notif::security("attempted access to locked cryptographic key", ALERT);
				}
			}

			// Access

			int getByteLength() { 
				securityCheck(); 
				keybytes.reveal();
				int result = keybytes.getLength(); 
				keybytes.hide();
				return result;
			}

			int getBitLength() {
				securityCheck();
				keybits.reveal();
				int result = keybits.getLength(); 
				keybits.hide();
				return result;
			}

			data::Bytes getBytes() {
				securityCheck();
				keybytes.reveal();
				data::Bytes result(keybytes.getLength());
				keybytes.copyTo(result);
				keybytes.hide();
				return result;
			}

			data::Bits getBits() {
				securityCheck();
				keybits.reveal();
				data::Bits result(keybits.getLength());
				keybits.copyTo(result);
				keybits.hide();
				return result;
			}

			unsigned char getByte(int index) {
				securityCheck();
				keybytes.reveal();
				unsigned char result = keybytes.get(index);
				keybytes.hide();
				return result;
			}

			int getBit(int index) {
				securityCheck();
				keybits.reveal();
				int result = keybits.get(index);
				keybits.hide();
				return result;
			}

			// File management

			void store(string filename) {
				securityCheck();
				keybytes.reveal();
				fileops::writeFile(filename, keybytes);
				keybytes.hide();
			}

			// Utility

			static Key random(int length, data::DataRepr dr=data::bits) {
				if (dr == data::bits) {
					return Key(csprng::bytes(length / 8));
				} else {
					return Key(csprng::bytes(length));
				}
			}

	};

};


// ksng/crypto/core/hash.h

using namespace std;

namespace hashing {

	class HashFunction {

		public:

			virtual data::Bytes hash(data::Bytes plaintext) = 0;

			data::Bytes hashFile(string filename, string store=(string)("NONE")) {
				data::Bytes h = hash(fileops::readFileBytes(filename));
				if (store != (string)("NONE")) { fileops::writeFile(store, h); }
				return h;
			}

	};

	class KeyedHashFunction {

		public:

			virtual data::Bytes hash(data::Bytes plaintext, key::Key k) = 0;

			data::Bytes fileHash(string filename, key::Key k, string store=(string)("NONE")) {
				data::Bytes h = hash(fileops::readFileBytes(filename), k);
				if (store != (string)("NONE")) { fileops::writeFile(store, h); }
				return h;
			}

	};

};



// ksng/crypto/core/suite.h

namespace suite {

	class CryptographicSuite {

		private:
		
			cipher::SymmetricCipher* sc;
			cipher::AsymmetricCipher* asc;
			hash::HashFunction* hf;
			hash::KeyedHashFunction khf;

		public:

			CryptographicSuite(
				cipher::SymmetricCipher* sc = nullptr,
				cipher::AsymmetricCipher* asc = nullptr,
				hash::HashFunction* hf = nullptr,
				hash::KeyedHashFunction* khf = nullptr
			): sc(sc), asc(asc), hf(hf), khf(khf) {}

			// Post-initialization setup

			void setSymmetricCipher(cipher::SymmetricCipher* sc_) { sc = sc_; }
			void setAsymmetricCipher(cipher::AsymmetricCipher* asc_) { asc = asc_; }
			void setHashFunction(hash::HashFunction* hf_) { hf = hf_; }
			void setKeyedHashFunction(hash::KeyedHashFunction* khf_) { khf = khf_; }

			// Cryptographic methods

			data::Bytes encryptSymmetric(data::Bytes plaintext, key::Key k) { return sc->encrypt(plaintext, k); }
			data::Bytes encryptAsymmetric(data::Bytes plaintext, key::Key k) { return asc->encrypt(plaintext, k); }
			data::Bytes decryptSymmetric(data::Bytes ciphertext, key::Key k) { return sc->decrypt(ciphertext, k); }
			data::Bytes decryptAsymmetric(data::Bytes ciphertext, key::Key k) { return asc->decrypt(ciphertext, k); }
			data::Bytes encryptFileSymmetric(string filename, key::Key k) { return sc->encryptFile(filename, k); }
			data::Bytes encryptFileAsymmetric(string filename, key::Key k) { return asc->encryptFile(filename, k); }
			data::Bytes decryptFileSymmetric(string filename, key::Key k) { return sc->decryptFile(filename, k); }
			data::Bytes decryptFileAsymmetric(string filename, key::Key k) { return asc->decryptFile(filename, k); }
			data::Bytes hash(data::Bytes plaintext) { return hf->hash(plaintext); }
			data::Bytes hashFile(string filename, string store=(string)("NONE")) { return hf->hashFile(filename, store); }
			data::Bytes keyedHash(data::Bytes plaintext, key::Key k) { return khf->hash(plaintext, k); }
			data::Bytes keyedHashFile(string filename, string store=(string)("NONE")) { return khf->hashFile(filename, store); }

	};

	using Suite = CryptographicSuite;
	using CS = CryptographicSuite;

};


// ksng/util/notif.h

using namespace std;

enum Severity {
	CAUTION = 1,
	WARNING = 2,
	ALERT = 3,
};

namespace colors {
	string HEADER = "\033[95m";
	string OKBLUE = "\033[94m";
	string OKCYAN = "\033[96m";
	string OKGREEN = "\033[92m";
	string WARNING = "\033[93m";
	string FAIL = "\033[91m";
	string ENDC = "\033[0m";
	string BOLD = "\033[1m";
	string UNDERLINE = "\033[4m";

	string colorize(string text, string color) {
		return color + text + ENDC;
	}
};

namespace notif {

	inline void fatal(string msg) {
		cerr << colors::colorize("\n\nKingslayer-NG: fatal system failure: " + msg + "\n\n", colors::FAIL + colors::BOLD) << endl;
		throw exception();
	}

	inline void error(string msg) {
		cerr << colors::colorize("\nKingslayer-NG: system failure: " + msg + "\n", colors::FAIL) << endl;
	}

	inline void warning(string msg) {
		cerr << colors::colorize("Kingslayer-NG: warning: " + msg, colors::WARNING) << endl;
	}

	inline void info(string msg) {
		cout << colors::colorize("Kingslayer-NG: " + msg, colors::OKCYAN) << endl;
	}

	inline void success(string msg) {
		cout << colors::colorize("Kingslayer-NG: success: " + msg, colors::OKGREEN) << endl;
	}

	void security(string msg, Severity severity) {
		if (severity == CAUTION) {
			cout << colors::colorize("Kingslayer-NG: security advises ", colors::HEADER) \
					+ colors::colorize("caution", colors::WARNING + colors::UNDERLINE + colors::BOLD) \
					+ colors::colorize(": " + msg, colors::HEADER);
		} else if (severity == WARNING) {
			cout << colors::colorize("Kingslayer-NG: ", colors::HEADER) \
					+ colors::colorize("security warning", colors::FAIL + colors::UNDERLINE + colors::BOLD) \
					+ colors::colorize(": " + msg, colors::HEADER);
		} else if (severity == ALERT) {
			cout << colors::colorize("\nKingslayer-NG: ", colors::HEADER) \
					+ colors::colorize("SECURITY ALERT", colors::FAIL + colors::UNDERLINE + colors::BOLD) \
					+ colors::colorize(": " + msg + "\n", colors::HEADER);
		} else {
			fatal("security system failure");
		}
	}

};


// ksng/util/data.h

const string HEX_ALPHABET[256] = {
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
	"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
	"B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
	"C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
	"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
	"E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
	"F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
};

using namespace std;

namespace data {

	enum DataRepr {
		bits = 1,
		bytes = 8
	};

	class Bits: public sda::SecureDataArray<int> {

		public:

			explicit Bits() {}

			explicit Bits(int length_, bool locked_=false, Severity severityLevel_=ALERT) {
				initialize(length_, locked_, severityLevel_);
			}

			Bits(initializer_list<int> initList) {
				length = initList.size();
				elements = new int[length];
				locked = false;
				securityLevel = ALERT;
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

			Bits operator^(Bits other) {
				int length = getLength();
				Bits result(length);
				for (int i = 0; i < length; i++) {
					result.set(i, get(i) ^ other.get(i));
				}
				return result;
			}

			string bin() {
				stringstream ss;
				ss << getLength() << " bits: ";
				for (int i = 0; i < getLength(); i++) {
					ss << get(i);
					if ((i + 1) % 8 == 0 && i != getLength() - 1) {
						ss << "-";
					}
				}
				return ss.str();
			}

			Bits subbits(int a, int b) {
				if (b < a) {
					return subbits(b, a);
				} else if (a == b) {
					Bits result(1);
					result.set(0, get(a));
					return result;
				} else if (a < b) {
					Bits result(b - a);
					for (int i = 0; i < b - a; i++) {
						if (a + i >= getLength()) { break; }
						result.set(i, get(a + i));
					}
					return result;
				} else {
					notif::fatal("impossible state: a < b and b > a; recommend system reboot");
					return Bits(0);
				}
			}

			void copyTo(Bits &dst, int offset = 0) {
				for (int i = 0; i < getLength(); i++) {
					if (i + offset >= dst.getLength()) {
						break;
					}
					dst.set(i + offset, get(i));
				}
			}

			Bits copyOfSize(int i) {
				Bits result(i);
				copyTo(result);
				return result;
			}
			Bits extended(int i) { return copyOfSize(getLength() + i); }
			Bits truncated(int i) { return copyOfSize(getLength() - 1); }

	};

	class Bytes: public sda::SecureDataArray<unsigned char> {

		public:

			explicit Bytes() {}

			explicit Bytes(int length_, bool locked_=false, Severity severityLevel_=ALERT) {
				initialize(length_, locked_, severityLevel_);
			}

			Bytes(initializer_list<unsigned char> initList) {
				length = initList.size();
				elements = new unsigned char[length];
				locked = false;
				securityLevel = ALERT;
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

			Bytes operator^(Bytes other) {
				Bytes result(getLength());
				for (int i = 0; i < getLength(); i++) {
					result.set(i, get(i) ^ other.get(i));
				}
				return result;
			}

			string hex() {
				stringstream ss;
				ss << getLength() << " bytes: ";
				for (int i = 0; i < getLength(); i++) {
					ss << HEX_ALPHABET[get(i)];
					if (i != getLength() - 1) {
						ss << "-";
					}
				}
				return ss.str();
			}

			Bytes subbytes(int a, int b) {
				if (b < a) {
					return subbytes(b, a);
				} else if (a == b) {
					Bytes result(1);
					result.set(0, get(a));
					return result;
				} else if (a < b) {
					Bytes result(b - a);
					for (int i = 0; i < b - a; i++) {
						if (a + i >= getLength()) { break; }
						result.set(i, get(a + i));
					}
					return result;
				} else {
					notif::fatal("impossible state: a < b and b > a; recommend system reboot");
					return Bytes(0);
				}
			}

			// Copying and modification

			void copyTo(Bytes &dst, int offset = 0) {
				for (int i = 0; i < getLength() && i + offset < dst.getLength(); i++) {
					dst.set(i + offset, get(i));
				}
			}

			Bytes copyOfSize(int i) {
				Bytes result(i);
				copyTo(result);
				return result;
			}
			Bytes extended(int i) { return copyOfSize(getLength() + i); }
			Bytes truncated(int i) { return copyOfSize(getLength() - 1); }

			string astext() {
				stringstream ss;
				for (int i = 0; i < getLength(); i++) {
					ss << get(i);
				}
				return ss.str();
			}

			unsigned short getShort(int index) {
				return ((unsigned short)(get(index)) * 256) + (unsigned short)(get(index + 1));
			}

			void loadShort(unsigned short value, int index) {
				set(index, value >> 8);
				set(index + 1, value % 256);
			}

			unsigned short getInt(int index) {
				return ((unsigned int)(get(index)) << 24) \
					   + ((unsigned int)(get(index + 1)) << 16) \
					   + ((unsigned int)(get(index + 2)) << 8) \
					   + (unsigned int)(get(index + 3));
			}

			void loadInt(unsigned int value, int index) {
				set(index, value >> 24);
				set(index + 1, (value >> 16) % 256);
				set(index + 2, (value >> 8) % 256);
				set(index + 3, value % 256);
			}

	};

	Bits bytesToBits(Bytes bytes) {
		Bits result(bytes.getLength() * 8);
		unsigned char byte;
		for (int i = 0; i < bytes.getLength(); i++) {
			byte = bytes.get(i);
			for (int j = 0; j < 8; j++) {
				result.set((i * 8) + j, (byte >> (8 - (j + 1))) % 2);
			}
		}
		return result;
	}

	Bytes bitsToBytes(Bits bits) {
		if (bits.getLength() % 8 != 0) { 
			notif::error("truncating bits so that byte conversion works"); 
			bits = bits.truncated(bits.getLength() % 8);
		}

		Bytes result(bits.getLength() / 8);
		unsigned char x;
		int bitOffset;

		for (int byteOffset = 0; byteOffset < bits.getLength() / 8; byteOffset++) {
			bitOffset = byteOffset * 8;
			x = 0;

			for (int i = 0; i < 8; i++) {
				x += bits.get(bitOffset + i) << (8 - (i + 1));
			}

			result.set(byteOffset, x);
		}

		return result;
	}

};


// ksng/util/xln.h

using namespace std;

namespace xln {

	class ExtraLargeNumber: public data::Bits {

		public:

			using data::Bits::Bits;

			ExtraLargeNumber(initializer_list<int> initList) {
				length = initList.size();
				elements = new int[length];
				locked = false;
				securityLevel = ALERT;
				int i = getLength() - 1;
				for (const auto& val : initList) {
					elements[i--] = val;
				}
			}

			// Comparison

			bool isZero() { for (int i = 0; i < getLength(); i++) { if (get(i) != 0) { return false; } } return true; }

			bool operator<(ExtraLargeNumber other) {
				for (int i = max(getLength(), other.getLength()); i >= 0; i--) {
					if (i < getLength() && i >= other.getLength()) {
						if (get(i) == 1) { return false; }
					} else if (i >= getLength() && i < other.getLength()) {
						if (other.get(i) == 1) { return true; }
					} else if (i < getLength() && i < other.getLength()) {
						if (get(i) ^ other.get(i) == 1) {
							if (get(i) == 1) { return false; }
							if (other.get(i) == 1) { return true; }
						}
					}
				}
				return false;
			}

			bool operator==(ExtraLargeNumber other) {
				for (int i = max(getLength(), other.getLength()); i >= 0; i--) {
					if (i < getLength() && i >= other.getLength()) {
						if (get(i) == 1) { return false; }
					} else if (i >= getLength() && i < other.getLength()) {
						if (other.get(i) == 1) { return false; }
					} else if (i < getLength() && i < other.getLength()) {
						if (get(i) ^ other.get(i) == 1) { return false; }
					}
				}
				return true;
			}

			bool operator>(ExtraLargeNumber other) {
				for (int i = max(getLength(), other.getLength()); i >= 0; i--) {
					if (i < getLength() && i >= other.getLength()) {
						if (get(i) == 1) { return true; }
					} else if (i >= getLength() && i < other.getLength()) {
						if (other.get(i) == 1) { return false; }
					} else if (i < getLength() && i < other.getLength()) {
						if (get(i) ^ other.get(i) == 1) {
							if (get(i) == 1) { return true; }
							if (other.get(i) == 1) { return false; }
						}
					}
				}
				return false;
			}

			// Logical operators

			ExtraLargeNumber operator^(ExtraLargeNumber other) {
				ExtraLargeNumber result(getLength());
				int i;
				int j;
				for (i = 0; i < getLength() && i < other.getLength(); i++) { 
					result.set(i, get(i) ^ other.get(i)); 
				}
				for (j = i; j < other.getLength(); j++) {
					result.set(j, get(j));
				}
				return result;
			}

			ExtraLargeNumber operator&(ExtraLargeNumber other) {
				ExtraLargeNumber result(getLength());
				int i;
				int j;
				for (i = 0; i < getLength() && i < other.getLength(); i++) { 
					result.set(i, get(i) & other.get(i)); 
				}
				for (j = i; j < other.getLength(); j++) {
					result.set(j, get(j));
				}
				return result;
			}

			ExtraLargeNumber operator|(ExtraLargeNumber other) {
				ExtraLargeNumber result(getLength());
				int i;
				int j;
				for (i = 0; i < getLength() && i < other.getLength(); i++) { 
					result.set(i, get(i) | other.get(i)); 
				}
				for (j = i; j < other.getLength(); j++) {
					result.set(j, get(j));
				}
				return result;
			}

			ExtraLargeNumber operator~() {
				ExtraLargeNumber result(getLength());
				for (int i = 0; i < getLength(); i++) { result.set(i, get(i) ^ 1); }
				return result;
			}

			// Arithmetic

			// * 2
			ExtraLargeNumber operator>>(int bits) {
				ExtraLargeNumber result(getLength() + bits);

				for (int i = 0; i < getLength(); i++) { result.set(i + bits, get(i)); }

				return result;
			}

			// / 2
			ExtraLargeNumber operator<<(int bits) {
				ExtraLargeNumber result(getLength() - bits);
				for (int i = 0; i < getLength() - bits; i++) { result.set(i, get(i)); }
				return result;
			}

			ExtraLargeNumber operator+(ExtraLargeNumber other) {
				ExtraLargeNumber result(getLength());

				int carry = 0;
				int a;
				int b;
				for (int i = 0; i < getLength(); i++) {
					if (i >= other.getLength()) { result.set(i, get(i)); continue; }
					if (i >= getLength()) { result.set(i, other.get(i)); continue; }
					a = get(i);
					b = other.get(i);
					result.set(i, ((a ^ b) ^ carry));
					carry = (a & b) | (a & carry) | (b & carry); 
				}

				return result;
			}

			ExtraLargeNumber operator-(ExtraLargeNumber other) {
				// calculate a - b
				ExtraLargeNumber a = copy();
				ExtraLargeNumber b(other.getLength());
				if (a.getLength() < b.getLength()) {
					a = a.withLeadingZeros(b.getLength() - a.getLength());
				}
				if (a.getLength() > b.getLength()) {
					b = b.withLeadingZeros(a.getLength() - b.getLength());
				}
				for (int i = 0; i < other.getLength(); i++) { b.set(i, other.get(i) ^ 1); }
				ExtraLargeNumber one(8);
				one.set(0, 1);
				b = b;
				return a + b + one;
			}

			ExtraLargeNumber operator*(ExtraLargeNumber other) {
				ExtraLargeNumber this_ = copy();
				ExtraLargeNumber result(getLength() + other.getLength());
				for (int i = 0; i < other.getLength(); i++) { 
					if (other.get(i) == 1) { 
						result = result + (copy() >> i);
					} 
				}
				return result;
			}

			ExtraLargeNumber operator%(ExtraLargeNumber m) {
				m = m.withoutLeadingZeros();
				ExtraLargeNumber a = copy();
				ExtraLargeNumber b = copy();
				ExtraLargeNumber z = {0}; // zero
				if (a < m) {
					return a;
				} else if (a == m) {
					return z;
				} else if (a > m) {
					while (a > m) {
						cout << "a " << a.num() << endl;
						b = a - m;
						a = b.copy();
					}
					return a;
				} else {
					notif::fatal("impossible state: a !< m & a != m & a !> m");
					return z;
				}
			}

			ExtraLargeNumber truncate(int x) {
				ExtraLargeNumber result(getLength() - x);
				for (int i = 0; i < getLength() - x; i++) { result.set(i, get(i)); }
				return result;
			}

			ExtraLargeNumber fixBitlength(int x) {
				return truncate(getLength() - x);
			}

			ExtraLargeNumber exponentiate(ExtraLargeNumber other) {
				ExtraLargeNumber this_ = copy();
				ExtraLargeNumber result(getLength());

				sda::SDA<ExtraLargeNumber> powersOfTwo(other.getLength()); // probably don't need this much space but whatever
				powersOfTwo.set(0, this_);
				for (int i = 1; i < other.getLength(); i++) {
					powersOfTwo.set(i, powersOfTwo.get(i - 1) * powersOfTwo.get(i - 1));
				}

				result.set(0, 1);
				for (int i = 0; i < other.getLength(); i++) {
					if (other.get(i) == 1) {
						result = (result * powersOfTwo.get(i));
					}
				}
				return result.withoutLeadingZeros();
			}

			ExtraLargeNumber binaryExponentiate(ExtraLargeNumber other) {
				return exponentiate(other).fixBitlength(getLength());
			}

			// utility

			ExtraLargeNumber copy() {
				ExtraLargeNumber result(getLength());
				for (int i = 0; i < getLength(); i++) { result.set(i, get(i)); }
				return result;
			}

			string num() {
				stringstream ss;
				for (int i = getLength() - 1; i >= 0; i--) {
					ss << get(i);
					if (i % 8 == 0 && i != 0) {
						ss << "-";
					}
				}
				return ss.str();
			}

			ExtraLargeNumber withoutLeadingZeros() {
				int toCut = 0;
				for (int i = getLength() - 1; i >= 0 && get(i) == 0; i--) { toCut++; }
				ExtraLargeNumber result(getLength() - toCut);
				for (int i = 0; i < getLength() - toCut; i++) { result.set(i, get(i)); }
				return result;
			}

			ExtraLargeNumber withLeadingZeros(int x) {
				ExtraLargeNumber result(getLength() + x);
				for (int i = 0; i < getLength(); i++) { result.set(i, get(i)); }
				for (int i = 0; i < x; i++) { result.set(i + getLength(), 0); }
				return result;
			}

	};

	using XLN = ExtraLargeNumber;

	XLN random(int bitlength) {
		data::Bits d = csprng::bits(bitlength);
		d.set(bitlength - 1, 1);
		XLN r(bitlength);
		for (int i = 0; i < bitlength; i++) { r.set(i, d.get(i)); }
		return r;
	}

	inline XLN randomEven(int bitlength) {
		XLN r = random(bitlength);
		r.set(0, 0);
		return r;
	}

	inline XLN randomOdd(int bitlength) {
		XLN r = random(bitlength);
		r.set(0, 1);
		return r;
	}

};



// ksng/util/csprng.h

namespace csprng {

	// Random number generation is entrusted to the Linux kernel
	// which generates random numbers based on hardware events
	// (and quantum buzz on the processor in newer updates)

	data::Bytes bytes(int count) {
		return fileops::readBytes("/dev/urandom", count);
	}

	data::Bits bits(int count) {
		data::Bits result = data::bytesToBits(bytes((count + 1) / 8));
		return result.truncated(result.getLength() % 8);
	}

};


// ksng/util/fileops.h

using namespace std;

namespace fileops {

	int getFileSize(const std::string& filename) {
		ifstream file(filename, ios::binary | ios::ate);
		if (!file.is_open()) {
			// Handle error: unable to open the file
			return -1;
		}

		int size = static_cast<int>(file.tellg());
		file.close();

		return size;
	}

	data::Bytes readBytes(const string& filename, int byteCount) {
		data::Bytes result(byteCount);
		ifstream file(filename);

		char chara = 0;
		int i = 0;
		if (file.is_open()) {
			while (!file.eof() && !(i == byteCount)) {
				file.get(chara);
				result.set(i, static_cast<unsigned char>(chara));
				i++;
			}
		}

		return result;
	}

	data::Bits readBits(const string& filename, int byteCount) {
		return data::bytesToBits(readBytes(filename, byteCount));
	}

	inline data::Bytes readFileBytes(const string& filename) {
		int fileSize = getFileSize(filename);
		return readBytes(filename, fileSize);
	}

	inline data::Bits readFileBits(const string& filename) {
		return data::bytesToBits(readFileBytes(filename));
	}

	int writeFile(string filename, data::Bytes data) {
		ofstream file(filename, ios::binary | ios::trunc);
		if (!file.is_open()) {
			notif::fatal("could not open file");
			return -1;
		}

		if (!file.good()) {
			notif::fatal("fatal error: could not write to file");
			return -2;
		}

		for (int i = 0; i < data.getLength(); i++) {
			file << static_cast<char>(data.get(i));
		}

		file.close();
		return 0;
	}

};


// ksng/util/prime.h

using namespace std;
using namespace xln;

namespace prime {

	class PrimalityTest {

		public:

			virtual void test(XLN n) = 0;
			bool isProbablePrime

	};

}


// ksng/util/dstruct/sll.h

namespace sll {

	template <typename T>
	class SecureLinkedListElement {

		private:

			T value;
			SecureLinkedListElement<T>* next;

			bool locked;

		public:

			explicit SecureLinkedListElement(T value): value(value), next(nullptr), locked(false) {}

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure linked list element", ALERT);
				} 
			}

			T getValue() { securityCheck(); return value; }
			void setValue(T value_) { securityCheck(); value = value_; }

			SecureLinkedListElement<T>* getNext() { securityCheck(); return next; }
			void setNext(SecureLinkedListElement<T>* next_) { securityCheck(); next = next_; }

	};

	template <typename T>
	using SLLE = SecureLinkedListElement<T>;

	template <typename T>
	class SecureLinkedList {

		private:

			SLLE<T>* head;

			bool locked;

		public:

			// Constructors and destructors

			explicit SecureLinkedList(): head(nullptr), locked(false) {}

			~SecureLinkedList() {
				SLLE<T>* current = head;
				SLLE<T>* nextNode;

				while (current != nullptr) {
					nextNode = current->getNext();
					delete current;
					current = nextNode;
				}
			}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure linked list", ALERT);
				} 
			}

			// Writing

			void prepend(T value) {
				securityCheck();

				SLLE<T>* newSLLE = new SLLE<T>(value);

				if (head != nullptr) { newSLLE->setNext(head); }

				head = newSLLE;
			}

			void insert(T value, int index) {
				securityCheck();

				SLLE<T>* newSLLE = new SLLE<T>(value);

				if (index == 0) {
					prepend(value);
					return;
				}

				SLLE<T>* current = head;
				for (int i = 0; i < index - 1 && current != nullptr; i++) { current = current->getNext(); }

				if (current == nullptr) {
					append(value);
					return;
				}

				newSLLE->setNext(current->getNext());
				current->setNext(newSLLE);
			}

			void append(T value) {
				securityCheck();

				SLLE<T>* newSLLE = new SLLE<T>(value);

				if (head == nullptr) {
					head = newSLLE;
				} else {
					SLLE<T>* current = head;
					while (current->getNext() != nullptr) {
						current = current->getNext();
					}

					current->setNext(newSLLE);
				}
			}

			void remove(int index) {
				securityCheck();

				if (head == nullptr) { return; }

				if (index == 0) {
					head = head->getNext();
					return;
				}

				SLLE<T>* current = head;
				for (int i = 0; i < index - 1 && current != nullptr; i++) { current = current->getNext(); }

				if (current == nullptr || current->getNext() == nullptr) { return; }

				SLLE<T>* temp = current->getNext();
				current->setNext(temp->getNext());
				delete temp;
			}

			void set(int index, T value) {
				securityCheck();

				SLLE<T>* current = head;
				for (int i = 0; i < index && current != nullptr; i++) { current = current->getNext(); }

				if (current == nullptr) {
					return T();
				}

				current->setValue(value);
			}

			// Reading

			T get(int index) {
				securityCheck();

				SLLE<T>* current = head;
				for (int i = 0; i < index && current != nullptr; i++) { current = current->getNext(); }

				if (current == nullptr) {
					return T();
				}

				return current->getValue();
			}

			int getLength() {
				securityCheck();
				
				int result = 0;
				SLLE<T>* current = head;
				for (int i = 0; current != nullptr; i++) { current = current->getNext(); result++; }
				return result;
			}

	};

	template <typename T>
	using SLL = SecureLinkedList<T>;

};


// ksng/util/dstruct/snode.h


namespace snode {

	template <typename T>
	class SecureNode {
		
		private:

			bool locked = false;
			string label;
			T value;
			sll::SecureLinkedList<SecureNode<T>*> children;

			int getIndex(string name) {
				for (int i = 0; i < children.getLength(); i++) {
					if (children.get(i)->getName() == name) {
						return i;
					}
				}
				return -1;
			}

		public:

			SecureNode() {}
			SecureNode(string label, T value): label(label), value(value) {}

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure linked list", ALERT);
				} 
			}

			T getValue() { securityCheck(); return value; }
			string getName() { securityCheck(); return label; }
			SecureNode<T> getChild(string name) { return *(children.get(getIndex(name))); }
			SecureNode<T> getChildByIndex(int i) { return *(children.get(i)); }

			void attach(SecureNode<T> child) {
				children.append(&child);
			}

			void detach(string name) {
				int index = getIndex(name);
				if (index != -1) {
					children.remove(index);
				}
			}

			bool has(string name) { return getIndex(name) != -1; }
			string getNameByIndex(int index) { return children.get(index)->getName(); }

			string repr() {
				stringstream ss;
				ss << "<" << label << ": " << value;
				if (children.getLength() > 0) {
					ss << " with children ";
					for (int i = 0; i < children.getLength(); i++) {
						ss << getChildByIndex(i).repr();
						if (i != children.getLength() - 1) {
							ss << ", ";
						}
					}
				}
				ss << ">";
				return ss.str();
			}

	};

	template <typename T>
	using SN = SecureNode<T>;

};


// ksng/util/dstruct/ssq.h

namespace ssq {

	template <typename T>
	class SecureStack {

		private:

			sll::SLL<T> linkedList;

		public:

			void push(T value) {
				linkedList.prepend(value);
			}

			T pop() {
				T r = linkedList.get(0);
				linkedList.remove(0);
				return r;
			}

			int getLength() { return l.getLength(); }

	};

	template <typename T>
	using SS = SecureStack<T>;

	template <typename T>
	class SecureQueue {

		private:

			sll::SecureLinkedList<T> l;

		public:

			void enqueue(T value) {
				l.append(value);
			}

			void enq(T value) { enqueue(value); }

			T dequeue() {
				T r = l.get(0);
				l.remove(0);
				return r;
			}

			T deq() { return dequeue(); }

			int getLength() { return l.getLength(); }

	};

	template <typename T>
	using SQ = SecureQueue<T>;

};


// ksng/util/dstruct/srec.h

namespace srec {

	template <typename T>
	class SecureRecord {

		private:

			sda::SDA<T> data;
			sda::SDA<string> labels;
			int length;

			int getIndex(string label) {
				for (int i = 0; i < length; i++) {
					if (labels.get(i) == label) {
						return i;
					}
				}
				return -1;
			}

		public:

			SecureRecord() {} // not recommended
			SecureRecord(sda::SDA<string> labels_): labels(labels_), length(labels_.getLength()) { data.initialize(labels.getLength()); }

			void reveal() { data.reveal(); labels.reveal(); }
			void hide() { data.hide(); labels.hide(); }

			int getLength() const { return length; }
			string getName(int index) const { return labels.get(index); }

			void set(string label, T value) { int index = getIndex(label); data.set(index, value); }
			T get(string label) { int index = getIndex(label); return data.get(index); }

			string repr() {
				stringstream ss;
				ss << "<";
				for (int i = 0; i < length; i++) {
					ss << labels.get(i) << "=" << data.get(i);
					if (i != length - 1) { ss << ", "; }
				}
				ss << ">";
				return ss.str();
			}

	};

	template <typename T>
	using SR = SecureRecord<T>;

	template <typename T>
	class SecureRecordFormat {

		private:

			sda::SDA<string> names;

		public:

			SecureRecordFormat() {}
			SecureRecordFormat(sda::SDA<string> names): names(names) {}

			SecureRecord<T> make() { return SecureRecord<T>(names); }

			void setNames(sda::SDA<string> names_) { names = names_; }

	};

};



// ksng/util/dstruct/sda.h

using namespace std;

namespace sda {

	bool DISALLOW_OUT_OF_RANGE = true;

	template <typename T>
	class SecureDataArray {

		protected:

			T* elements;
			int length;
			bool locked = false;
			Severity securityLevel;

		public:

			// Constructors & destructors

			explicit SecureDataArray() {}
			explicit SecureDataArray(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			SecureDataArray(initializer_list<T> initList) {
				length = initList.size();
				elements = new T[length];
				locked = false; // By default, not locked when initialized with a list
				securityLevel = ALERT; // Default security level
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

			void initialize(int length_, bool locked_=false, Severity securityLevel_=ALERT) { 
				length = length_;
				elements = new T[length];
				locked = locked_;
				securityLevel = securityLevel_;
			}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure data array", securityLevel);
				} 
			}

			// Access

			T& get(int index) {
				securityCheck();
				if ((index >= getLength() || index < 0) && DISALLOW_OUT_OF_RANGE) {
					notif::fatal("index out of range trying to get item from data array (segfault prevented)");
					return elements[0];
				}
				return elements[index];
			}

			void set(int index, T value) {
				securityCheck();
				if (index >= getLength() || index < 0) {
					notif::fatal("index out of range trying to set item of data array (segfault prevented)");
				}
				elements[index] = value;
			}

			const int getLength() {
				securityCheck();
				return length;
			}

			// Utility

			bool operator==(SecureDataArray<T> other) {
				if (getLength() != other.getLength()) { return false; }
				for (int i = 0; i < getLength(); i++) { if (get(i) != other.get(i)) { return false; } }
				return true;
			}

			string repr(string delimiter=(string)(" ")) {
				stringstream ss;
				for (int i = 0; i < getLength(); i++) {
					ss << get(i);
					if (i != getLength() - 1) {
						ss << delimiter;
					}
				}
				return ss.str();
			}

			SecureDataArray<T> subarray(int a, int b) {
				if (b < a) {
					return subarray(b, a);
				} else if (a == b) {
					SecureDataArray<T> result(1);
					result.set(0, get(a));
					return result;
				} else if (a < b) {
					SecureDataArray<T> result(b - a);
					for (int i = 0; i < b - a; i++) {
						result.set(i, get(a + i));
					}
					return result;
				} else {
					notif::fatal("impossible state: a < b and b > a; recommend system reboot");
				}
			}

			SecureDataArray<T> reverse() {
				int len = getLength();
				SecureDataArray<T> result(len);
				for (int i = 0; i < len; i++) { result.set((len - 1) - i, get(i)); }
				return result;
			}

			bool has(T value) {
				for (int i = 0; i < getLength(); i++) { if (get(i) == value) { return true; } }
				return false;
			}

	};

	template <typename T>
	using SDA = SecureDataArray<T>;

	template <typename T>
	class SecureDataMatrix {

		protected:

			T** elements;
			int rows;
			int cols;
			bool locked = false;
			Severity securityLevel;

		public:

			// Constructors

			SecureDataMatrix() {}
			SecureDataMatrix(int rows, int cols, bool locked=false, Severity securityLevel=ALERT) { initialize(rows, cols, locked, securityLevel); }
			void initialize(int rows_, int cols_, bool locked_, Severity securityLevel_) {
				rows = rows_;
				cols = cols_;
				locked = locked_;
				securityLevel = securityLevel_;
				elements = new T*[rows];
				for (int i = 0; i < rows; i++) {
					elements[i] = new T[cols];
				}
			}

			SecureDataMatrix(initializer_list<initializer_list<T>> initList) {
				initialize(initList.begin()->size(), initList.size(), false, ALERT);
				int i = 0, j = 0;
				for (const auto& row : initList) {
					for (const auto& val : row) {
						elements[i][j] = val;
						j++;
					} 
					i++;
					j = 0;
				}
				cout << endl;
			}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true;}
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure data matrix", securityLevel);
				} 
			}

			// Access

			T& getref(int row, int col) {
				securityCheck();
				if (row >= rows || col >= cols || row < 0 || col < 0) {
					notif::fatal("index out of range trying to get item from data matrix (segfault prevented)");
					return elements[0][0];
				}
				return elements[row][col];
			}

			T get(int row, int col) {
				securityCheck();
				if (row >= rows || col >= cols || row < 0 || col < 0) {
					notif::fatal("index out of range trying to get item from data matrix (segfault prevented)");
					return elements[0][0];
				}
				return elements[row][col];
			}

			void set(int row, int col, T value) {
				securityCheck();
				if (row >= rows || col >= cols || row < 0 || col < 0) {
					notif::fatal("index out of range trying to set item of data matrix (segfault prevented)");
				}
				elements[row][col] = value;
			}

			int getRows() { securityCheck(); return rows; }
			int getCols() { securityCheck(); return cols; }
			int getArea() { securityCheck(); return rows * cols; }

			SecureDataArray<T> getRow(int row) {
				SDA<T> result(cols);
				for (int col = 0; col < cols; col++) {
					result.set(col, get(row, col));
				}
				return result;
			}

			SecureDataArray<T> getCol(int col) {
				SDA<T> result(rows);
				for (int row = 0; row < rows; row++) {
					result.set(get(row, col));
				}
				return result;
			}

			string repr(string delimiter1=(string)(" "), string delimiter2=(string)("\n")) {
				stringstream ss;
				for (int i = 0; i < rows; i++) {
					for (int j = 0; j < cols; j++) {
						ss << get(i, j);
						if (j < cols - 1) {
							ss << delimiter1;
						}
					}
					if (i < rows - 1) {
						ss << delimiter2;
					}
				}
				return ss.str();
			}

	};

	template <typename T>
	using SDM = SecureDataMatrix<T>;

};


// ksng/nyx/ensemble.h




// ksng/nyx/nn.h

namespace nn {

	class Neuron

};


// ksng/nyx/m1.h

class m1 {

	using Entity = sda::SecureDataArray<double>;

	double distance(Entity a, Entity b) {
		double s = 0;
		for (int i = 0; i < a.getLength() && i < b.getLength(); i++) {
			s += pow(a.get(i) - b.get(i), 2);
		}
		return sqrt(s);
	}

	sda::SDA<string> eventRecordFormatNames = {"initial", "final", "action"};
	using Event = srec::SecureRecord<Entity>;
	Event makeEvent(Entity initialState, Entity finalState, Entity action) {
		Event result(eventRecordFormatNames);
		result.set("initial", initialState);
		result.set("final", finalState);
		result.set("action", action);
		return result;
	}

	using Memory = sll::SecureLinkedList<Event>;

	class NyxNextGen_Model1 {

	};

};



// ksng/hazmat/hazmat.h

using namespace std;

namespace hazmat {

	bool HAZMAT_ACCESS_ALLOWED = false;

	void hazmatWarning() {
		if (HAZMAT_ACCESS_ALLOWED) { return; }

		cout << colors::colorize("Kingslayer-NG: hazardous materials warning: hazmat was requested. Grant access? (y/n)", colors::WARNING);
		string usr;
		cin >> usr;

		if (usr == "yes" || usr == "y") {
			cout << colors::colorize("Hazardous materials access granted.", colors::WARNING) << endl;
			HAZMAT_ACCESS_ALLOWED = true;
		} else {
			notif::fatal("hazmat access denied");
		}
	}

	data::Bytes read(string name) {
		hazmatWarning();
		return fileops::readFileBytes("ksng/hazmat/hazmat/" + name + ".hazmat");
	}

	int execute(string name, string interpreter) {
		hazmatWarning();
		stringstream ss;
		ss << interpreter << "ksng/hazmat/hazmat/" << name;
		int result = system(ss.str().c_str());
		return result;
	}

	int executeProgram(string name) {
		return execute(name, (string)("./"));
	}

	int executeBash(string name) {
		return execute(name, (string)("bash "));
	}

	int executePython(string name) {
		return execute(name, (string)("python3 "));
	}

};


// ksng/crypta/direct/deltamine.h
// DELTA MINimization Electronically


namespace deltamine {

	class EncryptionDeltamine: public cryptanalyst::Cryptanalyst {

		protected:

			bool needsPlaintext() override { return true; }
			bool needsCiphertext() override { return true; }
			bool needsPlaintexts() override { return false; }
			bool needsCiphertexts() override { return false; }

			data::Bytes _cryptanalyze() override {
				data::Bits test = csprng::bits(targetSize);
				data::Bytes transient;
				data::Bytes encryption;
				data::Bits encryptionBits;
				data::Bits ciphertextBits
				sda::SDA<int> deltas(targetSize);
				int delta;

				while (true) {
					
					// Search for the best modification to make and then make it

					for (int i = 0; i < targetSize; i++) {
						test.set(i, test.get(i) ^ 1);
						transient = data::bitsToBytes(test);

						encryption = cryptosuite.encryptSymmetric(plaintext, key::Key(transient));
						

						deltas.set(i, delta);
					}

				}
			}

	};

};


// ksng/crypta/core/analyst.h

namespace cryptanalyst {

	// Cryptanalyst class - intended to be overridden

	class Cryptanalyst {

		private:

			bool hasPlaintext = false;
			bool hasCiphertext = false;
			bool hasPlaintexts = false;
			bool hasCiphertexts = false;

		protected:

			int targetSize;
			data::Bytes plaintext;
			data::Bytes ciphertext;
			sda::SDA<data::Bytes> plaintexts;
			sda::SDA<data::Bytes> ciphertexts;
			suite::CryptographicSuite cryptosuite;

			virtual data::Bytes _cryptanalyze() = 0;

			virtual bool needsPlaintext() = 0;
			virtual bool needsCiphertext() = 0;
			virtual bool needsPlaintexts() = 0;
			virtual bool needsCiphertexts() = 0;

		public:

			// setup

			Cryptanalyst(
				int ts = 256,
				cipher::SymmetricCipher* sc = nullptr,
				cipher::AsymmetricCipher* asc = nullptr,
				hash::HashFunction* hf = nullptr,
				hash::KeyedHashFunction* khf = nullptr
			): targetSize(ts) {
				cryptosuite.setSymmetricCipher(sc);
				cryptosuite.setAsymmetricCipher(asc);
				cryptosuite.setHashFunction(hf);
				cryptosuite.setKeyedHashFunction(khf);
			}

			void setTargetSize(int ts) { targetSize = ts; }

			void setSymmetricCipher(cipher::SymmetricCipher* cipher) { cryptosuite.setSymmetricCipher(cipher); }
			void setAsymmetricCipher(cipher::AsymmetricCipher* cipher) { cryptosuite.setAsymmetricCipher(cipher); }
			void setHashFunction(hash::HashFunction* hashf) { cryptosuite.setHashFunction(hashf); }
			void setKeyedHashFunction(hash::KeyedHashFunction* hashf) { cryptosuite.setKeyedHashFunction(hashf); }

			void setPlaintext(data::Bytes pt) { hasPlaintext = true; plaintext = pt; }
			void setCiphertext(data::Bytes ct) { hasCiphertext = true; ciphertext = ct; }
			void setPlaintexts(sda::SDA<data::Bytes> pts) { hasPlaintexts = true; plaintexts = pts; }
			void setCiphertexts(sda::SDA<data::Bytes> cts) { hasCiphertexts = true; ciphertexts = cts; }

			data::Bytes cryptanalyze() {
				if (
					(needsPlaintext() && !hasPlaintext)
					|| (needsCiphertext() && !hasCiphertext)
					|| (needsPlaintexts() && !hasPlaintexts)
					|| (needsCiphertexts() && !hasCiphertexts)
				) {
					notif::error("insufficient data to cryptanalyze");
					return data::Bytes(0);
				}

				return _cryptanalyze();
			}

	};

};
