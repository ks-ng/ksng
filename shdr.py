print("Kingslayer-NG: Loading super header ...")

print("  Importing os ...", end="")

import os

print("done.\n  Clearing existing super header ...")

fd = open("ksng.h", "w")
fd.write("// Kingslayer-NG\n// This file was originally SEVERAL DIFFERENT HEADERS and may not properly function!")
fd.close()
superheader = open("ksng.h", "a")
incl = []

i = 0
print("  Scanning for header files ...")
for directory, subdirectories, files in os.walk("ksng"):
	for f in files:
		if f.endswith(".h"):
			i += 1
			fn = directory + "/" + f
			print("\r    Copying header file \"" + fn + "\" ...", end="        ")
			fd = open(fn, "r")
			data = fd.read()
			superheader.write("\n\n// " + fn + "\n")
			for ln in data.split("\n"):
				if (not ln.startswith("#include")) and (not ln.startswith("#pragma")):
					superheader.write(ln + "\n")
				elif (not ln.startswith("#include \"")) and (not ln.startswith("#pragma")):
					incl.append(ln)
			fd.close()

print("\r  Successfully scanned", i, "header files, with", len(incl), "inclusions.")
print("  Preparing inclusions ...")
print("    Removing include duplicates ...", end="")
incl = list(set(incl))
print("done.\n    Prepending inclusions ...", end="")

superheader.close()

with open("ksng.h", "r") as sh:
	current = sh.read()

incl = incl[::-1]
incl.append("#pragma once")
incl = incl[::-1]

with open("ksng.h", "w") as sh:
	sh.write("\n".join(incl) + "\n" + current)

print("done.")
print("  Inclusions prepared.")
print("Super header file written. Hope it works.")