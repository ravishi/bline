blinedir = $(datadir)/bline
bin_PROGRAMS = bline
#bline_HEADERS = bline-filter.h\
#				bline-image.h\
#				bline-simple-filter.h\
#				bline-filter-simple.h
bline_SOURCES = main.c\
				#bline-image.c\
				#bline-filter.c\
				#bline-simple-filter.c\
				#bline-filter-simple.c
dist_bline_DATA = bline.glade bline_icon.png

bline_CFLAGS = @GLIB_CFLAGS@ @GEGL_CFLAGS@
bline_LDADD = @GLIB_LIBS@ @GEGL_LIBS@

CLEANFILES = *~
