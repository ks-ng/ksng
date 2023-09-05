from os import urandom
from dataclasses import dataclass
from secrets import choice
from typing import Iterable
from typing import Union
from secrets import randbelow
from secrets import token_bytes as randbytes
from random import randint
from random import SystemRandom
from struct import pack
from struct import unpack
from math import log
from math import sqrt
import sys # 62ee872f5faf8 62914b1e397b8 5f118d9faceca 6464e6fbc6465 8230625
from functools import cache

from kingslayer.crypto.crypto import*
from kingslayer.data.s16 import s as SBOX16
from kingslayer import utils

def secureShuffle(lst):
    # Create a copy of the list to avoid modifying the original list
    shuffled_lst = lst[:]
    cryptogen = SystemRandom()

    # Fisher-Yates shuffle algorithm
    n = len(shuffled_lst)
    for i in range(n - 1, 0, -1):
        j = cryptogen.randint(0, i)
        shuffled_lst[i], shuffled_lst[j] = shuffled_lst[j], shuffled_lst[i]

    return shuffled_lst

# ========== INTEGRITY & AUTHENTICITY ========== #

def contract(bits: list[int]) -> list[int]:
	output = []
	for index, bit in list(enumerate(bits))[:-1]:
		cx = 0
		for b in output:
			cx ^= b
		for b in bits:
			cx ^= b

		output.append(bit ^ cx ^ bits[index + 1])

	return output

def expand(bits: list[int]) -> list[int]:
	output = []
	cx = 1
	for index, bit in list(enumerate(bits))[:-1]:
		cx ^= bit
		cx ^= bits[index + 1]
		for b in output:
			cx ^= b
		output.append(cx)

	for _ in range(2):
		cx ^= bits[-1]
		for b in output:
			cx ^= b
		output.append(cx)

	return output

def rotateLeft(l: list, n: int=1) -> list:
	n %= len(l)
	rot = l[1:] + l[:1]
	if n == 1:
		return rot
	return rotateLeft(rot, n - 1)

def rotateRight(l: list, n: int=1) -> list:
	n %= len(l)
	rot = l[:-1] + l[:-1]
	if n == 1:
		return rot
	return rotateRight(rot, n - 1)

SBOX8 = {
		0: 28, 1: 179, 2: 173, 3: 185, 4: 159, 5: 97, 6: 53, 7: 14,
		8: 247, 9: 87, 10: 114, 11: 171, 12: 56, 13: 254, 14: 135, 15: 43,
		16: 23, 17: 89, 18: 9, 19: 243, 20: 24, 21: 141, 22: 174, 23: 104,
		24: 52, 25: 86, 26: 88, 27: 41, 28: 144, 29: 129, 30: 220, 31: 153,
		32: 232, 33: 36, 34: 158, 35: 44, 36: 107, 37: 39, 38: 248, 39: 110,
		40: 239, 41: 38, 42: 27, 43: 123, 44: 206, 45: 138, 46: 81, 47: 117,
		48: 119, 49: 55, 50: 7, 51: 13, 52: 241, 53: 120, 54: 128, 55: 250,
		56: 100, 57: 105, 58: 166, 59: 246, 60: 127, 61: 83, 62: 34, 63: 200,
		64: 103, 65: 59, 66: 199, 67: 231, 68: 142, 69: 132, 70: 226, 71: 49,
		72: 251, 73: 134, 74: 63, 75: 228, 76: 237, 77: 54, 78: 133, 79: 121,
		80: 70, 81: 209, 82: 37, 83: 190, 84: 213, 85: 64, 86: 76, 87: 122,
		88: 8, 89: 48, 90: 94, 91: 168, 92: 11, 93: 208, 94: 148, 95: 30,
		96: 50, 97: 17, 98: 4,99: 40, 100: 78, 101: 233, 102: 169, 103: 32,
		104: 172, 105: 188, 106: 26, 107: 31, 108: 12, 109: 115, 110: 33, 111: 183,
		112: 126, 113: 147, 114: 137, 115: 189, 116: 219, 117: 222, 118: 5, 119: 130,
		120: 191, 121: 111, 122: 204, 123: 118, 124: 240, 125: 6, 126: 46, 127: 156,
		128: 95, 129: 196, 130: 162, 131: 19, 132: 214, 133: 193, 134: 80, 135: 182,
		136: 238, 137: 205, 138: 217, 139: 244, 140: 113, 141: 195, 142: 16, 143: 216,
		144: 58, 145: 211, 146: 0, 147: 178, 148: 18, 149: 79, 150: 68, 151: 131,
		152: 65, 153: 161, 154: 155, 155: 62, 156: 212, 157: 253, 158: 201, 159: 22,
		160: 45, 161: 85, 162: 154, 163: 106, 164: 198, 165: 157, 166: 230, 167: 149,
		168: 96, 169: 202, 170: 151, 171: 180, 172: 93, 173: 21, 174: 184, 175: 145,
		176: 177, 177: 224, 178: 192, 179: 69, 180: 10, 181: 35, 182: 221, 183: 66,
		184: 116, 185: 163, 186: 150, 187: 124, 188: 207, 189: 227, 190: 75, 191: 3,
		192: 25, 193: 170, 194: 73, 195: 125, 196: 57, 197: 235, 198: 197, 199: 15,
		200: 215, 201: 71, 202: 101, 203: 67, 204: 42, 205: 229, 206: 2, 207: 210,
		208: 136, 209: 160, 210: 84, 211: 98, 212: 20, 213: 165, 214: 139, 215: 47,
		216: 194, 217: 164, 218: 223, 219: 234, 220: 82, 221: 90, 222: 29, 223: 143,
		224: 152, 225: 74, 226: 72, 227: 255, 228: 186, 229: 176, 230: 61, 231: 242,
		232: 102, 233: 77, 234: 92, 235: 249, 236: 140, 237: 167, 238: 112, 239: 91,
		240: 1, 241: 181, 242: 187, 243: 146, 244: 109, 245: 236, 246: 203, 247: 175,
		248: 99, 249: 60, 250: 218, 251: 108, 252: 51, 253: 245, 254: 225, 255: 252}

class CRASH256(Hash):
	name = "CRASH256"

	def hash(self, data: bytes) -> bytes:
		data = list(data)

		primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131]

		if len(data) != 32:
			index = -1
			initial = 0

			while len(data) > 32:
				index = (index + 1) % 32
				data[index] ^= data[-1]
				data.pop()

			while len(data) < 32:
				index += 1
				initial ^= data[index]
				data.append(primes[index] ^ initial)

		registers = [prime for prime in primes]

		for offset in range(32):
			for index, byte in enumerate(data):
				registers[index] ^= byte
				data[index] ^= registers[byte % 32]

			data = data[:31] + [data[31]]

		return bytes(registers)

class ACE(Hash):
	"""
	Automatic Correction of Errors - an error-correcting code
	Can correct a large number of errors in multiple words of n bytes, for any natural number n.

	Notably, errors can't be detected if the same bit in multiple bytes in a block are flipped an even number of times.
	However the chances of this happening are exceptionally small.
	"""

	length: int
	NO_ERRORS = "no errors"
	ERRORS_CORRECTED = "errors corrected"
	CORRECTION_FAILURE = "correction failure"

	def hash(self, data: bytes) -> bytes:
		if len(data) % self.length:
			raise SyntaxError("ACE data must be padded to 8 bytes.")
		words = [data[x * self.length:(x + 1) * self.length] for x in range(len(data) // self.length)]

		checks = []
		for word in words:
			check = 0
			for byte in word:
				check ^= byte
			checks.append(pack("!B", check))
		checkblock = b"".join(checks)

		datablock = words[0]
		if len(words) > 1:
			for word in words[1:]:
				datablock = utils.bytestringXOR(datablock, word)

		header = pack("!L", self.length)

		return header + datablock + checkblock

	def correct(self, data: bytes) -> tuple[bytes, bool]:
		dbh, checkblock = utils.split(bytes(self), self.length)
		header, datablock = utils.split(dbh, 4)
		checks = list(unpack("!" + str(len(checkblock)) + "B", checkblock))

		if unpack("!L", header) != self.length:
			raise TypeError("Can't correct this data with an invalid ACE checksum.")

		words = [data[x * self.length:(x + 1) * self.length] for x in range(len(data) // self.length)]

		corrected = False
		for index, word in enumerate(words):
			check = 0
			for byte in word:
				check ^= byte
			if check != checks[index]:
				if corrected:
					return data, self.CORRECTION_FAILURE
				corrected = True
				correction = datablock
				for otherWord in [w for w in words if w != word]:
					correction = utils.bytestringXOR(correction, otherWord)
				words[index] = correction
		corrected = self.ERRORS_CORRECTED if corrected else self.NO_ERRORS
		return b"".join(words), corrected

	@classmethod
	def ofLength(cls, length: int) -> type:
		class _ACE(cls):
			length = length
			name = "ACE-" + str(length)

		_ACE.__name__ = "ACE" + str(length)
		_ACE.__qualname__ = "kingslayer.crypto.queensuite.ACE" + str(length)

		return _ACE

	@classmethod
	def optimize(cls, data: bytes) -> type:
		return data, cls.ofLength(int(sqrt(len(data))))(data)

class ACE8(ACE):
	name = "ACE-8"
	length = 8
class ACE16(ACE):
	name = "ACE-16"
	length = 16
class ACE24(ACE):
	name = "ACE-24"
	length = 24
class ACE32(ACE):
	name = "ACE-32"
	length = 32
class ACE48(ACE):
	name = "ACE-48"
	length = 48
class ACE64(ACE):
	name = "ACE-64"
	length = 64
class ACE128(ACE):
	name = "ACE-128"
	length = 128
class ACE256(ACE):
	name = "ACE-256"
	length = 256
class ACE1000(ACE):
	name = "ACE-1000"
	length = 1000
class ACE2048(ACE):
	name = "ACE-2048"
	length = 2048

# ========== SYMMETRIC ENCRYPTION ========== #

@Cryptosystem.register
class Dyavol(StreamCryptosystem):

	identifier = b"DYAV"
	name = "Dyavol"

	class Key:
		def __init__(self, data: bytes):
			self.data = self.original = data

		def keystream(self, bytecount: int) -> bytes:
			transient = self.data
			ks = b""
			while len(ks) <= bytecount:
				transient = bytes(CRASH256(transient))
				ks += transient
			solution = ks[:bytecount]

			self.data = solution

			return solution

		def reset(self) -> None:
			self.data = self.original
		
		def encode(self) -> bytes:
			return self.original

		@classmethod
		def decode(cls, data: bytes) -> "Key":
			return cls(data)

class DalekoNeCherez(KeyAgreement):

	PUZZLE_SIZE = 12
	puzzleBlocks = []

	class DalekoHeader(bytes):

		FLAG_INITIALIZE   = 0x0001
		FLAG_FINISHED     = 0x0002
		FLAG_ACKNOWLEDGE  = 0x0004
		FLAG_PROCESSING   = 0x0008
		FLAG_KEY          = 0x0010
		FLAG_PUZZLEBLOCK  = 0x0020
		FLAG_PROTOCOL     = 0x0040
		FLAG_CRYPTOSYSTEM = 0x0080
		FLAG_KEY_HASH     = 0x0100
		FLAG_ACCEPT_HASH  = 0x0200
		FLAG_REJECT_HASH  = 0x0400
		FLAG_REQUEST      = 0x0800
		FLAG_RESET        = 0x1000
		FLAG_ABORT        = 0x2000
		FLAG_DROPPED      = 0x4000

		flag2repr = {
			0x0001: "initialize",
			0x0002: "finished",
			0x0004: "acknowledge",
			0x0008: "processing",
			0x0010: "key",
			0x0020: "puzzleblock",
			0x0040: "protocol",
			0x0080: "cryptosystem",
			0x0100: "key-hash",
			0x0200: "accept hash",
			0x0400: "reject hash",
			0x0800: "request",
			0x1000: "reset",
			0x2000: "abort",
			0x4000: "dropped"
		}

		def __init__(self, sequence: int, *flags: int) -> None:
			self.sequence, self.flags = sequence, flags

			self.flagint = 0
			for flag in self.flags:
				self.flagint |= flag

			bytes.__init__(self, [sequence >> 8, sequence % 256, self.flagint >> 8, self.flagint % 256])

		def __repr__(self) -> str:
			return "<DalekoNeCherez header: sequence number " + str(self.sequence) + ">"

	class FamiliarPuzzleBlock:

		class TagError(KeyError):
			pass

		def __init__(self, cryptography: Cryptosystem=Dyavol()) -> None:
			self.data = []

			tags = []

			for x in range(256):
				key = bytes([x])

				tag = urandom(7)
				while tag in tags:
					tag = urandom(7)
				tags.append(tag)

				salt = urandom(4)

				plaintext = tag + key
				ciphertext = cryptography.encrypt(plaintext, cryptography.Key(key + salt))

				self.data.append((salt, key, tag, ciphertext))

		def __iter__(self) -> Iterable[tuple[bytes, bytes, bytes, bytes]]:
			return iter(self.data)

		def __getitem__(self, identifier: bytes) -> bytes:
			for key, tag, ciphertext in self:
				if tag == identifier:
					return key
			raise DalekoNeCherez.FamiliarPuzzleBlock.TagError("No tag in this puzzle block.")

		def encode(self) -> bytes:
			return b"".join([salt + ciphertext for salt, key, tag, ciphertext in self])

	class UnfamiliarPuzzleBlock:

		class PuzzleBlockError(SyntaxError):
			pass

		class PuzzleSolutionError(RuntimeError):
			pass

		def __init__(self, data: bytes) -> None:
			if len(data) % DalekoNeCherez.PUZZLE_SIZE:
				raise self.PuzzleBlockError("Invalid puzzle block.")
			self.puzzles = [data[x * DalekoNeCherez.PUZZLE_SIZE:(x + 1) * DalekoNeCherez.PUZZLE_SIZE] for x in range(len(data) // DalekoNeCherez.PUZZLE_SIZE)]

		def random(self) -> bytes:
			return self.puzzles[randbelow(len(self.puzzles))]

		def solvePuzzle(self, puzzle: bytes, cryptography: Cryptosystem=Dyavol()) -> bytes:
			salt, puzzle = puzzle[:4], puzzle[4:]

			for x in range(256):
				byte = bytes([x])
				possibleKey = byte + salt
				k = cryptography.Key(possibleKey)

				decryption = cryptography.decrypt(puzzle, k)
				if puzzle[-1] == byte:
					return puzzle[:7]

			raise self.PuzzleSolutionError("Puzzle could not be solved (puzzle bytes: " + puzzle.hex() + ")")

	def regenerate(self, bytesRequired: int) -> None:

		self.puzzleBlocks = []

		for count in range(bytesRequired):

			print("\rGenerating puzzle blocks: ║" + ("█" * count) + (" " * (bytesRequired - count)) + "║ (" + str(count) + "/" + str(bytesRequired) + ")", end="")
			pb = self.FamiliarPuzzleBlock()
			self.puzzleBlocks.append(pb)
		print("\rGenerating puzzle blocks: █" + ("█" * bytesRequired) + "█ (" + str(count + 1) + "/" + str(bytesRequired) + ")")


		print("\rPuzzle blocks fully generated.")

	def setup(self):
		self.regenerate(32)

	def host(self) -> bytes:
		pass
