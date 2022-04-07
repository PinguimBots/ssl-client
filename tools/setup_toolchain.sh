#!/usr/bin/env sh

# TODO: make progress resumable on failure

no_cleanup=0
force_reconfigure=0
force_no_pkg_config=0
force_build_python=0
force_build_ninja=0
force_build_meson=0
force_build_compiler=0
force_build_cmake=0
force_build_all=0

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
        --force-build-all)
            force_no_pkg_config=1
            force_build_python=1
            force_build_ninja=1
            force_build_meson=1
            force_build_compiler=1
            force_build_cmake=1
        ;;
    esac

    shift
done

toolchain_file="$(pwd)/toolchain.sourceme.sh"

if [ $force_reconfigure -eq 0 ]
then
    if [ -f "$toolchain_file" ]; then exit 0; fi
fi

main()
{
    ## Checking for pkg-config (to find openssl if we must build python)

    if [ $force_no_pkg_config -ne 0 ]; then show "NOTE: --force-no-pkg-config is set"; fi
    find_program "pkg-config" "/pkg-config$" "pkg_config"

    if [ ! -z "$pkg_config" ] && [ $force_no_pkg_config -eq 0 ]
    then
        pkg_config_found=1
        show "Found ${GREEN}pkg-config${CRESET}, will use ${YELLOW}pkg_config=$pkg_config${CRESET}\n"
    else
        pkg_config_found=0
        show "${RED}Did not find pkg-config${CRESET}, if we need to build ${YELLOW}python3${CRESET} we'll need to build ${YELLOW}openssl${CRESET} too\n"
    fi

    ## Checking for python

    if [ $force_build_python -ne 0 ]; then show "NOTE: --force-build-python is set"; fi
    find_versioned_ge "Python" "3.9" "/python\\([0-9]\\+\\.\\?\\)*\$" "--version" "[0-9].*\$" "python"
    if [ ! -z "$python" ] && [ $force_build_python -eq 0 ]
    then
        python_found=1
        show "Found ${GREEN}Python $python_version${CRESET}\n"
    else
        python_found=0
        show "${RED}Did not find Python${CRESET}, will build ${YELLOW}Python 3.9.0${CRESET} from source instead"
        make_python
    fi

    ## Checking for Ninja.

    if [ $force_build_ninja -ne 0 ]; then show "NOTE: --force-build-ninja is set"; fi
    find_program "Ninja" "/ninja$" "ninja"
    if [ ! -z "$ninja" ] && [ $force_build_ninja -eq 0 ]
    then
        ninja_found=1
        show "Found ${GREEN}Ninja$CRESET\n"
    else
        ninja_found=0
        show "${RED}Did not find Ninja$CRESET, will build ${YELLOW}Ninja 1.10.2$CRESET from source instead"
        make_ninja
    fi

    ## Checking for meson.

    if [ $force_build_meson -ne 0 ]; then show "NOTE: --force-build-meson is set"; fi
    find_versioned_ge "meson" "0.60" "/meson$" "--version" ".*" "meson"
    if [ ! -z "$meson" ] && [ $force_build_meson -eq 0 ]
    then
        meson_found=1
        show "Found ${GREEN}Meson $meson_version$CRESET\n"
    else
        meson_found=0
        show "${RED}Did not find Meson$CRESET, will use ${YELLOW}Meson 0.60.0$CRESET from source instead"
        make_meson
    fi

    ## Checking for compiler.

    # Matches g++, g++-1, ..., g++-10, ...
    gcc_regex="g++\\(-[0-9]\\+\\)\\?"
    # Matches clang++, clang++-1, ..., clang++-10, ...
    clang_regex="clang++\\(-[0-9]\\+\\)\\?"

    if [ $force_build_compiler -ne 0 ]; then show "NOTE: --force-build-compiler is set"; fi
    find_versioned_ge "g++"     "11" "/$gcc_regex\$"   "-dumpfullversion" ".*"                               "gcc"
    find_versioned_ge "clang++" "13" "/$clang_regex\$" "--version"        "[0-9]\\+\\.\\([0-9]\\+\\.\\?\\)*" "clang"

    if [ ! -z "$clang" ] && [ $force_build_compiler -eq 0 ]
    then
        CC="$clang"
        CXX="$clang"
        show "Found ${GREEN}Clang $clang_version$CRESET in $clang"
        show "\tWill use ${YELLOW}CC=$clang$CRESET"
        show "\tWill use ${YELLOW}CXX=$clang$CRESET\n"
    elif [ ! -z "$gcc" ] && [ $force_build_compiler -eq 0 ]
    then
        CC="$gcc"
        CXX="$gcc"
        show "Found ${GREEN}GCC $gcc_version$CRESET in $gcc"
        show "\tWill use ${YELLOW}CC=$gcc$CRESET"
        show "\tWill use ${YELLOW}CXX=$gcc$CRESET\n"
    else
        show "${RED}No suitable compilers found$CRESET, will build ${YELLOW}Clang 13.0.0$CRESET and use as ${YELLOW}CC$CRESET and ${YELLOW}CXX$CRESET"
        make_clang
    fi
}

assure_dir() { if [ ! -d "$1" ]; then mkdir "$1"; fi }
clean()      { if [ -d "$1" ]; then rm -rf "$1"; fi }
show()       { printf '%b\n' "$1"; }

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
            0) show "$prefix/ $postfix\c" ;;
            1) show "$prefix- $postfix\c" ;;
            2) show "$prefix\\ $postfix\c" ;;
            3) show "$prefix| $postfix\c" ;;
        esac
        i=$(expr $i + 1)
        # change the speed of the spinner by altering the 1 below
        sleep 1
        show "$after"
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
    local fail_command=$2
    local log_out=$3
    local indent=$4
    local msg_build=$5
    local msg_fail=$6
    local msg_success=$7

    msg_build="$msg_build with $YELLOW$command$CRESET\n$indent\t-- See $logdir/$log_out for progress"

    eval $command > $logdir/$log_out 2>&1 &
    pid_waiting_spinner $! "$indent" "$msg_build" "\033[2K\033[F\033[2K\033[F"
    if [ $? -ne 0 ]
    then
        show "$indent${RED}Failed to $msg_fail$CRESET, aborting $YELLOW(See $logdir/$log_out)$CRESET"
        show "$indent\tRunning cleanup: $YELLOW$fail_command$CRESET"
        eval $fail_command
        exit 1
    fi
    show "$indent$msg_success with $YELLOW$command$CRESET\n$indent\t-- See $logdir/$log_out for output"
}

download()
{
    local name=$1
    local link=$2
    local outfile=$3
    local indent=$4

    if [ ! -f "$downloaddir/$outfile" ]
    then
        buildstep "wget -c $link -O $downloaddir/$outfile" "rm -f $downloaddir/$outfile" "${name}_download.txt" "$indent" "Downloading $name" "download $name" "$name downloaded"
    fi
}

extract_clean()
{
    local name=$1
    local what=$2
    local where=$3
    local indent=$4

    clean $where
    mkdir $where

    buildstep "tar -xvf \"$what\" -C \"$where\" --strip 1" "rm -rf \"$where\"" "${name}_extract.txt" "$indent" "Extracting $name" "extract $name" "$name extracted"
}

var_expand()
{
  if [ -z "${1-}" ] || [ $# -ne 1 ]; then
    printf 'var_expand: expected one argument\n' >&2;
    return 1;
  fi
  eval printf '%s' "\"\${$1?}\""
}

find_program()
{
    local name=$1
    local regex=$2
    local outvar=$3
    local indent=$4

    show "${indent}Searching for $YELLOW$name$CRESET"
    local maybe=$(show "$all_programs" | grep "$regex")
    set -- $maybe
    if [ $# -gt 0 ]; then eval "$outvar=$1"; fi
}

find_versioned_ge()
{
    local name=$1
    local min_ver=$2
    local program_regex=$3
    local version_command=$4
    local version_regex=$5
    local outvar=$6
    local indent=$7

    eval "$outvar=''"

    show "${indent}Seaching for $YELLOW$name >= $min_ver$CRESET"
    for p in $(show "$all_programs" | grep "$program_regex")
    do
        version=$($p $version_command 2>&1 | grep -o "$version_regex")

        if [ $(sh semver.sh "$version" "$min_ver") -ge 0 ]
        then
            show "$indent\t- $GREEN$version$CRESET => $p"
            # If is unset. Or if new is of higher version.
            if [ -z $(var_expand $outvar) ] || [ $(sh semver.sh "$version" "$(var_expand ${outvar}_version)") -gt 0 ]
            then
                eval "${outvar}_version=\"$version\""
                eval "$outvar=\"$p\""
            fi
        else show "\t- $RED$version$CRESET => $p"; fi
    done
}

pkg_config_check()
{
    local lib=$1

    eval "${lib}_found=0"

    if [ $pkg_config_found -ne 0 ]
    then
        $pkg_config --exists $1
        case $? in
            0) eval "${lib}_found=1";;
            *) eval "${lib}_found=0";;
        esac
    fi
}

make_python()
{
    show "\tChecking for ${YELLOW}libssl${CRESET}"
    pkg_config_check "libssl"
    if [ $libssl_found -ne 0 ]
    then
        # TODO: where is libssl ?
        libssl_install_dir=/usr/share/libssl
        show "Found ${GREEN}libssl${CRESET}, will use ${YELLOW}libssl_install_dir=$libssl_install_dir${CRESET}\n"
    else
        show "\t${RED}Did not find libssl${CRESET}, will build ${YELLOW}libssl 3.0.2${CRESET} from source instead"
        make_libssl
    fi

    download "Python" "https://www.python.org/ftp/python/3.9.0/Python-3.9.0.tgz" "Python-3.9.0.tgz" "\t"
    extract_clean "Python" "$downloaddir/Python-3.9.0.tgz" "$builddir/python" "\t"

    show "\tModifying python to use openssl"
    show "SSL=$libssl_install_dir"                                    >> $builddir/python/Modules/Setup
    show "_ssl _ssl.c \\"                                             >> $builddir/python/Modules/Setup
    show "	-DUSE_SSL -I\$(SSL)/include -I\$(SSL)/include/openssl \\" >> $builddir/python/Modules/Setup
    show "	-L\$(SSL)/lib64 -L\$(SSL)/lib -lssl -lcrypto"             >> $builddir/python/Modules/Setup

    case $no_cleanup in
        0) py_rm_cmd="cd $root; rm -r $builddir/python";;
        1) py_rm_cmd="cd $root";;
    esac

    mkdir $builddir/python/build
    cd $builddir/python/build
        # Commented out until i implement openssl building.
        #python_configure_command="./../configure --without-ensurepip --prefix=$python_install_dir CFLAGS=\"-I$(pwd)/../../openssl/installdir/include\" LDFLAGS=\"-L$(pwd)/../../openssl/installdir/lib64\""
        # --without-ensurepip since it causes 'make install' to fail when zlib is not installed.
        # That also means that we have no pip, but we'll make do.
        # Also don't do '--enable-optimizations' since it causes problems on systems with older toolchains (GCC ~4, etc).
        py_configure_cmd="./../configure --without-ensurepip --prefix=$installdir"

        buildstep "$py_configure_cmd" "$py_rm_cmd" "Python_conf.txt"    "\t" "Configuring Python"         "configure Python"        "Python configured"
        buildstep "make -j$(nproc)"   "$py_rm_cmd" "Python_build.txt"   "\t" "Building Python"            "build Python"            "Python built"
        buildstep "make install"      "$py_rm_cmd" "Python_install.txt" "\t" "Installing Python locally"  "install Python locally"  "Python installed locally"
    cd $root

    # Remember when I said we'll make do without pip, this is what I meant.
    # If you compile openssl from source you don't have the certs necessary to
    # do https stuff, that's why we need to install certifi (without pip)
    # and link it's certs to openssl's`.
    if [ $libssl_found -eq 0 ]; then make_certifi; fi

    python="$installdir/bin/python3"
    python_version="3.9.0"
    show "\t${GREEN}Python sucessfully installed${CRESET}\n"
}

make_libssl()
{
    # TODO: check if perl is suitable before making.
    make_perl

    clean $builddir/libssl

    libssl_install_dir=$installdir

    libssl_clone_cmd="git clone -b openssl-3.0.2 https://github.com/openssl/openssl.git $builddir/libssl"
    buildstep "$libssl_clone_cmd" "" "libssl_clone.txt" "\t\t" "Cloning libssl" "clone libssl" "libssl cloned"

    case $no_cleanup in
        0) libssl_rm_cmd="cd $root; rm -r $builddir/libssl";;
        1) libssl_rm_cmd="cd $root";;
    esac

    mkdir $builddir/libssl/build
    cd $builddir/libssl/build
        libssl_configure_cmd="./../Configure --prefix=$installdir --openssldir=$installdir"

        buildstep "$libssl_configure_cmd" "$libssl_rm_cmd" "libssl_configure.txt" "\t\t" "Configuring libssl"        "configure libssl"       "libssl configured"
        buildstep "make -j$(nproc)"       "$libssl_rm_cmd" "libssl_build.txt"     "\t\t" "Building libssl"           "build libssl"           "libssl built"
        buildstep "make install"          "$libssl_rm_cmd" "libssl_install.txt"   "\t\t" "Installing libssl locally" "install libssl locally" "libssl installed locally"
    cd $root

    show "\t\t${GREEN}libssl sucessfully installed${CRESET}, will use ${YELLOW}libssl_install_dir=$libssl_install_dir${CRESET}\n"
}

make_perl()
{
    download "Perl" "https://cpan.metacpan.org/authors/id/S/SH/SHAY/perl-5.34.1.tar.gz" "perl-5.34.1.tar.gz" "\t\t\t"
    extract_clean "Perl" "$downloaddir/perl-5.34.1.tar.gz" "$builddir/Perl" "\t\t\t"

    case $no_cleanup in
        0) perl_rm_cmd="cd $root; rm -rf $builddir/Perl";;
        1) perl_rm_cmd="cd $root";;
    esac

    perl_conf_cmd="sh Configure -d -e -Dextras=\"strict\" -Dinstallprefix=$installdir/perl -Dprefix=$installdir/perl"

    cd $builddir/Perl
        buildstep "$perl_conf_cmd"  "$perl_rm_cmd" "Perl_configure.txt" "\t\t\t" "Configuring Perl"        "configure Perl"       "Perl configured"
        buildstep "make -j$(nproc)" "$perl_rm_cmd" "Perl_build.txt"     "\t\t\t" "Building Perl"           "build Perl"           "Perl built"
        #buildstep "make test"       "$perl_rm_cmd" "Perl_test.txt"      "\t\t\t" "Testing Perl"            "test Perl"            "Perl tested"
        buildstep "make install"    "$perl_rm_cmd" "Perl_install.txt"   "\t\t\t" "Installing Perl locally" "install Perl locally" "Perl installed locally"
    cd $root
}

make_certifi()
{
    download "Python-certifi" "https://files.pythonhosted.org/packages/6c/ae/d26450834f0acc9e3d1f74508da6df1551ceab6c2ce0766a593362d6d57f/certifi-2021.10.8.tar.gz" "certifi-2021.10.8.tar.gz" "\t\t"
    extract_clean "Python-certifi" "$downloaddir/certifi-2021.10.8.tar.gz" "$builddir/certifi" "\t\t"

    cd $builddir/certifi
        # Will install using the highest priority python3 from path, which is ours (since we overridden $PATH).
        buildstep "python3 setup.py install" "cd $root; rm -r $builddir/certifi" "Python-Certifi_install.txt" "\t\t" "Installing Python-Certifi" "install Python-Certifi" "Python-Certifi installed"
    cd $root

    # link certifi's certs to $installdir, when you build openssl you have no certs.
    ln -sf $(python3 -m certifi) $installdir/cert.pem

    show "\t\t${GREEN}Python-Certifi and certs sucessfully installed${CRESET}\n"
}

make_ninja()
{
    clean "$builddir/ninja"

    ninja_clone_cmd="git clone -b v1.10.2 --depth 1 https://github.com/ninja-build/ninja.git $builddir/ninja"
    buildstep "$ninja_clone_cmd" "" "Ninja_clone.txt" "\t" "Cloning Ninja" "clone Ninja" "Ninja cloned"

    case $no_cleanup in
        0) ninja_rm_cmd="cd $root; rm -r $builddir/ninja";;
        1) ninja_rm_cmd="cd $root";;
    esac

    cd $builddir/ninja
        buildstep "python3 configure.py --bootstrap" "$ninja_rm_cmd" "Ninja_bootstrap.txt" "\t" "Bootstrapping Ninja" "bootstrap Ninja"  "Ninja bootstrapped"
    cd $root

    assure_dir $installdir/bin
    ln -sf $builddir/ninja/ninja $installdir/bin/ninja
    show "\t${GREEN}Ninja sucessfully installed${CRESET}\n"
}

make_meson()
{
    clean "$builddir/meson"

    meson_clone_cmd="git clone -b 0.60.0 --depth 1 https://github.com/mesonbuild/meson.git $builddir/meson"
    buildstep "$meson_clone_cmd" "" "Meson_clone.txt" "\t" "Cloning Meson" "clone Meson" "Meson cloned"

    assure_dir $installdir/bin
    ln -sf $builddir/meson/meson.py $installdir/bin/meson
    show "\t${GREEN}Meson sucessfully installed${CRESET}\n"
}

make_clang()
{
    if [ $force_build_cmake -ne 0 ]; then show "\tNOTE: --force-build-cmake is set"; fi
    find_versioned_ge "cmake" "3.13.3" "/cmake$" "--version" "\\([0-9]\\+\\.\\?\\)\\+$" "cmake" "\t"

    if [ ! -z "$cmake" ] && [ $force_build_cmake -eq 0 ]
    then
        show "\tFound ${GREEN}CMake $cmake_version$CRESET\n"
    else
        show "\t${RED}Did not find CMake$CRESET, will build ${YELLOW}CMake 3.23.0$CRESET from source instead"
        make_cmake
    fi

    download "Clang" "https://github.com/llvm/llvm-project/releases/download/llvmorg-13.0.0/llvm-project-13.0.0.src.tar.xz" "llvm-project-13.0.0.src.tar.xz" "\t"
    extract_clean "Clang" "$downloaddir/llvm-project-13.0.0.src.tar.xz" "$builddir/llvm-project" "\t"

    case $no_cleanup in
        0) clang_rm_cmd="cd $root; rm -r $builddir/llvm-project";;
        1) clang_rm_cmd="cd $root";;
    esac

    mkdir $builddir/llvm-project/build
    cd $builddir/llvm-project/build
        clang_conf_cmd="cmake -DLLVM_FORCE_USE_OLD_HOST_TOOLCHAIN=1 -DLLVM_ENABLE_PROJECTS=\"clang;openmp\" -GNinja -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=$installdir ../llvm"

        buildstep "$clang_conf_cmd" "$clang_rm_cmd" "Clang_configure.txt" "\t" "Configuring Clang"        "configure Clang"       "Clang configured"
        buildstep "ninja"           "$clang_rm_cmd" "Clang_build.txt"     "\t" "Building Clang"           "build Clang"           "Clang built"
        buildstep "ninja install"   "$clang_rm_cmd" "Clang_install.txt"   "\t" "Installing Clang locally" "install Clang locally" "Clang installed locally"
    cd $root

    CC="$installdir/bin/clang"
    CXX="$installdir/bin/clang++"
    show "\t${GREEN}Clang sucessfully installed${CRESET}"
    show "\t\tWill use ${YELLOW}CC=$CC${CRESET}"
    show "\t\tWill use ${YELLOW}CXX=$CXX${CRESET}\n"
}

make_cmake()
{
    download "CMake" "https://github.com/Kitware/CMake/releases/download/v3.23.0/cmake-3.23.0.tar.gz" "cmake-3.23.0.tar.gz" "\t\t"
    extract_clean "CMake" "$downloaddir/cmake-3.23.0.tar.gz" "$builddir/cmake" "\t\t"

    case $no_cleanup in
        0) cmake_rm_cmd="cd $root; rm -r $builddir/cmake";;
        1) cmake_rm_cmd="cd $root";;
    esac

    mkdir $builddir/cmake/build
    cd $builddir/cmake/build
        cmake_bootstrap_cmd="./../bootstrap -- -DCMAKE_USE_OPENSSL=OFF -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=$installdir"
        cmake_build_cmd="make -j$(nproc)"
        cmake_install_cmd="make install"

        buildstep "$cmake_bootstrap_cmd" "$cmake_rm_cmd" "CMake_bootstrap.txt" "\t\t" "Bootstrapping CMake"      "bootstrap CMake"       "CMake bootstrapped"
        buildstep "$cmake_build_cmd"     "$cmake_rm_cmd" "CMake_build.txt"     "\t\t" "Building CMake"           "build CMake"           "CMake built"
        buildstep "$cmake_install_cmd"   "$cmake_rm_cmd" "CMake_install.txt"   "\t\t" "Installing CMake locally" "install CMake locally" "CMake installed locally"
    cd $root

    show "\t\t${GREEN}CMake sucessfully installed${CRESET}\n"
}

# Call the script

all_programs=$(sh programs.sh)

CRESET='\033[0m' # Text Reset
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'

root=$(pwd)

downloaddir=$root/download
assure_dir $downloaddir

logdir=$root/logs
assure_dir $logdir

builddir=$root/build
assure_dir $builddir

installdir=$root/local
clean $installdir
assure_dir $installdir
export PATH=$installdir/bin:$installdir/perl/bin:$PATH
export PKG_CONFIG_PATH=$installdir/lib/pkgconfig:$installdir/lib64/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=$installdir/lib:$installdir/lib64:$LD_LIBRARY_PATH
export LIBRARY_PATH=$installdir/lib:$installdir/lib64:$LIBRARY_PATH
export PERL5LIB=$installdir/perl/lib:$PERL5LIB

main

## Then build the final file to be sourced.

show "Writing $YELLOW$toolchain_file$CRESET"

show "#!/usr/bin/env sh" > $toolchain_file
show "export PATH=$installdir/bin:$installdir/perl/bin:\$PATH" >> $toolchain_file
show "export PKG_CONFIG_PATH=$installdir/lib/pkgconfig:$installdir/lib64/pkgconfig:\$PKG_CONFIG_PATH" >> $toolchain_file
show "export LD_LIBRARY_PATH=$installdir/lib:$installdir/lib64:\$LD_LIBRARY_PATH" >> $toolchain_file
show "export LIBRARY_PATH=$installdir/lib:$installdir/lib64:\$LIBRARY_PATH" >> $toolchain_file
show "export PERL5LIB=$installdir/perl/lib:\$PERL5LIB" >> $toolchain_file
show "export CC=$CC"   >> $toolchain_file
show "export CXX=$CXX" >> $toolchain_file

show "Tooling setup ${GREEN}done${CRESET} :)\n"
