cmake -DCMAKE_TOOLCHAIN_FILE=$BLOCKSDS/cmake/BlocksDS.cmake -B build . -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DCMAKE_BUILD_TYPE=DEBUG
cmake --build build --parallel
