import os
from math import ceil, log, gcd
import threading
import sys
import time
import blessed
from typing import *
import random

# ph6268ef87e04d6

class Colors:
	reset = '\033[0m'
	bold = '\033[01m'
	disable = '\033[02m'
	underline = '\033[04m'
	reverse = '\033[07m'
	strikethrough = '\033[09m'
	invisible = '\033[08m'

	mainFg = ""
	mainBg = ""

	class fg:
		black = '\033[30m'
		red = '\033[31m'
		green = lime = '\033[32m'
		orange = '\033[33m'
		blue = '\033[34m'
		purple = '\033[35m'
		cyan = '\033[36m'
		lightgrey = '\033[37m'
		darkgrey = '\033[90m'
		lightred = '\033[91m'
		lightgreen = '\033[92m'
		yellow = '\033[93m'
		lightblue = '\033[94m'
		pink = '\033[95m'
		lightcyan = '\033[96m'

	class bg:
		black = '\033[40m'
		red = '\033[41m'
		green = '\033[42m'
		orange = '\033[43m'
		blue = '\033[44m'
		purple = '\033[45m'
		cyan = '\033[46m'
		lightgrey = '\033[47m'

	def __call__(self, string, *args):
		s = ""
		for arg in args:
			s = s + arg
		s = s + string + self.reset + self.mainFg + self.mainBg
		return s

	

term = blessed.Terminal()

def files(directory: str="./"):
	return [val.replace("\\", "/") for sublist in [[os.path.join(i[0], j) for j in i[2]] for i in os.walk(directory)] for val in sublist]

def bytestringXOR(a, b):
	if len(a) < len(b):
		return bytestringXOR(b, a)
	z = b""
	if len(a) > len(b):
		z = a[len(b):]
	return bytes([_a ^ _b for _a, _b in zip(a, b)]) + z

def pad(data: bytes, length: int) -> bytes:
	while len(data) % length != 0:
		data = data + b" "
	return data

def unpad(data: str or bytes) -> str or bytes:
	return data.strip()

def hexadecimalToBinary(n):
	return bin(int(n, 16))[2:]

def binaryToHexadecimal(n):
	return hex(int(n, 2))[2:]

def binaryToDecimal(n):
	return int(n, 2)

def decimalToBinary(n):
	return bin(n)[2:]

def decimalToHexadecimal(n):
	return _binaryToHexadecimal(_decimalToBinary(int(n)))

def binaryToBoolean(n):
	return [x == "1" for x in list(n)]

def booleanToBinary(l):
	output = ""
	for x in l:
		if x:
			output = output + "1"
		else:
			output = output + "0"

	return output

def stringToBinary(s):
	result = []
	for c in s:
		bits = bin(ord(c))[2:]
		bits = '00000000'[len(bits):] + bits
		result.extend([int(b) for b in bits])
	return "".join([str(x) for x in result])

def binaryToString(n):
	return "".join(chr(int(''.join(x), 2)) for x in zip(*[iter(n)]*8))

def stringToBoolean(s):
	return binaryToBoolean(stringToBinary(s))

def booleanToString(l):
	return binaryToString(booleanToBinary(l))

def bitlength(x: int) -> int:
	return ceil(log(x, 2))

def bytelength(x: int) -> int:
	return ceil(bitlength(x) / 8)

def intToBytes(x: int) -> bytes:
	if type(x) == bytes:
		return x
	h = hex(x)[2:]
	if len(h) % 2:
		h = "0" + h
	return bytes.fromhex(h)

def bytesToInt(b: bytes) -> int:
	if type(b) == int:
		return b
	return int("0" + b.hex(), 16)

def bytesToBits(b: bytes) -> list[int]:
	bits = []
	for byte in b:
		conversion = decompose(byte, 2)[::-1]
		while len(conversion) < 8:
			conversion.append(0)
		bits.append(conversion[::-1])
	return dissolve(bits)

def bitsToBytes(b: list[int]) -> bytes:
	bytelength = len(b) // 8
	integer = compose(b, 2)
	return int.to_bytes(integer, bytelength, "big")

def egcd(a: int, b: int) -> int:
	if a == 0:
		return (b, 0, 1)
	else:
		g, y, x = egcd(b % a, a)
		return (g, x - (b // a) * y, y)

def invmod(a: int, m: int) -> int:
	g, x, y = egcd(a, m)
	if g != 1:
		raise ValueError("Modular inverse does not exist.")
	else:
		return x % m

class Process:
	busy = False
	paused = False

	def __init__(self, task):
		self.task = task

	def __enter__(self) -> object:
		self.busy = True
		threading.Thread(target=self._task).start()
		if hasattr(self, "enter"):
			self.enter()
		return self

	def __exit__(self, exc: Exception or object, tb: str, value: int) -> None or False:
		self.busy = False
		if hasattr(self, "exit"):
			self.exit()
		if exc is not None:
			return False

	def _task(self) -> None:
		while self.busy:
			if not self.paused:
				self.task()
			else:
				time.sleep(0.05)

	def start(self) -> None:
		self.__enter__()

	def stop(self) -> None:
		self.__exit__(None, "", 0)

	def pause(self) -> None:
		self.paused = True

	def unpause(self) -> None:
		self.paused = False

	def runFor(self, t: int) -> None:
		with self:
			time.sleep(t)

	class Multiprocess:
		def __init__(self, *processes: tuple["Process"]) -> None:
			self.processes = list(processes)

		def __iter__(self) -> Iterable["Process"]:
			return iter(self.processes)

		def __enter__(self) -> None:
			for proc in self:
				proc.__enter__()

		def start(self) -> None:
			for proc in self:
				proc.__enter__()

		def stop(self) -> None:
			for proc in self:
				proc.__exit__(None, "", 0)

		def pause(self) -> None:
			for proc in self:
				self.paused = True

		def unpause(self) -> None:
			for proc in self:
				proc.paused = False

		def runFor(self, t: int) -> None:
			with self:
				time.sleep(t)

	Multi = Multiprocess

class CircularBuffer(Process.Multiprocess):
	def __init__(self, receive: Callable, process: Callable, size: int=100) -> None:
		self.receive = receive
		self.process = process
		self.size    = size

		self.output = []
		self.buffer = [None for _ in range(len(self))]
		self.available = [True for _ in range(len(self))]
		self.receivePointer = self.processPointer = -1

		def _receive():
			if self.available[self.receivePointer + 1 % len(self)]:
				self.receivePointer = self.receivePointer + 1 % len(self)
				self.buffer[self.receivePointer] = self.receive()
				self.available[self.receivePointer] = False
			else:
				time.sleep(0.01)

		def _process():
			if not self.available[self.processPointer + 1 % len(self)]:
				self.processPointer = self.processPointer + 1 % len(self)
				self.output.append(self.process(self.buffer[self.processPointer]))
				self.available[self.processPointer] = True
			else:
				time.sleep(0.01)

		self.receiver  = Process(_receive)
		self.processor = Process(_process)

		Multiprocess.__init__(self, self.receiver, self.processor)

	def __len__(self) -> int:
		return self.size

class Computation(Process):
	def task(self) -> None:
		if self.done:
			time.sleep(0.05)
			return
		self.result = self.execute()
		self.done = True

	def awaitResult(self):
		while not self.done:
			time.sleep(0.05)

def normalize(s: str, l: int) -> str:
	return s + "".join([" " for _ in range(l - len(s))]) if len(s) <= l else s[l - 2] + "..."

class Spinner:
	busy = False
	delay = 0.1

	@staticmethod
	def spinning_cursor():
		while 1: 
			for cursor in '|/-\\': yield cursor

	def __init__(self, text="", delay=0.05):
		self.text = text
		self.spinner_generator = self.spinning_cursor()
		if delay and float(delay): self.delay = delay

	def spinner_task(self):
		with term.cbreak(), term.hidden_cursor():
			while self.busy:
				sys.stdout.write(self.text + " " + next(self.spinner_generator))
				sys.stdout.flush()
				time.sleep(self.delay)
				sys.stdout.write("\r")
				sys.stdout.flush()

	def __enter__(self):
		self.busy = True
		threading.Thread(target=self.spinner_task).start()

	def __exit__(self, exception, value, tb):
		self.busy = False
		time.sleep(self.delay)
		print(self.text + " - done.")
		if exception is not None:
			return False
		
Pinwheel = Spinner
		
class Ellipsis:
	busy = False
	delay = 0.1

	@staticmethod
	def spinning_cursor():
		while 1: 
			for cursor in ["   ", ".  ", ".. ", "..."]: yield cursor

	def __init__(self, text="", delay=0.1):
		self.text = text
		self.spinner_generator = self.spinning_cursor()
		if delay and float(delay): self.delay = delay

	def spinner_task(self):
		with term.cbreak(), term.hidden_cursor():
			while self.busy:
				sys.stdout.write(self.text + " " + next(self.spinner_generator))
				sys.stdout.flush()
				time.sleep(self.delay)
				sys.stdout.write("\r")
				sys.stdout.flush()

	def __enter__(self):
		self.busy = True
		threading.Thread(target=self.spinner_task).start()

	def __exit__(self, exception, value, tb):
		self.busy = False
		time.sleep(self.delay)
		print(self.text + " ...done.")
		if exception is not None:
			return False
		
class Indent:
	def __init__(self, text="  "):
		self.lastStdoutWrite = sys.stdout.write
		self.text = text

	def __enter__(self):
		def wrt(string):
			self.lastStdoutWrite(self.text + string)
		sys.stdout.write = wrt

	def __exit__(self, exception, value, tb):
		sys.stdout.write = self.lastStdoutWrite
		if exception is not None:
			return False

class DataContainer(object):
	requiredArgs = []

	def __init__(self, *args, **kwargs):
		self.data = list(args)
		for key in kwargs.keys():
			setattr(self, key, kwargs[key])
		for arg in self.requiredArgs:
			assert hasattr(self, arg), "Must supply a " + arg + " argument for " + self.__name__ + "."

def rr(i,n):
	if not n:return i
	return rr(i[-1:]+i[:-1],n-1)

def rl(i,n):
	if not n:return i
	return rl(i[1:]+i[:1])

def phi(n: int) -> int:
	if n == 1:
		return 1

	x = 0
	for y in range(1, n):
		if gcd(y, n) == 1:
			x += 1
	return x

def isPrimitiveRoot(g, n):
	phiN = n - 1 # since phi(n) = n - 1 if n is prime
	powers = {pow(g, i, n) for i in range(phi, n)}
	return len(powers) == phiN

def getPrimitiveRoot(n):
	for g in range(2, n):
		if isPrimitiveRoot(g, n):
			return g

def dissolve(l):
	if type(l) == list:
		o = []
		for x in l:
			if type(x) in [list, tuple, set]:
				o += dissolve(x)
			else:
				o += [x]
		return o
	elif type(l) == dict:
		return dissolve(l.values())

def decompose(x: int, n: int=2, f: int=0) -> list[int]:
	"""Decompose a number into digits of any base in O(log n)."""
	output = []
	div, mod = divmod(x, n)
	while div:
		output.append(mod)
		div, mod = divmod(div, n)
	output.append(mod)
	if f != 0:
		while len(output) < f:
			output.append(0)
	return output[::-1]

def compose(l: list[int], n: int) -> int:
	"""Compose a number from digits in any base in O(log n)."""
	output = 0
	for index, item in enumerate(l[::-1]):
		output += item * (n ** index)
	return output

class KnownLengthBytestrings:
	def __init__(self, n):
		self.n = n
		self.current = bytes([0] * n)
		self.dying = False

	def __iter__(self):
		return self

	def __next__(self):
		if self.dying:
			raise StopIteration
		if self.current == bytes([255] * self.n):
			self.dying = True
		result = self.current
		self._increment()
		return result

	def _increment(self):
		for i in range(self.n - 1, -1, -1):
			if self.current[i] == 255:
				self.current = self.current[:i] + bytes([0]) + self.current[i+1:]
			else:
				self.current = self.current[:i] + bytes([self.current[i]+1]) + self.current[i+1:]
				break

class AllByteStrings:
	def __iter__(self):
		self.n = 1
		self.bs = iter(KnownLengthBytestrings(self.n))
		return self

	def __next__(self) -> bytes:
		try:
			return next(self.bs)
		except StopIteration:
			self.n += 1
			self.bs = iter(KnownLengthBytestrings(self.n))
			return next(self.bs)

def ackermann(m, n):
	if m == 0:
		return n + 1
	if n == 0:
		return ackermann(m - 1, 1)
	return ackermann(m - 1, ackermann(m, n - 1))

def cls():
	os.system("cls")
clear=cls

def sniff(count: int=0):
	from kingslayer.net.net import Network
	n = Network()
	i = n.Interface(n.hostaddr)
	while count >= 0:
		if count != 0:
			count -= 1
		print(i.receive())

def split(data, index):
	return data[:index], data[index:]

class Matrix:
	def __init__(self, values: list[list[int]]) -> None:
		self.values = values

	def n():
		pass

def kingslayerLineCount():
	x = 0
	for file in [f for f in files() if f.endswith(".py")]:
		with open(file, "rb") as f:
			x += f.read().count(b"\n")
	return x
