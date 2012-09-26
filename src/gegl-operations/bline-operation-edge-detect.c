#include <gegl.h>
#include <math.h>
#include <stdio.h>
#include "config.h"
#include "bline-operation-edge-detect.h"

#define EDGE_DETECT_RADIUS 1

// Sobel: vertical e horizontal
const int SOBEL_MASK[2][3][3] = {
    {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}},
    {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}},
};

static gboolean bline_operation_edge_detect_process (GeglOperation       *operation,
                                               GeglBuffer          *input,
                                               GeglBuffer          *output,
                                               const GeglRectangle *result,
                                               gint                 level);

static void     bline_edge                    (GeglBuffer          *src,
                                               const GeglRectangle *src_rect,
                                               GeglBuffer          *dst,
                                               const GeglRectangle *dst_rect,
                                               gboolean            horizontal,
                                               gboolean            vertical,
                                               gboolean            keep_signal);


G_DEFINE_TYPE (BlineOperationEdgeDetect, bline_operation_edge_detect, GEGL_TYPE_OPERATION_AREA_FILTER)

#define parent_class bline_operation_edge_detect_parent_class

static void
prepare (GeglOperation *operation)
{
  GeglOperationAreaFilter *area = GEGL_OPERATION_AREA_FILTER (operation);

  area->left = area->right = area->top = area->bottom = EDGE_DETECT_RADIUS;
  gegl_operation_set_format (operation, "input", babl_format ("Y u8"));
  gegl_operation_set_format (operation, "output", babl_format ("Y u8"));
}

static gboolean
bline_operation_edge_detect_process (GeglOperation       *operation,
                                     GeglBuffer          *input,
                                     GeglBuffer          *output,
                                     const GeglRectangle *result,
                                     gint                 level)
{
  GeglRectangle compute;

  compute = gegl_operation_get_required_for_output (operation, "input",result);

  bline_edge (input, &compute, output, result, TRUE, TRUE, TRUE);//o->horizontal, o->vertical, o->keep_signal);

  return  TRUE;
}

inline static gfloat
RMS(gfloat a, gfloat b)
{
  return sqrtf(a*a+b*b);
}

static void
bline_edge (GeglBuffer          *src,
            const GeglRectangle *src_rect,
            GeglBuffer          *dst,
            const GeglRectangle *dst_rect,
            gboolean            horizontal,
            gboolean            vertical,
            gboolean            keep_signal)
{

  gint x, y;
  gint offset;
  guchar *src_buf;
  guchar *dst_buf;

  gint src_width = src_rect->width;

  src_buf = g_new0 (guchar, src_rect->width * src_rect->height);
  dst_buf = g_new0 (guchar, dst_rect->width * dst_rect->height);

  gegl_buffer_get (src, src_rect, 1.0, babl_format ("Y u8"), src_buf,
                   GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

  offset = 0;

  // just copy :-)
  for (y=0; y<dst_rect->height; y++)
    for (x=0; x<dst_rect->width; x++)
      {
        guchar *srcpix = src_buf + (x + y * src_width);
        dst_buf[offset] = *srcpix;
        offset++;
      }

  gegl_buffer_set (dst, dst_rect, 0, babl_format ("Y u8"), dst_buf,
                   GEGL_AUTO_ROWSTRIDE);

  g_free (src_buf);
  g_free (dst_buf);
}

static void
bline_operation_edge_detect_init (BlineOperationEdgeDetect *self)
{
}

static void
bline_operation_edge_detect_class_init (BlineOperationEdgeDetectClass *klass)
{
  GObjectClass              *object_class    = G_OBJECT_CLASS (klass);
  GeglOperationClass        *operation_class = GEGL_OPERATION_CLASS (klass);
  GeglOperationFilterClass  *filter_class    = GEGL_OPERATION_FILTER_CLASS (klass);

  //object_class->set_property    = bline_operation_edge_detect_set_property;
  //object_class->get_property    = bline_operation_edge_detect_get_property;

  operation_class->prepare = prepare;
  filter_class->process = bline_operation_edge_detect_process;

  gegl_operation_class_set_keys (operation_class,
    "name"       , "bline:edge-detect",
    "categories" , "edge-detect",
    "description", "Specialized direction-dependent edge detection",
    NULL);
}
