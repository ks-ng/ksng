from typing import Iterable
from typing import Union
from typing import Callable

from itertools import chain
from itertools import product
from itertools import starmap

from math import sqrt
from math import log
from math import e, pi
from math import sin, cos
import cmath # for a qft thing

from kingslayer import utils

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
	def probable(self, targetState: tuple[int]) -> bool:
		p = self.probability(targetState)
		if p == 0.5:
			return 0.5
		if p > 0.5:
			return 1
		if p < 0.5:
			return 0

	@classmethod
	def zero(cls):
		return cls(1, 0)
	@classmethod
	def one(cls):
		return cls(0, 1)

	@classmethod
	def bits(cls, *bits):
		l = lambda bit: cls.one() if bit else cls.zero()
		return [l(bit) for bit in bits]

	def normalize(self):
		if len(self) == 2:
			amp0 = self.amplitude((0,))
			amp1 = self.amplitude((1,))
			if (abs(amp0) ** 2) + (abs(amp1) ** 2) != 1:
				magnitude = cmath.sqrt((amp0 ** 2) + (amp1 ** 2))
				normal = [amp0 / magnitude, amp1 / magnitude]
				self._amplitudes = normal
		else:
			raise SyntaxError("Can\'t properly normalize a superposition like that.")

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

	@classmethod
	def fromMap(cls, name: str, bitmap: list[tuple[list[int], list[int]]]=[[1, 0], [0, 1]]) -> "QuantumGate":
		l = lambda x: x[0]
		bitmap = sorted(bitmap, key=l)
		keys = [x[0] for x in bitmap]

		for binstring in product([0, 1], repeat=len(bitmap[0][0])):
			if binstring not in keys:
				raise SyntaxError("Incomplete map.")

		class _QuantumGateDerivative(cls):
			matrix = [x[1] for x in bitmap]

		_QuantumGateDerivative.__name__ = name
		_QuantumGateDerivative.__qualname__ = "kingslayer.q." + name
		return _QuantumGateDerivative

	@classmethod
	def fromTransform(cls, name: str, transform: Callable[[int], int], length: int=1) -> "QuantumGate":
		return cls.fromMap(name, [(utils.decompose(x), utils.decompose(transform(x))) for x in range(2 ** length)])

class QuantumRegister:
	def __init__(self, size: int):
		self.qubits = [QuantumState.zero()] * size

	def __len__(self) -> int:
		return len(self.qubits)

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

		self.normalize()

		return finalQubits

	def normalize(self) -> None:
		for qubit in self:
			qubit.normalize()

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

# Other gates

class QuantumFourierTransform(QuantumGate):
	def make(self, size: int) -> None:
		qft = [[0 for _ in range(2**size)] for _ in range(2**size)]

		for i in range(2**size):
			for j in range(2**size):
				qft[i][j] = (cmath.exp(2j * cmath.pi * i * j / (2**size)))

		normalization = cmath.sqrt(2**size)
		qft = [[element / normalization for element in row] for row in qft]

		return qft

class InverseQuantumFourierTransform(QuantumGate):
	def make(self, size: int) -> None:
		iqft = [[0 for _ in range(2**size)] for _ in range(2**size)]

		for i in range(2**size):
			for j in range(2**size):
				element = cmath.exp(-2j * cmath.pi * i * j / (2**size))
				iqft[i][j] = element.conjugate()

		normalization_factor = cmath.sqrt(2**size)
		iqft = [[element / normalization_factor for element in row] for row in iqft]

		return iqft
