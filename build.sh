mkdir -p build
cd build

cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. -Wno-dev
ninja -j $(nproc)

cd ..
cp build/compile_commands.json compile_commands.json
