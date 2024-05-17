echo "Compiling C++ file \"$1.cpp\" ..."
if (g++ $1.cpp -o $1 -lssl -lcrypto) then (echo "  g++ compiled C++ file successfully.") else (exit -1) fi
echo "Successfully compiled C++ file \"$1.cpp\". Executable file is \"$1\"."