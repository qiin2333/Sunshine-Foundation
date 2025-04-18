# install dependencies for C++ analysis
set -e

# update pacman
pacman --noconfirm -Syu

# install dependencies
dependencies=(
  "git"
  "mingw-w64-ucrt-x86_64-cmake"
  "mingw-w64-ucrt-x86_64-cppwinrt"
  "mingw-w64-ucrt-x86_64-curl-winssl"
  "mingw-w64-ucrt-x86_64-MinHook"
  "mingw-w64-ucrt-x86_64-miniupnpc"
  "mingw-w64-ucrt-x86_64-nlohmann-json"
  "mingw-w64-ucrt-x86_64-nodejs"
  "mingw-w64-ucrt-x86_64-nsis"
  "mingw-w64-ucrt-x86_64-onevpl"
  "mingw-w64-ucrt-x86_64-openssl"
  "mingw-w64-ucrt-x86_64-opus"
  "mingw-w64-ucrt-x86_64-toolchain"
)
pacman -S --noconfirm "${dependencies[@]}"

# build
mkdir -p build
cd build || exit 1
cmake \
  -DBUILD_DOCS=OFF \
  -G "MinGW Makefiles" ..
mingw32-make -j"$(nproc)"

# skip autobuild
echo "skip_autobuild=true" >> "$GITHUB_OUTPUT"
