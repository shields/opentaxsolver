#====================
#====== Macros ======
#====================

#====== OpenTaxSolver GUI programs; in ../../bin/ ======
OTS_GUI := \
	ots_gui2 \
	notify_popup

# List of OTS GUI programs in ../../bin/
EXE_OTS_GUI := $(patsubst %,../../bin/%, $(OTS_GUI))

#===================
#====== Rules ======
#===================

all: $(EXE_OTS_GUI)

# Pattern rule for GUI programs in ../../bin/
# includes dependency on any (all) gtk modules
$(EXE_OTS_GUI): ../../bin/%: %.c    gtk_utils.c gtk_utils.h  gtk_file_browser.c
	gcc -O -Wall `pkg-config --cflags gtk+-2.0` $< `pkg-config --libs gtk+-2.0`  -o $@

clean:
	rm -f $(EXE_OTS_GUI)



# Notes:
#  On some versions of Linux, it may be necessary to execute the following command
#  before compiling this make-file, so that pkg-config finds the gtk2+ libs.
#      export PKG_CONFIG_PATH=/usr/lib/pkgconfig
#  Also, be sure to install "gtk2-devel".

