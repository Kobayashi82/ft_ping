#!/bin/bash

set -e

VER=2.0
PREFIX="$HOME/.inetutils"
BIN="$HOME/local/bin"
SRC="$HOME/builds"
TMP="$SRC/inetutils-$VER"

echo ""
echo "Installing ping (inetutils-2.0)..."
echo ""

mkdir -p "$SRC" "$PREFIX" "$BIN"

cd "$SRC"

if [ ! -f "inetutils-$VER.tar.xz" ]; then
	wget "https://ftp.gnu.org/gnu/inetutils/inetutils-$VER.tar.xz"
fi

rm -rf "$TMP"
tar -xf "inetutils-$VER.tar.xz"
cd "$TMP"


> /dev/null 2>&1 ./configure --prefix="$PREFIX"

make > /dev/null 2>&1

cp ping/ping "$BIN/ping"
chmod +x "$BIN/ping"

# sudo setcap cap_net_raw+ep "$BIN/ping"
# sudo setcap cap_net_raw,cap_net_admin+ep "$BIN/ping"
# sudo setcap -r "$BIN/ping"

"$BIN/ping" -V

echo ""
echo "alias ping2='$BIN/ping'" >> ~/.bashrc
echo "alias ping2='$BIN/ping'" >> ~/.zshrc
echo "alias to '$BIN/ping' created with name 'ping2'."
echo ""
echo "please, restart the terminal or reload your .bashrc or .zshrc"
echo "with source ~/.bashrc or source ~/.zshrc"
echo ""
