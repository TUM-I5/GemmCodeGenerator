/**
 * @file
 * This file is part of GemmCodeGenerator.
 *
 * @author Alexander Heinecke (alexander.heinecke AT mytum.de, http://www5.in.tum.de/wiki/index.php/Alexander_Heinecke,_M.Sc.,_M.Sc._with_honors)
 *
 * @section LICENSE
 * Copyright (c) 2012-2014, Technische Universitaet Muenchen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @section DESCRIPTION
 * <DESCRIPTION>
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <immintrin.h>
#include <sys/time.h>

#include "gen_matmul_dense.hpp"

#ifndef ORDER_NUMBER
#define ORDER_NUMBER 56
#endif

#define REPS 10000

inline double sec(struct timeval start, struct timeval end) {
  return ((double)(((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)))) / 1.0e6;
}

void c_dgemm(double* a, double* b, double* c_gold, int exit_col) {
  int M;
#if defined(__SSE3__) && !defined(__AVX__)
  M = 2;

  switch (exit_col) {
    case 84:
      M = 56;
      break;

    case 56:
      M = 36;
      break;

    case 35:
      M = 20;
      break;

    case 20:
      M = 10;
      break;

    case 10:
      M = 4;
      break;

    case 4:
      M = 2;
      break;
  }

#endif
#if defined(__SSE3__) && defined(__AVX__)
  M = 4;

  switch (exit_col) {
    case 84:
      M = 56;
      break;

    case 56:
      M = 36;
      break;

    case 35:
      M = 20;
      break;

    case 20:
      M = 12;
      break;

    case 10:
      M = 4;
      break;

    case 4:
      M = 4;
      break;
  }

#endif
#if defined(__MIC__)
  M = 8;
  int K = 8;

  switch (exit_col) {
    case 84:
      M = 56;
      K = 84;
      break;

    case 56:
      M = 40;
      K = 56;
      break;

    case 35:
      M = 24;
      K = 40;
      break;

    case 20:
      M = 16;
      K = 24;
      break;

    case 10:
      M = 8;
      K = 16;
      break;

    case 4:
      M = 8;
      K = 8;
      break;
  }

  exit_col = K;
#endif

  for (int n = 0; n < 9; n++) {
    for (int k = 0; k < exit_col; k++) {
#pragma simd

      for (int m = 0; m < M; m++) {
        c_gold[(n * ORDER_NUMBER) + m] += a[(k * ORDER_NUMBER) + m] * b[(n * ORDER_NUMBER) + k];
      }
    }
  }
}

int main(int argc, char* argv[]) {
  // allocate
  double* a = (double*)_mm_malloc(ORDER_NUMBER * ORDER_NUMBER * sizeof(double), 64);
  double* b = (double*)_mm_malloc(ORDER_NUMBER * 9 * sizeof(double), 64);
  double* c = (double*)_mm_malloc(ORDER_NUMBER * 9 * sizeof(double), 64);
  double* c_gold = (double*)_mm_malloc(ORDER_NUMBER * 9 * sizeof(double), 64);

  // touch
  for (int i = 0; i < ORDER_NUMBER; i++) {
    for (int j = 0; j < ORDER_NUMBER; j++) {
      a[(j * ORDER_NUMBER) + i] = (double)(i + (j * ORDER_NUMBER));

      if (j < 9) {
        b[(j * ORDER_NUMBER) + i] = (double)(i + (j * 9));
        c[(j * ORDER_NUMBER) + i] = 0.0;
        c_gold[(j * ORDER_NUMBER) + i] = 0.0;
      }
    }
  }

  // C routine
  struct timeval start, end;
  gettimeofday(&start, NULL);

  for (int t = 0; t < REPS; t++) {
#if ORDER_NUMBER > 56
    c_dgemm(a, b, c_gold, 84);
#endif
#if ORDER_NUMBER > 35
    c_dgemm(a, b, c_gold, 56);
#endif
#if ORDER_NUMBER > 20
    c_dgemm(a, b, c_gold, 35);
#endif
#if ORDER_NUMBER > 10
    c_dgemm(a, b, c_gold, 20);
#endif
#if ORDER_NUMBER > 4
    c_dgemm(a, b, c_gold, 10);
#endif
#if ORDER_NUMBER > 1
    c_dgemm(a, b, c_gold, 4);
#endif
  }

  gettimeofday(&end, NULL);
  double total = sec(start, end);

  std::cout << total << "s for C" << std::endl;

  gettimeofday(&start, NULL);

  for (int t = 0; t < REPS; t++) {
#if ORDER_NUMBER > 56
    dense_test(a, b, c, 84);
#endif
#if ORDER_NUMBER > 35
    dense_test(a, b, c, 56);
#endif
#if ORDER_NUMBER > 20
    dense_test(a, b, c, 35);
#endif
#if ORDER_NUMBER > 10
    dense_test(a, b, c, 20);
#endif
#if ORDER_NUMBER > 4
    dense_test(a, b, c, 10);
#endif
#if ORDER_NUMBER > 1
    dense_test(a, b, c, 4);
#endif
  }

  gettimeofday(&end, NULL);
  total = sec(start, end);

  std::cout << total << "s for intrinsics" << std::endl;
#if ORDER_NUMBER == 56
  double gflops = 2.0*((56.0*36.0*9.0)+(35.0*20.0*9.0)+(20.0*12.0*9.0)+(10.0*4.0*9.0)+(4.0*4.0*9.0))*(double)REPS;
  std::cout << (gflops/(1000000000.0))/total << " GFLOPS for intrinsics" << std::endl;
#endif

  // check result
  double max_error = 0.0;

  for (int i = 0; i < ORDER_NUMBER; i++) {
    for (int j = 0; j < 9; j++) {
      //std::cout << c_gold[(j*ORDER_NUMBER)+i] << " " << c[(j*ORDER_NUMBER)+i] << std::endl;
      if (max_error < fabs( c_gold[(j * ORDER_NUMBER) + i] - c[(j * ORDER_NUMBER) + i]))
        max_error = fabs( c_gold[(j * ORDER_NUMBER) + i] - c[(j * ORDER_NUMBER) + i]);
    }
  }

  std::cout << "max. error: " << max_error << std::endl;

  // free
  _mm_free(a);
  _mm_free(b);
  _mm_free(c);
  _mm_free(c_gold);

  return 0;
}
