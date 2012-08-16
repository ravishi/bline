#ifndef __BLINE_IMAGE_H__
#define __BLINE_IMAGE_H__

#include <gtk/gtk.h>
#include <cv.h>
#include "bline-filter-simple.h"


G_BEGIN_DECLS

#define BLINE_TYPE_IMAGE            (bline_image_get_type ())
#define BLINE_IMAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BLINE_TYPE_IMAGE, BlineImage))
#define BLINE_IMAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BLINE_TYPE_IMAGE, BlineImageClass))
#define BLINE_IS_IMAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BLINE_TYPE_IMAGE))
#define BLINE_IS_IMAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BLINE_TYPE_IMAGE))
#define BLINE_IMAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BLINE_TYPE_IMAGE, BlineImageClass))

typedef struct _BlineImage      BlineImage;
typedef struct _BlineImageClass BlineImageClass;

struct _BlineImage
{
  GtkImage parent;

  /* private */
  gchar     *path;
  IplImage  *image;
};

struct _BlineImageClass
{
  GtkImageClass  parent_class;
};


GType           bline_image_get_type        (void) G_GNUC_CONST;

GtkWidget       *bline_image_new            (void);

GtkWidget       *bline_image_load           (const gchar *path);

gboolean        bline_image_save            (BlineImage *self,
                                             const gchar *path,
                                             GError **error);

const IplImage  *bline_image_get_original   (BlineImage *self);

void            bline_image_set_visible     (BlineImage *self,
                                             IplImage *image);

G_END_DECLS

#endif /* __BLINE_IMAGE_H__ */
