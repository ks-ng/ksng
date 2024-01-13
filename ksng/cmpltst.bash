#!/bin/bash
echo "Compiling test file with ID \"$1\" (filename ksng/tsts/tst_$1.cpp) ..."
if (g++ ksng/tsts/tst_$1.cpp -o tst) then (
	echo "Compiled test file successfully. Running test ..."
	echo ""
	./tst; echo ""
	echo "Test complete."
) else (
	exit -1
) fi