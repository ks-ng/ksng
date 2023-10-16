from typing import Callable

def decompose(n: int, b: int, s: int) -> list[int]:
	d, m = n, 0
	output = []
	while d > 0:
		d, m = divmod(d, b)
		output.append(m)
	while len(output) < s:
		output.append(0)
	return output[::-1]

def compose(l: list[int], b: int) -> int:
	output = 0
	for i, n in enumerate(l[::-1]):
		output += (b ** i) * n
	return output

def asBytes(obj, s: int):
	if type(obj) == list:
		obj = compose(obj, 2)
	decomp = decompose(obj, 256, s)
	return bytes(decomp)

def enforceTyping(func: Callable) -> Callable:
	def wrapper(*args, **kwargs):
		