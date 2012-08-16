#include <highgui.h>
#include <gtk/gtk.h>

#include "bline-image.h"


G_DEFINE_TYPE (BlineImage, bline_image, GTK_TYPE_IMAGE);


static void
bline_image_finalize (GObject *selfobj)
{
  BlineImage *self = BLINE_IMAGE (selfobj);

  if (self->image != NULL)
    cvReleaseImage (&self->image);

  if (self->path != NULL)
    g_free (self->path);

  G_OBJECT_CLASS (bline_image_parent_class)->finalize (selfobj);
}

static void
bline_image_class_init (BlineImageClass *cls)
{
  GObjectClass *objcls;

  objcls = G_OBJECT_CLASS (cls);

  objcls->finalize = bline_image_finalize;
}

static void
bline_image_init (BlineImage *self)
{
  self->image = NULL;
  self->path = NULL;
}

static void
bline_image_release_pixbuf_ipl_image (guchar *pixels,
                                      gpointer data)
{
  if (data != NULL)
    cvReleaseImage ((IplImage **) &data);
}

GtkWidget *
bline_image_new (void)
{
  return g_object_new (BLINE_TYPE_IMAGE, NULL);
}

/* FIXME this shouldn't be here :x
   FIXME also, it should be doing something useful, not just dupying the
   string :xx */
static gchar *
bline_util_canonical_path (const char *path)
{
  return g_strdup (path);
}

static gboolean
bline_image_load_internal (BlineImage *self)
{
  IplImage *image;

  image = cvLoadImage (self->path, CV_LOAD_IMAGE_GRAYSCALE);
  if (!image)
      return FALSE;

  self->image = image;

  bline_image_set_visible (self, self->image);

  return TRUE;
}

GtkWidget *
bline_image_load (const gchar *path)
{
  BlineImage    *self;
  gboolean      retval;

  g_return_val_if_fail (path != NULL, NULL);

  self = BLINE_IMAGE (bline_image_new ());
  self->path = bline_util_canonical_path (path);

  retval = bline_image_load_internal (self);

  if (retval)
      return GTK_WIDGET (self);
  else
    {
      g_object_unref (self);
      return NULL;
    }
}

gboolean
bline_image_save (BlineImage *self,
                  const gchar *path,
                  GError **error)
{
  GdkPixbuf *pixbuf;

  pixbuf = gtk_image_get_pixbuf (GTK_IMAGE (self));

  // TODO support other filetypes
  gdk_pixbuf_save (pixbuf, path, "png", error, NULL);

  if (!error || !*error)
    return TRUE;
  else
    return FALSE;
}

const IplImage *
bline_image_get_original (BlineImage *self)
{
  g_return_if_fail (BLINE_IS_IMAGE (self));

  return self->image;
}

void
bline_image_set_visible (BlineImage *self, IplImage *image)
{
  GdkPixbuf *pixbuf;
  IplImage  *rgbimage;

  if (image == NULL)
    {
      gtk_image_clear (GTK_IMAGE (self));
      return;
    }

  rgbimage = cvCreateImage (cvSize (image->width, image->height),
                            image->depth, 3);

  cvConvertImage (image, rgbimage, 0);

  pixbuf = gdk_pixbuf_new_from_data ((const guchar *) rgbimage->imageData,
                                     GDK_COLORSPACE_RGB,
                                     FALSE,
                                     rgbimage->depth,
                                     rgbimage->width,
                                     rgbimage->height,
                                     rgbimage->widthStep,
                                     bline_image_release_pixbuf_ipl_image,
                                     (gpointer) rgbimage);

  gtk_image_set_from_pixbuf (GTK_IMAGE (self), pixbuf);

  g_object_unref (G_OBJECT (pixbuf));
}
