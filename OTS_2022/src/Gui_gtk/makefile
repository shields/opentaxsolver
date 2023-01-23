all:  ../../bin/ots_gui2  ../../bin/notify_popup

# On some versions of Linux, it may be necessary to execute the following command
# before compiling this make-file, so that pkg-config finds the gtk2+ libs.
#      export PKG_CONFIG_PATH=/usr/lib/pkgconfig
# Also, be sure to install "gtk2-devel".

../../bin/ots_gui2:  ots_gui2.c  gtk_utils.c gtk_utils.h  gtk_file_browser.c
	gcc -O -Wall `pkg-config --cflags gtk+-2.0` ots_gui2.c  \
        `pkg-config --libs gtk+-2.0`  -o ../../bin/ots_gui2

../../bin/notify_popup:  notify_popup.c  gtk_utils.c gtk_utils.h
	gcc -O -Wall `pkg-config --cflags gtk+-2.0` notify_popup.c  \
        `pkg-config --libs gtk+-2.0`  -o ../../bin/notify_popup

clean:
	rm -f ../../bin/ots_gui2  ../../bin/notify_popup
