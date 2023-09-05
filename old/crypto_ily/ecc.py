# This task ended up being hard enough for its own file.

from dataclasses import dataclass # cuz i'm lazy like that

@dataclass
class PrimeGaloisField:
	prime: int

	def __contains__(self, value: "FieldElement") -> bool:
		return 0 <= value.value < self.prime
	
@dataclass
class FieldElement:
	value: int
	field: PrimeGaloisField

	def __repr__(self):
		return "0x" + f"{self.value:x}".zfill(64)
		
	@property
	def P(self) -> int:
		return self.field.prime

	def __add__(self, other: "FieldElement") -> "FieldElement":
		return FieldElement(
			value=(self.value + other.value) % self.P,
			field=self.field
		)

	def __sub__(self, other: "FieldElement") -> "FieldElement":
		return FieldElement(
			value=(self.value - other.value) % self.P,
			field=self.field
		)

	def __rmul__(self, scalar: int) -> "FieldValue":
		return FieldElement(
			value=(self.value * scalar) % self.P,
			field=self.field
		)

	def __mul__(self, other: "FieldElement") -> "FieldElement":
		return FieldElement(
			value=(self.value * other.value) % self.P,
			field=self.field
		)
		
	def __pow__(self, exponent: int) -> "FieldElement":
		return FieldElement(
			value=pow(self.value, exponent, self.P),
			field=self.field
		)

	def __truediv__(self, other: "FieldElement") -> "FieldElement":
		other_inv = other ** -1
		return self * other_inv
	
@dataclass
class EllipticCurve:
	a: int
	b: int

	field: PrimeGaloisField

	def __contains__(self, point: "Point") -> bool:
		x, y = point.x, point.y
		return y ** 2 == x ** 3 + self.a * x + self.b

	def __post_init__(self):
		self.a = FieldElement(self.a, self.field)
		self.b = FieldElement(self.b, self.field)

		if self.a not in self.field or self.b not in self.field:
			raise ValueError
		
I = lambda curve: Point(x=None, y=None, curve=curve)
		
@dataclass
class Point:
	x: int
	y: int

	curve: EllipticCurve

	def __post_init__(self):
		if self.x is None and self.y is None:
			return

		self.x = FieldElement(self.x, self.curve.field)
		self.y = FieldElement(self.y, self.curve.field)

		if self not in self.curve:
			raise ValueError
	
	def __add__(self, other):
		if self == I(self.curve):
			return other

		if other == I(self.curve):
			return self
		
		if self.x == other.x and self.y == (-1 * other.y):
			return I(self.curve)

		if self.x != other.x:
			x1, x2 = self.x, other.x
			y1, y2 = self.y, other.y

			s = (y2 - y1) / (x2 - x1)
			x3 = s ** 2 - x1 - x2
			y3 = s * (x1 - x3) - y1

			return self.__class__(
				x=x3.value,
				y=y3.value,
				curve=self.curve
			)

		if self == other and self.y == inf:
			return I(self.curve)

		if self == other:
			x1, y1, a = self.x, self.y, self.curve.a

			s = (3 * x1 ** 2 + a) / (2 * y1)
			x3 = s ** 2 - 2 * x1
			y3 = s * (x1 - x3) - y1

			return self.__class__(
				x=x3.value,
				y=y3.value,
				curve=self.curve
			)
		
	def __rmul__(self, scalar: int) -> "Point":
		current = self
		result = I(self.curve)
		while scalar:
			if scalar & 1:  # same as scalar % 2
				result = result + current
			current = current + current  # point doubling
			scalar >>= 1  # same as scalar / 2
		return result
	
@dataclass
class ParameterizedEllipticCurve:
	curve: EllipticCurve
	base: Point
	order: int
		
# ========== COMMON FIELDS & CURVES ========== #

secp256k1C = EllipticCurve(a=0, b=7, field=PrimeGaloisField(0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F))
secp256k1G = Point(x=0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798, y=0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8, curve=secp256k1C)
secp256k1N = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141

secp256k1 = ParameterizedEllipticCurve(curve=secp256k1C, base=secp256k1G, order=secp256k1N)