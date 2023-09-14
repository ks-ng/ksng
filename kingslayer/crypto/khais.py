# Kingslayer Hashing Algorithm for Integrity and Security

from kingslayer.crypto.cryptocore import Cryptosystem
from kingslayer.crypto.cryptocore import HashingAlgorithm

@Cryptosystem.register("KHAIS")
class KHAIS(HashingAlgorithm):
	def hash(self, data: bytes) -> bytes:
		for _ in range(8):
			output = []
			for byte in data:
				output.append(
					pow(byte + sum(output), byte, 256)
				)
			data = bytes(output)
		return data
