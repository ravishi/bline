#ifndef __BLINE_OPERATION_SOBEL_H__
#define __BLINE_OPERATION_SOBEL_H__

#include <gegl-plugin.h>
#include <operation/gegl-operation-area-filter.h>

G_BEGIN_DECLS

#define BLINE_TYPE_OPERATION_SOBEL             (bline_operation_sobel_get_type())
#define BLINE_OPERATION_SOBEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),BLINE_TYPE_OPERATION_SOBEL,BlineOperationSobel))
#define BLINE_OPERATION_SOBEL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),BLINE_TYPE_OPERATION_SOBEL,BlineOperationSobelClass))
#define BLINE_IS_OPERATION_SOBEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),BLINE_TYPE_OPERATION_SOBEL))
#define BLINE_IS_OPERATION_SOBEL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),BLINE_TYPE_OPERATION_SOBEL))
#define BLINE_OPERATION_SOBEL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),BLINE_TYPE_OPERATION_SOBEL,BlineOperationSobelClass))

typedef struct _BlineOperationSobel      BlineOperationSobel;
typedef struct _BlineOperationSobelClass BlineOperationSobelClass;
typedef struct _BlineOperationSobelPrivate         BlineOperationSobelPrivate;

struct _BlineOperationSobel {
  GeglOperationAreaFilter parent;
  BlineOperationSobelPrivate *priv;
};

struct _BlineOperationSobelClass {
  GeglOperationAreaFilterClass parent_class;
};

GType                   bline_operation_sobel_get_type    (void) G_GNUC_CONST;

BlineOperationSobel*    bline_operation_sobel_new         (void);

G_END_DECLS

#endif /* __BLINE_OPERATION_SOBEL_H__ */

