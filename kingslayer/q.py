from typing import Iterable
from typing import Union

from itertools import chain
from itertools import product
from itertools import starmap

from math import sqrt
from math import log
from math import e, pi
from math import sin, cos

X, Y, Z = "x", "y", "z"

exp = lambda x: e ** x
expi = lambda x: e ** (complex(0, 1) * x)
exppi = lambda x: e ** (pi * x)
expipi = lambda x: e ** (complex(0, 1) * pi * x)

# ===== QUANTUM STATE MANAGEMENT ===== #

# in this systen, alpha = 0 and beta = 1
# quantum states are superpositioned with the * binary operator
# and dissected with the ~ unary operator

class QuantumState:
	def __init__(self, *amplitudes: tuple[complex]) -> None:
		self._amplitudes = amplitudes

		self.size = log(len(self), 2)

		if self.size % 1:
			raise SyntaxError("Bad number of amplitudes; must be a power of two.")

		self.size = int(self.size)

	def __repr__(self) -> str:
		if tuple(self._amplitudes) == (1, 0):
			return "<Quantum 0>"
		if tuple(self._amplitudes) == (0, 1):
			return "<Quantum 1>"
		return "<QuantumState: " + ", ".join([repr(state) + "=" + repr(amplitude).replace("j", "i") for state, amplitude in self.amplitudes()]) + ">"

	def __len__(self) -> int:
		return len(self._amplitudes)

	def __iter__(self) -> Iterable[complex]:
		return iter(self._amplitudes)

	def __mul__(self, other: "QuantumState") -> "QuantumState":
		result = []
		for x in self:
			for y in other:
				result.append(x * y)
		return QuantumState(*result)

	def __invert__(self) -> "QuantumState":
		if self.size == 1:
			return [self]

		states = {index: {0: 0, 1: 0} for index in range(self.size)}
		for state, amplitude in self.amplitudes():
			for index, amptype in enumerate(state):
				states[index][amptype] += amplitude

		amps = [(states[index][0], states[index][1]) for index in range(len(states))]
		return list(starmap(QuantumState, amps))

	def amplitudes(self) -> Iterable[tuple[tuple[int], complex]]:
		rpt = int(log(len(self), 2))
		return zip(product([0, 1], repeat=rpt), self)

	def amplitude(self, targetState: tuple[int]) -> complex:
		for state, amplitude in self.amplitudes():
			if targetState == state:
				return amplitude
		return complex(0, 0)

	def probabilities(self) -> Iterable[tuple[tuple[int], float]]:
		return [(state, abs(amplitude) ** 2) for state, amplitude in self.amplitudes()]

	def probability(self, targetState: tuple[int]) -> float:
		return abs(self.amplitude(targetState)) ** 2

	@classmethod
	def zero(cls):
		return cls(1, 0)
	@classmethod
	def one(cls):
		return cls(0, 1)

# ===== QUANTUM REGISTRY ===== #

class QuantumGate:
	def __init__(self, *args, **kwargs):
		if not hasattr(self, "matrix"):
			self.matrix = self.make(*args, **kwargs)

	def __lshift__(self, state):
		return self.transform(state)

	def __rrshift__(self, state):
		return self.transform(state)

	def transform(self, vector: list[complex]) -> list[complex]:
		if len(vector) != len(self.matrix):
			raise ValueError("Bad vector for transformation.")

		result = [0] * len(vector)

		for i in range(len(vector)):
			for j in range(len(vector)):
				result[i] += self.matrix[i][j] * vector[j]

		return result

	# default make: supply the matrix yourself

	def make(self, matrix: list[list[complex]]) -> list[list[complex]]:
		return matrix

class QuantumRegister:
	def __init__(self, size: int):
		self.qubits = [QuantumState.zero()] * size

	def __repr__(self) -> str:
		return "<QuantumRegister: " + ", ".join(map(repr, self.qubits)) + ">"

	def __getitem__(self, index: int) -> QuantumState:
		return self.qubits[index]

	def __setitem__(self, index: int, value: QuantumState) -> QuantumState:
		self.qubits[index] = value

	def apply(self, gate: QuantumGate, *qubits: tuple[int]) -> list[QuantumState]:
		if len(qubits) == 0:
			return

		state = self[qubits[0]]
		if len(qubits) > 1:
			for qubit in qubits[1:]:
				state = state * self[qubit]

		initialVector = state._amplitudes
		finalVector = gate.transform(initialVector)

		resultant = QuantumState(*finalVector)
		finalQubits = ~resultant
		for qubit, index in zip(finalQubits, qubits):
			self[index] = qubit

		return finalQubits

# ===== COMMON QUANTUM GATES ===== #

class Identity(QuantumGate):
	matrix = [
		[1, 0],
		[0, 1]
	]

# Clifford gates

class Pauli(QuantumGate):
	def make(self, version: str=X) -> list[list[complex]]:
		if version == X:
			return [
				[0, 1],
				[1, 0]
			]

		if version == Y:
			return [
				[0,             complex(0, -1)],
				[complex(0, 1), 0             ]
			]

		if version == Z:
			return [
				[1, 0 ],
				[0, -1]
			]

		raise TypeError("Bad Pauli gate.")

class Phase(QuantumGate):
	matrix = [
		[1, 0            ],
		[0, complex(0, 1)]
	]

class SqrtX(QuantumGate):
	matrix = [
		[complex(0.5, 0.5),  complex(0.5, -0.5)],
		[complex(0.5, -0.5), complex(0.5, 0.5) ]
	]

class Hadamard(QuantumGate):
	matrix = [
		[1 / sqrt(2),  1 / sqrt(2)],
		[1 / sqrt(2), -1 / sqrt(2)]
	]

class ControlledNOT(QuantumGate):
	matrix = [
		[1, 0, 0, 0],
		[0, 1, 0, 0],
		[0, 0, 0, 1],
		[0, 0, 1, 0]
	]

class AnticontrolledNOT(QuantumGate):
	matrix = [
		[0, 1, 0, 0],
		[1, 0, 0, 0],
		[0, 0, 1, 0],
		[0, 0, 0, 1]
	]

class ControlledFlip(QuantumGate):
	matrix = [
		[1, 0, 0,  0],
		[0, 1, 0,  0],
		[0, 0, 1,  0],
		[0, 0, 0, -1]
	]

class DoubleControlledNOT(QuantumGate):
	matrix = [
		[1, 0, 0, 0],
		[0, 0, 1, 0],
		[0, 0, 0, 1],
		[0, 2, 0, 0]
	]

class Swap(QuantumGate):
	matrix = [
		[1, 0, 0, 0],
		[0, 0, 1, 0],
		[0, 1, 0, 0],
		[0, 0, 0, 1]
	]

class ImaginarySwap(QuantumGate):
	matrix = [
		[1, 0,             0,             0],
		[0, 0,             complex(0, 1), 0],
		[0, complex(0, 1), 0,             0],
		[0, 0,             0,             1]
	]

# Non-Clifford gates

class PhaseShift(QuantumGate):
	def make(self, angle):
		return [
			[1, 0          ],
			[0, expi(angle)]
		]

class PhaseT(QuantumGate):
	matrix = PhaseShift.make(None, pi / 4)

class ControlledPhase(QuantumGate):
	def make(self, angle):
		return [
			[1, 0, 0, 0          ],
			[0, 1, 0, 0          ],
			[0, 0, 1, 0          ],
			[0, 0, 0, expi(angle)]
		]

class ControlledPhaseS(QuantumGate):
	matrix = [
		[1, 0, 0, 0            ],
		[0, 1, 0, 0            ],
		[0, 0, 1, 0            ],
		[0, 0, 0, complex(0, 1)]
	]


# Rotation operator gates

class Rotation(QuantumGate):

	@staticmethod
	def _2x2mul(A, B):
		C = [[0, 0], [0, 0]]

		for i in range(2):
			for j in range(2):
				for k in range(2):
					C[i][j] += A[i][k] * B[k][j]

		return C

	def make(self, x=0, y=0, z=0):
		xm = [
			[cos(x / 2),                  complex(0, -1) * sin(x / 2)],
			[complex(0, -1) * sin(x / 2), cos(x / 2)                 ]
		]

		ym = [
			[cos(y / 2), -sin(y / 2)],
			[sin(y / 2), cos(y / 2) ],
		]

		zm = [
			[expi(-z / 2), 0          ],
			[0,            expi(z / 2)]
		]

		ms = [xm, ym, zm]

		result = [[1, 0], [0, 1]]

		for m in ms:
			result = self._2x2mul(result, m)

		return result

# Interaction gates

class XX(QuantumGate):
	def make(self, angle):
		return [
			[cos(angle / 2), 0, 0, complex(0, -1) * sin(angle / 2)],
			[0, cos(angle / 2), complex(0, -1) * sin(angle / 2), 0],
			[0, complex(0, -1) * sin(angle / 2), cos(angle / 2), 0],
			[complex(0, -1) * sin(angle / 2), 0, 0, cos(angle / 2)]
		]

class YY(QuantumGate):
	def make(self, angle):
		return [
			[cos(angle / 2), 0, 0, complex(0, 1) * sin(angle / 2)],
			[0, cos(angle / 2), complex(0, -1) * sin(angle / 2), 0],
			[0, complex(0, -1) * sin(angle / 2), cos(angle / 2), 0],
			[complex(0, 1) * sin(angle / 2), 0, 0, cos(angle / 2)]
		]

class ZZ(QuantumGate):
	def make(self, angle):
		return [
			[expi(-1 * angle / 2), 0, 0, 0],
			[0, expi(angle / 2), 0, 0],
			[0, 0, expi(angle / 2), 0],
			[0, 0, 0, expi(-1 * angle / 2)]
		]

class XY(QuantumGate):
	def make(self, angle):
		return [
			[1, 0, 0, 0],
			[0, cos(angle / 2), complex(0, -1) * sin(angle / 2), 0],
			[0, complex(0, -1) * sin(angle / 2), cos(angle / 2)],
			[0, 0, 0, 1]
		]

class SqrtSwap(QuantumGate):
	matrix = [
		[1, 0, 0, 0],
		[0, complex(0.5, 0.5), complex(0.5, -0.5), 0],
		[0, complex(0.5, -0.5), complex(0.5, 0.5), 0],
		[0, 0, 0, 1]
	]
