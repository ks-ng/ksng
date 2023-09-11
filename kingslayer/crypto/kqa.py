from kingslayer.q import QuantumRegister
from kingslayer.q import QuantumGate
from kingslayer.crypto.cryptocore import Cryptosystem
from kingslayer.crypto.cryptocore import SymmetricAlgorithm
from kingslayer.crypto.crux import CRUX

from functools import cache

@Cryptosystem.register("KQA")
class KQA(SymmetricAlgorithm):

	class Key:
		def __init__(self, data: bytes):
			self.data = self.original = data

		def __bytes__(self) -> bytes:
			return self.data

		@cache
		def gates(self) -> list[QuantumGate]:
			gates = []
			for 
