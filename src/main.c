#include <gegl.h>
#include <glib/gprintf.h>
#include <string.h>
#include <stdlib.h>

int
main (gint    argc,
      gchar **argv)
{
  gegl_init (&argc, &argv);

  {
    /* instantiate a graph */
    GeglNode *gegl = gegl_node_new ();

    /*
       This is the graph we're going to construct:

       .-----------.
       | display   |
       `-----------'
       |
       .-------.
       | over  |
       `-------'
       |   \
       |    \
       |     \
       |      |
       |   .------.
       |   | text |
       |   `------'
       .------------------.
       | fractal-explorer |
       `------------------'
       */

    /*< The image nodes representing operations we want to perform */
    GeglNode *display = gegl_node_create_child (gegl, "gegl:display");
    GeglNode *edge    = gegl_node_new_child (gegl, "operation", "bline:edge-detect", NULL);
    GeglNode *load    = gegl_node_new_child (gegl, "operation", "gegl:load",
                                             "path", argv[1],
                                             NULL);

    gegl_node_link_many (load, edge, display, NULL);

    gegl_node_process (display);

    getchar ();

    g_object_unref (gegl);
  }

  /* free resources globally used by GEGL */
  gegl_exit ();

  return 0;
}
