# Socket: used to interface with the sockets on the system

from socket import socket as Socket

from socket import AF_PACKET
from socket import AF_INET

from socket import SOCK_RAW
from socket import SOCK_STREAM
from socket import SOCK_DGRAM

from socket import IPPROTO_IP
from socket import IPPROTO_TCP
from socket import IPPROTO_UDP
from socket import IP_HDRINCL

from socket import htons

# ABC: abstract base classes

from abc import ABC
from abc import abstractclassmethod
from abc import abstractmethod

# Typing: you know

from typing import Any 
from typing import Iterable

# ===== Field, Layer and Packet classes ===== #

# === Fields: the instructions themselves === #

class Field(ABC):

	length: int

	def __init__(self, value: Any) -> None:
		self.value = value

	def __len__(self) -> int:
		return self.length

	@abstractclassmethod
	def fwd(cls, value: Any) -> list[int]:
		pass
	
	@abstractclassmethod
	def bwd(cls, encoding: list[int]) -> Any:
		pass
	
	def encode(self) -> list[int]:
		return self.fwd(self.value)

	@classmethod
	def decode(cls, data: list[int]) -> tuple["Field", list[int]]:
		return cls(cls.bwd(data[:cls.length])), data[cls.length:]
	
# === Layers: the sets of instructions === #

class Layer:
	desc: list[tuple[name, Field, Any, bool]]

	def __init__(self, **kwargs: dict[str, Any]) -> None:
		self.data = {}

		for name, field, default, _ in self.desc:
			if name in kwargs.keys():
				result = kwargs[name]
			else:
				result = default
			self.data[name] = field(result)

		self.checksum()

	def __getitem__(self, name: str) -> Any:
		return self.data[name].value
	
	def __setitem__(self, name: str, value: Any) -> None:
		self.data[name].value = value

	@property
	def fields(self) -> None:
		pass
	
	@fields.setter
	def fields(self, value: Any) -> SyntaxError:
		raise SyntaxError("Can\'t set the Layer.fields value after initialization.")
	
	@fields.getter
	def fields(self) -> Iterable[Field]:
		return iter([(name, self.data[name]) for name, _, __, ___ in self.desc])

	@property
	def values(self) -> None:
		pass
	
	@values.setter
	def values(self, value: Any) -> SyntaxError:
		raise SyntaxError("Can\'t set the Layer.values value after initialization.")
	
	@values.getter
	def values(self) -> Iterable[Field]:
		return iter([(name, field.value) for name, field in self.fields])

	def encode(self) -> list[int]:
		result = []
		for _, field in self.fields:
			result += field.encode()
		return result
	
	@classmethod
	def decode(cls, data: list[int]) -> tuple["Layer", list[int]]:
		args = {}
		for name, field, _, __ in cls.desc:
			f, data = field.decode(data)
			args[name] = f.value
		return cls(**args)

	def checksum(self) -> None:
		pass

# === Packets: the executed instructions === #