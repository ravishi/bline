#ifndef BLINE_OPERATION_EDGE_DETECT_H
#define BLINE_OPERATION_EDGE_DETECT_H

#include <gegl-plugin.h>
#include <operation/gegl-operation-area-filter.h>


#define BLINE_TYPE_OPERATION_EDGE_DETECT            (bline_operation_edge_detect_get_type ())
#define BLINE_OPERATION_EDGE_DETECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BLINE_TYPE_OPERATION_EDGE_DETECT, BlineOperationEdgeDetect))
#define BLINE_OPERATION_EDGE_DETECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  BLINE_TYPE_OPERATION_EDGE_DETECT, BlineOperationEdgeDetectClass))
#define BLINE_IS_OPERATION_EDGE_DETECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BLINE_TYPE_OPERATION_EDGE_DETECT))
#define BLINE_IS_OPERATION_EDGE_DETECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  BLINE_TYPE_OPERATION_EDGE_DETECT))
#define BLINE_OPERATION_EDGE_DETECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  BLINE_TYPE_OPERATION_EDGE_DETECT, BlineOperationEdgeDetectClass))


typedef struct _BlineOperationEdgeDetect BlineOperationEdgeDetect;
typedef struct _BlineOperationEdgeDetectClass BlineOperationEdgeDetectClass;

struct _BlineOperationEdgeDetect
{
  GeglOperationAreaFilter  parent_instance;
};

struct _BlineOperationEdgeDetectClass
{
  GeglOperationAreaFilterClass  parent_class;
};


GType   bline_operation_edge_detect_get_type (void) G_GNUC_CONST;

#endif
