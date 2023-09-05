
# EMPYREA is slower, but provably perfectly secure.
class EMPYREA1(Cryptosystem):
	name = "Electronic Message Protection using Young and Radical Encryption Algorithm 1"
	
	class Key:
		def __init__(self, data: bytes) -> None:
			self.data = data

		def codeData(self, length: int) -> tuple[bytes, list[int]]:
			pass

		def encode(self) -> bytes:
			return self.data
		
		@classmethod
		def decode(cls, data: bytes) -> "Key":
			return cls(data)

	def generateKeys(self, length: int) -> "Key":
		return self.Key(urandom(length // 8))
	
	def encrypt(self, data: bytes, key: "Key") -> bytes:
		keystream, swap = key.codeData(len(data))

		data = utils.bytestringXOR(data, keystream)
		
		for index1, index2 in enumerate(swap):
			data[index1], data[index2] = data[index2], data[index1]

		return data

	def decrypt(self, data: bytes, key: "Key") -> bytes:
		keystream, swap = key.codeData(len(data))

		for index1, index2 in enumerate(swap)[::-1]:
			data[index2], data[index1] = data[index1], data[index2]

		return utils.bytestringXOR(data, keystream)

class INFERNA1(FeistelCryptosystem):
	name = "Intelligent Non-balanced Feistel Encryption with Rotors and Novel Algorithms 1"

	blockSize = 32
	pivot = 12

	class Subkey(FeistelCryptosystem.Subkey):
		identifier = b"INFR"

	class Key(FeistelCryptosystem.Key):
		identifier = b"INFR"

		def deriveSubkeys(self) -> list["Subkey"]:
			sks = []
			last = self.data
			for _ in range(16):
				data = bytes(CRASH256(last))
				sks.append(INFERNA1.Subkey(data))
				last = data
			return sks

	def round(self, data, subkey):
		return bytes(CRASH256(utils.bytestringXOR(data, subkey.data)))

# non functional
class CALIBER(Cryptosystem):
	name = "Cryptographic Algorithm for Lightweight and Intelligent Bitstring Encryption with Reversibility"
	
	@dataclass
	class Key:
		data: bytes

		def bits(self) -> list[int]:
			return utils.decompose(utils.bytesToInt(self.data), 2)

		def next(self) -> object:
			return type(self)(utils.intToBytes(utils.compose(CRASH.f(self.data, len(self.data) * 8, 8 * (len(self.data) // 15)))))

	def encrypt(self, plaintext: bytes, key: "Key") -> bytes:
		keystream = b""
		while len(keystream) < len(plaintext):
			key = key.next()
			keystream += key.data[:1]

		return utils.bytestringXOR(keystream, plaintext)

	def decrypt(self, ciphertext: bytes, key: "Key") -> bytes:
		keystream = b""
		while len(keystream) < len(ciphertext):
			key = key.next()
			keystream += key.data

		return utils.bytestringXOR(keystream, ciphertext)


class VoiceOfTheLegion(Cryptosystem):
	# If you can break this, then you're not a cryptographer, you're a god.

	name = "Voice of The Legion version 1"

	identifier = b"VOTL"

	class Key:
		def __init__(self, data: bytes, previous: "Key" = None) -> None:
			self.data = data
			self.previous = previous

		def __repr__(self) -> str:
			return "<VoiceOfTheLegion.Key " + self.data.hex() + ">"

		@property
		def next(self) -> "Key":
			return type(self)(bytes(CRASH256(self.data)), previous=self)

		@property
		@cache
		def permutation(self) -> dict[int, int]:
			elements = [(x * (utils.bytesToInt(self.data) | 1)) % 32 for x in range(32)]
			p = []
			index = 0
			while len(p) < 32:
				p.append(
					elements.pop(self.data[index % len(self.data)] % len(elements))
				)
				index += 1
			return {x: y for x, y in zip(range(32), p)}

		@property
		@cache
		def antipermutation(self) -> dict[int, int]:
			return {value: key for key, value in zip(self.permutation.keys(), self.permutation.values())}

		def permute(self, data: bytes, permutation: dict[int, int], blockSize: int=4) -> bytes:
			bits = utils.bytesToBits(data)
			bits = [bits[permutation[x % len(permutation)] % len(bits)] for x in range(blockSize * 8)]
			return utils.bitsToBytes(bits)

		@property
		@cache
		def keystream(self) -> bytes:
			return bytes([a ^ b for a, b in zip(bytes(CRASH32(bytes([a ^ b for a, b in zip(self.data, b"VOTL")]))), b"VOTL")])

		def encode(self) -> bytes:
			return self.data

		@classmethod
		def fromBytes(cls, data: bytes) -> "Key":
			return cls(data)

	keyclass = Key

	def encrypt(self, plaintext: bytes, key: "Key", blockSize: int=4):
		while len(plaintext) % 4:
			plaintext += b"\x00"

		blocks = [plaintext[(x * blockSize):((x + 1) * blockSize)] for x in range(len(plaintext) // blockSize)]
		subkey = key.next

		ciphertext = b""

		for block in blocks:
			block = subkey.permute(block, subkey.permutation, blockSize)
			block = bytes([a ^ b for a, b in zip(block, subkey.keystream)])

			ciphertext += block
			subkey = subkey.next

		return ciphertext

	def decrypt(self, ciphertext: bytes, key: "Key", blockSize: int=4):
		blocks = [ciphertext[(x * blockSize):((x + 1) * blockSize)] for x in range(len(ciphertext) // blockSize)]
		subkey = key.next

		plaintext = b""

		for block in blocks:
			block = bytes([a ^ b for a, b in zip(block, subkey.keystream)])
			block = subkey.permute(block, subkey.antipermutation, blockSize)

			plaintext += block
			subkey = subkey.next

		return plaintext

class ScriptOfTheLegion:

	class Script:
		def __init__(self, puzzles: list[bytes]=None, transient: bytes=b"") -> None:
			if puzzles != None:
				self._puzzles = puzzles
				return
			
			self._puzzles = []

			for x in range(256):
				print("\rLoading script ... (" + str(round(x / 256 * 100, 4)) + "%)                        ", end="")
				byte = bytes([x])
				tag = self._getNewTag()
				plaintext = tag + byte
				ciphertext = ScriptOfTheLegion.symmetric.encrypt(plaintext, ScriptOfTheLegion.symmetric.Key(transient + byte))
				self._puzzles.append((tag, byte, ciphertext))

			self._puzzles = secureShuffle(self._puzzles)

			print("\rScript loaded successfully, correcting errors ...")

			keysToRegenerate = []
			for tag, key, ciphertext in self._puzzles:
				if len(ciphertext) != 4:
					keysToRegenerate.append(key)

			self._puzzles = [(tag, key, ciphertext) for tag, key, ciphertext in self._puzzles if len(ciphertext) == 4]

			for byte in keysToRegenerate:
				tag = self._getNewTag()
				plaintext = tag + byte
				ciphertext = ScriptOfTheLegion.symmetric.encrypt(plaintext, ScriptOfTheLegion.symmetric.Key(transient + byte))
				self._puzzles.append((tag, byte, ciphertext))

		def __iter__(self) -> Iterable[tuple[bytes, bytes]]:
			return iter(self._puzzles)

		def __getitem__(self, name: bytes) -> bytes:
			for tag, key, ciphertext in self:
				if tag == name:
					return key

		def _getNewTag(self) -> bytes:
			tag = urandom(3)
			while tag in self.tags or len(tag) != 3: # idk ok?
				tag = urandom(3)
			return tag

		@property
		def tags(self) -> list[bytes]:
			return [tag for tag, key, ciphertext in self]
		@property
		def puzzles(self) -> list[bytes]:
			return [ciphertext for tag, key, ciphertext in self]

		def encode(self) -> bytes:
			return b"".join(self.puzzles)

		@staticmethod
		def crack(data: bytes, transient: bytes=b"") -> tuple[bytes, bytes]:
			if len(data) % 4:
				raise SyntaxError("Bad puzzle set.")

			puzzles = [data[x * 8:(x + 1) * 8] for x in range(len(data) // 8)]
			puzzle = puzzles[randbelow(len(puzzles))]

			possibleKeys = [bytes([x]) for x in range(256)]

			while len(possibleKeys) > 0:
				testKey = possibleKeys.pop(randbelow(len(possibleKeys)))
				decryption = ScriptOfTheLegion.symmetric.decrypt(puzzle, ScriptOfTheLegion.symmetric.Key(transient + testKey))
				tag, key == decryption[:3], decryption[3]
				if testKey == key:
					return tag, key	