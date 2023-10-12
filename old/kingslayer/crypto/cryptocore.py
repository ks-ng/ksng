from abc import abstractmethod
from abc import ABC

from functools import cache

from kingslayer.utils import *
from kingslayer.main import Kingslayer

class Cryptosystem(ABC):
	REGISTRATIONS = {}

	def register(*names):
		def deco(cls):
			for name in names:
				Cryptosystem.REGISTRATIONS[name] = cls

			return cls

		return deco

	@staticmethod
	def lookup(name: str) -> "Cryptosystem":
		return Cryptosystem.REGISTRATIONS[name]

class SymmetricAlgorithm(Cryptosystem):

	# abstracted methods

	Key: type

	@abstractmethod
	def encrypt(self, data: bytes, key: "Key") -> bytes:
		pass

	@abstractmethod
	def decrypt(self, data: bytes, key: "Key") -> bytes:
		pass

	# extensions
	def load(self, file: str) -> "Key":
		with open(file, "rb") as f:
			return self.Key(f.read())

	def dump(self, key: "Key", file: str) -> None:
		with open(file, "wb") as f:
			f.write(bytes(key))

	def encryptFile(self, file: str, key: "Key") -> bytes:
		with open(file, "rb") as f:
			plaintext = f.read()

		ciphertext = self.encrypt(plaintext, key)

		with open(file, "wb") as f:
			f.write(ciphertext)

		return ciphertext

	def decryptFile(self, file: str, key: "Key") -> bytes:
		with open(file, "rb") as f:
			ciphertext = f.read()

		plaintext = self.decrypt(ciphertext, key)

		with open(file, "wb") as f:
			f.write(plaintext)

		return plaintext

class AsymmetricAlgorithm(Cryptosystem):

	# abstracted methods

	pass

class HashingAlgorithm(Cryptosystem):

	def __init__(self, data: bytes) -> None:
		self.hashdata = self.hash(data)

	@abstractmethod
	def hash(self, data: bytes) -> bytes:
		pass

	def __bytes__(self) -> bytes:
		return self.hashdata

	@cache
	def __int__(self) -> int:
		return compose(bytesToBits(bytes(self)), 2)

	@cache
	def bits(self) -> list[int]:
		return bytesToBits(bytes(self))

@Kingslayer.Command.assemble
def encrypt(crypto, file, keyfile):
	crypto = Cryptosystem.lookup(crypto)
	key = crypto.load(keyfile)
	crypto.encryptFile(file, key)

@Kingslayer.Command.assemble
def decrypt(crypto, file, keyfile):
	crypto = Cryptosystem.lookup(crypto)
	key = crypto.load(keyfile)
	crypto.decryptFile(file, key)
