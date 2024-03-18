#pragma once
#include <cmath>
#include "../util/dstruct/sda.h"
#include "complex.h"
#include <bitset>
#include <time.h>
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