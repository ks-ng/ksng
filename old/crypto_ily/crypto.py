from os import urandom
from os import listdir
from os.path import isfile
from os.path import exists
from os import urandom
from os import getcwd
from typing import Union
from typing import Iterable
from typing import Callable
from typing import Any
from math import log
from math import lcm
from math import ceil
from math import gcd
from math import sqrt
from random import randint
from random import randbytes
from random import choice
from secrets import randbits
from secrets import randbelow
from secrets import choice as secureChoice
from numpy import array
from struct import pack
from struct import unpack

from hashlib import sha1
from hashlib import sha224
from hashlib import sha256
from hashlib import sha384
from hashlib import sha512
from hashlib import sha3_224
from hashlib import sha3_256
from hashlib import sha3_384
from hashlib import sha3_512
from hashlib import md5 
from hashlib import new # so MD4 can be added
from zlib import crc32 # sobbing

from datetime import datetime
from time import time 
from time import sleep

from abc import ABC
from abc import abstractmethod

from kingslayer import utils

Number = Union[int, float]
KINGSLAYER_SIGNATURE_FORMAT = b"""\
----- BEGIN KINGSLAYER AUTHENTICATED DOCUMENT -----
--- BEGIN DOCUMENT ---
{document}
--- END DOCUMENT ---
--- BEGIN SIGNATURE ---
{signature}
--- END SIGNATURE ---
----- END KINGSLAYER AUTHENTICATED DOCUMENT -----"""

def _pad(data: bytes, length: int=8) -> bytes:
		while len(data) % length != 0:
			data += b" "
		return data

class InvalidClassError(Exception):
	pass

ice = InvalidClassError("Can't use the base class, must use a subclass.")

def shuffle(l: list) -> list:
	output = []
	inputs = [x for x in l] # cuz i don't wanna break anything
	while len(inputs):
		output.append(inputs.pop(randbelow(len(inputs))))
	return output

def randsign() -> int:
	# returns -1 or 1 randomly
	val = randbits(1)
	if not val:
		return -1
	return val

NUMS = b"KINGSLAYER, DESTROYING CASTLES IN THE SKY"
# nothing up my sleeve

class Prime(int):
	def solovayStrassen(self, rounds: int=77):
		n = self.real
		for _ in range(rounds):
			a = randint(1, n)
			while gcd(a, n) != 1:
				a = randint(1, n)
			if pow(a, (n - 1) / 2, n) != self.legendre(a, n):
				return False
		return True

	def millerRabin(self, rounds: int=64):
		n = self.real

		s, d = 1, n // 2
		for _ in range(rounds):
			a = randint(2, n - 2)
			x = pow(a, d, n)
			for _ in range(s):
				y = pow(x, 2, n)
				if y == 1 and x != 1 and x != n - 1:
					return False
				x = y
			if y != 1:
				return False
		return True

	@staticmethod
	def legendre(a: int, p: int) -> int:
		return pow(a, (p - 1) / 2, p)

	@classmethod
	def get(cls, bitlength: int) -> "Prime":
		while True:
			n = randbits(bitlength)
			n = cls(n | 1)
			if n.millerRabin():
				return n

	@classmethod
	def getProduct(cls, bitlength: int) -> tuple[int, "Prime", "Prime"]:
		p = cls.get(bitlength)
		q = cls.get(bitlength)
		return p * q, p, q

# ========== INTEGRITY & AUTHENTICITY ========== #

class Hash:
	name = "name"

	# ===== CRYPTOGRAPHIC METHODS ===== #
	
	def __init__(self, bytestring: Union[bytes, str]=b"") -> None:
		if type(bytestring) == str:
			bytestring = bytestring.encode("utf-8")
		elif type(bytestring) != bytes:
			bytestring = bytes(bytestring)
		self.hashbytes = self.hash(bytestring)

	def __repr__(self) -> str:
		return "<" + self.name + " " + hex(int(self)) + ">"
	
	def __eq__(self, other: object) -> bool:
		return bytes(self) == bytes(other)
	
	def __len__(self) -> int:
		return ceil(log(int(self), 2))
		
	def __int__(self) -> int:
		return int.from_bytes(self.hashbytes, "big")

	def __bytes__(self) -> bytes:
		return self.hashbytes

	def __add__(self, other: Number) -> Number:
		return int(self) + other

	def __sub__(self, other: Number) -> Number:
		return int(self) - other

	def __mul__(self, other: Number) -> Number:
		return int(self) * other

	def __div__(self, other: Number) -> Number:
		return int(self) / other

	def __floordiv__(self, other: Number) -> Number:
		return int(self) // other

	def __mod__(self, other: int) -> int:
		return int(self) % other

	def __xor__(self, other: object) -> object:
		return type(self).decode(utils.bytestringXOR(bytes(self), bytes(other)))

	def __getitem__(self, index):
		return self.hashbytes[index]

	def __ixor__(self, other: object) -> None:
		self.hashbytes = utils.bytestringXOR(bytes(self), bytes(other))

	def hex(self) -> str:
		return hex(int(self))

	def bits(self) -> list[int]:
		return utils.decompose(utils.bytesToInt(bytes(self)), 2)

	@classmethod
	def fromBits(cls, b: list[int]) -> object:
		h = cls(b"")
		h.hashbytes = utils.intToBytes(utils.compose(b, 2))
		return h

	@classmethod
	def decode(cls, b: bytes) -> object:
		h = cls(b"")
		h.hashbytes = b
		return h

	@classmethod
	def fromHex(cls, hex: str) -> object:
		if hex[0:2] == "0x":
			hex = hex[2:]
		h = cls(b"")
		h.hashbytes = utils.intToBytes(int(hex, 16))
		return h

	# ===== CRYPTANALYTIC METHODS ===== #

	@classmethod
	def preimageCollision(cls, hash: object, verbose: bool=True) -> bytes:
		print("Attempting hash brute-force ...")
		length = len(cls()) * 2
		attempt = randbytes(length)
		if verbose:
			while cls(attempt) != hash:
				attempt = randbytes(length)
				print("\r" + cls.name + " - " + repr(cls(x)) + " - " + repr(cls(y)) + (" " * 10), end="")
		else:
			while cls(attempt) != hash:
				attempt = randbytes(length)
		print("Collision found: " + repr(attempt))
		return attempt

	@classmethod
	def birthdayCollision(cls, verbose: bool=True) -> tuple[bytes, bytes]:
		print("Attempting hash brute-force ...")
		length = len(cls()) * 2
		x = randbytes(length)
		y = randbytes(length)
		if verbose:
			while cls(x) != cls(y):
				x = randbytes(length)
				y = randbytes(length)
				print("\r" + cls.name + " - " + repr(cls(x)) + " - " + repr(cls(y)) + (" " * 10), end="")
		else:
			while cls(x) != cls(y):
				x = randbytes(length)
				y = randbytes(length)
		print("Collision found: " + repr(x) + "/" + repr(y))
		return x, y

	@classmethod
	def generateHashTable(cls, plaintexts: list[Union[str, bytes]], export: str, verbose: bool=True) -> None:
		if not verbose:
			hashes = {cls(plaintext).hex(): plaintext for plaintext in plaintexts}
		else:
			hashes = {}
			for index, plaintext in enumerate(plaintexts):
				print("\rGenerating hash table ... (" + str(round((index + 1) / len(plaintexts))) + "%)", end="  ")
				hashes[cls(plaintext).hex()] = plaintext
			
		if verbose:
			print("\rHash table fully generated, initiating export ...")

		data = "\n".join([h + " " + hashes[h] for h in hashes.keys()])
		with open(export, "w") as f:
			f.write(data)
		
		if verbose:
			print("Export complete.")

class MerkleDamgard(Hash):
	function: Callable = None
	blocksize: int = 32 # 256-bit

	def hash(self, data: bytes, iv: bytes=NUMS):
		blocks = [b"".join(x) for x in batched(data, self.blocksize)]
		last = self.function(iv, blocks[0])
		for block in blocks[1:]:
			last = self.function(last, block)
		final = self.function(last, utils.intToBytes(len(data) * 8))
		return final

class HMAC(Hash):
	hash: Hash = None
	blocksize: int = None

	def __init__(self, bytestring: bytes, key: object) -> None:
		if type(key) != bytes:
			key = key.encode()

		self.hashbytes = self.auth(bytestring, key)

	def auth(self, bytestring: bytes, key: object) -> None:
		raise NotImplementedError

	def reverse(self):
		raise AttributeError

	def collide(self):
		raise AttributeError

OPAD = b"\x5c"
IPAD = b"\x36"

class RFC2104HMAC(HMAC):
	# HMAC format compliant with RFC 2104

	hash: Hash = None

	def auth(self, bytestring: bytes, key: object) -> bytes:
		eck = key.encode()
		if len(eck) > self.blocksize:
			eck = self.hash(eck)
		if len(eck) < self.blocksize:
			eck += b"\x00" * (self.blocksize - len(eck))

		return bytes(self.hash(
			utils.bytestringXOR(eck, OPAD * len(eck)) + 
			bytes(self.hash(
				utils.bytestringXOR(eck, IPAD * len(eck)) +
				bytestring
			))
		))

# MD family 

class MD4(Hash):
	name = "MD4"
	identifier = b"MD4 "

	def hash(self, bytestring: bytes) -> bytes:
		h = new("md4")
		h.update(bytestring)
		return h.digest()

class MD5(Hash):
	name = "MD5"

	def hash(self, bytestring: bytes) -> bytes:
		h = md5()
		h.update(bytestring)
		return h.digest()

# Keccak family
class SHA3_224(Hash):
	name = "SHA3-224"

	def hash(self, bytestring: bytes) -> bytes:
		h = sha3_512()
		h.update(bytestring)
		return h.digest()

class SHA3_256(Hash):
	name = "SHA3-256"
	identifier = b"3256"

	def hash(self, bytestring: bytes) -> bytes:
		h = sha3_512()
		h.update(bytestring)
		return h.digest()

class SHA3_384(Hash):
	name = "SHA3-384"

	def hash(self, bytestring: bytes) -> bytes:
		h = sha3_512()
		h.update(bytestring)
		return h.digest()

class SHA3_512(Hash):
	name = "SHA3-512"
	identifier = b"3512"

	def hash(self, bytestring: bytes) -> bytes:
		h = sha3_512()
		h.update(bytestring)
		return h.digest()

def pad(data, length, padding) -> bytes:
	while len(data) < length:
		data += padding
	return data

def fold(data: bytes, padding: bytes=b"\x88") -> bytes:
	if len(data) % 2:
		data += padding
	pivot = len(data)
	return utils.bytestringXOR(data[:pivot], data[pivot:])

def stretch(data: bytes, factor: int=2):
	fd = fold(data)
	data += fd * ((factor - 1) * 2)
	return data

# Kingslayer Key Derivation Subroutine
class KKDS(Hash):
	name = "KKDS"
	identifier = b"KKDS"

	def hash(self, bytestring: bytes) -> bytes:
		splits = []
		for _ in range(99):
			bytestring = bytes(SHA3_512(bytestring)) + NUMS
			splits.append(bytestring)
		for split in splits:
			split += NUMS
		for _ in range(999):
			for index, split in enumerate(splits):
				splits[index] = bytes(SHA3_512(split))

		output = bytes(len(splits[0]))
		for split in splits:
			output = utils.bytestringXOR(output, split)
		return output

# ========== ENCRYPTION ========== #

class Cryptosystem:
	# The "Cryptosystem" base class.

	# Adds support for file management.

	name = None
	hashingAlgorithm = None
	secure = True

	CRYPTOSYSTEMS = []

	# ===== CRYPTOGRAPHIC METHODS ===== #

	class Key:
		pass

	def __init__(self) -> None:
		if not self.secure:
			print("WARNING: " + self.name + " is not secure!")

	def __repr__(self) -> str:
		if self.secure:
			sec = "secure"
		else:
			sec = "not secure"
		s = "<" + "".join([x for x in self.name if x in "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-"]) + "; " + sec + ")>"
		return s

	# Key management
	def generateKeys(self, *args, **kwargs) -> None:
		raise ice
		
	def storeKey(self, file: str, key: object) -> None:
		if not exists(getcwd() + "\\" + file):
			with open(getcwd() + "\\" + file, "wb+") as f:
				f.write(self.identifier + key.encode())
				return
		raise FileExistsError("Cannot overwrite file with key (file: " + getcwd() + "\\" + file + ").")

	def readKey(self, file: str) -> object:
		if exists(getcwd() + "/" + file):
			with open(getcwd() + "/" + file, "rb") as f:
				key = self.Key.decode(f.read()[4:])

			return key
		raise FileNotFoundError("Key file \"" + getcwd() + "/" + file + "\" not found.")
		
	def generateKeyFile(self, file: str) -> None:
		self.storeKey(file, self.generateKeys())
		
	# Encryption and decryption
	def encrypt(self, *args, **kwargs) -> None:
		raise ice
		
	def decrypt(self, *args, **kwargs) -> None:
		raise ice
		
	def encryptFile(self, file: str, keyfile: str) -> bytes:
		if type(keyfile) == str:
			with open(keyfile, "rb") as f:
				key = self.keyclass.decode(f.read())
		else:
			key = keyfile

		with open(file, "rb") as f:
			plaintext = f.read()

		ciphertext = self.encrypt(plaintext, key)

		with open(file, "wb") as f:
			f.write(ciphertext)

		return ciphertext

	def decryptFile(self, file: str, keyfile: str) -> bytes:
		if type(keyfile) == str:
			with open(keyfile, "rb") as f:
				key = self.keyclass.decode(f.read())
		else:
			key = keyfile

		with open(file, "rb") as f:
			ciphertext = f.read()

		plaintext = self.decrypt(ciphertext, key)

		with open(file, "wb") as f:
			f.write(plaintext)

		return plaintext

	def encryptDirectory(self, directory: str, keyfile: str) -> int:
		filesEncrypted = 0
		for file in utils.files(directory):
			self.encryptFile(file, keyfile)
			filesEncrypted += 1
		return filesEncrypted

	def decryptDirectory(self, directory: str, keyfile: str) -> int:
		filesDecrypted = 0
		for file in utils.files(directory):
			self.decryptFile(file, keyfile)
			filesDecrypted += 1
		return filesDecrypted

	def encryptMessage(self, message: str, key) -> str:
		return self.encrypt(message.encode("utf-8"), key).hex()

	def decryptMessage(self, message: str, key) -> str:
		return self.decrypt(bytes.fromhex(message), key).decode("utf-8")

	# Digital signature
	def sign(self, *args, **kwargs) -> None:
		raise ice

	def verify(self, *args, **kwargs) -> None:
		raise ice

	def generateSignatureFile(self, file: str, key: object) -> bytes:
		if exists(file):
			with open(file, "rb") as f:
				document = f.read()

			signature = self.sign(document)
			timeOfSignature = datetime.now()

			data = KINGSLAYER_SIGNATURE_FORMAT.format(
				document=document,
				signature=signature
			)

			with open(file + ".sig", "wb+") as f:
				f.write(data)

			return data

		raise FileNotFoundError("No file named \"" + file + "\".")

	def isKey(self, key: object) -> bool:
		if not hasattr(key, "identifier"):
			return False

		return key.identifier == self.identifier

	@staticmethod
	def lookup(identifier: Union[bytes, object]) -> object:
		if type(identifier) != bytes:
			identifier = identifier.identifier

		for cs in Cryptosystem.CRYPTOSYSTEMS:
			if cs.identifier == identifier:
				return cs
		raise KeyError

	# ===== CRYPTANALYTIC METHODS ===== #

	def ciphertextOnlyBruteForce(self, ciphertext: bytes, iv: Any=256, plaintextGenerator: Callable=lambda: randbytes(8)) -> object:
		"""The slowest attack. Randomly guesses plaintext-key pairs until one 
		works. Runs in O(2^n)."""
		key = self.generateKeys(iv)
		pt = plaintextGenerator()
		while self.encrypt(pt, key) != ciphertext:
			key = self.generateKeys(iv)
			pt = plaintextGenerator()
		return key

	def knownPlaintextBruteForce(self, plaintext: bytes, ciphertext: bytes, iv: Any=256) -> object:
		"""Faster, but still exponential time. Guesses keys until the decrypted 
		ciphertext equals the plaintext. Runs in O(sqrt(2^n))."""
		key = self.generateKeys(iv)
		while self.decrypt(ciphertext, key) != plaintext:
			key = self.generateKeys(iv)
		return key
		
	# ===== Registering ===== #

	@staticmethod
	def register(cls) -> type:
		Cryptosystem.CRYPTOSYSTEMS.append(cls)
		return cls

class BlockCryptosystem(Cryptosystem):
	blocksize: int

	def encrypt(self, *args, **kwargs):
		raise NotImplementedError("No mode of operation added. Try " + type(self).__name__ + ".CBC.encrypt instead.")

	def decrypt(self, *args, **kwargs):
		raise NotImplementedError("No mode of operation added. Try " + type(self).__name__ + ".CBC.decrypt instead.")

	@property
	def ECB(self) -> Cryptosystem:
		class _ECB(type(self)):
			secure = False

			def encrypt(cbc, plaintext: bytes, key: object, iv: bytes=None) -> bytes:
				if len(plaintext) % (self.blocksize // 8):
					raise RuntimeError("Data not padded before encryption.")

				blocks = [x for x in [plaintext[x * (self.blocksize // 8):(x + 1) * (self.blocksize // 8)] for x in range(len(plaintext) // 8)] if len(x) > 0]

				return b"".join([self.encryptBlock(block, key) for block in blocks])

			def decrypt(self, ciphertext: bytes, key: AES.Key) -> bytes:
				if len(ciphertext) % (self.blocksize // 8):
					raise RuntimeError("Data not padded before encryption.")

				blocks = [x for x in [ciphertext[x * (self.blocksize // 8):(x + 1) * (self.blocksize // 8)] for x in range(len(ciphertext) // 8)] if len(x) > 0]

				return b"".join([self.decryptBlock(block, key) for block in blocks])

		_ECB.__name__ = self.__name__ + "_ECB"
		_ECB.__qualname__ = "kingslayer.crypto." + _ECB.__name__
		return _ECB()

	@property
	def CBC(self) -> Cryptosystem:
		bs = (self.blocksize // 8)
		class _CBC(type(self)):
			def encrypt(cbc, plaintext: bytes, key: object, iv: bytes=None) -> bytes:
				if iv == None:
					iv = bytes(SHA3_256(key.value)[:bs])

				output = []

				blocks = [x for x in [plaintext[x * bs:(x + 1) * bs] for x in range(len(plaintext) // 8)] if len(x) > 0]

				for block in blocks:
					c = self.encryptBlock(utils.bytestringXOR(block, iv), key)
					iv = c
					output.append(c)
				
				return b"".join(output)

			def decrypt(cbc, plaintext: bytes, key: object, iv: bytes=None) -> bytes:
				if iv == None:
					iv = bytes(SHA3_256(key.value)[:bs])

				output = []

				blocks = [x for x in [plaintext[x * bs:(x + 1) * bs] for x in range(len(plaintext) // 8)] if len(x) > 0]

				for block in blocks:
					p = self.decryptBlock(block, key)
					output.append(utils.bytestringXOR(iv, p))
					iv = block
				
				return b"".join(output)

		_CBC.__name__ = type(self).__name__ + "_CBC"
		_CBC.__qualname__ = "kingslayer.crypto." + _CBC.__name__
		return _CBC()

	@property
	def PCBC(self) -> Cryptosystem:
		bs = (self.blocksize // 8)
		class _PCBC(type(self)):
			def encrypt(pcbc, plaintext: bytes, key: object, iv: bytes=None) -> bytes:
				if iv == None:
					iv = bytes(SHA3_256(key.value)[:bs])

				output = []

				blocks = [x for x in [plaintext[x * bs:(x + 1) * bs] for x in range(len(plaintext) // 8)] if len(x) > 0]

				for block in blocks:
					c = self.encryptBlock(utils.bytestringXOR(block, iv), key)
					iv = utils.bytestringXOR(block, c)
					output.append(c)
				
				return b"".join(output)

			def decrypt(pcbc, plaintext: bytes, key: object, iv: bytes=None) -> bytes:
				if iv == None:
					iv = bytes(SHA3_256(key.value)[:64])

				output = []

				blocks = [x for x in [plaintext[x * bs:(x + 1) * bs] for x in range(len(plaintext) // 8)] if len(x) > 0]

				for block in blocks:
					p = self.decryptBlock(block, key)
					output.append(bytestringXOR(iv, p))
					iv = bytestringXOR(block, p)
				
				return b"".join(output)

		_PCBC.__name__ = type(self).__name__ + "_PCBC"
		_PCBC.__qualname__ = "kingslayer.crypto." + _PCBC.__name__
		return _PCBC()

	@property
	def CFB(self) -> Cryptosystem:
		bs = (self.blocksize // 8)
		class _CFB(type(self)):
			def encrypt(cfb, plaintext: bytes, key: object, iv: bytes=None) -> bytes:
				if iv == None:
					iv = bytes(SHA3_256(key.value)[:16])

				output = []

				blocks = [x for x in [plaintext[x * bs:(x + 1) * bs] for x in range(len(plaintext) // 8)] if len(x) > 0]

				for block in blocks:
					c = self.encryptBlock(iv, key)
					c = utils.bytestringXOR(c, block)
					iv = c
					output.append(c)
				
				return b"".join(output)

			def decrypt(cfb, plaintext: bytes, key: object, iv: bytes=None) -> bytes:
				if iv == None:
					iv = bytes(SHA3_256(key.value)[:64])

				output = []

				blocks = [x for x in [plaintext[x * bs:(x + 1) * bs] for x in range(len(plaintext) // 8)] if len(x) > 0]

				for block in blocks:
					output.append(utils.bytestringXOR(block, self.encryptBlock(iv, key)))
					iv = block
				
				return b"".join(output)

		_CFB.__name__ = type(self).__name__ + "_CFB"
		_CFB.__qualname__ = "kingslayer.crypto." + _CFB.__name__
		return _CFB()

	@property
	def CTR(self) -> Cryptosystem:
		raise NotImplementedError("sorry")

class StreamCryptosystem(Cryptosystem):
	def encrypt(self, plaintext: bytes, key: object) -> bytes:
		keystream = key.keystream(len(plaintext))
		return utils.bytestringXOR(plaintext, keystream)

	def decrypt(self, ciphertext: bytes, key: object) -> bytes:
		keystream = key.keystream(len(ciphertext))
		return utils.bytestringXOR(ciphertext, keystream)

class FeistelCryptosystem(Cryptosystem):

	SUBKEY = "SUBKEY"
	KEY = "KEY"

	blocksize: int = NotImplemented
	pivot:     int = NotImplemented

	class _FeistelBaseKey:
		def __init__(self, value: bytes) -> None:
			self.value = value

		def encode(self) -> bytes:
			return self.value

		@classmethod
		def decode(cls, data: bytes) -> object:
			return cls(data)

		@classmethod
		def random(cls) -> object:
			return cls(randbytes(256))

	class Subkey(_FeistelBaseKey):
		type = "SUBKEY"

	class Key(_FeistelBaseKey):
		type = "KEY"

		def deriveSubkeys(self) -> list[object]:
			raise NotImplementedError

	# Feistel functions

	def round(self, data: bytes, subkey: object) -> bytes:
		raise NotImplementedError

	def _round(self, data: bytes, subkey: object) -> bytes:
		r0, l0 = data[:self.pivot], data[self.pivot:]
		l1 = r0
		r1 = utils.bytestringXOR(l0, self.round(r0, subkey))
		return r1 + l1

	def _operate(self, data: bytes, subkeys: list[object]):
		for subkey in subkeys:
			data = self._round(data, subkey)
		return data

	# Basic functions (do not override)

	def encrypt(self, data: bytes, key: object) -> bytes:
		blocks = [data[x * self.blocksize:(x + 1) * self.blocksize] for x in range(len(data) // self.blocksize)]
		return b"".join([self._operate(block, key.deriveSubkeys()) for block in blocks])

	def decrypt(self, data: bytes, key: object) -> bytes:
		blocks = [b"".join(batch) for batch in batched(data, self.blocksize)]
		return b"".join([self._operate(block, key.deriveSubkeys()[::-1]) for block in blocks])
		
class BalancedFeistelCryptosystem(FeistelCryptosystem):

	def __init__(self):
		if self.blocksize % 2:
			raise SyntaxError("Balanced Feistel cipher can't have an odd number of bytes in the right or left halves.")

		self.pivot = self.blocksize // 2

# ========== RANDOM NUMBER GENERATION ========== #

class RandomNumberGenerator:
	def nextBits(self, n: int) -> list[int]:
		return [self.nextBit() for _ in range(n)]

	def nextUnsignedInt(self, n: int) -> int:
		return utils.compose(self.nextBits(n), 2)

	def nextSignedInt(self, n: int) -> int:
		sgn = 1 if self.nextBit else -1
		return sgn * self.nextUnsignedInt(n)

	def nextUnsignedIntModulo(self, n: int) -> int:
		return self.nextUnsignedInt(ceil(log(n, 2)) + 3) % n

	def nextBytes(self, n: int) -> bytes:
		return utils.intToBytes(self.nextUnsignedInt(n * 8))

	def choice(self, l: list) -> Any:
		return l[self.nextUnsignedIntModulo(len(l))]

	def shuffle(self, l: list) -> list:
		output = []
		options = list(range(len(l)))
		while len(output) < len(l):
			output.append(l[options.pop(self.choice(options))])
		return output

class _BaseLFSR(RandomNumberGenerator):
	parameters: list[str] = []

	def __init__(self, length: int=1, **kwargs):
		self.length = length

		self.state = [randint(0, 1) for _ in range(len(self))] # state[-1] is next to be outputted

		for key, value in zip(kwargs.keys(), kwargs.values()):
			if key not in self.parameters:
				raise SyntaxError("Invalid parameter for " + type(self).__name__ + " \"" + key + "\".")
			setattr(self, key, value)

		for param in self.parameters:
			if not hasattr(self, param):
				raise SyntaxError("Parameter \"" + param + "\" is required for " + type(self).__name__ + "s.")

	def __len__(self) -> int:
		return self.length

	def __lshift__(self, state: Union[int, bytes, list[int]]) -> None:
		if type(state) == bytes:
			state = int(state.hex(), 16)
		if type(state) == int:
			state = utils.decompose(state, 2)
		if len(state) != len(self):
			raise RuntimeError("Invalid state for a " + str(len(self)) + "-bit LFSR.")
		self.state = state

	def __getitem__(self, index: int) -> int:
		return self.state[index]

	def nextBit(self) -> int:
		output = self.state.pop()

		self.state = self.state[::-1]
		self.state.append(self.feedback())
		self.state = self.state[::-1]

		return output

	def feedback(self) -> int:
		raise NotImplementedError("Can't use _BaseLFSR. Try FibonacciLFSR.")

class FibonacciLFSR(_BaseLFSR):
	parameters = [
		"taps" # list of int
	]

	def feedback(self) -> int:
		output = 0
		for tap in self.taps:
			if not 0 <= tap < len(self):
				raise SyntaxError("Tap index out of range.")

			output ^= self.state[tap]

		return output

class PolynomialFeedbackGenerator(RandomNumberGenerator):
	def __init__(self, modulus: int, *coefficients: tuple[int], iv: int=7) -> int:
		self.coefficients = coefficients[::-1]
		self.modulus = modulus
		self.state = iv

	def next(self) -> int:
		output = 0
		for exponent, coefficient in enumerate(self.coefficients):
			output += coefficient * (self.state ** exponent)
		self.state = output % self.modulus
		return self.state

	def nextBit(self) -> int:
		return self.next() % 2

class Hashchain(RandomNumberGenerator):
	def __init__(self, hash: Hash, iv: bytes):
		self.state = hash(iv)
		self.hash = hash

	def next(self) -> bytes:
		self.state = self.hash(self.state)
		return bytes(self.state)

	def nextBit(self) -> int:
		self.next()
		return int(self.state) % 2

	@classmethod
	def default(cls) -> "Hashchain":
		return cls(SHA3_512, urandom(64))

class SubstitutionBox(dict):
	def __inv__(self) -> "SubstitutionBox":
		return self.inverse

	@property
	def inverse(self) -> "SubstitutionBox":
		return {value: key for key, value in zip(self.keys(), self.values())}

	@classmethod
	def fromDictionary(cls, d: dict) -> object:
		return cls(d)

	@classmethod
	def derangement(cls, rng: RandomNumberGenerator=Hashchain.default(), bitlength: int=8):
		alphabet = [_ for _ in range(2 ** bitlength)]
		used = []
		sbox = {}

		for character in alphabet:
			subalphabet = [char for char in alphabet if char not in (used + [character])]
			codeCharacter = rng.choice(subalphabet)
			sbox[character] = codeCharacter
			used.append(codeCharacter)

		return cls.fromDictionary(sbox)

# === OLD SYSTEMS === #

class RC5(Cryptosystem):
	name = "Rivest Cipher 5"
	identifier = b"ARC5"
	secure = False

class MerkleHellmanKnapsack(Cryptosystem):
	# The Merkle-Hellman Knapsack Cryptosystem

	# Really cool cryptosystem, but Shamir found a polynomial-time public key-breaking
	# attack. I included it for fun, and to test my implementation skills. It's not 
	# secure, but it's awfully complicated, making this perfect for testing myself.

	name = "Merkle-Hellman Knapsack"
	identifier = b"MHK "
	secure = False

	PADDING = b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x10"

	class PublicKey:
		identifier = b"MHK "

		def __init__(self, *items: set[int]) -> None:
			self.items = list(items)

		def __iter__(self) -> Iterable:
			return iter(self.items)

		def __getitem__(self, index: int) -> int:
			return self.items[index]

		def encode(self) -> bytes:
			return MHK.PADDING.join([utils.intToBytes(item) for item in self])

		@classmethod
		def decode(cls, data: bytes) -> object:
			return cls(*[utils.bytesToInt(item) for item in data.split(MHK.PADDING)])

	class PrivateKey:
		identifier = b"MHK "

		def __init__(self, w: int, q: int, r: int) -> None:
			self.w = w
			self.q = q
			self.r = r

	class Keypair:
		identifier = b"MHK "

		def __init__(self, pub, priv):
			self.pub = pub
			self.priv = priv

	def generateKeys(self, blocksize: int=1000, factor: int=10) -> object:
		w = []
		for _ in range(blocksize):
			output.append(sum(w) + randint(1, factor))

		q = sum(w) + randint(1, factor * blocksize)
		r = q
		while gcd(q, r) != 1:
			r = random.randint(2, q)

		b = [(r * wi) % q for wi in w]

		pub = MHK.PublicKey(*b)
		priv = MHK.PrivateKey(w, q, r)
		kp = MHK.Keypair(pub, priv)
		return kp

	def encrypt(self, plaintext: bytes, pub: object) -> bytes:
		if type(pub) == MHK.Keypair:
			pub = pub.pub
		
		integer = utils.bytesToInt(plaintext)
		bits = [char == "1" for char in bin(integer)[2:]]

		ciphertext = sum([item for item, bit in zip(pub, bits) if bit])
		return utils.bytesToInt(plaintext)

	def decrypt(self, ciphertext: bytes, kp: object) -> bytes:
		# Yes, you need to know the whole keypair to decrypt.
		# I didn't write this asymmetric, don't blame me.
		inv = utils.invmod(kp.priv.r, kp.priv.q)
		composed = utils.bytesToInt(ciphertext) * inv
		composed %= kp.priv.q

		decomposed = []
		while composed > 0:
			for index, item in enumerate(kp.pub):
				if item > composed:
					encryptedBit = kp.pub[index - 1]
					decomposed.append(encryptedBit)
					composed -= encryptedBit
					break
				raise SyntaxError("Decryption error.")

		# decomposed is now a list of the proper bits
		bits = []
		for item in kp.pub:
			if item in decomposed:
				bits.append(True)
			else:
				bits.append(False)

		output = 0
		for index, bit in bits:
			if bit:
				output += 2 ** index

		plaintext = utils.intToBytes(output)
		return plaintext

MHK = MerkleHellmanKnapsack

class DSS(Cryptosystem):
	# The Digital Signature Standard (aka Digital Signature Algorithm)!
	#
	# No, I didn't write this one either.

	name = "Digital Signature Standard"
	identifier = b"DSSA"

	PADDING = b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x10"
	LOW_SECURITY = (1024, 160)
	MEDIUM_SECURITY = (2048, 224)
	HIGH_SECURITY = (2048, 256)
	HIGHEST_SECURITY = (3072, 256)

	class DSSParameters:
		def __init__(self, security: tuple[int, int], hashingAlgorithm: Hash=SHA3_256) -> None:
			l, n = security

			self.q = getprime(n)
			self.p = 0
			while (p - 1) % q != 0:
				self.p = getprime(l)

			h = randint(2, p-2)

			self.g = (h ** ((p-1) / q)) % p
			self.algorithm = hashingAlgorithm

		def __repr__(self) -> str:
			return "<DSS parameters: p=" + str(self.p) + ", q=" + str(self.q) + ", g=" + str(self.g) + ">"

	class PublicKey:
		def __init__(self, value: int, params: object) -> None:
			self.value = value
			self.params = params

		def __repr__(self) -> str:
			return "<DSS public key: " + self.hex() + " under " + repr(self.params) + ">"
		
		def __int__(self) -> int:
			return int(self.value)

		def __bytes__(self) -> bytes:
			return utils.intToBytes(int(self))

		def hex(self) -> str:
			return bytes(self).hex()

		def encode(self) -> bytes:
			return bytes(int(self))

	class PrivateKey:
		def __init__(self, value: int, params: object) -> None:
			self.value = value
			self.params = params

		def __repr__(self) -> str:
			return "<DSS private key: " + self.hex() + " under " + repr(self.params) + ">"
		
		def __int__(self) -> int:
			return int(self.value)

		def __bytes__(self) -> bytes:
			return utils.intToBytes(int(self))

		def hex(self) -> str:
			return bytes(self).hex()

	class Keypair:
		def __init__(self, x: int, params: object) -> None:
			if type(x) == str:
				x = x.encode("utf-8")
			if type(x) == bytes:
				x = utils.bytesToInt(x)
			x %= params.q - 1
			self.priv = DSS.PrivateKey(x, params)
			self.pub = DSS.PublicKey(pow(params.g, x, params.p), params)

	def sign(self, plaintext: bytes, priv: object) -> bytes:
		if type(priv) == self.Keypair:
			priv = priv.priv

		r = 0
		s = 0
		while (r == 0) or (s == 0):
			k = randint(1, priv.params.q)
			r = pow(params.g, k, params.p)
			s = utils.invmod(k, q) * (int(H(plaintext)) + (int(priv) * r))

		return utils.intToBytes(r) + self.PADDING + utils.intToBytes(s)

	def verify(self, plaintext: bytes, signature: bytes, pub: object) -> bool:
		if type(pub) == self.Keypair:
			pub = pub.pub

		r, s = tuple(signature.split(self.PADDING))
		r = utils.intToBytes(r)
		s = utils.intToBytes(s)

		if not (0 < r < pub.params.q and 0 < s < pub.params.q):
			return False

		w = utils.invmod(s, q)
		u1 = (int(pub.params.algorithm(plaintext)) * w) % pub.params.q
		u2 = (r * w) % pub.params.q

		v = (((g ** u1) * (pub.value ** u2)) % p) % q
		return v == r

DSA = DSS

# === THE KINGSLAYER ENCRYPTION SUITE === #

class HERETIC(Cryptosystem):
	# Hash-based Encryption with Reduced Encipherment and Triply-secure and Intelligent Cryptography

	# Simple and fast, but suffers from some of the same faults that RC4 does.
	# Included really only as a tribute to my previous projects.
	# ... well, that's not really true. It's also reasonably simple so that anybody
	# could write up a HERETIC implementation, so it's kind of like CipherSaber; you
	# can look that one up.

	# Still, much faster than the other algorithms.

	name = "Hash-based Encryption with Reduced Encipherment and Triply-secure and Intelligent Cryptography"
	identifier = b"HERE"

	class Key:
		identifier = b"HERE"

		def __init__(self, value: Union[str, bytes], shielding: int=13, salt: bytes=b"KINGSLAYER", hashingAlgorithm: Hash=SHA3_512) -> None:
			self.value = value
			self.shielding = shielding
			self.salt = salt
			self.hashingAlgorithm = hashingAlgorithm
			self.id = randint(0, 4294967295)

		def __repr__(self) -> str:
			return "<HERETIC.Key>"

		def keystream(self, n: int) -> bytes:
			bits = ""

			hsh = self.hashingAlgorithm(self.value)

			for _ in range(n):
				for __ in range(self.shielding):
					hsh = self.hashingAlgorithm(bytes(hsh) + self.salt)
				if int(hsh) % 2:
					bits += "1"
				else:
					bits += "0"

			return utils.intToBytes(utils.binaryToDecimal(bits))

		def encode(self) -> bytes:
			return self.value
			
		@classmethod
		def decode(cls, data: bytes) -> object:
			return cls(data)

	def generateKeys(self, bitlength: int=256, shielding: int=13, salt: bytes=b"KINGSLAYER", hashingAlgorithm: Hash=SHA3_512) -> object:
		return self.Key(urandom(int(bitlength / 8)), shielding, salt, hashingAlgorithm)

	def encrypt(self, plaintext: bytes, key: object) -> bytes:
		return utils.bytestringXOR(plaintext, key.keystream(len(plaintext) * 8))

	def decrypt(self, ciphertext: bytes, key: object) -> bytes:
		return utils.bytestringXOR(ciphertext, key.keystream(len(ciphertext) * 8))

class MIMICS(Cryptosystem):
	# Multiple Integer-factorization Modular Inverse Cryptosystem

	# You have NO IDEA how long it took to figure this one out. Asymmetric
	# cryptography is hard! I figured it out eventually though, and it passes 
	# unit tests.

	name = "Multiple Integer-factorization Modular Inverse CryptoSystem"
	identifier = b"MIMX"

	PADDING = b"\xff\xff\xff\xff\x00\x00\x00\x00\xff\xff\xff\xff"

	class PublicKey:
		identifier = b"MIMX"

		def __init__(self, product: int, modulus: int) -> None:
			self.product = product
			self.modulus = modulus
			self.id = None

		def __repr__(self) -> str:
			return "<MIMICS public key: " + str(self.product) + " % " + str(self.modulus) + " (key id " + str(self.id) + ")>"

		def encode(self) -> bytes:
			return utils.intToBytes(self.product) + MIMICS.PADDING + utils.intToBytes(self.modulus)

		@classmethod
		def decode(cls, data: bytes) -> object:
			spl = data.split(MIMICS.PADDING)
			prod = utils.bytesToInt(spl[0])
			mod = utils.bytesToInt(spl[-1])
			return cls(prod, mod)

	class PrivateKey:
		identifier = b"MIMX"

		def __init__(self, factors: list[int], modulus: int) -> None:
			self.product = 1
			for factor in factors:
				self.product *= factor
			self.modulus = modulus
			self.invmods = [utils.invmod(prime, self.modulus) for prime in factors]
			del factors
			self.id = randint(0, 4294967295)

		def __iter__(self) -> Iterable:
			return iter(self.invmods)

		def __repr__(self) -> str:
			return "<MIMICS private key: " + "*".join([str(invmod) for invmod in self]) + "*" + str(self.product) + "=1 % " + str(self.modulus) + " (key id " + str(self.id) + ")>"

		def public(self) -> object:
			k = MIMICS.PublicKey(self.product, self.modulus)
			k.id = self.id
			return k

	class Keypair:
		identifier = b"MIMX"

		def __init__(self, pub: object, priv: object) -> None:
			self.pub = pub
			self.priv = priv
			self.id = self.priv.id

		def __repr__(self) -> str:
			return "<MIMICS keypair: " + repr(self.pub) + "/" + repr(self.priv) + ">"

		@classmethod
		def fromPrivate(cls, priv: object) -> None:
			return cls(priv.public(), priv)

	def generateKeys(self, bitlength: int=2048, keylength: int=2) -> object:
		primes = []
		for _ in range(keylength):
			primes.append(getprime(bitlength))

		priv = self.PrivateKey(primes + [2], 2 ** bitlength)
		return self.Keypair.fromPrivate(priv)

	def encrypt(self, plaintext: bytes, pub: object) -> bytes:
		if type(pub) == MIMICS.Keypair:
			pub = pub.pub

		if len(plaintext) > log(pub.modulus, 2):
			blocksize = int((pub.modulus, 2))
			subtexts = [plaintext[i:i + blocksize] for i in range(0, len(plaintext), blocksize)]
			return b"".join([self.encrypt(subtext) for subtext in subtexts])

		integer = utils.bytesToInt(plaintext)
		ciphertext = (integer * pub.product) % pub.modulus
		return utils.intToBytes(ciphertext)[1:]

	def decrypt(self, ciphertext: bytes, priv: object) -> bytes:
		if type(priv) == MIMICS.Keypair:
			priv = priv.priv

		if len(ciphertext) > log(priv.modulus, 2):
			blocksize = int((priv.modulus, 2))
			subtexts = [ciphertext[i:i + blocksize] for i in range(0, len(ciphertext), blocksize)]
			return b"".join([self.decrypt(subtext) for subtext in subtexts])

		integer = utils.bytesToInt(ciphertext)
		for invmod in priv.invmods:
			integer = (integer * invmod) % priv.modulus

		return utils.intToBytes(integer)[1:] # i think?

	def sign(self, plaintext: bytes, priv: object) -> bytes:
		return self.decrypt(plaintext, priv)

	def verify(self, plaintext: bytes, signature: bytes, pub: object) -> bool:
		return plaintext == self.encrypt(signature, pub)
		
class AAVARICE(Cryptosystem):
	# Just in case they have a quantum computer.

	name = "Asymmetric Algorithm using Vector Addition Rotors for Intelligent Cryptographic Encryption"
	identifier = b"AAVA"

	class Vector:
		def __init__(self, *values: set[int]) -> None:
			self.values = list(values)

		def __iter__(self) -> Iterable:
			return iter(self.values)
		
		def __add__(self, other: object) -> object:
			return Vector(*[x + y for x, y in zip(self, other)])

		def __iadd__(self, other: object) -> None:
			self.values = (self + other).values

		@classmethod
		def random(cls, size: int=16, allowNegative: bool=False) -> None:
			sgn = lambda: 1
			if allowNegative:
				sgn = randsign
			return cls([randbelow(256) * sgn() for _ in range(size)])

	class PublicKey:
		identifier = b"AVRA"
		
		def __init__(self, initial: object, final: object, *options: set[object]) -> None:
			self.initial = initial
			self.final = final
			self.options = list(options)

		def __iter__(self) -> Iterable:
			return iter(self.options)

	class PrivateKey:
		identifier = b"AVRA"

		def __init__(self, initial: object, final: object, options: list[object], path: list[object]) -> None:
			self.initial = initial
			self.path = path
			self.options = options
			if final:
				self.final = final
			else:
				self.final = initial
				for vector in self:
					self.final += vector

		def __iter__(self) -> Iterable:
			return iter(self.path)

		def public(self) -> object:
			return AVRASEC.PublicKey(self.initial, self.final, *shuffle(self.options))

	class Keypair:
		identifier = b"AVRA"

		def __init__(self, pub: object, priv: object) -> None:
			self.pub = pub
			self.priv = priv
			self.id = self.priv.id

		def __repr__(self) -> str:
			return "<AVRASEC keypair: " + repr(self.pub) + "/" + repr(self.priv) + ">"

		@classmethod
		def fromPrivate(cls, priv: object) -> None:
			return cls(priv.public(), priv)

	def generateKeys(self, blocksize: int=16, length: int=16, fakes: int=16):
		initial = self.Vector.random(blocksize)

		path = [initial]
		for _ in range(length):
			current = path[-1]
			items = []
			for item in current:
				items.append(randint(-1 * item, 255 - item))
			path.append(self.Vector(*items))

		final = path[-1]
		path = path[1:-1]

		options = [vec for vec in path]
		for _ in range(fakes):
			options.append(self.Vector.random(blocksize, True))
		
		return self.Keypair.fromPrivate(self.privateKey(initial, final, options, path))

class CHARMLESS1(Cryptosystem):
	# Cryptosystem using Hashing Algorithms with Reversible Mixing for Lightweight Encryption Security Systems

	# The end.

	name = "Cryptosystem using Hashing Algorithms with Reversible Mixing for Lightweight Encryption Security Systems 1"
	identifier = b"CHR1"

	class Key:
		def __init__(self, value: bytes) -> None:
			self.value = KKDS(value)

		def _data(self, length: int, rounds: int) -> list[tuple]:
			outputKeystreams = []
			outputSwaps = []

			state = self.value
			
			for _ in range(rounds):
				swapspace = [x for x in range(length)]
				keystream = b""
				swap = []

				for iteration in range(length):
					state = SHA3_256(state)
					
					index = int(state) % len(swapspace)
					swap.append(swapspace.pop(index))

					keystream += bytes(state)[-1:]

				outputKeystreams.append(keystream)
				outputSwaps.append(swap)

			return [_ for _ in zip(outputKeystreams, outputSwaps)]

		def encode(self) -> bytes:
			return bytes(self.value)

		@classmethod
		def decode(cls, data: bytes) -> object:
			key = cls(b"Hello World!")
			key.value = data
			return key	

	def _mix(self, data: bytes, swap: list[int]) -> bytes:
		output = [None for _ in range(len(data))]
		for index, location in enumerate(swap):
			output[location] = data[index]
		return b"".join([int.to_bytes(x, 1, "big") for x in output])
	
	def _unmix(self, data: bytes, swap: list[int]) -> bytes:
		output = []
		for index, location in enumerate(swap):
			output.append(data[swap[index]])
		return b"".join([int.to_bytes(x, 1, "big") for x in output])

	def generateKeys(self, length: int=256) -> object:
		return self.Key(urandom(int(length / 8)))

	def encrypt(self, plaintext: bytes, key: object, rounds: int=17) -> bytes:
		ciphertext = plaintext
		codeData = key._data(len(plaintext), rounds)
		for keystream, swap in codeData:
			ciphertext = self._mix(ciphertext, swap)
			ciphertext = utils.bytestringXOR(ciphertext, keystream)

		return ciphertext

	def decrypt(self, ciphertext: bytes, key: object, rounds: int=17) -> bytes:
		plaintext = ciphertext
		codeData = reversed(key._data(len(plaintext), rounds))
		for keystream, swap in codeData:
			plaintext = utils.bytestringXOR(plaintext, keystream)
			plaintext = self._unmix(plaintext, swap)

		return plaintext

class CHARMLESS2(FeistelCryptosystem):
	name = "Cryptosystem using Hashing Algorithms and Reversible Mixing for Lightweight Encryption Security Systems 2"
	identifier = b"CHR2"

	blocksize = 32
	pivot = 16

	class Subkey(FeistelCryptosystem.Subkey):
		pass

	class Key(FeistelCryptosystem.Key):
		def deriveSubkeys(self):
			state = bytes(SHA3_256(self.value))
			subkeys = []
			for _ in range(17):
				subkeys.append(CHARMLESS2.Subkey(state))
				state = bytes(SHA3_256(state))
			return subkeys

	def round(self, data: bytes, subkey: object):
		pass	

# ========== KEY AGREEMENT PROTOCOLS ========== #

class KeyAgreement:
	name: str = ""
	identifier: bytes = b"\x00\x00\x00\x00"

	def __init__(self, conn: object) -> None:
		self.conn = conn
		if hasattr(self, "setup"):
			self.setup()

	def send(self, data: bytes) -> None:
		self.conn.send(data)

	def receive(self) -> bytes:
		return self.conn.receive()

	def run(self) -> bytes:
		raise NotImplementedError

	def host(self) -> bytes:
		raise NotImplementedError

# ========== P=NP PROBLEMS ========== # 

# Because some cryptosystems depend on these, I guess.
# Also I didn't have anywhere else to put them.

class Satisfiability:

	# This class actually contains a polynomial-time algorithm
	# for solving n-SAT, which is a proof that P=NP.

	EXCLUDE = 0
	SOLUTIONS = 1

	class Parenthetical:
		def __init__(self, variables: list[str]=["x", "y", "z"], inverts: list[str]=[]) -> None:
			self.variables = variables
			self.exclude = []
			self.solutions = []
			self.check = None
			self.inverts = inverts

		def _invert(self, variable: str) -> None:
			for exclude in self.exclude:
				exclude[variable] = not exclude[variable]
			for solution in self.solutions:
				solution[variable] = not solution[variable]

		def invert(self) -> None:
			for invert in self.inverts:
				self._invert(invert)

	class AND(Parenthetical):
		def solve(self) -> None:
			self.solutions = [{var: True for var in self.variables}]
			self.check = Satisfiability.SOLUTIONS
			self.invert()
		
	class OR(Parenthetical):
		def solve(self) -> None:
			self.exclude = [{var: False for var in self.variables}]
			self.check = Satisfiability.EXCLUDE
			self.invert()

	class Question:
		def __init__(self, type, *parentheticals) -> None:
			self.type = type
			self.parentheticals = list[parentheticals]

		def __iter__(self) -> Iterable:
			return iter(self.parentheticals)

		def solve(self) -> tuple[list[dict[str, bool]], list[dict[str, bool]]]:
			# Return a list of solutions and exclusions to the problem, if they even exist
			# Polynomial=O(n), I think
			for parenthetical in self:
				parenthetical.solve()

			solutions = []
			exclude = []
			for parenthetical in self:
				if parenthetical.check == Satisfiability.EXCLUDE:
					exclude.append(parenthetical.exclude)
				elif parenthetical.check == Satisfiability.SOLUTIONS:
					for solution in parenthetical.solutions:
						if solution not in exclude:
							solutions.append(solution)

				solutions = [solution for solution in solutions if solution not in exclude]

			return solutions, exclude

# ========== MISCELLANEOUS ========== #

class Secret:
	def __init__(self, data: bytes) -> None:
		self.data = data
	
	def __repr__(self) -> str:
		return "<Secret " + self.data.hex() + ">"

	def __str__(self) -> str:
		return repr(self.data)

	def __len__(self) -> int:
		return len(self.data)

	def __invert__(self) -> bytes:
		return self.data

	def split(self, count: int=2) -> tuple[bytes]:
		if count == 2:
			rand = urandom(len(self))
			return (Secret(rand), Secret(utils.bytestringXOR(rand, ~self)))
		p, q = self.split(2)
		return p, *q.split(count - 1)

	def __add__(self, other: object) -> object:
		return Secret(utils.bytestringXOR(~self, ~other))

class Block:
	def __init__(self, data: bytes, value: bytes=b"KINGSLAYER, DESTROYING CASTLES IN THE SKY", algorithm: Hash=SHA3_512) -> None:
		self.hash = value
		self.data = data
		self.timestamp = int(round(time(), 6) * 1e+6)
		self.algorithm = algorithm

	def __bytes__(self) -> bytes:
		return bytes(self.value) + bytes(self.hash) + utils.intToBytes(self.timestamp)

	def attach(self, newData: bytes) -> object:
		return Block(newData, bytes(self), algorithm=self.algorithm)

class IntegerFactorization(utils.Computation):
	def PolhigHellman(x: int, n: int) -> int:
		pass

	def __init__(self, orderFinding: Callable[[int, int], int]=None) -> None:
		self.findOrder = orderFinding if orderFinding != None else self.PohligHellman