#ifndef __BLINE_FILTER_H__
#define __BLINE_FILTER_H__

#include <gtk/gtk.h> /* FIXME: we just need gboolean! */

typedef enum _BlineFilterSimple BlineFilterSimple;

const int SOBEL_MASK[2][3][3];
const int SCHARR_MASK[2][3][3];
const int ROBINSON_MASK[8][3][3];
const int KIRSCH_MASK[8][3][3];


IplImage *      bline_apply_simple_filter      (const IplImage *image,
                                                int nmask,
                                                const int *mask,
                                                int threshold);

#endif /* __BLINE_FILTER_H__ */
