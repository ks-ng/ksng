from typing import Callable
import shlex

class Kingslayer:

	PREFIX = " > "
	TRUE = ["yes", "true", "True", "y", "1"]
	FALSE = ["no", "false", "False", "n", "0"]

	commands = []

	def __init__(self):
		print("Kingslayer loaded with commands " + ", ".join(map(lambda cmd: cmd.name, self.commands)) + ".")

	class InvalidCommandError(NameError):
		pass

	class Command:

		class IncorrectCommandNameError(NameError):
			pass

		def __init__(self, name: str, function: Callable) -> None:
			self.function = function
			self.name = name

		def __call__(self, command: str) -> None:
			cmd = [x.strip() for x in command.split(" ") if len(x.strip()) > 0]
			if cmd[0] != self.name:
				raise self.IncorrectCommandNameError("Command name incorrect.")

			args, kwargs = self.parse(" ".join(cmd[1:]))

			try:
				return self.function(*args, **kwargs)
			except:
				print("Command failed; check syntax and output.")

		@staticmethod
		def parse(arg_string):
			args = []
			kwargs = {}

			tokens = shlex.split(arg_string)

			i = 0

			try:
				while i < len(tokens):
					token = tokens[i]

					if token.startswith("--"):
						key, value = token[2:], tokens[i+1]
						kwargs[key] = value
						i += 2  # Skip the key and value tokens
					else:
						args.append(token)
						i += 1  # Move to the next token
			except IndexError:
				raise Kingslayer.InvalidCommandError("Invalid command syntax.")

			return args, kwargs

		@staticmethod
		def assemble(func) -> Callable:
			Kingslayer.commands.append(Kingslayer.Command(func.__name__, func))
			return func

	def exec(self, command):
		for cmd in self.commands:
			try:
				return cmd(command)
			except self.Command.IncorrectCommandNameError:
				pass
		raise self.InvalidCommandError("Invalid command syntax.")

	def cmdloop(self):
		print("Kingslayer, destroying castles in the sky ...")

		try:
			while True:
				try:
					cmd = input(self.PREFIX)
				except ValueError:
					exit(0)
				try:
					self.exec(cmd)
				except self.InvalidCommandError:
					print("Invalid command.")
		except KeyboardInterrupt:
			pass

		print("\n\nKingslayer, come and collect us from the night!")

@Kingslayer.Command.assemble
def quit():
	print("Kingslayer, come and collect us from the night!")
	exit(0)

@Kingslayer.Command.assemble
def clear():
	for _ in range(200):
		print()

