include setup.mk

# Project target name
TARGET		= SLUS_034

#TOOLS
MKPSXISO    = tools/mkpsxiso.exe
MKPSXISO_XML= iso_build/LastSurvivor.xml
BIN_FOLDER  = bin
ISO_FOLDER  = iso

#EMULATOR
EMUBIN = D:\\Games\\Emuladores\\Playstation\\ePSXe.exe
EMU_CMD = $(EMUBIN) -nogui -loadbin iso/$(TARGET).34.LastSurvivor.cue

# Searches for C, C++ and S (assembler) files in local directory
CFILES		= $(notdir $(wildcard *.c))
CPPFILES 	= $(notdir $(wildcard *.cpp))
AFILES		= $(notdir $(wildcard *.s))

#UI
UI_DIR = ui
UI =  $(notdir $(wildcard $(UI_DIR)/*.s))
UIFILES = $(addprefix build/$(UI_DIR)/,$(UI:.s=.o)) \

#ENGINE
ENGINE_DIR = engine
ENGINE =  $(notdir $(wildcard $(ENGINE_DIR)/*.cpp))
ENGINEFILES = $(addprefix build/$(ENGINE_DIR)/,$(ENGINE:.cpp=.o))

#MAIN
OFILES		= $(addprefix build/,$(CFILES:.c=.o)) \
			$(addprefix build/,$(CPPFILES:.cpp=.o)) \
			$(addprefix build/,$(AFILES:.s=.o)) 


# Project specific include and library directories
# (use -I for include dirs, -L for library dirs)
INCLUDE	 	+=	-I. -Iscenario -I$(ENGINE_DIR) -I$(UI_DIR)
LIBDIRS		+=

# Libraries to link
LIBS		= -lpsxgpu -lpsxgte -lpsxspu -lpsxetc -lpsxapi -lc

# C compiler flags
CFLAGS		= -O3 -fno-builtin -fdata-sections -ffunction-sections -Wno-narrowing -march=r3000 -mtune=r3000

# C++ compiler flags
CPPFLAGS	= $(CFLAGS) -fno-exceptions \
						-fno-rtti \
						-fno-unwind-tables \
						-fno-threadsafe-statics \
						-fno-use-cxa-atexit \
						-Wno-narrowing \
						-fconcepts-ts \
						-std=c++20
						
# Assembler flags
AFLAGS		= -msoft-float

# Linker flags
LDFLAGS		= -Ttext=0x80010000 -gc-sections \
			-T $(GCC_BASE)/mipsel-unknown-elf/lib/ldscripts/elf32elmip.x

# Toolchain programs
CC			= $(PREFIX)gcc
CXX			= $(PREFIX)g++
AS			= $(PREFIX)as
LD			= $(PREFIX)ld

all: $(OFILES) $(UIFILES) $(ENGINEFILES)
	@mkdir -p $(BIN_FOLDER)
	$(LD) $(LDFLAGS) $(LIBDIRS) $(OFILES) $(UIFILES) $(ENGINEFILES) $(LIBS)  -o bin/$(TARGET)
	elf2x -q $(BIN_FOLDER)/$(TARGET) $(BIN_FOLDER)/$(TARGET).exe
	elf2x -q $(BIN_FOLDER)/$(TARGET) $(BIN_FOLDER)/$(TARGET).34

iso: all
	@mkdir -p $(ISO_FOLDER)
	@rm -rf $(ISO_FOLDER)/*.cue $(ISO_FOLDER)/*.bin
	@$(MKPSXISO) $(MKPSXISO_XML)
	@mv *.cue $(ISO_FOLDER)
	@mv *.bin $(ISO_FOLDER)

run: iso
	$(EMU_CMD)

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	
build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(AFLAGS) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

build/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(AFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf build $(BIN_FOLDER) $(ISO_FOLDER) *.bin *.cue