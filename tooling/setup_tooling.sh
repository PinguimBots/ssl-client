#!/usr/bin/env sh

# TODO: Make me into a proper script.
#     - Check if tool is installed before downloading.
#     - Check if version compatibility if tool already installed.
#     - Clean up output.
#     - Display what we are doing.
#     - What happens on Ctrl+C ?

if [ ! -d "downloaded" ]; then mkdir downloaded; fi
if [ ! -d "bin" ];        then mkdir bin; fi

# So we can use the tools as we are building them.
export PATH="$(pwd)/bin:$PATH"

# Meson requires urllib with https support, which requires python with openssl.
if [ ! -d "openssl" ]
then
    git clone -b openssl-3.0.2 https://github.com/openssl/openssl.git

    mkdir openssl/build
    cd openssl/build
        ./../Configure --prefix=$(pwd)/../installdir --openssldir=$(pwd)/../installdir
        make -j$(nproc)
        make install
    cd ../..
fi
export PATH=$(pwd)/openssl/installdir/bin:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/openssl/installdir/lib64
export LIBRARY_PATH=$LIBRARY_PATH:$(pwd)/openssl/installdir/lib64

if [ ! -d "python" ]
then
    wget -c https://www.python.org/ftp/python/3.9.0/Python-3.9.0.tgz -O downloaded/Python-3.9.0.tgz
    mkdir python
    tar -xf downloaded/Python-3.9.0.tgz -C python --strip 1

    echo "SSL=$(pwd)/openssl/installdir"                              >> $(pwd)/python/Modules/Setup
    echo "_ssl _ssl.c \\"                                             >> $(pwd)/python/Modules/Setup
    echo "	-DUSE_SSL -I\$(SSL)/include -I\$(SSL)/include/openssl \\" >> $(pwd)/python/Modules/Setup
    echo "	-L\$(SSL)/lib64 -lssl -lcrypto"                           >> $(pwd)/python/Modules/Setup

    mkdir python/build
    cd python/build
        ./../configure --with-ensurepip=install --prefix=$(pwd)/../installdir --enable-optimizations CFLAGS="-I$(pwd)/../../openssl/installdir/include" LDFLAGS="-L$(pwd)/../../openssl/installdir/lib64"
        make -j$(nproc)
        make install
    cd ../..

    export PATH=$(pwd)/python/installdir/bin:$PATH
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/python/installdir/lib
    export LIBRARY_PATH=$LIBRARY_PATH:$(pwd)/python/installdir/lib

    pip3 install certifi
    ln -sf $(python3 -m certifi) $(pwd)/openssl/installdir/cert.pem
else
    export PATH=$(pwd)/python/installdir/bin:$PATH
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/python/installdir/lib
    export LIBRARY_PATH=$LIBRARY_PATH:$(pwd)/python/installdir/lib
fi

if [ ! -d "ninja" ]
then
    git clone -b v1.10.2 https://github.com/ninja-build/ninja.git

    cd ninja; ./configure.py --bootstrap; cd ..

    ln -sf $(pwd)/ninja/ninja $(pwd)/bin/ninja
fi

if [ ! -d "meson" ]
then
    git clone -b 0.60.0 --depth 1 https://github.com/mesonbuild/meson.git
    ln -sf $(pwd)/meson/meson.py $(pwd)/bin/meson
fi

# Needed by clang.
if [ ! -d "cmake" ]
then
    wget -c https://github.com/Kitware/CMake/releases/download/v3.23.0/cmake-3.23.0.tar.gz -O downloaded/cmake-3.23.0.tar.gz
    mkdir cmake
    tar -xf downloaded/cmake-3.23.0.tar.gz -C cmake --strip 1

    mkdir cmake/build
    cd cmake/build
        ./../bootstrap -- -DCMAKE_USE_OPENSSL=OFF -DCMAKE_BUILD_TYPE=release
        make -j$(nproc)
    cd ../..
fi
export PATH="$(pwd)/cmake/build/bin:$PATH"

if [ ! -d "llvm-project" ]
then
    git clone -b llvmorg-13.0.0 --depth 1 https://github.com/llvm/llvm-project.git

    mkdir llvm-project/build
    cd llvm-project/build
        cmake -DLLVM_ENABLE_PROJECTS="clang;openmp" -GNinja -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=$(pwd)/../installdir ../llvm
        ninja install
    cd ../..
fi

echo "#!/usr/bin/env sh" > tooling.sh
echo "export PATH=$(pwd)/bin:\$PATH" >> tooling.sh
if [ -d "openssl" ]
then
    echo "export PATH=$(pwd)/openssl/installdir/bin:\$PATH" >> tooling.sh
    echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$(pwd)/openssl/installdir/lib64" >> tooling.sh
    echo "export LIBRARY_PATH=\$LIBRARY_PATH:$(pwd)/openssl/installdir/lib64"       >> tooling.sh
fi
if [ -d "python" ]
then
    echo "export PATH=$(pwd)/python/installdir/bin:\$PATH" >> tooling.sh
    echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$(pwd)/python/installdir/lib" >> tooling.sh
    echo "export LIBRARY_PATH=\$LIBRARY_PATH:$(pwd)/python/installdir/lib"       >> tooling.sh
fi
if [ -d "cmake" ]; then echo "export PATH=$(pwd)/cmake/build/bin:\$PATH" >> tooling.sh; fi
if [ -d "llvm-project" ]
then
    echo "export PATH=$(pwd)/llvm-project/installdir/bin:\$PATH" >> tooling.sh
    echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$(pwd)/llvm-project/installdir/lib" >> tooling.sh
    echo "export LIBRARY_PATH=\$LIBRARY_PATH:$(pwd)/llvm-project/installdir/lib"       >> tooling.sh
    echo "export CC=clang" >> tooling.sh
    echo "export CXX=clang++" >> tooling.sh
fi
