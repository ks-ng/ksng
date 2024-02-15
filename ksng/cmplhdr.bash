echo "Compiling header file \"$1\" (filename ksng/$1.h) ..."
if (g++ ksng/$1.h) then (echo "Compiled header successfully.") else (exit -1) fi