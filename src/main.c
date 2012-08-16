#include <config.h>
#include <string.h>
#include <stdlib.h>
#include <cv.h>
#include <gtk/gtk.h>
#include <highgui.h>
#include "bline-image.h"
#include "bline-filter.h"
#include "bline-simple-filter.h"

// this will be defined by the Makefile
#undef UI_FILE
#define UI_FILE     "bline.glade"
#define ICON_FILE   "bline_icon.png"

typedef struct _ChData ChData;
struct _ChData
{
  GtkWidget *main_window;
  GtkWidget *about_dialog;
  GtkWidget *content_box;
  GtkWidget *combo;
  GtkWidget *control_box;
  GtkWidget *active_filter;
  GtkWidget *save_as;
  GtkWidget *close;
};


static GtkWidget *      bline_main_get_active_image     (GtkWindow *self);


static GtkWidget *
bline_main_get_active_image (GtkWindow *self)
{
  gpointer image;

  image = g_object_get_data (G_OBJECT (self), "bline-image");

  if (image != NULL)
    return GTK_WIDGET (image);
  else
    return NULL;
}

static ChData *
bline_main_get_chdata (GtkWindow *self)
{
  return (ChData *) g_object_get_data (G_OBJECT (self), "bline-chdata");
}

static void
bline_main_set_chdata (GtkWindow *self, ChData *data)
{
  g_object_set_data (G_OBJECT (self), "bline-chdata", (gpointer) data);
}

static void
bline_clear_gtk_container_aux_cb (GtkWidget *widget,
                                  gpointer data)
{
  gtk_widget_destroy (widget);
}

static void
bline_clear_gtk_container (GtkContainer *container)
{
  gtk_container_foreach (container,
                         bline_clear_gtk_container_aux_cb,
                         NULL);
}

static void
bline_main_apply_filter (GtkWindow *self, BlineFilter *filter)
{
  GtkWidget         *image;
  const IplImage    *ipl;
  IplImage          *new_ipl;

  image = bline_main_get_active_image (self);

  g_return_if_fail (BLINE_IS_IMAGE (image));

  ipl = bline_image_get_original (BLINE_IMAGE (image));

  if (ipl != NULL)
    {
      new_ipl = bline_filter_apply (filter, ipl);
      bline_image_set_visible (BLINE_IMAGE (image), new_ipl);
    }
}

static void
bline_main_combo_box_on_changed (GtkWidget *combo,
                                 gpointer data)
{
  GtkTreeIter   iter;
  GtkTreeModel  *model;
  GObject       *object;
  BlineFilter   *filter;
  BlineImage    *image;

  object = NULL;

  image = BLINE_IMAGE (bline_main_get_active_image (GTK_WINDOW (data)));

  if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &iter))
    {
      model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));
      gtk_tree_model_get (model, &iter, 0, &object, -1);

      if (BLINE_IS_FILTER (object))
        {
          ChData *chdata;

          filter = BLINE_FILTER (object);

          chdata = bline_main_get_chdata (GTK_WINDOW (data));

          // hide previous controls
          // FIXME shouldn't we "remove" the widget instead of just hiding it?
          if (chdata->active_filter)
            gtk_widget_hide (chdata->active_filter);

          chdata->active_filter = GTK_WIDGET (filter);

          if (GTK_IS_WIDGET (filter))
            {
              gtk_box_pack_start (GTK_BOX (chdata->control_box),
                                  GTK_WIDGET (filter), TRUE, TRUE, 0);

              gtk_widget_show (GTK_WIDGET (filter));
            }

          // apply the filter
          bline_main_apply_filter (GTK_WINDOW (data), filter);
        }
    }
}

static void
bline_main_on_filter_changed (BlineFilter *filter,
                              gpointer data)
{
  g_return_if_fail (GTK_IS_WINDOW (data));

  bline_main_apply_filter (GTK_WINDOW (data), filter);
}

static void
bline_main_add_filter (GtkWindow *self, BlineFilter *filter)
{
  ChData        *chdata;
  GtkTreeIter   iter;
  GtkListStore  *store;

  g_return_if_fail (BLINE_IS_FILTER (filter));

  chdata = bline_main_get_chdata (self);

  store = GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX (chdata->combo)));

  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, G_OBJECT (filter), -1);

  g_signal_connect (G_OBJECT (filter),
                    "changed",
                    G_CALLBACK (bline_main_on_filter_changed),
                    (gpointer) self);
}

static void
bline_main_cell_layout (GtkCellLayout *layout,
                        GtkCellRenderer *cell,
                        GtkTreeModel *model,
                        GtkTreeIter *iter,
                        gpointer data)
{
  GObject       *object;
  const gchar   *title;

  title = NULL;
  object = NULL;

  gtk_tree_model_get (model, iter, 0, &object, -1);

  if (BLINE_IS_FILTER (object))
    {
      BlineFilter *filter;

      filter = BLINE_FILTER (object);
      title = bline_filter_get_title (filter);

      g_object_set (cell, "text", title, NULL);

      g_object_unref (object);
    }
  else
      g_object_set (cell, "text", "(nil)", NULL);
}

static void
bline_main_set_active_image (GtkWindow *self,
                             BlineImage *image)
{
  GtkWidget *old;
  ChData    *chdata;
  gboolean  active;

  old = bline_main_get_active_image (self);

  chdata = bline_main_get_chdata (self);

  if (old != NULL)
    gtk_widget_destroy (GTK_WIDGET (image));

  active = (image == NULL) ? FALSE : TRUE;

  // activate/deactivate the save and close menu items
  gtk_widget_set_sensitive (chdata->save_as, active);
  gtk_widget_set_sensitive (chdata->close, active);
  gtk_widget_set_sensitive (chdata->combo, active);

  if (!active)
    return;

  g_object_set_data (G_OBJECT (self), "bline-image", image);

  gtk_box_pack_start (GTK_BOX (chdata->content_box), GTK_WIDGET (image), TRUE, TRUE, 0);

  gtk_widget_show (GTK_WIDGET (image));
}


static void
bline_main_setup_filter_selector (GtkWindow *self)
{
  ChData            *chdata;
  GtkWidget         *combo;
  GtkListStore      *store;
  GtkCellRenderer   *cell;

  chdata = bline_main_get_chdata (self);

  combo = chdata->combo;

  store = gtk_list_store_new (1, G_TYPE_OBJECT);

  gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));

  cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), cell, TRUE);
  gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
                                      cell,
                                      bline_main_cell_layout,
                                      NULL,
                                      NULL);

  g_signal_connect (G_OBJECT (combo),
                    "changed",
                    G_CALLBACK (bline_main_combo_box_on_changed),
                    (gpointer) self);

  gtk_widget_set_sensitive (GTK_WIDGET (combo), FALSE);

  /* add all filters */
  bline_main_add_filter (self,
    BLINE_FILTER (bline_simple_filter_new_with_title ("Sobel", 2, (const int *) SOBEL_MASK)));

  bline_main_add_filter (self,
    BLINE_FILTER (bline_simple_filter_new_with_title ("Sobel (horizontal)", 1, (const int *) SOBEL_MASK)));

  bline_main_add_filter (self,
    BLINE_FILTER (bline_simple_filter_new_with_title ("Sobel (vertical)", 1, (const int *) &SOBEL_MASK[1])));

  bline_main_add_filter (self,
    BLINE_FILTER (bline_simple_filter_new_with_title ("Scharr", 2, (const int *) SCHARR_MASK)));

  bline_main_add_filter (self,
    BLINE_FILTER (bline_simple_filter_new_with_title ("Robinson", 8, (const int *) ROBINSON_MASK)));

  bline_main_add_filter (self,
    BLINE_FILTER (bline_simple_filter_new_with_title ("Kirsch", 8, (const int *) KIRSCH_MASK)));
}


G_MODULE_EXPORT void
bline_main_on_open (GtkWidget *widget,
                    gpointer data)
{
  GtkWidget *dialog;
  GtkWidget *main;

  main = GTK_WIDGET (data);
  dialog = gtk_file_chooser_dialog_new ("Open file",
                                        GTK_WINDOW (main),
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                        NULL);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      gchar         *filename;
      BlineImage    *image;

      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

      image = BLINE_IMAGE (bline_image_load (filename));

      bline_main_set_active_image (GTK_WINDOW (main), image);

      g_free (filename);
    }

  gtk_widget_destroy (dialog);
}

gchar *
bline_util_get_suggested_name (BlineImage *image)
{
  gchar *r = g_new (gchar, strlen("image.png") + 1);
  strcpy (r, "image.png");
  return r;
}

G_MODULE_EXPORT void
bline_main_on_save_as (GtkWidget *widget,
                       gpointer data)
{
  GtkWindow     *main;
  GtkWidget     *dialog;
  BlineImage    *image;
  GError        *error = NULL;
  gchar         *name;

  main = GTK_WINDOW (data);

  image = BLINE_IMAGE (bline_main_get_active_image (main));

  dialog = gtk_file_chooser_dialog_new ("Save as...",
                                        main,
                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                        NULL);

  gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog),
                                                  TRUE);

  name = bline_util_get_suggested_name (image);

  gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), name);

  g_free (name);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      gchar *filename;

      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

      if (!bline_image_save (image,
                             filename,
                             &error))
        g_warning ("Error while saving file");
      else
        g_message ("Image saved to file");

      g_free (filename);
    }
  
  gtk_widget_destroy (dialog);
}


G_MODULE_EXPORT void
bline_main_on_close (GtkWidget *widget,
                     gpointer data)
{
  bline_main_set_active_image (GTK_WINDOW (data), NULL);
}

G_MODULE_EXPORT void
bline_main_on_quit (GtkWidget *widget,
                    gpointer data)
{
  gtk_widget_destroy (GTK_WIDGET (data));
}


G_MODULE_EXPORT void
bline_main_on_about (GtkWidget *widget,
                     gpointer data)
{
  GtkWidget         *main;
  ChData            *chdata;
  GtkAboutDialog    *dialog;

  main = GTK_WIDGET (data);

  chdata = bline_main_get_chdata (GTK_WINDOW (main));

  dialog = GTK_ABOUT_DIALOG (chdata->about_dialog);

  gtk_about_dialog_set_version (dialog, PACKAGE_VERSION);

  gtk_dialog_run (GTK_DIALOG (dialog));

  gtk_widget_hide (GTK_WIDGET (dialog));
}

G_MODULE_EXPORT void
bline_main_on_destroy (GtkWidget *main,
                       gpointer data)
{
  gpointer  chdata;

  chdata = g_object_get_data (G_OBJECT (main), "data");

  g_free (chdata);

  gtk_main_quit ();
}

int
main (int argc,
      char *argv[])
{
  ChData        *data;
  GtkBuilder    *builder;
  GError        *error = NULL;

  gtk_init (&argc, &argv);

  builder = gtk_builder_new ();

  if (!gtk_builder_add_from_file (builder, UI_FILE, &error))
    {
      g_warning ("%s", error->message);
      //g_free(error);
      return EXIT_FAILURE;
    }

  data = g_new (ChData, 1);

  data->main_window = GTK_WIDGET (gtk_builder_get_object (builder, "main_window"));
  data->about_dialog = GTK_WIDGET (gtk_builder_get_object (builder, "about_dialog"));
  data->content_box = GTK_WIDGET (gtk_builder_get_object (builder, "content_box"));
  data->combo = GTK_WIDGET (gtk_builder_get_object (builder, "combobox"));
  data->control_box = GTK_WIDGET (gtk_builder_get_object (builder, "control_box"));
  data->active_filter = NULL;
  data->close = GTK_WIDGET (gtk_builder_get_object (builder, "gtk-close"));
  data->save_as = GTK_WIDGET (gtk_builder_get_object (builder, "gtk-save-as"));

  /* store the ch data on the window itself, so we can acess that data from
     within our callbacks */
  bline_main_set_chdata (GTK_WINDOW (data->main_window), data);

  gtk_builder_connect_signals (builder, data);

  g_object_unref (builder);

  bline_main_setup_filter_selector (GTK_WINDOW (data->main_window));

  if (1)
    {
      GdkPixbuf *pixbuf;
      GError *error = NULL;

      pixbuf = gdk_pixbuf_new_from_file (ICON_FILE, &error);

      if (error == NULL)
        gtk_window_set_icon (GTK_WINDOW (data->main_window), pixbuf);
    }

  gtk_widget_show (data->main_window);

  gtk_main ();

  return EXIT_SUCCESS;
}
