from os import urandom

from abc import ABC
from abc import abstractstaticmethod

# === Hashing - verifying data === #

class Hash(ABC):

	def __init__(self, data: bytes) -> None:
		self.hashdata = self.hash(data)
		
	def __len__(self) -> int:
		return len(self.hashdata)
		
	@abstractstaticmethod
	def hash(data: bytes) -> bytes:
		raise NotImplementedError

# === Encryption - securing data === #

class Encryption(ABC):

	def __init__(self
