SUBDIRS = gegl-operations .

blinedir = $(datadir)/bline

bin_PROGRAMS = bline

bline_SOURCES = main.c

dist_bline_DATA = bline.glade bline_icon.png

bline_CFLAGS = @GLIB_CFLAGS@ @GEGL_CFLAGS@ -DBLINE_OPERATIONS_DIR="\"$(datadir)/operations\""
bline_LDADD = @GLIB_LIBS@ @GEGL_LIBS@ 

CLEANFILES = *~
