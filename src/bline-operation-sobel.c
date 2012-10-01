#include <math.h>
#include "bline-operation-sobel.h"

static void     bline_operation_sobel_class_init (BlineOperationSobelClass *klass);
static void     bline_operation_sobel_init       (BlineOperationSobel *obj);
static void     bline_operation_sobel_finalize   (GObject *obj);

static void     prepare (GeglOperation *operation);
static gboolean process (GeglOperation       *operation,
                         GeglBuffer          *input,
                         GeglBuffer          *output,
                         const GeglRectangle *result,
                         gint                level);
static void     sobel   (GeglBuffer          *src,
                         const GeglRectangle *src_rect,
                         GeglBuffer          *dst,
                         const GeglRectangle *dst_rect,
                         gboolean            horizontal,
                         gboolean            vertical,
                         gboolean            keep_signal);

#define SOBEL_RADIUS 1

#define BLINE_OPERATION_SOBEL_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                                   BLINE_TYPE_OPERATION_SOBEL, \
                                                   BlineOperationSobelPrivate))

enum
{
  PROP_0,
  PROP_BLINE_OPERATION_SOBEL_HORIZONTAL,
  PROP_BLINE_OPERATION_SOBEL_VERTICAL,
  N_PROPERTIES
};

struct _BlineOperationSobelPrivate
{
  gboolean vertical;
  gboolean horizontal;
};

static GeglOperationAreaFilterClass *parent_class = NULL;

static GParamSpec *bline_operation_sobel_properties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE (BlineOperationSobel, bline_operation_sobel, GEGL_TYPE_OPERATION_AREA_FILTER);

static void
set_property (GObject    *object,
              guint      property_id,
              const      GValue *value,
              GParamSpec *pspec)
{
  BlineOperationSobel *self = BLINE_OPERATION_SOBEL (object);

  switch (property_id)
    {
    case PROP_BLINE_OPERATION_SOBEL_HORIZONTAL:
      self->priv->horizontal = g_value_get_boolean (value);
      break;
    case PROP_BLINE_OPERATION_SOBEL_VERTICAL:
      self->priv->vertical = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
get_property (GObject    *object,
              guint      property_id,
              GValue     *value,
              GParamSpec *pspec)
{
  BlineOperationSobel *self = BLINE_OPERATION_SOBEL (object);

  switch (property_id)
    {
    case PROP_BLINE_OPERATION_SOBEL_HORIZONTAL:
      g_value_set_boolean (value, self->priv->horizontal);
      break;
    case PROP_BLINE_OPERATION_SOBEL_VERTICAL:
      g_value_set_boolean (value, self->priv->vertical);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
bline_operation_sobel_class_init (BlineOperationSobelClass *klass)
{
  GObjectClass              *object_class = G_OBJECT_CLASS (klass);
  GeglOperationClass        *operation_class = GEGL_OPERATION_CLASS (klass);
  GeglOperationFilterClass  *operation_filter_class = GEGL_OPERATION_FILTER_CLASS (klass);

  parent_class                      = g_type_class_peek_parent (klass);

  object_class->finalize            = bline_operation_sobel_finalize;
  object_class->set_property        = set_property;
  object_class->get_property        = get_property;
  operation_class->prepare          = prepare;
  operation_filter_class->process   = process;

  bline_operation_sobel_properties[PROP_BLINE_OPERATION_SOBEL_HORIZONTAL] =
    g_param_spec_boolean ("horizontal",
                          "Horizontal",
                          "Horizontal detection",
                          TRUE,
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  bline_operation_sobel_properties[PROP_BLINE_OPERATION_SOBEL_VERTICAL] =
    g_param_spec_boolean ("vertical",
                          "Vertical",
                          "Vertical detection",
                          TRUE,
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     bline_operation_sobel_properties);

  g_type_class_add_private (object_class, sizeof(BlineOperationSobelPrivate));

  gegl_operation_class_set_keys (operation_class,
                                 "name"     , "bline:sobel",
                                 "categries", "edge-detect",
                                 "description", "Specialized direction-dependent edge detection",
                                 NULL);
}

static void
bline_operation_sobel_init (BlineOperationSobel *obj)
{
  obj->priv = BLINE_OPERATION_SOBEL_GET_PRIVATE(obj);
  obj->priv->vertical = TRUE;
  obj->priv->horizontal = TRUE;
}

static void
bline_operation_sobel_finalize (GObject *obj)
{
  G_OBJECT_CLASS(parent_class)->finalize (obj);
}

BlineOperationSobel*
bline_operation_sobel_new (void)
{
  return BLINE_OPERATION_SOBEL (g_object_new (BLINE_TYPE_OPERATION_SOBEL, NULL));
}

static void
prepare    (GeglOperation *operation)
{
  GeglOperationAreaFilter *area = GEGL_OPERATION_AREA_FILTER (operation);
  area->left = area->right = area->top = area->bottom = SOBEL_RADIUS;
  gegl_operation_set_format (operation, "input", babl_format ("Y float"));
  gegl_operation_set_format (operation, "output", babl_format ("Y float"));
}

static gboolean
process    (GeglOperation       *operation,
            GeglBuffer          *input,
            GeglBuffer          *output,
            const GeglRectangle *result,
            gint                level)
{
  BlineOperationSobel *self = BLINE_OPERATION_SOBEL (operation);
  GeglRectangle       compute;

  compute = gegl_operation_get_required_for_output (operation, "input", result);
  sobel (input, &compute, output, result,
         self->priv->horizontal, self->priv->vertical, TRUE);
  return  TRUE;
}

inline static gfloat
RMS (gfloat a, gfloat b)
{
  return sqrtf (a*a+b*b);
}

static void
sobel (GeglBuffer          *src,
       const GeglRectangle *src_rect,
       GeglBuffer          *dst,
       const GeglRectangle *dst_rect,
       gboolean            horizontal,
       gboolean            vertical,
       gboolean            keep_signal)
{
  gint x, y;
  gfloat *src_buf;
  gfloat *dst_buf;

  gint src_width = src_rect->width;

  src_buf = g_new0 (gfloat, src_rect->width * src_rect->height);
  dst_buf = g_new0 (gfloat, dst_rect->width * dst_rect->height);

  gegl_buffer_get (src, src_rect, 1.0, babl_format ("Y float"),
                   src_buf, GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

#define GSPIX(buf, w, x, y)  ((buf[x + ((y) * w)]))

  for (y = 0; y < dst_rect->height; y++)
    for (x = 0; x < dst_rect->width; x++)
      {
        gfloat hor_grad = 0.0f;
        gfloat ver_grad = 0.0f;
        gfloat     grad = 0.0f;

        gint i = x + SOBEL_RADIUS;
        gint j = y + SOBEL_RADIUS;

        if (horizontal)
          {
            /* [ -1, 0, 1 ]
               [ -2, 0, 2 ]
               [ -1, 0, 1 ] */
            hor_grad =
             -1.0f * GSPIX(src_buf, src_width, i-1, j-1)
             +1.0f * GSPIX(src_buf, src_width, i+1, j-1)
             -2.0f * GSPIX(src_buf, src_width, i-1, j)
             +2.0f * GSPIX(src_buf, src_width, i+1, j);
             -1.0f * GSPIX(src_buf, src_width, i-1, j+1)
             +1.0f * GSPIX(src_buf, src_width, i+1, j+1);
          }

        if (vertical)
          {
            /* [ -1, -2, -1 ]
               [  0,  0,  0 ]
               [  1,  2,  1 ] */
            ver_grad =
             -1.0f * GSPIX(src_buf, src_width, i-1, j-1)
             +1.0f * GSPIX(src_buf, src_width, i-1, j+1)
             -2.0f * GSPIX(src_buf, src_width,   i, j-1)
             +2.0f * GSPIX(src_buf, src_width,   i, j+1)
             -1.0f * GSPIX(src_buf, src_width, i+1, j-1)
             +1.0f * GSPIX(src_buf, src_width, i+1, j+1);
          }

        if (horizontal && vertical)
          grad = RMS(hor_grad, ver_grad) / 1.41f;
        else
          grad = fabsf(hor_grad + ver_grad);

        dst_buf[x + y * dst_rect->width] = grad;
      }

  gegl_buffer_set (dst, dst_rect, 1.0, babl_format ("Y float"),
                   dst_buf, GEGL_AUTO_ROWSTRIDE);

  g_free (src_buf);
  g_free (dst_buf);
}

/* vim: set ft=gnuc */
