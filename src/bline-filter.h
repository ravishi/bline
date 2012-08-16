#ifndef _BLINE_FILTER_H
#define _BLINE_FILTER_H

#include <glib-object.h>
#include <cv.h>


G_BEGIN_DECLS

#define BLINE_TYPE_FILTER                   (bline_filter_get_type ())
#define BLINE_FILTER(obj)                   (G_TYPE_CHECK_INSTANCE_CAST ((obj), BLINE_TYPE_FILTER, BlineFilter))
#define BLINE_IS_FILTER(obj)                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BLINE_TYPE_FILTER))
#define BLINE_FILTER_GET_INTERFACE(inst)    (G_TYPE_INSTANCE_GET_INTERFACE ((inst), BLINE_TYPE_FILTER, BlineFilterInterface))


typedef struct _BlineFilter             BlineFilter;
typedef struct _BlineFilterInterface    BlineFilterInterface;

struct _BlineFilter
{
};

struct _BlineFilterInterface {
  GTypeInterface    parent;

  const gchar   *(*get_title)   (BlineFilter *self);
  IplImage      *(*apply)       (BlineFilter *self, const IplImage *image);
  void          (*changed)      (BlineFilter *, gpointer);
};


GType       bline_filter_get_type           (void);

const gchar *bline_filter_get_title         (BlineFilter *self);

IplImage    *bline_filter_apply             (BlineFilter *self,
                                             const IplImage *image);

void        bline_filter_notify_changed     (BlineFilter *self);


G_END_DECLS

#endif /* _BLINE_FILTER_H */
