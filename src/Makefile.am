## Process this file with automake to generate a Makefile.in
## To build all programas with GTK+ uncomment these lines.
#AM_CFLAGS = @GTK_CFLAGS@
#AM_LDADD = @GTK_LIBS@

blinedir = $(datadir)/bline
bin_PROGRAMS = bline
bline_HEADERS = bline-filter.h\
				bline-image.h\
				bline-simple-filter.h\
				bline-filter-simple.h
bline_SOURCES = main.c\
				bline-image.c\
				bline-filter.c\
				bline-simple-filter.c\
				bline-filter-simple.c
dist_bline_DATA = bline.glade bline_icon.png

bline_CFLAGS = -DUI_FILE='"$(blinedir)/bline.glade"' @OPENCV_CFLAGS@ @GLIB_CFLAGS@ @GTK_CFLAGS@
bline_LDADD = @GLIB_LIBS@ @GTK_LIBS@ @OPENCV_LIBS@

CLEANFILES = *~
