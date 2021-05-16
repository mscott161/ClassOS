#/bin/bash
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
make clean
make all
cd bin
qemu-system-i386 -hda ./os.bin -vga std