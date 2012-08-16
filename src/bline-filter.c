#include "bline-filter.h"
#include <cv.h>

enum
{
  CHANGED,
  LAST_SIGNAL
};

static guint bline_filter_signals[LAST_SIGNAL] = {0};


static void
bline_filter_base_init (gpointer g_class)
{
  static gboolean is_initialized = FALSE;

  if (!is_initialized)
    {
      /**
       * BlineFilter::changed
       *
       * Emitted when a filter is changed. One can listen to that to update
       * the image accordingly with the new parameters.
       */
      bline_filter_signals[CHANGED] =
       g_signal_new ("changed",
                     G_TYPE_FROM_CLASS (g_class),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET (BlineFilterInterface, changed),
                     NULL /* accumulator */,
                     NULL /* accumulator data */,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE /* return type */,
                     0, /* n_params */
                     NULL /* param_types */);

      is_initialized = TRUE;
    }
}


GType
bline_filter_get_type (void)
{
  static GType iface_type = 0;
  if (iface_type == 0)
    {
      static const GTypeInfo info = {
        sizeof (BlineFilterInterface),
        bline_filter_base_init,
        NULL,
      };

      iface_type = g_type_register_static (G_TYPE_INTERFACE, "BlineFilter",
                                           &info, 0);
    }

  return iface_type;
}


const gchar *
bline_filter_get_title (BlineFilter *self)
{
  g_return_if_fail (BLINE_IS_FILTER (self));

  return BLINE_FILTER_GET_INTERFACE (self)->get_title (self);
}

IplImage *
bline_filter_apply (BlineFilter *self,
                    const IplImage *image)
{
  g_return_if_fail (BLINE_IS_FILTER (self));

  return BLINE_FILTER_GET_INTERFACE (self)->apply (self, image);
}

void
bline_filter_notify_changed (BlineFilter *self)
{
  g_return_if_fail (BLINE_IS_FILTER (self));

  g_signal_emit (self, bline_filter_signals[CHANGED], 0, self);
}
