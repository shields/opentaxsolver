OTS_GUI2.c - This directory contains source code for a new alternate version
of the Open Tax Solver (OTS) Graphical User Interface (GUI).  This version
is based on the GIMP Tool Kit (Gtk) widget library.  Gtk supports modern
platforms from a single source code program with a modern look and feel.

The environment variable "PDF_VIEWER" can be used to set which PDF-Viewer 
the the OTS-GUI will use, as well as the path to it.


OTS_GUI3.c - Upcoming version for the OTS-GUI for compilation under the
newer Gtk-3 graphics library.  Included for initial testing and development.
To compile:   
		make -f make_gtk3
