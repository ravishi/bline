#ifndef _BLINE_SIMPLE_FILTER_H
#define _BLINE_SIMPLE_FILTER_H

#include <gtk/gtk.h>
#include "bline-filter.h"

G_BEGIN_DECLS

#define BLINE_TYPE_SIMPLE_FILTER bline_simple_filter_get_type()

#define BLINE_SIMPLE_FILTER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BLINE_TYPE_SIMPLE_FILTER, BlineSimpleFilter)) 
#define BLINE_SIMPLE_FILTER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BLINE_TYPE_SIMPLE_FILTER, BlineSimpleFilterClass))
#define BLINE_IS_SIMPLE_FILTER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BLINE_TYPE_SIMPLE_FILTER)) 
#define BLINE_IS_SIMPLE_FILTER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BLINE_TYPE_SIMPLE_FILTER)) 
#define BLINE_SIMPLE_FILTER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BLINE_TYPE_SIMPLE_FILTER, BlineSimpleFilterClass))


typedef struct _BlineSimpleFilter BlineSimpleFilter;
typedef struct _BlineSimpleFilterClass BlineSimpleFilterClass;


struct _BlineSimpleFilter
{
  GtkVBox   parent;

  gchar     *title;
  gint      *mask;
  gint      nmask;
  GtkWidget *scale;
};

struct _BlineSimpleFilterClass
{
  GtkVBoxClass parent_class;
};


GType               bline_simple_filter_get_type        (void) G_GNUC_CONST;

GtkWidget           *bline_simple_filter_new            (void);

GtkWidget           *bline_simple_filter_new_with_title (const gchar *title,
                                                         int nmasks,
                                                         const int *mask);

const gchar         *bline_simple_filter_get_title      (BlineFilter *filter);


G_END_DECLS

#endif /* _BLINE_SIMPLE_FILTER_H */
