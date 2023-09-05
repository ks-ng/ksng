from typing import Iterable
from typing import Union

from collections.abc import Iterable as IterableBase

from numpy import array
from numpy.linalg import inv as matrixInverse
from math import ceil, floor

Scalar = Union[int, float]

def _fixRoundingError(value: Scalar) -> Scalar:
	diff1 = 1 - (value % 1)
	diff2 = value % 1

	if min(diff1, diff2) > 0.000001:
		return value
	
	if diff2 > 0.5:
		return ceil(value)
	return floor(value)

class Vector:
	def __init__(self, *dimensions: tuple[Scalar]) -> None:
		self.dimensions = list(dimensions)

	def __repr__(self) -> str:
		return "<" + repr(self.dimensions)[1:-1] + ">"

	def __len__(self) -> int:
		return len(self.dimensions)

	def __iter__(self) -> Iterable[Scalar]:
		return iter(self.dimensions)

	def __getitem__(self, index: int) -> Scalar:
		return self.dimensions[index]

	def __setitem__(self, index: int, value: Scalar) -> None:
		self.dimensions[index] = value

	def __add__(self, vector: "Vector") -> "Vector":
		return type(self)(*[x + y for x, y in zip(self, vector)])
	def __radd__(self, vector):
		return self + vector
	def __iadd__(self, vector):
		return self + vector

	def __mul__(self, factor: Scalar) -> "Vector":
		return type(self)(*[x * factor for x in self])
	def __rmul__(self, factor):
		return self * factor
	def __imul__(self, factor):
		return self * factor

	def __sub__(self, vector: "Vector") -> "Vector":
		return type(self)(*[x - y for x, y in zip(self, vector)])
	def __isub__(self, vector):
		return self - vector

	def __truediv__(self, factor: Scalar) -> "Vector":
		return type(self)(*[x / factor for x in self])
	def __itruediv__(self, factor):
		return self / factor

	def __mod__(self, modulus: Scalar) -> "Vector":
		return type(self)(*[x % modulus for x in self])
	def __imod__(self, modulus):
		return self % modulus

	@classmethod
	def zeros(cls, dimension: int) -> "Vector":
		return cls(*([0] * dimension))

	@classmethod
	def one(cls, dimension: int, index: int, value: Scalar=1) -> "Vector":
		dimensions = [0 for _ in range(dimension)]
		dimensions[index] = value
		return cls(*dimensions)

	@staticmethod
	def gaussianElimination(matrix: list["Vector"]):
		# Gaussian elimination method to solve a system of linear equations
		n = len(matrix)

		for i in range(n):
			max_row = i
			for k in range(i + 1, n):
				if abs(matrix[k][i]) > abs(matrix[max_row][i]):
					max_row = k
			matrix[i], matrix[max_row] = matrix[max_row], matrix[i]

			for k in range(i + 1, n):
				factor = matrix[k][i] / matrix[i][i]
				matrix[k] -= matrix[i] * factor

		for i in range(n - 1, -1, -1):
			matrix[i] /= matrix[i][i]
			for k in range(i - 1, -1, -1):
				factor = matrix[k][i]
				matrix[k] -= matrix[i] * factor

		return matrix

class Lattice:

	class DimensionError(ValueError): pass

	def __init__(self, *basis: Iterable[Vector]) -> None:
		if len(basis) == 0:
			raise SyntaxError("Must provide at least two basis vectors.")
		if len(basis) == 1:
			if isinstance(basis[0], IterableBase):
				Lattice.__init__(self, *basis[0])

		self.dimension = None
		for vector in basis:
			if type(vector) != Vector:
				raise TypeError("Can only use Vectors to initialize a Lattice.")

			if self.dimension == None:
				self.dimension = len(vector)

			if self.dimension != len(vector):
				raise self.DimensionError("All vectors used to initialize a Lattice must be of the same dimensions.")

		self.basis = list(basis)

	def __repr__(self) -> str:
		return "<Lattice of basis <" + repr(self.basis)[1:-1] + "> (dimension " + str(self.dimension) + ")>"

	def __len__(self) -> int:
		return self.dimension

	def __iter__(self) -> Iterable[Vector]:
		return iter(self.basis)

	def __lshift__(self, value: Iterable[int]) -> Vector:
		if len(value) != len(self):
			raise self.DimensionError("Invalid dimension for a lattice of dimension " + str(len(self)) + ".")

		scaledBases = [basis * factor for basis, factor in zip(self, value)]

		result = Vector.zeros(len(self))
		for basis in scaledBases:
			result += basis
		return result

	def __rrshift__(self, value: Iterable[int]) -> Vector:
		return self << value

	def __rshift__(self, vector: Vector) -> list[int]:
		a = array([
			[basis[x] for x in range(len(self))]
			for basis in self
		])
		b = array([
			[x] for x in vector
		])
		
		solution = matrixInverse(a).T.dot(b)

		return Vector(*[_fixRoundingError(x[0]) for x in solution])

	@classmethod
	def euclidean(cls, dimensions: int) -> "Lattice":
		return cls(*[Vector.z])