#!/usr/bin/env sh

# TODO:
#     Write a function for simple checks (e.g: python >= 3.9, meson >= 0.6, ...).
#     I'm not convinced it can be generic enough to handle the compiler checks, but
#     the simpler ones I'm pretty sure can be automated.
# TODO: openssl.
# TODO: add elapsed time to spinner.

no_cleanup=0
force_reconfigure=0
force_no_pkg_config=0
force_build_python=0
force_build_ninja=0
force_build_meson=0
force_build_compiler=0
force_build_cmake=0

while [ $# -gt 0 ]; do
    case $1 in
        --no-cleanup) no_cleanup=1;;
        --force-reconfigure) force_reconfigure=1;;
        --force-no-pkg-config) force_no_pkg_config=1;;
        --force-build-python) force_build_python=1;;
        --force-build-ninja) force_build_ninja=1;;
        --force-build-meson) force_build_meson=1;;
        --force-build-compiler) force_build_compiler=1;;
        --force-build-cmake) force_build_cmake=1;;
    esac

    shift
done

if [ $force_reconfigure -eq 0 ]
then
    if [ -f "tooling.sh" ]; then exit 0; fi
fi

all_programs=$(./programs.sh)

CRESET='\033[0m' # Text Reset
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'

pid_waiting_spinner()
{
    local PID=$1
    PID_WAITING_SPINNER_WAITING_FOR=$PID
    local prefix=$2
    local postfix=$3
    local after=$4
    local i=0

    while [ -d /proc/$PID ]
    do
        case $(expr $i % 4) in
            0) echo "$2/ $3\c" ;;
            1) echo "$2- $3\c" ;;
            2) echo "$2\\ $3\c" ;;
            3) echo "$2| $3\c" ;;
        esac
        i=`expr ${i} + 1`
        # change the speed of the spinner by altering the 1 below
        sleep 1
        echo "$4"
    done

    unset PID_WAITING_SPINNER_WAITING_FOR
    wait $PID
    return $?
}

pid_waiting_spinner_cancel()
{
    if [ ! -z "$PID_WAITING_SPINNER_WAITING_FOR" ]
    then
        kill -9 $PID_WAITING_SPINNER_WAITING_FOR
        wait $PID_WAITING_SPINNER_WAITING_FOR
    else
        exit 1
    fi
}

trap pid_waiting_spinner_cancel INT

buildstep()
{
    local command=$1
    local log_out=$2
    local indent=$3
    local msg_build=$4
    local msg_build_clear=$5
    local msg_fail=$6
    local fail_command=$7
    local msg_success=$8

    msg_build="$msg_build with ${YELLOW}$command${CRESET}\n$indent\tSee $log_out for progress"

    eval $command > $log_out 2>&1 &
    pid_waiting_spinner $! "$indent" "$msg_build" "$msg_build_clear"
    if [ $? -ne 0 ]
    then
        echo "$indent${RED}Failed to $msg_fail${CRESET}, aborting ${YELLOW}(See $log_out)${CRESET}"
        echo "$indent\tRunning cleanup: ${YELLOW}$fail_command${CRESET}"
        eval $fail_command
        exit 1
    fi
    echo "$indent$msg_success with ${YELLOW}$command${CRESET}\n$indent\tSee $log_out for output"
}


## Checking for pkg-config (to find openssl if we must build python)

echo "Checking for ${YELLOW}pkg-config${CRESET}"
if [ $force_no_pkg_config -eq 0 ]
then
    maybe_pkg_config=$(echo "$all_programs" | grep "/pkg-config$")
    set -- $maybe_pkg_config
    if [ $# -gt 0 ]; then pkg_config=$1; fi
fi

if [ ! -z "$pkg_config" ]
then
    pkg_config_found=1
    echo "Found ${GREEN}pkg-config${CRESET}, will use ${YELLOW}pkg_config=$pkg_config${CRESET}\n"
else
    pkg_config_found=0
    echo "${RED}Did not find pkg-config${CRESET}, if we need to build ${YELLOW}python3${CRESET} we'll need to build ${YELLOW}openssl${CRESET} too\n"
fi

## Checking for python

python_min_version="3.9"

echo "Searching for a suitable python installation (>= $python_min_version)..."

if [ $force_build_python -eq 0 ]
then
    for p in $(echo "$all_programs" | grep "/python\\([0-9]\\+\\.\\?\\)*\$")
    do
        # Python < 3.4 prints version info to stderr instead of stdout
        version=$($p --version 2>&1 | grep -o "[0-9].*\$")

        if [ $(./semver.sh "$version" "$python_min_version") -ge 0 ]
        then
            echo "\t${GREEN}$version${CRESET} in $p"
            # If is unset.
            if [ -z "$python_version_found" ]
            then
                python_version="$version"
                python="$p"
            # Or if new is of higher version.
            elif [ $(./semver.sh "$version" "$python_version") -gt 0 ]
            then
                python_version="$version"
                python="$p"
            fi
        else echo "\t${RED}$version${CRESET} in $p"; fi
    done
fi

if [ ! -z "$python" ]
then
    python_found=1
    echo "Found ${GREEN}Python $python_version${CRESET}, will use ${YELLOW}python=$python${CRESET}\n"
else
    python_found=0
    echo "${RED}Did not find Python${CRESET}, will build ${YELLOW}Python 3.9.0${CRESET} from source instead"

    if [ ! -d "downloaded" ]; then mkdir downloaded; fi

    if [ -d "python" ]; then rm -r python; fi

    if [ ! -f "downloaded/Python-3.9.0.tgz" ]
    then
        echo "\tDownloading Python to $(pwd)/downloaded/Python-3.9.0.tgz"
        wget -c -q https://www.python.org/ftp/python/3.9.0/Python-3.9.0.tgz -O downloaded/Python-3.9.0.tgz
    fi

    mkdir python
    echo "\tExtracting $(pwd)/downloaded/Python-3.9.0.tgz to $(pwd)/python"
    tar -xf downloaded/Python-3.9.0.tgz -C python --strip 1

    echo "\tModifying python to use openssl"
    #echo "SSL=$(pwd)/openssl/installdir"                              >> $(pwd)/python/Modules/Setup
    #echo "_ssl _ssl.c \\"                                             >> $(pwd)/python/Modules/Setup
    #echo "	-DUSE_SSL -I\$(SSL)/include -I\$(SSL)/include/openssl \\" >> $(pwd)/python/Modules/Setup
    #echo "	-L\$(SSL)/lib64 -lssl -lcrypto"                           >> $(pwd)/python/Modules/Setup

    if [ $no_cleanup -eq 0 ]; then
        py_rm_cmd="cd ../..; rm -r python"
    else
        py_rm_cmd="cd ../.."
    fi
    py_conf_out="$(pwd)/python_config_out.txt"
    py_build_out="$(pwd)/python_build_out.txt"
    py_install_out="$(pwd)/python_install_out.txt"
    py_install_dir="$(pwd)/python/installdir"
    mkdir python/build
    cd python/build
        # Commented out until i implement openssl building.
        #python_configure_command="./../configure --with-ensurepip=install --prefix=$python_install_dir --enable-optimizations CFLAGS=\"-I$(pwd)/../../openssl/installdir/include\" LDFLAGS=\"-L$(pwd)/../../openssl/installdir/lib64\""
        py_configure_cmd="./../configure --with-ensurepip=install --prefix=$py_install_dir --enable-optimizations"
        py_build_cmd="make -j$(nproc)"
        py_install_cmd="make install"

        buildstep "$py_configure_cmd" "$py_conf_out"    "\t" "Configuring Python"         "\033[2K\033[F\033[2K\033[F" "configure Python"       "$py_rm_cmd" "Python configured"
        buildstep "$py_build_cmd"     "$py_build_out"   "\t" "Building Python"            "\033[2K\033[F\033[2K\033[F" "build Python"           "$py_rm_cmd" "Python built"
        buildstep "$py_install_cmd"   "$py_install_out" "\t" "Installing Python locally"  "\033[2K\033[F\033[2K\033[F" "install Python locally" "$py_rm_cmd" "Python installed locally"
    cd ../..

    # TODO: certifi if no openssl

    python="$py_install_dir/python"
    echo "\t${GREEN}Python sucessfully installed${CRESET}, will use ${YELLOW}python=$python${CRESET}\n"
fi

## Checking for Ninja.

echo "Checking for ${YELLOW}Ninja${CRESET}"
if [ $force_build_ninja -eq 0 ]
then
    maybe_ninja=$(echo "$all_programs" | grep "/ninja$")
    set -- $maybe_ninja
    if [ $# -gt 0 ]; then ninja=$1; fi
fi

if [ -z "$ninja" ]
then
    ninja_found=0

    echo "${RED}Did not find Ninja${CRESET}, will build ${YELLOW}Ninja 1.10.2${CRESET} from source instead"

    if [ -d "ninja" ]; then rm -r ninja; fi


    ninja_rm_cmd=""
    ninja_clone_out="ninja_clone_out.txt"

    ninja_clone_cmd="git clone -b v1.10.2 --depth 1 https://github.com/ninja-build/ninja.git"
    buildstep "$ninja_clone_cmd" "$ninja_clone_out" "\t" "Cloning Ninja" "\033[2K\033[F\033[2K\033[F" "clone Ninja" "$ninja_rm_cmd" "Ninja cloned"

    ninja_bootstrap_out="$(pwd)/ninja_bootstrap_out.txt"
    cd ninja
        if [ $no_cleanup -eq 0 ]; then
            ninja_rm_cmd="cd ../..; rm -r ninja"
        else
            ninja_rm_cmd="cd ../.."
        fi
        ninja_bootstrap_cmd="$python configure.py --bootstrap"
        buildstep "$ninja_bootstrap_cmd" "$ninja_clone_out" "\t" "Bootstrapping Ninja" "\033[2K\033[F\033[2K\033[F" "bootstrap Ninja" "$ninja_rm_cmd" "Ninja bootstrapped"
    cd ..

    if [ ! -d "ninja/bin" ]; then mkdir ninja/bin; fi
    ln -sf $(pwd)/ninja/ninja $(pwd)/ninja/bin/ninja
    ninja="$(pwd)/ninja/bin/ninja"
    echo "\t${GREEN}Ninja sucessfully installed${CRESET}, will use ${YELLOW}ninja=$ninja${CRESET}\n"
else
    ninja_found=1
    echo "Found ${GREEN}ninja${CRESET}, will use ${YELLOW}ninja=$ninja${CRESET}\n"
fi

## Checking for meson.

echo "Checking for ${YELLOW}Meson${CRESET}"
if [ $force_build_meson -eq 0 ]
then
    maybe_meson=$(echo "$all_programs" | grep "/meson$")
    set -- $maybe_meson
    if [ $# -gt 0 ]; then meson=$1; fi
fi

if [ -z "$meson" ]
then
    meson_found=0

    echo "${RED}Did not find Meson${CRESET}, will use ${YELLOW}Meson 0.60.0${CRESET} from source instead"

    if [ -d "meson" ]; then rm -r meson; fi

    meson_rm_cmd=""
    meson_clone_out="meson_clone_out.txt"

    meson_clone_cmd="git clone -b 0.60.0 --depth 1 https://github.com/mesonbuild/meson.git"
    buildstep "$meson_clone_cmd" "$meson_clone_out" "\t" "Cloning Meson" "\033[2K\033[F\033[2K\033[F" "clone Meson" "$meson_rm_cmd" "Meson cloned"

    if [ ! -d "meson/bin" ]; then mkdir meson/bin; fi
    ln -sf $(pwd)/meson/meson.py $(pwd)/meson/bin/meson
    meson="$python $(pwd)/meson/bin/meson"
    echo "\t${GREEN}Meson sucessfully installed${CRESET}, will use ${YELLOW}meson=$meson${CRESET}\n"
else
    meson_found=1
    echo "Found ${GREEN}Meson${CRESET}, will use ${YELLOW}meson=$meson${CRESET}\n"
fi

## Checking for compiler.

gcc_min_version="11"
clang_min_version="13"

echo "Searching for suitable compilers (GCC >= $gcc_min_version OR Clang >= $clang_min_version)..."

cpp_regex="c++"
# Matches g++, g++-0, ..., g++-10, ...
gcc_regex="g++\\(-[0-9]\\+\\)\\?"
# Same as gcc_regex
clang_regex="clang++\\(-[0-9]\\+\\)\\?"
compiler_regex="/\\($cpp_regex\\|$gcc_regex\\|$clang_regex\\)\$"

if [ $force_build_compiler -eq 0 ]
then
    for c in $(echo "$all_programs" | grep "$compiler_regex")
    do
        compiler_version=$($c --version)

        case "$compiler_version" in
            *clang*)
                version=$(echo "$compiler_version" | grep -o "[0-9]\\+\\.\\([0-9]\\+\\.\\?\\)*")
                if [ $(./semver.sh "$version" "$clang_min_version") -ge 0 ]
                then
                    echo "\tClang ${GREEN}$version${CRESET} in $c"
                    # If is unset.
                    if [ -z "$clang_version_found" ]
                    then
                        clang_version_found="$version"
                        clang_exe_found="$c"
                    # Or if new is of higher version.
                    elif [ $(./semver.sh "$version" "$clang_version_found") -gt 0 ]
                    then
                        clang_version_found="$version"
                        clang_exe_found="$c"
                    fi
                else echo "\tClang ${RED}$version${CRESET} in $c"; fi
                ;;
            *)
                version=$(echo "$compiler_version" | grep -o "\\(\\([0-9]\\+\\)\\(\\.\\|-\\)\\?\\)*$")
                if [ $(./semver.sh "$version" "$gcc_min_version") -ge 0 ]
                then
                    echo "\tGCC   ${GREEN}$version${CRESET} in $c"
                    # If is unset.
                    if [ -z "$gcc_version_found" ]
                    then
                        gcc_version_found="$version"
                        gcc_exe_found="$c"
                    # Or if new is of higher version.
                    elif [ $(./semver.sh "$version" "$gcc_version_found") -gt 0 ]
                    then
                        gcc_version_found="$version"
                        gcc_exe_found="$c"
                    fi
                else echo "\tGCC   ${RED}$version${CRESET} in $c"; fi
                ;;
        esac
    done
fi

if [ ! -z "$clang_exe_found" ]
then
    cmake_found=0
    cxx_found=1
    cc_found=1
    CC="$clang_exe_found"
    CXX="$clang_exe_found"
    echo "Found ${GREEN}Clang $clang_version_found${CRESET} in $clang_exe_found"
    echo "\tWill use ${YELLOW}CC=$clang_exe_found${CRESET}"
    echo "\tWill use ${YELLOW}CXX=$clang_exe_found${CRESET}\n"
elif [ ! -z "$gcc_exe_found" ]
then
    cmake_found=0
    cxx_found=1
    cc_found=1
    CC="$gcc_exe_found"
    CXX="$gcc_exe_found"
    echo "Found ${GREEN}GCC $gcc_version_found${CRESET} in $gcc_exe_found"
    echo "\tWill use ${YELLOW}CC=$gcc_exe_found${CRESET}"
    echo "\tWill use ${YELLOW}CXX=$gcc_exe_found${CRESET}\n"
else
    cxx_found=0
    cc_found=0
    echo "${RED}No suitable compilers found${CRESET}, will build ${YELLOW}Clang 13.0.0${CRESET} and use that as ${YELLOW}CC${CRESET} and ${YELLOW}CXX${CRESET}"

    echo "\tChecking for ${YELLOW}CMake${CRESET}"
    if [ $force_build_cmake -eq 0 ]
    then
        maybe_cmake=$(echo "$all_programs" | grep "/cmake$")
        set -- $maybe_cmake
        if [ $# -gt 0 ]; then cmake=$1; fi
    fi

    if [ -z "$cmake" ]
    then
        cmake_found=0

        echo "\t${RED}Did not find CMake${CRESET}, will build ${YELLOW}CMake 3.23.0${CRESET} from source instead"

        if [ ! -d "downloaded" ]; then mkdir downloaded; fi

        if [ -d "cmake" ]; then rm -r cmake; fi

        if [ ! -f "downloaded/cmake-3.23.0.tar.gz" ]
        then
            echo "\t\tDownloading CMake to $(pwd)/downloaded/cmake-3.23.0.tar.gz"
            wget -c -q https://github.com/Kitware/CMake/releases/download/v3.23.0/cmake-3.23.0.tar.gz -O downloaded/cmake-3.23.0.tar.gz
        fi

        mkdir cmake
        echo "\t\tExtracting $(pwd)/downloaded/cmake-3.23.0.tar.gz to $(pwd)/cmake"
        tar -xf downloaded/cmake-3.23.0.tar.gz -C cmake --strip 1

        if [ $no_cleanup -eq 0 ]; then
            cmake_rm_cmd="cd ../..; rm -r cmake"
        else
            cmake_rm_cmd="cd ../.."
        fi
        cmake_install_dir="$(pwd)/cmake/installdir"
        cmake_bootstrap_out="$(pwd)/cmake_bootstrap_out.txt"
        cmake_build_out="$(pwd)/cmake_build_out.txt"
        cmake_install_out="$(pwd)/cmake_install_out.txt"
        mkdir cmake/build
        cd cmake/build
            cmake_bootstrap_cmd="./../bootstrap -- -DCMAKE_USE_OPENSSL=OFF -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=$cmake_install_dir"
            cmake_build_cmd="make -j$(nproc)"
            cmake_install_cmd="make install"

            buildstep "$cmake_bootstrap_cmd" "$cmake_bootstrap_out" "\t\t" "Bootstrapping CMake"      "\033[2K\033[F\033[2K\033[F" "bootstrap CMake"       "$cmake_rm_cmd" "CMake bootstrapped"
            buildstep "$cmake_build_cmd"     "$cmake_build_out"     "\t\t" "Building CMake"           "\033[2K\033[F\033[2K\033[F" "build CMake"           "$cmake_rm_cmd" "CMake built"
            buildstep "$cmake_install_cmd"   "$cmake_install_out"   "\t\t" "Installing CMake locally" "\033[2K\033[F\033[2K\033[F" "install CMake locally" "$cmake_rm_cmd" "CMake installed locally"
        cd ../..

        cmake="$cmake_install_dir/bin/cmake"
        echo "\t\t${GREEN}CMake sucessfully installed${CRESET}, will use ${YELLOW}cmake=$cmake${CRESET}\n"
    else
        cmake_found=1
        echo "\tFound ${GREEN}CMake${CRESET}, will use ${YELLOW}cmake=$cmake${CRESET}\n"
    fi

    clang_rm_cmd=""
    clang_clone_out="clang_clone_out.txt"
    clang_install_dir="$(pwd)/llvm-project/installdir"

    clang_clone_cmd="git clone -b llvmorg-13.0.0 --depth 1 https://github.com/llvm/llvm-project.git"
    buildstep "$clang_clone_cmd" "$clang_clone_out" "\t" "Cloning Clang" "\033[2K\033[F\033[2K\033[F" "clone Clang" "$clang_rm_cmd" "Clang cloned"

    if [ $no_cleanup -eq 0 ]; then
        clang_rm_cmd="cd ../..; rm -r llvm-project"
    else
        clang_rm_cmd="cd ../.."
    fi
    clang_configure_out="$(pwd)/clang_configure_out.txt"
    clang_build_out="$(pwd)/clang_build_out.txt"
    clang_install_out="$(pwd)/clang_install_out.txt"

    mkdir llvm-project/build
    cd llvm-project/build
        clang_configure_cmd="$cmake -DLLVM_ENABLE_PROJECTS=\"clang;openmp\" -GNinja -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=$clang_install_dir ../llvm"
        clang_build_cmd="$ninja"
        clang_install_cmd="$ninja install"

        buildstep "$clang_configure_cmd" "$clang_configure_out" "\t" "Configuring Clang"        "\033[2K\033[F\033[2K\033[F" "configure Clang"       "$clang_rm_cmd" "Clang configured"
        buildstep "$clang_build_cmd"     "$clang_build_out"     "\t" "Building Clang"           "\033[2K\033[F\033[2K\033[F" "build Clang"           "$clang_rm_cmd" "Clang built"
        buildstep "$clang_install_cmd"   "$clang_install_out"   "\t" "Installing Clang locally" "\033[2K\033[F\033[2K\033[F" "install Clang locally" "$clang_rm_cmd" "Clang installed locally"
    cd ../..

    CC="$clang_install_dir/bin/clang"
    CXX="$clang_install_dir/bin/clang++"
    echo "\t${GREEN}Clang sucessfully installed${CRESET}"
    echo "\t\tWill use ${YELLOW}CC=$CC${CRESET}"
    echo "\t\tWill use ${YELLOW}CXX=$CXX${CRESET}\n"
fi


## Build the final file to be sourced.

echo "Writing ${YELLOW}tooling.sh${CRESET}..."

echo "#!/usr/bin/env sh" > tooling.sh
if [ $python_found -eq 0 ]
then
    echo "export PATH=$py_install_dir/bin:\$PATH"                                 >> tooling.sh
    echo "export PKG_CONFIG_PATH=$py_install_dir/lib/pkgconfig:\$PKG_CONFIG_PATH" >> tooling.sh
    echo "export LD_LIBRARY_PATH=$py_install_dir/lib:\$LD_LIBRARY_PATH"           >> tooling.sh
    echo "export LIBRARY_PATH=$py_install_dir/lib:\$LIBRARY_PATH"                 >> tooling.sh
fi
if [ $ninja_found -eq 0 ]; then echo "export PATH=$(pwd)/ninja/bin:\$PATH" >> tooling.sh; fi
if [ $meson_found -eq 0 ]; then echo "export PATH=$(pwd)/meson/bin:\$PATH" >> tooling.sh; fi
echo "export CC=$CC"   >> tooling.sh
echo "export CXX=$CXX" >> tooling.sh
if [ $cxx_found -eq 0 ]
then
    echo "export PATH=$clang_install_dir/bin:\$PATH" >> tooling.sh
    echo "export LD_LIBRARY_PATH=$clang_install_dir/lib:\$LD_LIBRARY_PATH"           >> tooling.sh
    echo "export LIBRARY_PATH=$clang_install_dir/lib:\$LIBRARY_PATH"                 >> tooling.sh
    if [ $cmake_found -eq 0 ]; then echo "export PATH=$cmake_install_dir/bin:\$PATH" >> tooling.sh; fi
fi

echo "Tooling setup ${GREEN}done${CRESET} :)\n"
