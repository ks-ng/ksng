echo "Compiling assembly file \"$1.asm\" ..."
if (nasm -f elf32 -o x.o $1.asm) then (echo "  nasm assembled code successfully.") else (exit -1) fi
if (ld -m elf_i386 -o $1 x.o) then (echo "  ld linked code successfully.") else (exit -2) fi
if (rm x.o) then (echo "  Successfully removed x.o.") else (exit -3) fi
echo "Successfully compiled \"$1.asm\". Executable file is \"$1\"."