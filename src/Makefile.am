blinedir = $(datadir)/bline

bin_PROGRAMS = bline

bline_HEADERS = \
	bline-operation-sobel.h

bline_SOURCES = main.c \
	bline-operation-sobel.c

dist_bline_DATA = bline.glade bline_icon.png

bline_CFLAGS = @GLIB_CFLAGS@ @GEGL_CFLAGS@
bline_LDADD = @GLIB_LIBS@ @GEGL_LIBS@ 

CLEANFILES = *~
