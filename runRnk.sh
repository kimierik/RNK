




case "$1" in
    -b|--build)
        g++ -g rnkCompiler.cpp -o build/Compiler &&  ./build/Compiler -d > build/asem.s&&
        as -g -o build/asem.o build/asem.s && gcc -o build/asem build/asem.o -nostdlib -static && ./build/asem ; echo -e "\nerror code: $?"
        ;;
    -d|--debug)
        echo "debugging info"
        g++ rnkCompiler.cpp -o build/Compiler &&  ./build/Compiler -b
        ;;
    *)
        echo "
        -b,      --build        builds and runs the program
        -d,      --debug        prints debug information from the program
        "
        ;;

esac


