class Qubit:

	"""
	Qubit!
	"""

	alpha: complex
	beta: complex

	def __init__(self, value):
		if value:
			alpha = 0j
			beta = 1+0j
		else:
			alpha = 1+0j
			beta = 0j

	def __getitem__(self, index: int):
		return (alpha, beta)[index]

class Register:

	"""
	Quantum register class!
	"""

	def __init__(self, size: int, iv=0) -> None:
		self.qubits = [Qubit(iv) for _ in range(size)]

	def ket(index: int):
		result = 1
		for 