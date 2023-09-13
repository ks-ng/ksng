from kingslayer.q import *

import random

# Define the Quantum Fourier Transform gate
def qft_gate(size):
	matrix = [[expi(2 * pi * i * j / size) for i in range(size)] for j in range(size)]
	return QuantumGate(matrix)

# Create a function to apply the QFT gate to a quantum register
def apply_qft(register):
	size = len(register)
	gate = qft_gate(size)
	register.apply(gate, *range(size))

# Define the modular exponentiation function (used later)
def modular_exponentiation(base, exponent, modulus):
	result = 1
	base = base % modulus
	while exponent > 0:
		if exponent % 2 == 1:
			result = (result * base) % modulus
		base = (base * base) % modulus
		exponent = exponent // 2
	return result

# Define a function to find the greatest common divisor (GCD) using Euclidean algorithm
def gcd(a, b):
	while b:
		a, b = b, a % b
	return a

# Create a function to find the period 'r' using Shor's algorithm
def find_period(n):
	# Step 1: Choose a random integer 'a' between 2 and 'n-2'
	print("initializing a")
	a = random.randint(2, n - 2)
	
	# Step 2: Compute the greatest common divisor (GCD) of 'a' and 'n'
	print("getting gcd")
	common_divisor = gcd(a, n)
	
	# If the GCD is greater than 1, we've found a non-trivial factor of 'n'
	if common_divisor > 1:
		return common_divisor
	
	# Step 3: Initialize quantum registers
	print("initializing registers")
	size = int(log(n, 2)) + 1
	x_register = QuantumRegister(size)
	f_register = QuantumRegister(size)
	
	# Step 4: Prepare the superposition in 'x_register'
	print("applying hadamard")
	for i in range(size):
		x_register.apply(Hadamard(), i)
	
	# Step 5: Implement the modular exponentiation operator
	print("applying modular exponentiation")
	for i in range(size):
		exponent = 2 ** i
		result = modular_exponentiation(a, exponent, n)
		phase = 2 * pi * result / n
		gate = QuantumGate(matrix=[[exppi(phase), 0], [0, exppi(phase)]])  # Permutation matrix
		x_register.apply(gate, i)
	
	# Step 6: Apply the QFT to 'x_register'
	print("applying quantum Fourier transform")
	apply_qft(x_register)
	
	# Step 7: Measure 'x_register' to get a possible 'r' value
	print("measuring register")
	measured_x = [x_register[i].probable((0,)) for i in range(size)]
	measured_x.reverse()
	measured_x = int("".join(map(str, measured_x)), 2)
	
	# Step 8: Compute the continued fractions of 'measured_x' / 2^size
	print("getting continued fractions")
	fractions = continued_fractions(measured_x, 2 ** size)
	
	# Step 9: Find the possible 'r' values from the continued fractions
	print("searching continued fractions")
	for fraction in fractions:
		denominator = fraction[1]
		possible_r = denominator
		if possible_r % 2 == 0:
			period = modular_exponentiation(a, possible_r // 2, n)
			if (period != 1) and ((period * period) % n == 1):
				print("success")
				return gcd(period - 1, n)
	
	# If we can't find the period, return None (algorithm failed)
	print("fail")
	return None

# Define a function to factorize 'n' using Shor's algorithm
def shor_factorize(n):
	while True:
		factor = find_period(n)
		if factor is not None and factor != n:
			return factor, n // factor


