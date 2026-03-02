# --- NARZĘDZIA ---
CXX = g++
ASM = nasm
LD = ld

# --- FLAGI KOMPILACJI ---
# Dodano: -mno-red-zone (niezbędne w kernelu 64-bit!) oraz -fno-pic -fno-pie (naprawia błąd GLOBAL_OFFSET_TABLE)
CXXFLAGS = -m64 -ffreestanding -O1 -Wall -Wextra -fno-exceptions -fno-rtti -mno-sse -mno-sse2 -mno-mmx -mno-red-zone -fno-pic -fno-pie -Iinclude
ASMFLAGS = -f elf64

# Zmieniono na 64-bitową architekturę (elf_x86_64)
LDFLAGS = -m elf_x86_64 -T linker.ld -z noexecstack

# --- KATALOGI ---
BUILD_DIR = build
SRC_DIR = src

# --- AUTOMATYCZNE WYSZUKIWANIE PLIKÓW ---
CPP_SOURCES = $(wildcard $(SRC_DIR)/*/*.cpp)
ASM_SOURCES = $(wildcard $(SRC_DIR)/*/*.asm)

CPP_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_SOURCES))
ASM_OBJECTS = $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

OBJECTS = $(ASM_OBJECTS) $(CPP_OBJECTS)

# Nasz gotowy kernel
TARGET = $(BUILD_DIR)/myos.bin

# --- REGUŁY BUDOWANIA ---
ISO_TARGET = $(BUILD_DIR)/myos.iso
ISODIR = $(BUILD_DIR)/isodir

all: $(TARGET) $(ISO_TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(@D)
	$(ASM) $(ASMFLAGS) $< -o $@

$(ISO_TARGET): $(TARGET)
	@echo "Tworzenie obrazu ISO..."
	@mkdir -p $(ISODIR)/boot/grub
	@cp $(TARGET) $(ISODIR)/boot/myos.bin
	@cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $(ISO_TARGET) $(ISODIR)

# Zmieniono QEMU na wersję 64-bitową
run: $(ISO_TARGET)
	qemu-system-x86_64 -cdrom $(ISO_TARGET)

clean:
	rm -rf $(BUILD_DIR)/*