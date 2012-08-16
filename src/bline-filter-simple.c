#include <limits.h>
#include <cv.h>
#include "bline-filter-simple.h"



// Sobel: vertical e horizontal
const int SOBEL_MASK[2][3][3] = {
    {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}},
    {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}},
};

// Scharr
const int SCHARR_MASK[2][3][3] = {
    {{3,0,-3}, {10,0,-10}, {3,0,-3}},
    {{3,10,3}, {0,0,0}, {-3,-10,-3}}
};


// Robinson
const int ROBINSON_MASK[8][3][3] = {
    {{1,2,1}, {0,0,0}, {-1,-2,-1}},
    {{2,1,0}, {1,0,-1}, {0,-1,-2}},
    {{1,0,-1}, {2,0,-2}, {1,0,-1}},
    {{0,-1,-2}, {1,0,-1}, {2,1,0}},
    {{-1,-2,-1}, {0,0,0}, {1,2,1}},
    {{-2,-1,0}, {-1,0,1}, {0,1,2}},
    {{-1,0,1}, {-2,0,2}, {-1,0,1}},
    {{0,1,2}, {-1,0,1}, {-2,-1,0}},
};

//Kirsch
const int KIRSCH_MASK[8][3][3] = {
    {{5,5,5}, {-3,0,-3}, {-3,-3,-3}},
    {{-3,-3,-3}, {-3,0,-3}, {5,5,5}},
    {{-3,5,5}, {-3,0,5}, {-3,-3,-3}},
    {{-3,-3,-3}, {5,0,-3}, {5,5,-3}},
    {{5,5,-3}, {5,0,-3}, {-3,-3,-3}},
    {{-3,-3,-3}, {-3,0,5}, {-3,5,5}},
    {{5,-3,-3}, {5,0,-3}, {5,-3,-3}},
    {{-3,-3,5}, {-3,0,5}, {-3,-3,5}}
};

IplImage *
bline_apply_simple_filter (const IplImage *img,
                           int filtersNumber,
                           const int * mask,
                           int threshold)
{
  IplImage      *border;
  int           i, j, k, m, n;
  int           step;
  int           sum, min, max;
  int           *tmp, *t;
  const uchar   *src;
  uchar         *dst;
  double        factor;

  min = INT_MAX;
  max = 0;

  src = (const uchar *) img->imageData;
  step = img->widthStep / sizeof (uchar);

  tmp = malloc (sizeof (int) * img->height * img->width);

  // for i e j caminham pela imagem aplicando o filtro
  for (i = 1; i < img->height-2; i++)
    {
      for (j = 1; j < img->width-2; j++)
        {
          t = tmp + (i * img->width + j);

          *t = 0;

          // for m e n calculam a variação de luminosidade no ponto
          for(k = 0; k < filtersNumber; k++)
            {
              sum = 0;
              for (m = -1; m < 2; m++)
                {
                  for (n = -1; n < 2; n++)
                    // Realiza as convoluções
                    sum += src[(i + m) * step + j + n] * mask [k * 9 + (m + 1) * 3 + n + 1];
                }
              *t += abs(sum);
            }

          if (*t > max)
            max = *t;
          if (*t < min)
            min = *t;
        }
    }
  //Fim

  // agora que temos todos os valores do filtro, seu máximo e seu mínimo,
  // podemos normalizar os valores e criar a imagem com a borda
  border = cvCreateImage (cvSize (img->width, img->height),
                          IPL_DEPTH_8U, 1);

  dst = (uchar *) border->imageData;

  factor = 255.0 / (double) (max - min);

  for (i = 1; i < img->height - 2; i++)
    {
      for (j = 1; j < img->width - 2; j++)
        {
          sum = tmp[i * img->width + j] * factor;

          if (threshold)
            {
              if (sum < threshold)
                sum = 0;
              else
                sum = 255;
            }

          dst[i * step + j] = sum;
        }
    }

  free (tmp);

  return border;
}
