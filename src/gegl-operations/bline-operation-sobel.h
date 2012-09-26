#ifndef BLINE_OPERATION_SOBEL_H
#define BLINE_OPERATION_SOBEL_H

#include <gegl-plugin.h>
#include <operation/gegl-operation-area-filter.h>


#define BLINE_TYPE_OPERATION_SOBEL            (bline_operation_sobel_get_type ())
#define BLINE_OPERATION_SOBEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BLINE_TYPE_OPERATION_SOBEL, BlineOperationSobel))
#define BLINE_OPERATION_SOBEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  BLINE_TYPE_OPERATION_SOBEL, BlineOperationSobelClass))
#define BLINE_IS_OPERATION_SOBEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BLINE_TYPE_OPERATION_SOBEL))
#define BLINE_IS_OPERATION_SOBEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  BLINE_TYPE_OPERATION_SOBEL))
#define BLINE_OPERATION_SOBEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  BLINE_TYPE_OPERATION_SOBEL, BlineOperationSobelClass))


typedef struct _BlineOperationSobel BlineOperationSobel;
typedef struct _BlineOperationSobelClass BlineOperationSobelClass;

struct _BlineOperationSobel
{
  GeglOperationAreaFilter  parent_instance;
};

struct _BlineOperationSobelClass
{
  GeglOperationAreaFilterClass  parent_class;
};


GType   bline_operation_sobel_get_type (void) G_GNUC_CONST;

#endif
