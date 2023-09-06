from kingslayer.crypto.cryptocore import Cryptosystem
from kingslayer.crypto.cryptocore import SymmetricAlgorithm

@Cryptosystem.register("Dyavolsky-1", "D1")
class Dyavolsky(SymmetricAlgorithm):

	class Key:
		def __init__(self, data: bytes) -> None:
			self.data = data

		def __bytes__(self) -> bytes:
			return self.data
