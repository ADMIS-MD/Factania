cmake -DCMAKE_TOOLCHAIN_FILE=$BLOCKSDS/cmake/BlocksDS.cmake -B build . -DCMAKE_EXPORT_COMPILE_COMMANDS=On
cd build
make
