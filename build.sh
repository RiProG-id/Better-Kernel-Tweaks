#!/bin/sh
set -e
check_command() {
	if ! command -v "$1" >/dev/null 2>&1; then
		echo "Error: Command '$1' not found. Please install it to continue." >&2
		exit 1
	fi
}
REALPATH=$(dirname "$(realpath "$0")")
SOURCE_DIR="$REALPATH/source"
MODULE_DIR="$REALPATH/module"
if [ -f /system/build.prop ]; then
	OS="Android"
else
	OS=$(uname -s)
fi
ARCH=$(uname -m)
echo "Build environment: $OS/$ARCH"
echo "Project path: $REALPATH"
echo "Checking for required tools..."
check_command clang
check_command strip
check_command zip
CC=clang
CFLAGS="-O3 -flto -Wall -Wextra"
LDFLAGS="-flto"
TARGET=BKT
SRCS="main.c"
echo "Changing to source directory: $SOURCE_DIR"
cd "$SOURCE_DIR"
echo "Cleaning previous build artifacts..."
rm -f *.o "$TARGET"
echo "Compiling source files..."
$CC $CFLAGS -c $SRCS
$CC $LDFLAGS -o "$TARGET" *.o
echo "Stripping binary..."
strip "$TARGET"
echo "Cleaning up object files..."
rm -f *.o
echo "Build complete: $TARGET"
echo "Preparing module for packaging..."
VERSION=$(grep 'version=' "$MODULE_DIR/module.prop" | cut -d'=' -f2)
ZIP_NAME="$REALPATH/Better-Kernel-Tweaks-$VERSION.zip"
OUTPUT_DIR="$MODULE_DIR/system/bin"
mkdir -p "$OUTPUT_DIR"
mv "$TARGET" "$OUTPUT_DIR"
echo "Creating zip archive: $ZIP_NAME"
cd "$MODULE_DIR"
rm -f "$ZIP_NAME"
zip -r9 "$ZIP_NAME" ./*
echo "Cleaning up temporary files..."
cd "$REALPATH"
rm -rf "$MODULE_DIR/system"
echo "----------------------------------------"
echo "Module successfully created: $ZIP_NAME"
echo "----------------------------------------"
