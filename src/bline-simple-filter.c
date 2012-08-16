#include <cv.h>
#include "bline-simple-filter.h"
#include "bline-filter-simple.h"


enum {
  BLINE_SIMPLE_FILTER_PROP_0,

  BLINE_SIMPLE_FILTER_PROP_TITLE
};

static void     bline_simple_filter_on_threshold_changed    (GtkWidget *widget,
                                                             gpointer data);

static void     bline_simple_filter_filter_init             (BlineFilterInterface *iface);

static void     bline_simple_filter_set_title               (BlineSimpleFilter *self,
                                                             const gchar *title);


G_DEFINE_TYPE_WITH_CODE (BlineSimpleFilter, bline_simple_filter, GTK_TYPE_VBOX,
                         G_IMPLEMENT_INTERFACE (BLINE_TYPE_FILTER,
                                                bline_simple_filter_filter_init));

static void
bline_simple_filter_get_property (GObject    *object,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  BlineSimpleFilter *self;
  
  self = BLINE_SIMPLE_FILTER (object);

  switch (property_id)
    {
    case BLINE_SIMPLE_FILTER_PROP_TITLE:
      g_value_set_string (value, self->title);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
bline_simple_filter_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  BlineSimpleFilter *self;
  
  self = BLINE_SIMPLE_FILTER (object);

  switch (property_id)
    {
    case BLINE_SIMPLE_FILTER_PROP_TITLE:
      bline_simple_filter_set_title (self, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
bline_simple_filter_dispose (GObject *object)
{
  G_OBJECT_CLASS (bline_simple_filter_parent_class)->dispose (object);
}

static void
bline_simple_filter_finalize (GObject *object)
{
  BlineSimpleFilter *self;

  g_return_if_fail (BLINE_IS_SIMPLE_FILTER (object));

  self = BLINE_SIMPLE_FILTER (object);

  g_free (self->title);
  g_free (self->mask);

  G_OBJECT_CLASS (bline_simple_filter_parent_class)->finalize (object);
}

static void
bline_simple_filter_class_init (BlineSimpleFilterClass *klass)
{
  GObjectClass  *object_class;
  GParamSpec    *pspec;
  
  object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = bline_simple_filter_get_property;
  object_class->set_property = bline_simple_filter_set_property;
  object_class->dispose = bline_simple_filter_dispose;
  object_class->finalize = bline_simple_filter_finalize;

  pspec = g_param_spec_string ("title",
                               "Simple filter construct prop",
                               "Set filter's title",
                               "(nil)",
                               G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

  g_object_class_install_property (object_class,
                                   BLINE_SIMPLE_FILTER_PROP_TITLE,
                                   pspec);
}

static IplImage *
bline_simple_filter_apply (BlineFilter *filter,
                           const IplImage *image)
{
  BlineSimpleFilter *self;
  gint              threshold;

  self = BLINE_SIMPLE_FILTER (filter);

  threshold = (gint) gtk_range_get_value (GTK_RANGE (self->scale));

  return bline_apply_simple_filter (image, self->nmask, self->mask,
                                    threshold);
}

static void
bline_simple_filter_init (BlineSimpleFilter *self)
{
  self->title = NULL;

  self->scale = gtk_hscale_new_with_range (0, 255, 1);

  gtk_box_pack_start (GTK_BOX (self), GTK_WIDGET (self->scale), FALSE, FALSE, 0);

  gtk_scale_set_digits (GTK_SCALE (self->scale), 0);

  g_signal_connect (G_OBJECT (self->scale),
                    "value-changed",
                    G_CALLBACK (bline_simple_filter_on_threshold_changed),
                    (gpointer) self);

  // FIXME is this really necessary?
  gtk_widget_show (self->scale);
}

static void
bline_simple_filter_filter_init (BlineFilterInterface *iface)
{
  iface->get_title = bline_simple_filter_get_title;
  iface->apply = bline_simple_filter_apply;
}

GtkWidget *
bline_simple_filter_new (void)
{
  return g_object_new (BLINE_TYPE_SIMPLE_FILTER, NULL);
}


GtkWidget *
bline_simple_filter_new_with_title (const gchar *title,
                                    int nmask,
                                    const int *mask)
{
  BlineSimpleFilter *self;

  self = g_object_new (BLINE_TYPE_SIMPLE_FILTER,
                       "title", title,
                       NULL);

  self->nmask = nmask;
  self->mask = (gint *) g_memdup ((gconstpointer) mask,
                                  sizeof (int) * nmask * 9);

  return GTK_WIDGET (self);
}

static void
bline_simple_filter_on_threshold_changed (GtkWidget *scale,
                                          gpointer data)
{
  g_return_if_fail (BLINE_IS_SIMPLE_FILTER (data));

  bline_filter_notify_changed (BLINE_FILTER (data));
}

const gchar *
bline_simple_filter_get_title (BlineFilter *filter)
{
  GValue            val = {0, };
  
  g_value_init (&val, G_TYPE_STRING);
  g_object_get_property (G_OBJECT (filter), "title", &val);

  return g_value_get_string (&val);
}

static void
bline_simple_filter_set_title (BlineSimpleFilter *self, const gchar *title)
{
  gchar *new_title;

  g_return_if_fail (BLINE_IS_SIMPLE_FILTER (self));

  new_title = g_strdup (title);
  g_free (self->title);
  self->title = new_title;

  g_object_notify (G_OBJECT (self), "title");
}
