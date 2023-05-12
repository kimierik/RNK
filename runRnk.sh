g++ rnkCompiler.cpp -o build/Compiler &&  ./build/Compiler > build/asem.s&&
as -o build/asem.o build/asem.s && gcc -o build/asem build/asem.o -nostdlib -static && ./build/asem ; echo -e "\nerror code: $?"
