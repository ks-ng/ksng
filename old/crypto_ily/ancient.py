from math import ceil

from kingslayer import utils
from kingslayer.crypto.crypto import*

class Magma(BalancedFeistelCryptosystem):
	blockSize = 64

	class Key(BalancedFeistelCryptosystem.Key):
		def deriveSubkeys(self) -> list[FeistelCryptosystem.Subkey]:
			blocks = [self.data[x * 8:(x + 1) * 8] for x in range(len(self.data) // 8)]
			return (blocks * 3) + blocks[::-1]

	def round(data: bytes, subkey):
		pass

GOST28147_89 = Magma

class DES(Cryptosystem):
	ip = [
		58, 50, 42, 34, 26, 18, 10, 2,
		60,	52,	44,	36,	28,	20,	12,	4,
		62,	54,	46,	38,	30,	22,	14,	6,
		64,	56,	48,	40,	32,	24,	16,	8,
		57,	49,	41,	33,	25,	17,	9,	1,
		59,	51,	43,	35,	27,	19,	11,	3,
		61,	53,	45,	37,	29,	21,	13,	5,
		63,	55,	47,	39,	31,	23,	15,	7
	]

	fp = [
		40,	8,	48,	16,	56,	24,	64,	32,
		39,	7,	47,	15,	55,	23,	63,	31,
		38,	6,	46,	14,	54,	22,	62,	30,
		37,	5,	45,	13,	53,	21,	61,	29,
		36,	4,	44,	12,	52,	20,	60,	28,
		35,	3,	43,	11,	51,	19,	59,	27,
		34,	2,	42,	10,	50,	18,	58,	26,
		33,	1,	41,	9,	49,	17,	57,	25
	]

	p = [
		16,	7,	20,	21,	29,	12,	28,	17,
		1,	15,	23,	26,	5,	18,	31,	10,
		2,	8,	24,	14,	32,	27,	3,	9,
		19,	13,	30,	6,	22,	11,	4,	25
	]

	e = [
		32,	1,	2,	3,	4,	5,
		4,	5,	6,	7,	8,	9,
		8,	9,	10,	11,	12,	13,
		12,	13,	14,	15,	16,	17,
		16,	17,	18,	19,	20,	21,
		20,	21,	22,	23,	24,	25,
		24,	25,	26,	27,	28,	29,
		28,	29,	30, 31,	32,	1
	]

	s = (
		(
			14, 4,  13, 1,  2,  15, 11, 8,  3,  10, 6,  12, 5,  9,  0,  7,
			0,  15, 7,  4,  14, 2,  13, 1,  10, 6,  12, 11, 9,  5,  3,  8,
			4,  1,  14, 8,  13, 6,  2,  11, 15, 12, 9,  7,  3,  10, 5,  0,
			15, 12, 8,  2,  4,  9,  1,  7,  5,  11, 3,  14, 10, 0,  6,  13,
		), (
			15, 1,  8,  14, 6,  11, 3,  4,  9,  7,  2,  13, 12, 0,  5,  10,
			3,  13, 4,  7,  15, 2,  8,  14, 12, 0,  1,  10, 6,  9,  11, 5,
			0,  14, 7,  11, 10, 4,  13, 1,  5,  8,  12, 6,  9,  3,  2,  15,
			13, 8,  10, 1,  3,  15, 4,  2,  11, 6,  7,  12, 0,  5,  14, 9
		), (
			10, 0,  9,  14, 6,  3,  15, 5,  1,  13, 12, 7,  11, 4,  2,  8,
			13, 7,  0,  9,  3,  4,  6,  10, 2,  8,  5,  14, 12, 11, 15, 1,
			13, 6,  4,  9,  8,  15, 3,  0,  11, 1,  2,  12, 5,  10, 14, 7,
			1,  10, 13, 0,  6,  9,  8,  7,  4,  15, 14, 3,  11, 5,  2,  12
		), (
			7,  13, 14, 3,  0,  6,  9,  10, 1,  2,  8,  5,  11, 12, 4,  15,
			13, 8,  11, 5,  6,  15, 0,  3,  4,  7,  2,  12, 1,  10, 14, 9,
			10, 6,  9,  0,  12, 11, 7,  13, 15, 1,  3,  14, 5,  2,  8,  4,
			3,  15, 0,  6,  10, 1,  13, 8,  9,  4,  5,  11, 12, 7,  2,  14,
		), (
			2,  12, 4,  1,  7,  10, 11, 6,  8,  5,  3,  15, 13, 0,  14, 9,
			14, 11, 2,  12, 4,  7,  13, 1,  5,  0,  15, 10, 3,  9,  8,  6,
			4,  2,  1,  11, 10, 13, 7,  8,  15, 9,  12, 5,  6,  3,  0,  14,
			11, 8,  12, 7,  1,  14, 2,  13, 6,  15, 0,  9,  10, 4,  5,  3,
		), (
			12, 1,  10, 15, 9,  2,  6,  8,  0,  13, 3,  4,  14, 7,  5,  11,
			10, 15, 4,  2,  7,  12, 9,  5,  6,  1,  13, 14, 0,  11, 3,  8,
			9,  14, 15, 5,  2,  8,  12, 3,  7,  0,  4,  10, 1,  13, 11, 6,
			4,  3,  2,  12, 9,  5,  15, 10, 11, 14, 1,  7,  6,  0,  8,  13,
		), (
			4,  11,  2, 14, 15, 0,  8,  13, 3,  12, 9,  7,  5,  10, 6,  1,
			13, 0,  11, 7,  4,  9,  1,  10, 14, 3,  5,  12, 2,  15, 8,  6,
			1,  4,  11, 13, 12, 3,  7,  14, 10, 15, 6,  8,  0,  5,  9,  2,
			6,  11, 13, 8,  1,  4,  10, 7,  9,  5,  0,  15, 14, 2,  3,  12,
		), (
			13, 2,  8,  4,  6,  15, 11, 1,  10, 9,  3,  14, 5,  0,  12, 7,
			1,  15, 13, 8,  10, 3,  7,  4,  12, 5,  6,  11, 0,  14, 9,  2,
			7,  11, 4,  1,  9,  12, 14, 2,  0,  6,  10, 13, 15, 3,  5,  8,
			2,  1,  14, 7,  4,  10, 8,  13, 15, 12, 9,  0,  3,  5,  6,  11,
		)
	)

	pc1 = (
		56, 48, 40, 32, 24, 16, 8,
		0,  57, 49, 41, 33, 25, 17,
		9,  1,  58, 50, 42, 34, 26,
		18, 10, 2,  59, 51, 43, 35,
		62, 54, 46, 38, 30, 22, 14,
		6,  61, 53, 45, 37, 29, 21,
		13, 5,  60, 52, 44, 36, 28,
		20, 12, 4,  27, 19, 11, 3,
	)
	
	pc2 = (
		13, 16, 10, 23, 0,  4,
		2,  27, 14, 5,  20, 9,
		22, 18, 11, 3,  25, 7,
		15, 6,  26, 19, 12, 1,
		40, 51, 30, 36, 46, 54,
		29, 39, 50, 44, 32, 47,
		43, 48, 38, 55, 33, 52,
		45, 41, 49, 35, 28, 31,
	)

	def _permute(self, data: list[int], permutation: list[int]) -> list[int]:
		return [data[x] for x in permutation]

	class Subkey(BalancedFeistelCryptosystem.Subkey):
		pass

	class Key(BalancedFeistelCryptosystem.Key):
		def deriveSubkeys(self):
			pass

	def round(self, data: bytes, subkey: "Subkey"):
		combination = utils.bytestringXOR(subkey.data, data)
		bits = utils.decompose(int(combination.hex(), 16), 2)
		subblocks = [utils.compose(bits[x * 6:(x + 1) * 6], 2) for x in range(8)]
		for index, subblock in enumerate(subblocks):
			subblocks[index] = self._permute(subblock, self.s[index])

		sbits = []
		for subblock in subblocks:
			sbits += utils.decompose(subblock, 2)

		output = self._permute(sbits, self.p)
		return utils.intToBytes(utils.compose(output, 2))
	
class RC4(StreamCryptosystem):
	# Rivest Cipher 4

	# Alternatively "Ron's Code 4", this cipher is used on WEP-encrypted networks
	# despite being widely regarded as insecure (see Fluhrer-Mantin-Shamir attack)
	# and can be cryptanalyzed easily. One known plaintext of a length n allows an
	# attacker to read the first n bytes (or bits, depending) of future ciphertexts
	# when RC4 is improperly implemented.

	name = "Rivest Cipher 4" # alternatively Ron's Code 4
	identifier = b"ARC4"
	secure = False

	class Key:
		identifier = b"ARC4"
		def __init__(self, data: bytes):
			self.data = data
			self.schedule = self.schedule()

		def __len__(self) -> int:
			return len(self.data)

		def schedule(self) -> bytes:
			s = list(range(256))
			j = 0
			for i in range(256):
				j = (j + s[i] + utils.bytesToInt(self.data[i % len(self)])) % 256
				s[i], s[j] = s[j], s[i]
			return s

		def stream(self, nbytes: int) -> bytes:
			i = j = 0
			k = b""
			for _ in range(nbytes):
				i = (i + 1) % 256
				j = (j + self.schedule[i]) % 256
				self.schedule[i], self.schedule[j] = self.schedule[j], self.schedule[i]
				x = self.schedule[(self.schedule[i] + self.schedule[j]) % 256]
				k += utils.intToBytes(x)
			return k

		def reset(self) -> None:
			self.schedule = self.schedule()

		def encode(self) -> bytes:
			return self.data

		@classmethod
		def fromBytes(cls, data: bytes) -> object:
			return cls(data)

		@classmethod
		def random(cls) -> object:
			return cls(randbytes(128))

ARC4 = RC4 # hehe

class A51(Cryptosystem):
	class Key:
		def __init__(self, data: bytes):
			self.data = data
			self.bits = utils.decompose(utils.bytesToInt(self.data), 2)

			self.lfsr1 = FibonacciLFSR(19, taps=[18, 17, 16, 13])
			self.lfsr2 = FibonacciLFSR(22, taps=[21, 20, 10])
			self.lfsr3 = FibonacciLFSR(23, taps=[22, 21, 20, 7])

			block1 = self.bits[:19]
			block2 = self.bits[19:22]
			block3 = self.bits[23:]

			self.lfsr1 << block1
			self.lfsr2 << block2
			self.lfsr3 << block3

		def __iter__(self) -> Iterable[FibonacciLFSR, int]:
			return iter([
				(self.lfsr1, 8),
				(self.lfsr2, 10),
				(self.lfsr3, 10)
			])

		def _nextBit(self) -> int:
			bits = []
			for lfsr, clocking in self:
				bits.append(lfsr[clocking])
			
			if len([x for x in bits if x == 1]) > len([x for x in bits if x == 0]):
				majority = 1
			else:
				majority = 0
			
			output = 0
			for lfsr, clocking in self:
				if clocking == majority:
					output ^= lfsr.nextBit()

			return output

		def keystream(self, bytecount: int) -> bytes:
			bitcount = bytecount * 8

			bits = [self._nextBit() for bit in bitcount]
			return utils.intToBytes(compose(bits, 2))

		def reset(self) -> None:
			type(self).__init__(self, self.data)

class DiffieHellmanKeyExchange(KeyAgreement):

	class DiffieHellmanParameters:
		PADDING = b"\xff\xff\xff\xff\x00\x00\x00\x00\xff\xff\xff\xff"

		def __init__(self, bitlength: int=2048) -> None:
			self.p = getprime(bitlength)
			self.g = utils.getPrimitiveRoot(self.p)

		def encode(self) -> bytes:
			return utils.intToBytes(self.p) + self.PADDING + utils.iuntToBytes(self.g)

		@classmethod
		def fromParameters(cls, p, g):
			dhp = cls(11) # because the prime-getting algorithm dislikes n<=10
			dhp.p, dhp.g = p, g
			return dhp
		
		@classmethod
		def fromBytes(cls, data: bytes) -> object:
			data = data.split(cls.PADDING)
			p, g = utils.bytesToInt(data[0]), utils.bytesToInt(data[1])
			return cls.fromParameters(p, g)

	def run(self, params: DiffieHellmanParameters) -> bytes:
		a = randint(2, params.p)
		A = pow(params.g, a, params.p)
		self.send(utils.intToBytes(A))
		_B = self.receive()
		B = utils.bytesToInt(_B)
		s = pow(B, a, params.p)
		return s

	def host(self, params: DiffieHellmanParameters) -> bytes:
		_A = self.recv()
		A = utils.intToBytes(_A)
		b = random.randint(2, params.p)
		B = pow(params.g, b, params.p)
		self.send(utils.intToBytes(B))
		s = pow(A, b, params.p)
		return s