from socket import socket as Socket
from socket import AF_INET
from socket import AF_PACKET
from socket import SOCK_RAW
from socket import SOCK_STREAM
from socket import SOCK_DGRAM
from socket import IPPROTO_RAW
from socket import htons
from socket import gethostbyname
from socket import gethostname

from uuid import getnode

from abc import ABC
from abc import abstractmethod

from typing import Any
from typing import Union
from typing import Iterable

from functools import cached_property
from functools import cache

from kingslayer import utils

Bitstring = list[int]

# ===== CONSTANTS ===== #

HOSTMAC = ":".join(map(str, utils.decompose(getnode(), 256)))
HOSTV4 = gethostbyname(gethostname())

NULLMAC = "00:00:00:00:00:00"
NULLV4 = "0.0.0.0"
BROADMAC = "ff:ff:ff:ff:ff:ff"
BROADV4 = "255.255.255.255"

def chk(blocks: list[Bitstring]) -> int:
	blocks = list(map(lambda x: utils.compose(x, 2), blocks))

	s = sum(blocks)
	while s > 2 ** 16:
		s = s + (s % (2 ** 16))
	return s ^ 0xFFFF

# ===== PACKET STRUCTURE ===== #

# === Fields === #

class Field(ABC):

	def __init__(self, **kwargs):
		for key in kwargs.keys():
			if (key.startswith("__") and key.endswith("__")) or (key in ["encode", "decode"]):
				raise KeyError("Bad argument \"" + key + "\".")
			setattr(self, key, kwargs[key])

	@abstractmethod
	def encode(self, value):
		raise NotImplementedError

	@abstractmethod
	def decode(self, data):
		raise NotImplementedError

class NumberField(Field):

	@cached_property
	def maximum(self):
		return (2 ** self.length) - 1

	def encode(self, value: int) -> Bitstring:
		if value > self.maximum:
			raise OverflowError(str(value) + " too big for " + str(self.length) + "-bit field.")

		bits = utils.decompose(value)[::-1]
		while len(bits) < self.length:
			bits.append(0)
		return bits[::-1]

	def decode(self, data: Bitstring) -> tuple[int, Bitstring]:
		return utils.compose(data[:self.length], 2), data[self.length:]

class IPv4Field(Field):

	CHAR = NumberField(length=8)

	def encode(self, value: str) -> Bitstring:
		return utils.dissolve([self.CHAR.encode(int(x)) for x in value.split(".")])

	def decode(self, data: Bitstring) -> tuple[str, Bitstring]:
		if len(data) < 32:
			raise SyntaxError("Too little data to decode an IPv4 field.")

		block = data[:32]
		extra = data[32:]

		chars = [block[x * 8:(x + 1) * 8] for x in range(4)]
		chars = [utils.compose(char, 2) for char in chars]
		return ".".join(map(str, chars)), extra

class MACField(Field):

	@staticmethod
	def gethex(char: int) -> str:
		if char < 16:
			return "0" + hex(char)[2:]
		elif char < 256:
			return hex(char)[2:]
		else:
			raise OverflowError

	def encode(self, value: str) -> Bitstring:
		blocks = list(map(lambda x: int(x, 16), value.split(":")))

		bits = []

		for block in blocks:
			bits += utils.decompose(block, 2)

		return bits

	def decode(self, data: Bitstring) -> tuple[str, Bitstring]:
		data, extra = data[:48], data[48:]

		blocks = [data[x * 8:(x + 1) * 8] for x in range(6)]

		blocks = list(map(lambda x: utils.compose(x, 2), blocks))
		return ":".join(list(map(self.gethex, blocks))), extra

class DataField(Field):

	def encode(self, value: bytes) -> Bitstring:

		if len(value) > self.length:
			raise OverflowError("Data too large to pack into a " + str(self.length) + "-byte field.")

		while len(value) < self.length:
			value += b"\x00"

		return utils.bytesToBits(value)

	def decode(self, data: Bitstring) -> tuple[bytes, Bitstring]:
		return utils.bitsToBytes(data[:self.length * 8]), data[self.length * 8:]

# === Layers === #

class Layer:
	# [(name, field type, default, important?)]
	description: list[tuple[str, Field, Any, bool]]

	def __init__(self, **kwargs) -> None:
		self.values = {}
		self.encodings = {}
		for name, field, default, _ in self.description:
			if name in kwargs.keys():
				self.values[name] = kwargs[name]
				self.encodings[name] = field.encode(kwargs[name])
			else:
				self.values[name] = default
				self.encodings[name] = field.encode(default)

	@cache
	def __repr__(self) -> str:
		return "<" + type(self).__name__ + ": " + " ".join([name + "=" + repr(self[name]) for name, _, __, important in self.description if important]) + ">"

	def __bytes__(self) -> bytes:
		return self.bytes()

	def __getitem__(self, name: str) -> Any:
		return self.values[name]

	def __setitem__(self, n: str, value: Any) -> None:
		for name, field, _, __ in self.description:
			if n == name:
				self.values[name] == value
				self.encodings[name] = field.encode(value)

	# construction operators

	def __truediv__(self, layer: "Layer") -> "Packet":
		return Packet(self, layer)

	# utilities

	def bits(self) -> list[int]:
		bits = []
		for encoding in self.encodings.values():
			bits += encoding
		return bits

	def bytes(self) -> bytes:
		return utils.bitsToBytes(self.bits())

	@classmethod
	def decode(cls, data: Bitstring) -> tuple["Layer", Bitstring]:
		bytesMode = type(data) == bytes

		if bytesMode:
			data = utils.bytesToBits(data)

		decodings = {}
		for name, field, _, __ in cls.description:
			decodings[name], data = field.decode(data)

		if bytesMode:
			data = utils.bitsToBytes(data)

		return cls(**decodings), data

# === Packets === #

class Packet:
	def __init__(self, *layers: tuple[Layer]) -> None:
		self.layers = layers

	def __repr__(self) -> str:
		return "<" + " / ".join(map(repr, self.layers)) + ">"

	def __contains__(self, l):
		for layer in self:
			if (type(layer).__name__.lower() == l.__name__.lower()) or (type(layer) == l):
				return layer
		raise KeyError

	def __iter__(self) -> Iterable[Layer]:
		return iter(self.layers)

	def __bytes__(self) -> bytes:
		encoding = b""
		for layer in self:
			encoding = encoding + bytes(layer)
		return encoding

	def __getitem__(self, name: str) -> Layer:
		for layer in self:
			if (type(layer).__name__.lower() == name.lower()) or (type(layer) == name):
				return layer
		raise KeyError("Packet doesn\'t have a " + repr(name) + " layer.")

	# Construction operators

	def __truediv__(self, new: Union[Layer, "Packet"]) -> "Packet":
		if issubclass(new, Layer):
			newLayers = [new]
		elif type(new) == Packet:
			newLayers = new.layers
		else:
			raise TypeError("Can only add Layer or Packet to a Packet via /.")

		layers = self.layers + newLayers
		return type(self)(*layers)

	def checksum(self) -> None:
		for layer in self:
			if hasattr(layer, "checksum"):
				layer.checksum(self)

# === Common layers === #

class Ethernet(Layer):
	description = [
		("dst",       MACField(),             "00:00:00:00:00:00", True ),
		("src",       MACField(),             "ff:ff:ff:ff:ff:ff", True ),
		("ethertype", NumberField(length=16), 0x0800,              False)
	]

	# 802.1Q support: Ethernet._1Q gives a modified layer
	class _1Q(Layer):
		description = [
			("dst",       MACField(),             "00:00:00:00:00:00", True ),
			("src",       MACField(),             "ff:ff:ff:ff:ff:ff", True ),
			("d1q",       NumberField(length=32), 0,                   False),
			("ethertype", NumberField(length=16), 0x0800,              False)
		]

class ARP(Layer):
	description = [
		("htype", NumberField(length=16), 1,        False),
		("ptype", NumberField(length=16), 0x0800,   False),
		("hlen",  NumberField(length=8),  6,        False),
		("plen",  NumberField(length=8),  4,        False),
		("op",    NumberField(length=16), 1,        True ),
		("sha",   MACField(),             HOSTMAC,  True ),
		("spa",   IPv4Field(),            HOSTV4,   True ),
		("tha",   MACField(),             BROADMAC, True ),
		("tpa",   IPv4Field(),            NULLV4,   True )
	]

class IP(Layer):
	description = [
		("version",  NumberField(length=4),  4,      False),
		("ihl",      NumberField(length=4),  5,      False),
		("dscp",     NumberField(length=6),  0,      False),
		("ecn",      NumberField(length=2),  0,      False),
		("length",   NumberField(length=16), 20,     False),
		("id",       NumberField(length=16), 0,      False),
		("reserved", NumberField(length=1),  1,      False),
		("df",       NumberField(length=1),  0,      False),
		("mf",       NumberField(length=1),  0,      False),
		("foffset",  NumberField(length=13), 0,      False),
		("ttl",      NumberField(length=8),  255,    False),
		("protocol", NumberField(length=8),  17,     False),
		("checksum", NumberField(length=16), 0,      False),
		("src",      IPv4Field(),            HOSTV4, True ),
		("dst",      IPv4Field(),            HOSTV4, True )
	]

	def checksum(self, pkt):
		self["checksum"] = 0
		bits = self.bits()
		blocks = [bits[x * 16:(x + 1) * 16] for x in range(len(bits) // 16)]
		self["checksum"] = chk(blocks)

class UDP(Layer):
	description = [
		("srcp",     NumberField(length=16), 0,  True ),
		("dstp",     NumberField(length=16), 0,  True ),
		("length",   NumberField(length=16), 28, False),
		("checksum", NumberField(length=16), 0,  False)
	]

# ===== NETWORK INTERFACES ===== #

class Interface(ABC):
	socket: Socket

	# parsing

	@staticmethod
	def decodePacket(self, data: bytes) -> Packet:
		bits = data.bytesToBits(data)
		eth, bits = Ethernet.decode(bits)

		return eth

	# representation

	def __repr__(self) -> str:
		return "<" + type(self).__name__ + " network interface>"

	# abstractions

	@abstractmethod
	def send(self, pkt):
		raise NotImplementedError

	@abstractmethod
	def receive(self):
		raise NotImplementedError

	# extra syntax

	def __lshift__(self, pkt):
		self.send(pkt)

	def __rrshift__(self, pkt):
		self.send(pkt)

# === Simple interfaces === #

class RawInterface(Interface):
	socket = Socket(AF_PACKET, SOCK_RAW, htons(3))

	def __init__(self):
		self.socket.bind(("eth0", 0))

	def send(self, pkt: Packet):
		self.socket.send(bytes(pkt))

	def receive(self):
		return self.socket.recvfrom(1500)[0]

class EthernetInterface(Interface):
	socket = Socket(AF_PACKET, SOCK_RAW, htons(3))

	def __init__(self):
		self.socket.bind(("eth0", 0))

	def send(self, pkt: Packet):
		if Ethernet not in pkt:
			raise SyntaxError("Can\'t send an Ethernet packet without such a layer.")
		self.socket.send(bytes(pkt))

	def receive(self):
		data = self.socket.recvfrom(1500)[0]

		bits = utils.bytesToBits(data)

		return self.decodePacket(bits)
