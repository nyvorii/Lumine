# --- NARZĘDZIA ---
CXX = g++
ASM = nasm
LD = ld

# --- FLAGI KOMPILACJI ---
# Zwróć uwagę na -Iinclude na samym końcu – to mówi kompilatorowi, gdzie są pliki .h!
CXXFLAGS = -m32 -ffreestanding -O1 -Wall -Wextra -fno-exceptions -fno-rtti -mno-sse -mno-sse2 -mno-mmx -Iinclude
ASMFLAGS = -f elf32
# Dodane -z noexecstack, żeby wyciszyć ostrzeżenie linkera
LDFLAGS = -m elf_i386 -T linker.ld -z noexecstack

# --- KATALOGI ---
BUILD_DIR = build
SRC_DIR = src

# --- AUTOMATYCZNE WYSZUKIWANIE PLIKÓW ---
# Szuka wszystkich plików .cpp i .asm w podkatalogach folderu src/
CPP_SOURCES = $(wildcard $(SRC_DIR)/*/*.cpp)
ASM_SOURCES = $(wildcard $(SRC_DIR)/*/*.asm)

# Zamienia ścieżki plików źródłowych na ścieżki plików obiektowych (.o) w folderze build/
CPP_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_SOURCES))
ASM_OBJECTS = $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

# Wszystkie pliki .o razem (bardzo ważne: pliki bootowania powinny linkować się zazwyczaj na początku, 
# ale Twój linker.ld i tak dba o sekcję .boot, więc kolejność tutaj nie ma krytycznego znaczenia)
OBJECTS = $(ASM_OBJECTS) $(CPP_OBJECTS)

# Nasz gotowy kernel
TARGET = $(BUILD_DIR)/myos.bin

# --- REGUŁY BUDOWANIA ---

# Nowe zmienne dla ISO
ISO_TARGET = $(BUILD_DIR)/myos.iso
ISODIR = $(BUILD_DIR)/isodir

# Domyślna reguła
all: $(TARGET) $(ISO_TARGET)

# Linkowanie ostatecznego pliku myos.bin
$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

# Kompilacja plików C++
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Kompilacja plików Asemblera
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(@D)
	$(ASM) $(ASMFLAGS) $< -o $@

# Tworzenie pliku ISO z użyciem GRUBa
$(ISO_TARGET): $(TARGET)
	@echo "Tworzenie obrazu ISO..."
	@mkdir -p $(ISODIR)/boot/grub
	@cp $(TARGET) $(ISODIR)/boot/myos.bin
	@cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $(ISO_TARGET) $(ISODIR)

# Uruchamianie w QEMU jako płyta CD (odpala ISO!)
run: $(ISO_TARGET)
	qemu-system-i386 -cdrom $(ISO_TARGET)

# Sprzątanie
clean:
	rm -rf $(BUILD_DIR)/*