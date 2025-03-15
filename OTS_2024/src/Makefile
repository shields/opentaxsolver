#============================
# Makefile for OpenTaxSolver
#============================

# Compiler options
CC      =  cc
CFLAGS  =
COPTIM  = -O -Wall 
LDFLAGS = -lm

# For MSYS1.0, define "CFLAGS = -std=c99"
# in your build env.

# Note that under Windows, in an MSYS2 shell, the ".exe" suffix is implied for
# the EXE_* file list macro values. This maintains compatiblity with *nix
# platforms, which don't use the ".exe" suffix for programs.  However, in
# MSYS1, this handling is not automatic; currently, MSYS1 is being used to
# build standalone Windows versions of OTS, since MSYS 2.0 produces exe files
# which fault at startup. TODO: Resolve the MSYS2 build issue, and provide
# instructions for setting up a Windows MINGW build environment.

# EFS = Executable File Suffix; not-defined (empty) for Linux. Includes the "."
# separator.  To detect whether a ".exe" suffix should be added to executable
# file names, The "MSYSTEM" environment variable is detected. If detected,
# regardless of value, a ".exe" file extension is used.

ifneq ($(strip $(MSYSTEM)),)
    EFS := .exe
endif

#====================
#====== Macros ======
#====================

#====== Taxsolver programs; in ../bin/ ======
# Add new taxsolver modules to this list
TAXSOLVE := \
    taxsolve_US_1040_2024 \
    taxsolve_US_1040_Sched_C_2024 \
    taxsolve_PA_40_2024 \
    taxsolve_NJ_1040_2024 \
    taxsolve_VA_760_2024 \
    taxsolve_NC_D400_2024 \
    taxsolve_OH_IT1040_2024 \
    taxsolve_NY_IT201_2024 \
    taxsolve_MA_1_2024 \
    taxsolve_CA_540_2024 \
    taxsolve_HSA_f8889 \
    taxsolve_f8606 \
    taxsolve_US_1040_Sched_SE_2024 \
    taxsolve_f8812_2024 \
    taxsolve_f8829_2024 \
    taxsolve_f8995_2024 \
    taxsolve_f8959_2024 \
    taxsolve_f8960_2024 \
    taxsolve_f2210_2024 \
    taxsolve_CA_5805_2024 \
    taxsolve_AZ_140_2024 \
    taxsolve_MI_1040_2024 \
    taxsolve_OR_40_2024

# List of taxsolver programs in ../bin/
EXE_TAXSOLVE := $(patsubst %,../bin/%$(EFS), $(TAXSOLVE))

#====== IO programs; in ../bin/ ======
IO := \
     universal_pdf_file_modifier \
     convert_results2xfdf

# List of ior programs in ../bin/
EXE_IO := $(patsubst %,../bin/%$(EFS), $(IO))

#====== Top level programs ; in ../ ======
TOP := \
     Run_taxsolve_GUI 

# List of top level programs
EXE_TOP := $(patsubst %,../%$(EFS), $(TOP))


#===================
#====== Rules ======
#===================

all: $(EXE_TAXSOLVE) $(EXE_IO) $(EXE_TOP)

# Pattern rule for taxsolver programs in ../bin/
# includes dependency on taxsolve_routines.c
$(EXE_TAXSOLVE): ../bin/%$(EFS): %.c taxsolve_routines.c
	$(CC) $(CFLAGS) $(COPTIM) -o $@ $< $(LDFLAGS)

# Pattern rule for io programs in ../bin/
$(EXE_IO): ../bin/%$(EFS): %.c
	$(CC) $(CFLAGS) $(COPTIM) -o $@ $< $(LDFLAGS)

# Pattern rule for top level programs in ../
$(EXE_TOP): ../%$(EFS): %.c
	$(CC) $(CFLAGS) $(COPTIM) -o $@ $< $(LDFLAGS)


clean:
	rm -fv $(EXE_TAXSOLVE) $(EXE_IO) $(EXE_TOP)

#
#
#
#
