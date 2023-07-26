#ifndef _MATH_FUNC_
#define _MATH_FUNC_



#include <math.h>
#include <stdio.h>
#include <stdlib.h>

inline double MAX_DBL(double x, double y) {
  if (x < y)
    return y;
  else
    return x;
}
inline int MAX_INT(int x, int y) { return (x > y) ? x : y; }
inline int MIN_INT(int x, int y) { return (x < y) ? x : y; }
inline int CEIL_DIV(int x, int y) {
  return (int)ceil(((double)x) / ((double)y));
}
inline int FLOOR_DIV(int x, int y) { return (int)((double)x / (double)y); }
// inline int MAX_INT(int x, int y)
//{
// if(x < y)
//	return y;
// else
//	return x;
//	return x ^ ((x ^ y) & -(x < y));
// }

// inline int MIN_INT(int x, int y)
//{
// if(x > y)
//	return y;
// else
//	return x;
//	return y ^ ((x ^ y) & -(x < y));
// }

inline double MIN_DBL(double x, double y) {
  if (x > y)
    return y;
  else
    return x;
}

inline int ABS_INT(int x) {
  // int mask = x >> 31;
  // return ((-x) & mask) | (x & ~mask);

  if (x > 0)
    return x;
  else
    return -x;
}
inline double ABS_DBL(double x) {
  if (x > 0)
    return x;
  else
    return -x;
}

// return a random number between min(inclusivly) and max(exclusivly)
inline int mat_func_get_rand_int(int min, int max) {
  if (min == max)
    return min;
  return (rand() % (max - min)) + min;
}

inline int mat_func_get_rand_int_diff(int min, int max, int no) {
  int n = mat_func_get_rand_int(min, max);
  while (n == no)
    n = mat_func_get_rand_int(min, max);

  return n;
}

inline double mat_func_get_rand_double() { return rand() / (double)RAND_MAX; }

inline double mat_abs_diff(double x1, double x2, double y1, double y2) {
  return ABS_DBL(x1 - x2) + ABS_DBL(y1 - y2);
}

inline double mat_func_get_max_smaller_than(double **mat, int dim,
                                            double max_value) {
  double maxv = -max_value;
  int i, j;
  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      if (mat[i][j] > maxv && mat[i][j] < max_value)
        maxv = mat[i][j];
  return maxv;
}
inline float mat_func_get_max_smaller_than_f(float **mat, int dim,
                                             float max_value) {
  float maxv = -max_value;
  int i, j;
  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      if (mat[i][j] > maxv && mat[i][j] < max_value)
        maxv = mat[i][j];
  return maxv;
}

inline double mat_func_get_rand_double_between(double min, double max) {
  return (max - min) * mat_func_get_rand_double() + min;
}

inline double *math_func_get_double_array(int n) {
  return (double *)malloc(sizeof(double) * n);
}
inline double **math_func_get_double_matrix(int n, int m) {
  double **p = (double **)malloc(sizeof(double *) * n);
  int i;
  for (i = 0; i < n; i++)
    p[i] = (double *)malloc(sizeof(double) * m);
  return p;
}
inline void math_func_free_double_array(double *a) { free(a); }
inline void math_func_free_double_matrix(double **a, int n) {
  int i = 0;
  for (i = 0; i < n; i++)
    if (a[i] != NULL)
      free(a[i]);
  free(a);
}

inline double haversine_distance(double th1, double ph1, double th2,
                                 double ph2) {
  double dx, dy, dz;
  ph1 -= ph2;
  ph1 *= (3.1415926536 / 180);
  th1 *= (3.1415926536 / 180);
  th2 *= (3.1415926536 / 180);

  dx = cos(ph1) * cos(th1) - cos(th2);
  dy = sin(ph1) * cos(th1);
  dz = sin(th1) - sin(th2);
  return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371;
}

inline unsigned int jenkins_one_at_a_time_hash(char *key, int len) {
  unsigned int hash, i;
  for (hash = i = 0; i < len; ++i) {
    hash += key[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}


#endif