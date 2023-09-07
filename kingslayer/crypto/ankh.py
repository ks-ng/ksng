from kingslayer.crypto.cryptocore import Cryptosystem
from kingslayer.crypto.cryptocore import SymmetricAlgorithm

@Cryptosystem.register("ANKH")
class Dyavolsky(SymmetricAlgorithm):

	class Key:
		def __init__(self, data: bytes) -> None:
			self.data = data

		def __bytes__(self) -> bytes:
			return self.data
