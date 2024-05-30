import os

print("Kingslayer-NG: Fixing redundant includes ...")

print("  Scanning for header files ...")

full = ""
fl = ""
for root, dirs, files in os.walk("ksng"):
	for file in files:
		if file.endswith(".h"):
			print("    Found header file \"" + os.path.join(root, file) + "\". ", end="")
			print("Checking for \"once\" pragma ...", end="")
			with open(os.path.join(root, file), "r") as f:
				fl = f.read().split("\n")[0].strip()
				full = f.read()
			print("first line: \"" + fl + "\", ", end="")
			if fl == "#pragma once":
				print("header ok")
			else:
				print("fixing header ...", end="")
				with open(os.path.join(root, file), "w") as f:
					f.write("#pragma once\n" + full)
				print("done")