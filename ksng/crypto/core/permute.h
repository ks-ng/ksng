#pragma once
#include "../../util/data.h"

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

	void fixPermutation(data::Bytes& data) { 
		for (int i = 0; i < 256) { while (data.has(data.get(i))) { data.set(i, data.get(i) + 1); } }
	}

};