rm -rf build
cmake -B build -DCMAKE_DEPENDS_USE_COMPILER=OFF
cmake --build build