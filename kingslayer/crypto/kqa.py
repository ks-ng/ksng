from kingslayer.q import QuantumRegister
from kingslayer.q import QuantumGate
from kingslayer.crypto.cryptocore import Cryptosystem
from kingslayer.crypto.cryptocore import SymmetricAlgorithm

@Cryptosystem.register("KQA")
class KQA(SymmetricAlgorithm):

	class Key:
		def __init__(self, data: bytes):
			self.data = data

		def __bytes__(self) -> bytes:
			return self.data

		def permutation(self) -> list[list[int]]:
			