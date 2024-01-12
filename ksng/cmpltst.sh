clear
echo "Compiling test \"$1\" (expected location: ksng/tsts/tst_$1.cpp) ..."
if (g++ ksng/tsts/tst_$1.cpp -o ./tst)
then (echo "Compilation successful."; echo "Running program ..."; sleep 1; ./tst)
else (echo "Compilation error, could not execute file.")
fi