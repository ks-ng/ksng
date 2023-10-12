from kingslayer import q
from kingslayer.crypto.cryptocore import HashingAlgorithm

class QSH(HashingAlgorithm):
	def hash(self, data: bytes) -> bytes:
		
