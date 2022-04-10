#!/usr/bin/env sh

# TODO: make downloads/clones retry X times.
# TODO: implement skipping and resuming for libssl and zlib

resume_last_call=0 # Uses $resume_file to figure out what is already built.
install_cleanup=0 # Deletes $installdir before stating.
no_build_cleanup=0 # Doesn't cleanup build artifacts in $builddir, incompatible with --resume-last-call.
force_reconfigure=0 # Ignore $toolchain_file, forces --ignore-last-call=0.
force_no_pkg_config=0 # Don't use pkg-config, implies libssl and zlib will be built from source if needed

ignore_system_python=0
ignore_system_perl=0
ignore_system_ninja=0
ignore_system_meson=0
ignore_system_compiler=0
ignore_system_cmake=0
ignore_system_all=0

skip_python=0
skip_perl=0
skip_ninja=0
skip_meson=0
skip_compiler=0
skip_cmake=0

# Set by $resume_file
already_built_python=0
already_built_perl=0
already_built_ninja=0
already_built_meson=0
already_built_compiler=0
already_built_cmake=0

while [ $# -gt 0 ]; do
    case $1 in
        --resume-last-call) resume_last_call=1;;
        --install-cleanup) install_cleanup=1;;
        --no-build-cleanup) no_build_cleanup=1;;
        --force-reconfigure) force_reconfigure=1;;
        --force-no-pkg-config) force_no_pkg_config=1;;

        --ignore-system-python) ignore_system_python=1;;
        --ignore-system-perl) ignore_system_perl=1;;
        --ignore-system-ninja) ignore_system_ninja=1;;
        --ignore-system-meson) ignore_system_meson=1;;
        --ignore-system-compiler) ignore_system_compiler=1;;
        --ignore-system-cmake) ignore_system_cmake=1;;
        --ignore-system-all)
            force_no_pkg_config=1
            ignore_system_python=1
            ignore_system_perl=1
            ignore_system_ninja=1
            ignore_system_meson=1
            ignore_system_compiler=1
            ignore_system_cmake=1
        ;;

        --skip-python) skip_python=1;;
        --skip-perl) skip_perl=1;;
        --skip-ninja) skip_ninja=1;;
        --skip-meson) skip_meson=1;;
        --skip-compiler) skip_compiler=1;;
        --skip-cmake) skip_cmake=1;;
    esac

    shift
done

toolchain_file="$(pwd)/toolchain.sourceme.sh"
resume_file="$(pwd)/resume.internal.sourceme.sh"

if [ $force_reconfigure -eq 0 ]
then
    if [ -f "$toolchain_file" ]; then exit 0; fi
else
    resume_last_call=0
fi

do_dependency()
{
    local displayname=$1
    local name=$2
    local build_version=$3
    local indent=$4

    local make_func_name="make_$name"
    local skip_var="skip_$name"
    local ignore_system_var="ignore_system_$name"
    local already_built_var="already_built_$name"

    local executable=$(var_expand "$name")
    if [ ! -z "$executable" ]
    then 
        local executable_version=$(var_expand "${name}_version")
    fi

    if [ $(var_expand "$already_built_var") -eq 1 ]
    then
        show "$indent$YELLOW$displayname skipped (already built from last time)$CRESET\n"
        show "export $already_built_var=1" >> $resume_file
    elif [ $(var_expand "$skip_var") -eq 1 ]
    then
        show "$indent$YELLOW$displayname skipped$CRESET\n"
        show "export $skip_var=1" >> $resume_file
    elif [ ! -z "$executable" ] && [ $(var_expand "$ignore_system_var") -eq 0 ]
    then
        show "${indent}Found $GREEN$displayname $executable_version$CRESET\n"
    else
        show "$indent${RED}Did not find $displayname$CRESET, will make $YELLOW$displayname $build_version$CRESET from source instead"
        eval "$make_func_name"
        show "export $already_built_var=1" >> $resume_file
    fi
}

main()
{
    ## Checking for pkg-config (to find openssl if we must build python)

    if [ $force_no_pkg_config -ne 0 ]; then show "NOTE: --force-no-pkg-config is set"; fi
    find_program "pkg-config" "/pkg-config$" "pkg_config"

    if [ ! -z "$pkg_config" ] && [ $force_no_pkg_config -eq 0 ]
    then
        pkg_config_found=1
        show "Found ${GREEN}pkg-config$CRESET, will use ${YELLOW}pkg_config=$pkg_config$CRESET\n"
    else
        pkg_config_found=0
        show "${RED}Did not find pkg-config$CRESET, if we need to build ${YELLOW}python3$CRESET we'll need to build ${YELLOW}openssl$CRESET and ${YELLOW}zlib$CRESET too\n"
    fi

    ## Checking for python

    find_versioned_ge "Python" "3.9" "/python\\([0-9]\\+\\.\\?\\)*\$" "extract_python_version" "python"
    do_dependency "Python" "python" "3.9"

    ## Checking for Ninja.

    find_program "Ninja" "/ninja$" "ninja"
    do_dependency "Ninja" "ninja" "1.10.2"

    ## Checking for meson.

    find_versioned_ge "Meson" "0.60" "/meson$" "extract_meson_version" "meson"
    do_dependency "Meson" "meson" "0.60.0"

    ## Checking for compiler.

    # Matches g++, g++-1, ..., g++-10, ...
    gcc_regex="g++\\(-[0-9]\\+\\)\\?"
    # Matches clang++, clang++-1, ..., clang++-10, ...
    clang_regex="clang++\\(-[0-9]\\+\\)\\?"

    find_versioned_ge "clang++" "13" "/$clang_regex\$" "extract_clang_version" "clang"
    find_versioned_ge "g++"     "11" "/$gcc_regex\$"   "extract_gcc_version"   "gcc"

    # TODO: not really necessary since its the last thing to be built but maybe implement already_built_compiler ?
    if [ $skip_compiler -eq 1 ]
    then
        show "${YELLOW}Compiler skipped$CRESET\n"
        show "export skip_compiler=1" >> $resume_file
    elif [ ! -z "$clang" ] && [ $ignore_system_compiler -eq 0 ]
    then
        CC="$clang"
        CXX="$clang"
        show "Found ${GREEN}Clang $clang_version$CRESET in $clang"
        show "\tWill use ${YELLOW}CC=$clang$CRESET"
        show "\tWill use ${YELLOW}CXX=$clang$CRESET\n"
    elif [ ! -z "$gcc" ] && [ $ignore_system_compiler -eq 0 ]
    then
        CC="$gcc"
        CXX="$gcc"
        show "Found ${GREEN}GCC $gcc_version$CRESET in $gcc"
        show "\tWill use ${YELLOW}CC=$gcc$CRESET"
        show "\tWill use ${YELLOW}CXX=$gcc$CRESET\n"
    else
        show "${RED}No suitable compilers found$CRESET, will build ${YELLOW}Clang 13.0.0$CRESET and use as ${YELLOW}CC$CRESET and ${YELLOW}CXX$CRESET"
        make_compiler
    fi
}

assure_dir() { if [ ! -d "$1" ]; then mkdir "$1"; fi }
clean()      { if [ -d "$1" ]; then rm -rf "$1"; fi }
show()       { printf '%b\n' "$1"; }

show_then_run()
{
    local cmd="$1"
    local indent="$2"
    show "${indent}Running command $YELLOW$1$CRESET";
    eval "$1"
}

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
        show "$indent${RED}Aborting: Failed to $msg_fail$CRESET with $YELLOW$command$CRESET"
        show "$indent\t-- See $logdir/$log_out for status"
        if [ -z "$fail_command" ]; then show_then_run "$fail_command" "$indent\t"; fi
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
{ eval printf '%s' "\"\${$1}\""; }

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
    local extract_version_command=$4
    local outvar=$5
    local indent=$6

    eval "$outvar=''"

    show "${indent}Seaching for $YELLOW$name >= $min_ver$CRESET"
    for p in $(show "$all_programs" | grep "$program_regex")
    do
        version="$($extract_version_command $p)"

        if [ $(sh semver.sh "$version" "$min_ver") -ge 0 ]
        then
            show "$indent\t- $GREEN$version$CRESET => $p"
            # If is unset. Or if new is of higher version.
            if [ -z $(var_expand $outvar) ] || [ $(sh semver.sh "$version" "$(var_expand ${outvar}_version)") -gt 0 ]
            then
                eval "${outvar}_version=\"$version\""
                eval "$outvar=\"$p\""
            fi
        else show "$indent\t- $RED$version$CRESET => $p"; fi
    done
}

extract_python_version()
{ $1 --version 2>&1 | grep -o "[0-9].*\$"; }

extract_perl_version()
{ $1 --version | grep -o "v\\([0-9]\\+\\.\\?\\)\\+"; }

extract_cmake_version()
{ $1 --version | grep -o "\\([0-9]\\+\\.\\?\\)\\+$"; }

extract_meson_version()
{ $1 --version | grep -o ".*"; }

extract_clang_version()
{ $1 --version | grep -o "[0-9]\\+\\.\\([0-9]\\+\\.\\?\\)*"; }

extract_gcc_version()
{
    $1 -dumpfullversion &> /dev/null

    if [ $? -ne 0 ]; then $1 -dumpversion
    else                  $1 -dumpfullversion
    fi
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
    show "\tChecking for ${YELLOW}zlib$CRESET"
    pkg_config_check "zlib"
    if [ $zlib_found -ne 0 ]
    then
        zlib_flags="$($pkg_config zlib --cflags) $($pkg_config zlib --libs)"
        show "\tFound ${GREEN}zlib$CRESET, will use ${YELLOW}zlib_flags=$zlib_flags$CRESET\n"
    else
        show "\t${RED}Did not find zlib$CRESET, will build ${YELLOW}zlib 1.2.12$CRESET from source instead"
        make_zlib
    fi

    show "\tChecking for ${YELLOW}libssl$CRESET"
    pkg_config_check "libssl"
    if [ $libssl_found -ne 0 ]
    then
        libssl_flags="$($pkg_config libssl --cflags) $($pkg_config libssl --libs)"
        show "\tFound ${GREEN}libssl$CRESET, will use ${YELLOW}libssl_flags=$libssl_flags$CRESET\n"
    else
        show "\t${RED}Did not find libssl$CRESET, will build ${YELLOW}libssl 3.0.2$CRESET from source instead"
        make_libssl
    fi

    download "Python" "https://www.python.org/ftp/python/3.9.0/Python-3.9.0.tgz" "Python-3.9.0.tgz" "\t"
    extract_clean "Python" "$downloaddir/Python-3.9.0.tgz" "$builddir/python" "\t"

    # Meson requires the ssl module.
    show "\tModifying Python to build the ssl module"
        show_then_run "show \"_ssl _ssl.c -DUSE_SSL $libssl_flags\" >> $builddir/python/Modules/Setup" "\t\t"
    # Meson requires the zlib module.
    show "\tModifying Python to build the zlib module"
        show_then_run "show \"zlib zlibmodule.c $zlib_flags\" >> $builddir/python/Modules/Setup" "\t\t"

    case $no_build_cleanup in
        0) py_rm_cmd="cd $root; rm -r $builddir/python";;
        1) py_rm_cmd="cd $root";;
    esac

    mkdir $builddir/python/build
    cd $builddir/python/build
        # --without-ensurepip since it causes 'make install' to fail when zlib is not installed.
        # That also means that we have no pip, but we'll make do.
        # Also don't do '--enable-optimizations' since it causes problems on systems with older toolchains (GCC ~4, etc).
        py_configure_cmd="./../configure --without-ensurepip --prefix=$installdir"

        buildstep "$py_configure_cmd" "$py_rm_cmd" "Python_conf.txt"    "\t" "Configuring Python" "configure Python" "Python configured"
        buildstep "make -j$(nproc)"   "$py_rm_cmd" "Python_build.txt"   "\t" "Building Python"    "build Python"     "Python built"
        buildstep "make install"      "$py_rm_cmd" "Python_install.txt" "\t" "Installing Python"  "install Python"   "Python installed"
    cd $root

    # Remember when I said we'll make do without pip, this is what I meant.
    # If you compile openssl from source you don't have the certs necessary to
    # do https stuff, that's why we need to install certifi (without pip)
    # and link it's certs to openssl's`.
    if [ $libssl_found -eq 0 ]
    then
        show "\n\t\tWe built ${YELLOW}libssl$CRESET from source so we have no certs, will build ${YELLOW}certifi$CRESET and source their certs instead"
        make_certifi
    fi

    show "\t${GREEN}Python sucessfully installed${CRESET}\n"
}

make_libssl()
{
    find_versioned_ge "Perl" "5.32.0" "/perl\\([0-9]\\+\\.\\?\\)*\$" "extract_perl_version" "perl" "\t\t"
    do_dependency "Perl" "perl" "5.34.1" "\t\t"

    clean $builddir/libssl

    libssl_clone_cmd="git clone -b openssl-3.0.2 --depth 1 https://github.com/openssl/openssl.git $builddir/libssl"
    buildstep "$libssl_clone_cmd" "" "libssl_clone.txt" "\t\t" "Cloning libssl" "clone libssl" "libssl cloned"

    case $no_build_cleanup in
        0) libssl_rm_cmd="cd $root; rm -r $builddir/libssl";;
        1) libssl_rm_cmd="cd $root";;
    esac

    mkdir $builddir/libssl/build
    cd $builddir/libssl/build
        libssl_configure_cmd="./../Configure --prefix=$installdir --openssldir=$installdir"

        buildstep "$libssl_configure_cmd" "$libssl_rm_cmd" "libssl_configure.txt" "\t\t" "Configuring libssl" "configure libssl" "libssl configured"
        buildstep "make -j$(nproc)"       "$libssl_rm_cmd" "libssl_build.txt"     "\t\t" "Building libssl"    "build libssl"     "libssl built"
        buildstep "make install"          "$libssl_rm_cmd" "libssl_install.txt"   "\t\t" "Installing libssl"  "install libssl"   "libssl installed"
    cd $root

    libssl_flags="-I$installdir/include -I$installdir/include/openssl -L$installdir/lib64 -L$installdir/lib -lssl -lcrypto"

    show "\t\t${GREEN}libssl sucessfully installed$CRESET, will use ${YELLOW}libssl_flags=$libssl_flags$CRESET\n"
}

make_perl()
{
    download "Perl" "https://cpan.metacpan.org/authors/id/S/SH/SHAY/perl-5.34.1.tar.gz" "perl-5.34.1.tar.gz" "\t\t\t"
    extract_clean "Perl" "$downloaddir/perl-5.34.1.tar.gz" "$builddir/Perl" "\t\t\t"

    case $no_build_cleanup in
        0) perl_rm_cmd="cd $root; rm -rf $builddir/Perl";;
        1) perl_rm_cmd="cd $root";;
    esac

    perl_conf_cmd="sh Configure -d -e -Dextras=\"strict\" -Dinstallprefix=$installdir/perl -Dprefix=$installdir/perl"

    cd $builddir/Perl
        buildstep "$perl_conf_cmd"  "$perl_rm_cmd" "Perl_configure.txt" "\t\t\t" "Configuring Perl" "configure Perl" "Perl configured"
        buildstep "make -j$(nproc)" "$perl_rm_cmd" "Perl_build.txt"     "\t\t\t" "Building Perl"    "build Perl"     "Perl built"
        buildstep "make install"    "$perl_rm_cmd" "Perl_install.txt"   "\t\t\t" "Installing Perl"  "install Perl"   "Perl installed"
    cd $root

    show "\t\t\t${GREEN}Perl sucessfully installed$CRESET\n"
}

make_zlib()
{
    download "zlib" "https://www.zlib.net/zlib-1.2.12.tar.gz" "zlib-1.2.12.tar.gz" "\t\t"
    extract_clean "zlib" "$downloaddir/zlib-1.2.12.tar.gz" "$builddir/zlib" "\t\t"

    case $no_build_cleanup in
        0) zlib_rm_cmd="cd $root; rm -rf $builddir/zlib";;
        1) zlib_rm_cmd="cd $root";;
    esac

    zlib_conf_cmd="sh Configure -d -e -Dextras=\"strict\" -Dinstallprefix=$installdir/perl -Dprefix=$installdir/perl"

    cd $builddir/zlib
        buildstep "./configure --prefix=$installdir"  "$zlib_rm_cmd" "zlib_conf.txt"    "\t\t" "Configuring zlib" "configure zlib" "zlib configured"
        buildstep "make -j$(nproc)"                   "$zlib_rm_cmd" "zlib_build.txt"   "\t\t" "Building zlib"    "build zlib"     "zlib built"
        buildstep "make install"                      "$zlib_rm_cmd" "zlib_install.txt" "\t\t" "Installing zlib"  "install zlib"   "zlib installed"
    cd $root

    zlib_flags="-I$installdir/include -L$installdir/lib64 -L$installdir/lib -lz"
    show "\t\t${GREEN}zlib sucessfully installed$CRESET, will use ${YELLOW}zlib_flags=$zlib_flags$CRESET\n"
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

    case $no_build_cleanup in
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

make_compiler()
{
    if [ $ignore_system_cmake -ne 0 ]; then show "\tNOTE: --ignore-system-cmake is set"; fi
    find_versioned_ge "cmake" "3.13.3" "/cmake$" "extract_cmake_version" "cmake" "\t"
    do_dependency "CMake" "cmake" "3.23.0" "\t"

    download "Clang" "https://github.com/llvm/llvm-project/releases/download/llvmorg-13.0.0/llvm-project-13.0.0.src.tar.xz" "llvm-project-13.0.0.src.tar.xz" "\t"
    extract_clean "Clang" "$downloaddir/llvm-project-13.0.0.src.tar.xz" "$builddir/llvm-project" "\t"

    case $no_build_cleanup in
        0) clang_rm_cmd="cd $root; rm -r $builddir/llvm-project";;
        1) clang_rm_cmd="cd $root";;
    esac

    mkdir $builddir/llvm-project/build
    cd $builddir/llvm-project/build
        clang_conf_cmd="cmake -DLLVM_FORCE_USE_OLD_HOST_TOOLCHAIN=1 -DLLVM_ENABLE_PROJECTS=\"clang;openmp\" -GNinja -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=$installdir ../llvm"

        buildstep "$clang_conf_cmd" "$clang_rm_cmd" "Clang_configure.txt" "\t" "Configuring Clang"  "configure Clang" "Clang configured"
        buildstep "ninja"           "$clang_rm_cmd" "Clang_build.txt"     "\t" "Building Clang"     "build Clang"     "Clang built"
        buildstep "ninja install"   "$clang_rm_cmd" "Clang_install.txt"   "\t" "Installing Clang"   "install Clang"   "Clang installed"
    cd $root

    CC="$installdir/bin/clang"
    CXX="$installdir/bin/clang++"
    show "\t${GREEN}Clang sucessfully installed${CRESET}"
    show "\t\tWill use ${YELLOW}CC=$CC${CRESET}"
    show "\t\tWill use ${YELLOW}CXX=$CXX${CRESET}\n"

    show "export CC=$CC" >> $resume_file
    show "export CXX=$CXX" >> $resume_file
}

make_cmake()
{
    download "CMake" "https://github.com/Kitware/CMake/releases/download/v3.23.0/cmake-3.23.0.tar.gz" "cmake-3.23.0.tar.gz" "\t\t"
    extract_clean "CMake" "$downloaddir/cmake-3.23.0.tar.gz" "$builddir/cmake" "\t\t"

    case $no_build_cleanup in
        0) cmake_rm_cmd="cd $root; rm -r $builddir/cmake";;
        1) cmake_rm_cmd="cd $root";;
    esac

    mkdir $builddir/cmake/build
    cd $builddir/cmake/build
        cmake_bootstrap_cmd="./../bootstrap -- -DCMAKE_USE_OPENSSL=OFF -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=$installdir"
        cmake_build_cmd="make -j$(nproc)"
        cmake_install_cmd="make install"

        buildstep "$cmake_bootstrap_cmd" "$cmake_rm_cmd" "CMake_bootstrap.txt" "\t\t" "Bootstrapping CMake" "bootstrap CMake" "CMake bootstrapped"
        buildstep "$cmake_build_cmd"     "$cmake_rm_cmd" "CMake_build.txt"     "\t\t" "Building CMake"      "build CMake"     "CMake built"
        buildstep "$cmake_install_cmd"   "$cmake_rm_cmd" "CMake_install.txt"   "\t\t" "Installing CMake"    "install CMake"   "CMake installed"
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
if [ $install_cleanup -eq 1 ]; then clean $installdir; fi
assure_dir $installdir
export PATH=$installdir/bin:$installdir/perl/bin:$PATH
export PKG_CONFIG_PATH=$installdir/lib/pkgconfig:$installdir/lib64/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=$installdir/lib:$installdir/lib64:$LD_LIBRARY_PATH
export LIBRARY_PATH=$installdir/lib:$installdir/lib64:$LIBRARY_PATH
export PERL5LIB=$installdir/perl/lib:$PERL5LIB

if [ $resume_last_call -eq 1 ] && [ -f $resume_file ]; then . $resume_file; fi
if [ -f $resume_file ]; then rm $resume_file; fi

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
