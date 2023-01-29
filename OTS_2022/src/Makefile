#============================
# Makefile for OpenTaxSolver
#============================

# Compiler options
CC      =  cc
CFLAGS  =
COPTIM  = -O -Wall

# Note that under Windows, in an MSYS shell, the ".exe" suffix is implied
# for the EXE_* file list macro values. This maintains compatiblity with
# *nix platforms, which don't use the ".exe" suffix for programs.

#====================
#====== Macros ======
#====================

#====== Taxsolver programs; in ../bin/ ======
# Add new taxsolver modules to this list
TAXSOLVE := \
    taxsolve_US_1040_2022 \
    taxsolve_US_1040_Sched_C_2022 \
    taxsolve_PA_40_2022 \
    taxsolve_NJ_1040_2022 \
    taxsolve_VA_760_2022 \
    taxsolve_NC_D400_2022 \
    taxsolve_OH_IT1040_2022 \
    taxsolve_NY_IT201_2022 \
    taxsolve_MA_1_2022 \
    taxsolve_CA_540_2022 \
    taxsolve_HSA_f8889 \
    taxsolve_f8606 \
    taxsolve_US_1040_Sched_SE_2022 \
    taxsolve_f8829_2022 \
    taxsolve_f8995_2022 \
    taxsolve_f8959_2022 \
    taxsolve_f8960_2022 \
    taxsolve_f2210_2022 \
    taxsolve_CA_5805_2022

# List of taxsolver programs in ../bin/
EXE_TAXSOLVE := $(patsubst %,../bin/%, $(TAXSOLVE))

#====== IO programs; in ../bin/ ======
IO := \
     universal_pdf_file_modifier \
     convert_results2xfdf

# List of ior programs in ../bin/
EXE_IO := $(patsubst %,../bin/%, $(IO))

#====== Top level programs ; in ../ ======
TOP := \
     Run_taxsolve_GUI 

# List of top level programs
EXE_TOP := $(patsubst %,../%, $(TOP))


#===================
#====== Rules ======
#===================

all: $(EXE_TAXSOLVE) $(EXE_IO) $(EXE_TOP)

# Pattern rule for taxsolver programs in ../bin/
# includes dependency on taxsolve_routines.c
$(EXE_TAXSOLVE): ../bin/%: %.c taxsolve_routines.c
	$(CC) $(CFLAGS) $(COPTIM) -o $@ $< 

# Pattern rule for io programs in ../bin/
$(EXE_IO): ../bin/%: %.c
	$(CC) $(CFLAGS) $(COPTIM) -o $@ $< 

# Pattern rule for top level programs in ../
$(EXE_TOP): ../%: %.c
	$(CC) $(CFLAGS) $(COPTIM) -o $@ $< 


clean:
	rm -fv $(EXE_TAXSOLVE) $(EXE_IO) $(EXE_TOP)

