from socket import socket as Socket
from socket import AF_INET
from socket import SOCK_STREAM
from hashlib import sha3_512
from os import urandom

class Knight:

    def __init__(self, addr: tuple[str, int], whitelist: list[tuple[str, int]], squires: list[tuple[str, int]], password: bytes) -> None:

        # Security and core operations
        self.whitelist = whitelist
        self.squires = squires
        self.salt = urandom(16)
        self.passwordHash = sha3_512(self.salt + password).digest()

        # Queen socket setup
        self.queenSocket = Socket(AF_INET, SOCK_STREAM)
        self.queenSocket.bind(addr)
        self.queenSocket.listen(1)
        self.connection = None
        self.connectedAddress = None
        
        # Squire sockets setup
        self.squireSockets = [Socket(AF_INET, SOCK_STREAM)] * len(self.squires)
        for i, sock in enumerate(self.squireSockets):
            sock.bind((addr[0], addr[1] + i))
            sock.listen(1)

    def passwordCorrect(self, password: bytes) -> bool:
        return sha3_512(self.salt + password).digest() == self.passwordHash
    
    def setupConnections() -> None:
        for sock, squireAddress in zip(self.squireSockets, self.squires):
            