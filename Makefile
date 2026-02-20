# --- Zmienne ---
CXX = g++
ASM = nasm
LD = ld

CXXFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
ASMFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

# --- Cele (Targets) ---

# Domyślny cel, odpala się po wpisaniu samego 'make'
all: myos.iso

# Kompilacja C++
kernel.o: kernel.cpp
	$(CXX) $(CXXFLAGS) -c kernel.cpp -o kernel.o

# Kompilacja Asemblera
boot.o: boot.asm
	$(ASM) $(ASMFLAGS) boot.asm -o boot.o

# Łączenie (Linkowanie)
myos.bin: boot.o kernel.o linker.ld
	$(LD) $(LDFLAGS) boot.o kernel.o -o myos.bin

# Tworzenie pliku ISO
myos.iso: myos.bin grub.cfg
	mkdir -p isodir/boot/grub
	cp myos.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o myos.iso isodir

# Odpalanie w QEMU
run: myos.iso
	qemu-system-i386 -cdrom myos.iso

# Sprzątanie folderu z plików tymczasowych
clean:
	rm -rf *.o myos.bin myos.iso isodir