/*
 * trans.c - 矩阵转置B=A^T
 *每个转置函数都必须具有以下形式的原型：
 *void trans（int M，int N，int a[N][M]，int B[M][N]）；
 *通过计算，块大小为32字节的1KB直接映射缓存上的未命中数来计算转置函数。
 */
#include "cachelab.h"
#include <stdio.h>
int is_transpose(int M, int N, int A[N][M], int B[M][N]);
char transpose_submit_desc[] =
    "Transpose submission"; //请不要修改“Transpose_submission”

void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {

  //                          请在此处添加代码
  //*************************************Begin********************************************************
  int i, j, c;
  int a[8];

  if (M == 64 || M == 32) {
    int p, q;

    for (q = 0; q < N; q += 8) {
      p = q == 0 ? 8 : 0;
      for (i = q; i < q + 4; ++i) {
        for (c = 0; c < 8; ++c) {
          a[c] = A[i + 4][q + c];
        }
        for (c = 0; c < 8; ++c) {
          B[i][p + c] = a[c];
        }
      }

      for (i = 0; i < 4; ++i) {
        for (j = i + 1; j < 4; ++j) {
          a[0] = B[q + i][p + j];
          B[q + i][p + j] = B[q + j][p + i];
          B[q + j][p + i] = a[0];

          a[0] = B[q + i][p + j + 4];
          B[q + i][p + j + 4] = B[q + j][p + i + 4];
          B[q + j][p + i + 4] = a[0];
        }
      }

      for (i = q; i < q + 4; ++i) {
        for (c = 0; c < 8; ++c) {
          a[c] = A[i][q + c];
        }
        for (c = 0; c < 8; ++c) {
          B[i][q + c] = a[c];
        }
      }

      for (i = q; i < q + 4; ++i) {
        for (j = i + 1; j < q + 4; ++j) {
          a[0] = B[i][j];
          B[i][j] = B[j][i];
          B[j][i] = a[0];

          a[0] = B[i][j + 4];
          B[i][j + 4] = B[j][i + 4];
          B[j][i + 4] = a[0];
        }
      }

      for (i = 0; i < 4; ++i) {
        a[0] = B[q + i][q + 4];
        a[1] = B[q + i][q + 5];
        a[2] = B[q + i][q + 6];
        a[3] = B[q + i][q + 7];

        B[q + i][q + 4] = B[q + i][p + 0];
        B[q + i][q + 5] = B[q + i][p + 1];
        B[q + i][q + 6] = B[q + i][p + 2];
        B[q + i][q + 7] = B[q + i][p + 3];

        B[q + i][p + 0] = a[0];
        B[q + i][p + 1] = a[1];
        B[q + i][p + 2] = a[2];
        B[q + i][p + 3] = a[3];
      }

      for (i = 0; i < 4; ++i) {
        for (c = 0; c < 8; ++c) {
          B[q + i + 4][q + c] = B[q + i][p + c];
        }
      }

      for (p = 0; p < M; p += 8) {
        if (p == q) {
          continue;
        } else {
          for (i = p; i < p + 4; ++i) {
            for (c = 0; c < 8; ++c) {
              a[c] = A[i][q + c];
            }

            B[q + 0][i] = a[0];
            B[q + 1][i] = a[1];
            B[q + 2][i] = a[2];
            B[q + 3][i] = a[3];
            B[q + 0][i + 4] = a[4];
            B[q + 1][i + 4] = a[5];
            B[q + 2][i + 4] = a[6];
            B[q + 3][i + 4] = a[7];
          }

          for (j = q; j < q + 4; ++j) {
            a[0] = A[p + 4][j];
            a[1] = A[p + 5][j];
            a[2] = A[p + 6][j];
            a[3] = A[p + 7][j];

            a[4] = B[j][p + 4];
            a[5] = B[j][p + 5];
            a[6] = B[j][p + 6];
            a[7] = B[j][p + 7];

            B[j][p + 4] = a[0];
            B[j][p + 5] = a[1];
            B[j][p + 6] = a[2];
            B[j][p + 7] = a[3];

            B[j + 4][p + 0] = a[4];
            B[j + 4][p + 1] = a[5];
            B[j + 4][p + 2] = a[6];
            B[j + 4][p + 3] = a[7];
          }

          for (i = p + 4; i < p + 8; ++i) {
            for (c = 0; c < 4; ++c) {
              a[c] = A[i][q + c + 4];
            }

            for (c = 0; c < 4; ++c) {
              B[q + c + 4][i] = a[c];
            }
          }
        }
      }
    }
  } else if (M == 61) {
    for (j = 0; j < M / 8 * 8; j += 8)
      for (i = 0; i < N / 8 * 8; i++) {
        for (c = 0; c < 8; ++c) {
          a[c] = A[i][j + c];
        }
        for (c = 0; c < 8; ++c) {
          B[j + c][i] = a[c];
        }
      }
    for (i = N / 8 * 8; i < N; ++i)
      for (j = M / 8 * 8; j < M; ++j) {
        a[0] = A[i][j];
        B[j][i] = a[0];
      }

    for (i = 0; i < N; ++i)
      for (j = M / 8 * 8; j < M; ++j) {
        a[0] = A[i][j];
        B[j][i] = a[0];
      }

    for (i = N / 8 * 8; i < N; ++i)
      for (j = 0; j < M; ++j) {
        a[0] = A[i][j];
        B[j][i] = a[0];
      }
  }

  //**************************************End**********************************************************
}

/*
 * 我们在下面定义了一个简单的方法来帮助您开始，您可以根据下面的例子把上面值置补充完整。
 */

/*
 * 简单的基线转置功能，未针对缓存进行优化。
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, tmp;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; j++) {
      tmp = A[i][j];
      B[j][i] = tmp;
    }
  }
}

/*
 * registerFunctions-此函数向驱动程序注册转置函数。
 *在运行时，驱动程序将评估每个注册的函数并总结它们的性能。这是一种试验不同转置策略的简便方法。
 */
void registerFunctions() {
  /* 注册解决方案函数  */
  registerTransFunction(transpose_submit, transpose_submit_desc);

  /* 注册任何附加转置函数 */
  registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose -
 * 函数检查B是否是A的转置。在从转置函数返回之前，可以通过调用它来检查转置的正确性。
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
  int i, j;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; ++j) {
      if (A[i][j] != B[j][i]) {
        return 0;
      }
    }
  }
  return 1;
}
