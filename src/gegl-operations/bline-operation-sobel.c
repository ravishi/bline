#include <gegl.h>
#include <math.h>
#include <stdio.h>
#include "config.h"
#include "bline-operation-sobel.h"

#define SOBEL_RADIUS    1
#define _(a)            a

static gboolean bline_operation_sobel_process (GeglOperation       *operation,
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


G_DEFINE_TYPE (BlineOperationSobel, bline_operation_sobel, GEGL_TYPE_OPERATION_AREA_FILTER)

#define parent_class bline_operation_sobel_parent_class

static void
bline_operation_sobel_class_init (BlineOperationSobelClass *klass)
{
  GObjectClass              *object_class    = G_OBJECT_CLASS (klass);
  GeglOperationClass        *operation_class = GEGL_OPERATION_CLASS (klass);
  GeglOperationFilterClass  *filter_class    = GEGL_OPERATION_FILTER_CLASS (klass);

  //object_class->set_property    = bline_operation_sobel_set_property;
  //object_class->get_property    = bline_operation_sobel_get_property;

  gegl_operation_class_set_keys (operation_class,
    "name"       , "bline:sobel",
    "categories" , "edge-detect",
    "description", _("Specialized direction-dependent edge detection"),
    NULL);
}

static void
bline_operation_sobel_init (BlineOperationSobel *self)
{
}


static void
prepare (GeglOperation *operation)
{
  GeglOperationAreaFilter *area = GEGL_OPERATION_AREA_FILTER (operation);

  area->left = area->right = area->top = area->bottom = SOBEL_RADIUS;
  gegl_operation_set_format (operation, "input", babl_format ("RGBA float"));
  gegl_operation_set_format (operation, "output", babl_format ("RGBA float"));
}

static gboolean
bline_operation_sobel_process (GeglOperation       *operation,
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

  gint x,y;
  gint offset;
  gfloat *src_buf;
  gfloat *dst_buf;

  gint src_width = src_rect->width;

  src_buf = g_new0 (gfloat, src_rect->width * src_rect->height * 4);
  dst_buf = g_new0 (gfloat, dst_rect->width * dst_rect->height * 4);

  gegl_buffer_get (src, src_rect, 1.0, babl_format ("RGBA float"), src_buf, GEGL_AUTO_ROWSTRIDE,
                   GEGL_ABYSS_NONE);

  offset = 0;

  for (y=0; y<dst_rect->height; y++)
    for (x=0; x<dst_rect->width; x++)
      {

        gfloat hor_grad[3] = {0.0f, 0.0f, 0.0f};
        gfloat ver_grad[3] = {0.0f, 0.0f, 0.0f};
        gfloat gradient[4] = {0.0f, 0.0f, 0.0f, 0.0f};

        gfloat *center_pix = src_buf + ((x+SOBEL_RADIUS)+((y+SOBEL_RADIUS) * src_width)) * 4;

        gint c;

        if (horizontal)
          {
            gint i=x+SOBEL_RADIUS, j=y+SOBEL_RADIUS;
            gfloat *src_pix = src_buf + (i + j * src_width) * 4;

            for (c=0;c<3;c++)
                hor_grad[c] +=
                    -1.0f*src_pix[c-4-src_width*4]+ src_pix[c+4-src_width*4] +
                    -2.0f*src_pix[c-4] + 2.0f*src_pix[c+4] +
                    -1.0f*src_pix[c-4+src_width*4]+ src_pix[c+4+src_width*4];
          }

        if (vertical)
          {
            gint i=x+SOBEL_RADIUS, j=y+SOBEL_RADIUS;
            gfloat *src_pix = src_buf + (i + j * src_width) * 4;

            for (c=0;c<3;c++)
                ver_grad[c] +=
                  -1.0f*src_pix[c-4-src_width*4]-2.0f*src_pix[c-src_width*4]-1.0f*src_pix[c+4-src_width*4] +
                  src_pix[c-4+src_width*4]+2.0f*src_pix[c+src_width*4]+     src_pix[c+4+src_width*4];
        }

        if (horizontal && vertical)
          {
            for (c=0;c<3;c++)
              // normalization to [0, 1]
              gradient[c] = RMS(hor_grad[c],ver_grad[c])/1.41f;
          }
        else
          {
            if (keep_signal)
              {
                for (c=0;c<3;c++)
                  gradient[c] = hor_grad[c]+ver_grad[c];
              }
            else
              {
                for (c=0;c<3;c++)
                  gradient[c] = fabsf(hor_grad[c]+ver_grad[c]);
              }
          }

        //alpha
        gradient[3] = center_pix[3];

        for (c=0; c<4;c++)
          dst_buf[offset*4+c] = gradient[c];

        offset++;
      }

  gegl_buffer_set (dst, dst_rect, 0, babl_format ("RGBA float"), dst_buf,
                   GEGL_AUTO_ROWSTRIDE);
  g_free (src_buf);
  g_free (dst_buf);
}
