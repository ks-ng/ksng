#pragma once
#include "fileops.h"

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