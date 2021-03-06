#!/bin/bash
#
# Compiles and installs a Linux/x86_64 -> Linux/ARM crosstool based on LLVM37 and
# LLVM37-GCC-4.2 using SVN snapshots in provided tarballs.

set -o nounset
set -o errexit

echo -n "Welcome to LLVM37 Linux/X86_64 -> Linux/ARM crosstool "
echo "builder/installer; some steps will require sudo privileges."

readonly INSTALL_ROOT="${INSTALL_ROOT:-/usr/local/crosstool}"
# Both $USER and root *must* have read/write access to this dir.
readonly SCRATCH_ROOT=$(mktemp -d "${TMPDIR:-/tmp}/llvm37-project.XXXXXX")
readonly SRC_ROOT="${SCRATCH_ROOT}/src"
readonly OBJ_ROOT="${SCRATCH_ROOT}/obj"

readonly CROSS_HOST="x86_64-unknown-linux-gnu"
readonly CROSS_TARGET="arm-none-linux-gnueabi"
readonly CROSS_MARCH="${CROSS_MARCH:-armv6}"

readonly CODE_SOURCERY="${INSTALL_ROOT}/codesourcery"
readonly CODE_SOURCERY_PKG_PATH="${CODE_SOURCERY_PKG_PATH:-${HOME}/codesourcery}"
readonly CODE_SOURCERY_HTTP="http://www.codesourcery.com/sgpp/lite/arm/portal/package1787/public"
readonly CODE_SOURCERY_PKG="arm-2007q3-51-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2"
readonly CODE_SOURCERY_ROOT="${CODE_SOURCERY}/arm-2007q3"
readonly CODE_SOURCERY_BIN="${CODE_SOURCERY_ROOT}/bin"
# Make sure ${CROSS_TARGET}-* binutils are in command path
export PATH="${CODE_SOURCERY_BIN}:${PATH}"

readonly CROSS_TARGET_AS="${CODE_SOURCERY_BIN}/${CROSS_TARGET}-as"
readonly CROSS_TARGET_LD="${CODE_SOURCERY_BIN}/${CROSS_TARGET}-ld"

readonly SYSROOT="${CODE_SOURCERY_ROOT}/${CROSS_TARGET}/libc"

readonly LLVM37_PKG_PATH="${LLVM37_PKG_PATH:-${HOME}/llvm37-project/snapshots}"

# Latest SVN revisions known to be working in this configuration.
readonly LLVM37_DEFAULT_REV="74530"
readonly LLVM37GCC_DEFAULT_REV="74535"

readonly LLVM37_PKG="llvm37-${LLVM37_SVN_REV:-${LLVM37_DEFAULT_REV}}.tar.bz2"
readonly LLVM37_SRC_DIR="${SRC_ROOT}/llvm37"
readonly LLVM37_OBJ_DIR="${OBJ_ROOT}/llvm37"
readonly LLVM37_INSTALL_DIR="${INSTALL_ROOT}/${CROSS_TARGET}/llvm37"

readonly LLVM37GCC_PKG="llvm37-gcc-4.2-${LLVM37GCC_SVN_REV:-${LLVM37GCC_DEFAULT_REV}}.tar.bz2"
readonly LLVM37GCC_SRC_DIR="${SRC_ROOT}/llvm37-gcc-4.2"
readonly LLVM37GCC_OBJ_DIR="${OBJ_ROOT}/llvm37-gcc-4.2"
readonly LLVM37GCC_INSTALL_DIR="${INSTALL_ROOT}/${CROSS_TARGET}/llvm37-gcc-4.2"

readonly MAKE_OPTS="${MAKE_OPTS:--j2}"

# Params:
#   $1: directory to be created
#   $2: optional mkdir command prefix, e.g. "sudo"
createDir() {
  if [[ ! -e $1 ]]; then
    ${2:-} mkdir -p $1
  elif [[ -e $1 && ! -d $1 ]]; then
    echo "$1 exists but is not a directory; exiting."
    exit 3
  fi
}

sudoCreateDir() {
  createDir $1 sudo
  sudo chown ${USER} $1
}

# Prints out and runs the command, but without logging -- intended for use with
# lightweight commands that don't have useful output to parse, e.g. mkdir, tar,
# etc.
runCommand() {
  local message="$1"
  shift
  echo "=> $message"
  echo "==> Running: $*"
  $*
}

runAndLog() {
  local message="$1"
  local log_file="$2"
  shift 2
  echo "=> $message; log in $log_file"
  echo "==> Running: $*"
  # Pop-up a terminal with the output of the current command?
  # e.g.: xterm -e /bin/bash -c "$* >| tee $log_file"
  $* &> $log_file
  if [[ $? != 0 ]]; then
    echo "Error occurred: see most recent log file for details."
    exit
  fi
}

installCodeSourcery() {
  # Unpack the tarball, creating the CodeSourcery dir, if necessary.
  if [[ ! -d ${CODE_SOURCERY_ROOT} ]]; then
    sudoCreateDir ${CODE_SOURCERY}
    cd ${CODE_SOURCERY}
    if [[ -e ${CODE_SOURCERY_PKG_PATH}/${CODE_SOURCERY_PKG} ]]; then
      runCommand "Unpacking CodeSourcery in ${CODE_SOURCERY}" \
          tar jxf ${CODE_SOURCERY_PKG_PATH}/${CODE_SOURCERY_PKG}
    else
      echo -n "CodeSourcery tarball not found in "
      echo "${CODE_SOURCERY_PKG_PATH}/${CODE_SOURCERY_PKG}"
      echo -n "Fix the path or download it from "
      echo "${CODE_SOURCERY_HTTP}/${CROSS_TARGET}/${CODE_SOURCERY_PKG}"
      exit
    fi
  else
    echo "CodeSourcery install dir already exists; skipping."
  fi

  # Verify our CodeSourcery toolchain installation.
  if [[ ! -d "${SYSROOT}" ]]; then
    echo -n "Error: CodeSourcery does not contain libc for ${CROSS_TARGET}: "
    echo "${SYSROOT} not found."
    exit
  fi

  for tool in ${CROSS_TARGET_AS} ${CROSS_TARGET_LD}; do
    if [[ ! -e $tool ]]; then
      echo "${tool} not found; exiting."
      exit
    fi
  done
}

installLLVM37() {
  if [[ -d ${LLVM37_INSTALL_DIR} ]]; then
    echo "LLVM37 install dir ${LLVM37_INSTALL_DIR} exists; skipping."
    return
  fi

  sudoCreateDir ${LLVM37_INSTALL_DIR}

  # Unpack LLVM37 tarball; should create the directory "llvm37".
  cd ${SRC_ROOT}
  runCommand "Unpacking LLVM37" tar jxf ${LLVM37_PKG_PATH}/${LLVM37_PKG}

  # Configure, build, and install LLVM37.
  createDir ${LLVM37_OBJ_DIR}
  cd ${LLVM37_OBJ_DIR}
  runAndLog "Configuring LLVM37" ${LLVM37_OBJ_DIR}/llvm-configure.log \
      ${LLVM37_SRC_DIR}/configure \
      --disable-jit \
      --enable-optimized \
      --prefix=${LLVM37_INSTALL_DIR} \
      --target=${CROSS_TARGET} \
      --with-llvm37gccdir=${LLVM37GCC_INSTALL_DIR}
  runAndLog "Building LLVM37" ${LLVM37_OBJ_DIR}/llvm-build.log \
      make ${MAKE_OPTS}
  runAndLog "Installing LLVM37" ${LLVM37_OBJ_DIR}/llvm37-install.log \
      make ${MAKE_OPTS} install
}

installLLVM37GCC() {
  if [[ -d ${LLVM37GCC_INSTALL_DIR} ]]; then
    echo "LLVM37-GCC install dir ${LLVM37GCC_INSTALL_DIR} exists; skipping."
    return
  fi

  sudoCreateDir ${LLVM37GCC_INSTALL_DIR}

  # Unpack LLVM37-GCC tarball; should create the directory "llvm37-gcc-4.2".
  cd ${SRC_ROOT}
  runCommand "Unpacking LLVM37-GCC" tar jxf ${LLVM37_PKG_PATH}/${LLVM37GCC_PKG}

  # Configure, build, and install LLVM37-GCC.
  createDir ${LLVM37GCC_OBJ_DIR}
  cd ${LLVM37GCC_OBJ_DIR}
  runAndLog "Configuring LLVM37-GCC" ${LLVM37GCC_OBJ_DIR}/llvm37gcc-configure.log \
      ${LLVM37GCC_SRC_DIR}/configure \
      --enable-languages=c,c++ \
      --enable-llvm37=${LLVM37_INSTALL_DIR} \
      --prefix=${LLVM37GCC_INSTALL_DIR} \
      --program-prefix=llvm37- \
      --target=${CROSS_TARGET} \
      --with-arch=${CROSS_MARCH} \
      --with-as=${CROSS_TARGET_AS} \
      --with-ld=${CROSS_TARGET_LD} \
      --with-sysroot=${SYSROOT}
  runAndLog "Building LLVM37-GCC" ${LLVM37GCC_OBJ_DIR}/llvm37gcc-build.log \
      make
  runAndLog "Installing LLVM37-GCC" ${LLVM37GCC_OBJ_DIR}/llvm37gcc-install.log \
      make install
}

echo "Building in ${SCRATCH_ROOT}; installing in ${INSTALL_ROOT}"

createDir ${SRC_ROOT}
createDir ${OBJ_ROOT}

installCodeSourcery
installLLVM37
installLLVM37GCC

echo "Done."
