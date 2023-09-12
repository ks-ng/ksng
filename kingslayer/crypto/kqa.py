from kingslayer.q import QuantumRegister
from kingslayer.q import QuantumGate
from kingslayer.crypto.cryptocore import Cryptosystem
from kingslayer.crypto.cryptocore import SymmetricAlgorithm
from kingslayer.crypto.crux import CRUX

from functools import cache
from math import log
from math import ceil

def isPowerOfTwo(n: int) -> bool:
	return log(n, 2) % 1 == 0
def nextPowerOfTwo(n: int) -> int:
	return 2 ** ceil(log(n, 2))

@Cryptosystem.register("KQA")
class KQA(SymmetricAlgorithm):

	class Key:
		def __init__(self, data: bytes):
			self.data = self.original = data

		def __bytes__(self) -> bytes:
			return self.data

		@cache
		def _blocks(self) -> list[list[list[int]]]:
			blocks = []
			block = []
			for byte in self.data:
				if byte in block:
					blocks.append(block)
					block = []
				block.append(byte)

			correctedBlocks = []
			for block in blocks:
				if not isPowerOfTwo(len(block)):
					for x in range(len(block)):
						if utils.decompose(x, 2) not in block:
							block.append(utils.decompose(x, 2))
						if isPowerOfTwo(len(block)):
							break
				correctedBlocks.append(block)
			
			for block in correctedBlocks:
				if not isPowerOfTwo(len(block)):
					raise SyntaxError("Rewrite the block correction function pls!!")
				
			return correctedBlocks
		
		@cache
		def _gates(self) -> list[QuantumGate]:
			return [QuantumGate(block) for block in self._blocks()]