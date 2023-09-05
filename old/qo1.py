from math import log
from math import e
from math import pi
from math import sqrt
from math import sin
from math import cos
from itertools import product

from random import choices

from typing import Union
from typing import Iterable
from typing import Any
from typing import Callable

ISOLATION = "isolation"
COLLECTIVE = "collective"

def exp(exponent: Union[int, float, complex]) -> Union[float, complex]:
	return e ** exponent

i = complex(0, 1)
sqrt2 = sqrt(2)
invsqrt2 = 1 / sqrt2

# ===== BASIC STATE MANAGEMENT ===== #

def kronecker(matrix1, matrix2):
	rows1, cols1 = len(matrix1), len(matrix1[0])
	rows2, cols2 = len(matrix2), len(matrix2[0])
	result = [[0] * (cols1 * cols2) for _ in range(rows1 * rows2)]

	for i in range(rows1):
		for j in range(cols1):
			for k in range(rows2):
				for l in range(cols2):
					result[i * rows2 + k][j * cols2 + l] = matrix1[i][j] * matrix2[k][l]

	return result

def dot(vector, matrix):
	if len(vector) != len(matrix[0]):
		raise ValueError("Number of columns in the matrix must match the length of the column vector.")

	result = [0] * len(matrix)

	for i in range(len(matrix)):
		for j in range(len(vector)):
			result[i] += vector[j] * matrix[i][j]
	return result


class QuantumState:
	def __init__(self, *amplitudes: tuple[complex, ...]) -> None:
		self.amplitudes = list(amplitudes)

		if log(len(self), 2) % 1:
			raise IndexError("Bad number of amplitudes.")

		self.count = int(log(len(self), 2))
		self.length = len(self)

	def __repr__(self) -> str:
		return "<QuantumState " + repr(self.amplitudes) + ">"

	def __iter__(self) -> Iterable[complex]:
		return iter(self.amplitudes)

	def __len__(self) -> int:
		return len(self.amplitudes)

	def __mul__(self, other: "QuantumState") -> "QuantumState":
		return self._superposition(self, other)

	def __imul__(self, other: "QuantumState") -> "QuantumState":
		return self * other

	@staticmethod
	def _superposition(s1: "QuantumState", s2: "QuantumState") -> "QuantumState":
		result = []
		for i in s1:
			extension = [i * j for j in s2]
			result.extend(extension)
		return QuantumState(*result)

	@staticmethod
	def superposition(*states) -> "QuantumState":
		if len(states) == 0:
			return None
		if len(states) == 1:
			return states[0]
		if len(states) == 2:
			return QuantumState._superposition(*states)
		return QuantumState.superposition(QuantumState._superposition(states[0], states[1]), *states[2:])

	@classmethod
	def foldstate(cls, s: tuple[Union["QuantumState", complex], ...]) -> tuple[tuple[tuple[...]]]:
		if type(s) == cls:
			return cls.foldstate(s.amplitudes)

		split = len(s) // 2

		if len(s) == 1:
			return s[0]

		return [cls.foldstate(s[:split]), cls.foldstate(s[split:])]

	def monopositions(self, qubitize: bool=False) -> tuple["QuantumState"]:
		amplitudes = {i: {"alpha": complex(0, 0), "beta": complex(0, 0)} for i in range(self.count)}

		binstrings = product(["alpha", "beta"], repeat=self.count)

		for value, binstring in zip(self, binstrings):
			for index, amptype in zip(range(self.count), binstring):
				amplitudes[index][amptype] += value

		quantumStates = []
		for value in amplitudes.values():
			quantumStates.append(type(self)(value["alpha"], value["beta"]))

		if qubitize:
			quantumStates = [Qubit(*quantumState) for quantumState in quantumStates]

		return quantumStates

# ===== QUANTUM REGISTRY ===== #

class Qubit(QuantumState):

	mode = ISOLATION

	@property
	def alpha(self):
		return self.amplitudes[0]

	@alpha.setter
	def alpha(self, value):
		if self.mode == COLLECTIVE:
			raise SyntaxError("Qubit is in collective mode.")
		self.amplitudes[0] = value

	@property
	def beta(self):
		return self.amplitudes[1]

	@beta.setter
	def beta(self, value):
		if self.mode == COLLECTIVE:
			raise SyntaxError("Qubit is in collective mode.")
		self.amplitudes[1] = value

	def __post_init__(self) -> None:
		if self.count > 1:
			raise IndexError("Quantum state too large for a single qubit.")
		self.amplitudes = list(self.amplitudes) # ensures mutability

	def __repr__(self) -> str:
		return "<Q 0:" + repr(self.alpha) + " 1:" + repr(self.beta) + ">"

	@classmethod
	def bit(cls, bit: bool) -> "Qubit":
		if bit:
			return cls(0, 1)
		return cls(1, 0)

	def collapse(self) -> int:
		p0 = abs(self.alpha) ** 2
		p1 = abs(self.beta) ** 2

		results = choices([0, 1], weights=(p0, p1), k=1)
		result = results[0]

		if result == 0:
			self.alpha = 1
			self.beta = 0
			return 0

		elif result == 1:
			self.alpha = 0
			self.beta = 1
			return 1

class QuantumRegister:

	def __init__(self, length: int) -> None:
		self.qubits = [Qubit.bit(0)] * length
		self.superposition = QuantumState.superposition(*self.qubits)
		self.mode = ISOLATION

	def __repr__(self) -> str:
		if self.mode == ISOLATION:
			return "<QuantumRegister <Isolation Mode> " + repr(self.qubits) + ">"
		if self.mode == COLLECTIVE:
			return "<QuantumRegister <Collective Mode> " + repr(self.superposition) + ">"

	def __len__(self) -> int:
		return len(self.qubits)

	def __getitem__(self, index: Union[int, slice]) -> None:
		if self.mode == ISOLATION:
			return self.qubits[index]
		raise SyntaxError("Quantum register is in collective mode.")

	def __iter__(self) -> Iterable[Qubit]:
		return iter(self.qubits)

	def __invert__(self) -> None:
		self.toggle()

	def isolate(self) -> None:
		if self.mode == ISOLATION:
			return
		self.mode = ISOLATION
		self.qubits = self.superposition.monopositions(qubitize=True)
		for qubit in self:
			qubit.mode = ISOLATION

	def collectivize(self) -> None:
		if self.mode == COLLECTIVE:
			return
		self.mode = COLLECTIVE
		self.quantumState = QuantumState.superposition(*self.qubits)
		for qubit in self:
			qubit.mode = COLLECTIVE

	def toggle(self) -> None:
		if self.mode == ISOLATION:
			self.collectivize()
		elif self.mode == COLLECTIVE:
			self.isolate()

		return self

	def measure(self, *indices: tuple[int], interpretation: Callable[[tuple[int]], Any]=lambda x: x) -> Any:
		result = []
		for index in sorted(indices):
			result.append(self[index].collapse())
		return interpretation(result)

	def apply(self, gate: "QuantumGate", index: int) -> None:
		if self.mode == ISOLATION:
			raise SyntaxError("Quantum register is in isolation mode.")

		if index == 0:
			matrix = gate.matrix
			for _ in range(len(self) - 1):
				matrix = kronecker(matrix, [[1, 0], [0, 1]])

		for x in matrix: print(x)

		self.quantumState.amplitudes = dot(self.quantumState.amplitudes, matrix)

# ===== QUANTUM GATES ===== #

class QuantumGate:

	def __init__(self, **kwargs):
		self.params = kwargs
		if not hasattr(self, "matrix"):
			self.matrix = self.make(**self.params)

		self.size = log(len(self.matrix), 2)
		if self.size % 1:
			raise SyntaxError("Bad matrix size.")
		self.size = int(self.size)

	def __len__(self) -> int:
		return self.size

	def __getitem__(self, name: str) -> Any:
		return self.params[name]

	def __repr__(self) -> str:
		return "<" + self.__name__ + " of qubitsize " + str(self.size) + ", " + ", ".join([param + "=" + self.params[param] for param in self.params.keys()]) + ">"

class Identity(QuantumGate):
	matrix = [
		[1, 0],
		[0, 1]
	]

class Phase(QuantumGate):
	def make(phase):
		v = exp(complex(0, phase))
		return [
			[v, 0],
			[0, v]
		]

# Clifford gates

class PauliX(QuantumGate):
	matrix = [
		[0, 1],
		[1, 0]
	]

class PauliY(QuantumGate):
	matrix = [
		[0, -i],
		[i,  0]
	]

class PauliZ(QuantumGate):
	matrix = [
		[1,  0],
		[0, -1],
	]

class SqrtZ(QuantumGate):
	matrix = [
		[1, 0],
		[0, i]
	]

class SqrtX(QuantumGate):
	matrix = [
		[complex(0.5, 0.5),  complex(0.5, -0.5)],
		[complex(0.5, -0.5), complex(0.5, 0.5) ]
	]

class Hadamard(QuantumGate):
	matrix = [
		[invsqrt2,  invsqrt2],
		[invsqrt2, -invsqrt2]
	]

class XOR(QuantumGate):
	# aka Controlled-NOT

	matrix = [
		[1, 0, 0, 0],
		[0, 1, 0, 0],
		[0, 0, 0, 1],
		[0, 0, 1, 0]
	]

class XNOR(QuantumGate):
	# aka Anticontrolled-NOT

	matrix = [
		[0, 1, 0, 0],
		[1, 0, 0, 0],
		[0, 0, 1, 0],
		[0, 0, 0, 1]
	]

class CPF(QuantumGate):
	# Controlled phase flip

	matrix = [
		[1, 0, 0,  0],
		[0, 1, 0,  0],
		[0, 0, 1,  0],
		[0, 0, 0, -1]
	]

class DCNOT(QuantumGate):
	matrix = [
		[1, 0, 0, 0],
		[0, 0, 1, 0],
		[0, 0, 0, 1],
		[0, 1, 0, 0]
	]

class Swap(QuantumGate):
	matrix = [
		[1, 0, 0, 0],
		[0, 0, 1, 0],
		[0, 1, 0, 0],
		[0, 0, 0, 1]
	]

class ImSwap(QuantumGate):
	matrix = [
		[1, 0, 0, 0],
		[0, 0, i, 0],
		[0, i, 0, 0],
		[0, 0, 0, 1]
	]

# non-Clifford gates

class PhaseShift(QuantumGate):
	def make(phase):
		v = exp(complex(0, phase))
		return [
			[1, 0],
			[0, v]
		]

class PhaseT(PhaseShift):
	def __init__(self):
		PhaseShift.__init__(self, phase=pi / 4)


