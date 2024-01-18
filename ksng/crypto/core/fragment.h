#include "../../util/csprng.h"
#include "../../util/data.h"

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