from os import getcwd
from zlib import decompress
from linecache import getline

from typing import Iterable

def readFile(file: str) -> str:
	with open(getcwd() + "/kingslayer/data/" + file, "r") as f:
		return f.read()

def readBinary(file: str) -> bytes:
	with open(getcwd() + "/kingslayer/data/" + file, "rb") as f:
		return f.read()
	
def readDictionary(file: str) -> list[str]:
	return readFile(file).split("\n")

class Lines:
	n: int = 0

	def __init__(self, file: str) -> None:
		self.file = file

	def __iter__(self) -> Iterable:
		self.n = 0
		return self

	def __next__(self) -> None:
		ln = getline(self.file, self.n)
		if ln != "":
			self.n += 1
			return ln.strip()
		raise StopIteration