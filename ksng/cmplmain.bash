echo "Compiling ksng/main.cpp ..."
if (g++ ksng/main.cpp -o kingslayer -lssl -lcrypto) then (
	echo "Kingslayer main CLI compiled successfully. Run using \"./kingslayer\"."
	echo ""
) else (
	exit -1
) fi