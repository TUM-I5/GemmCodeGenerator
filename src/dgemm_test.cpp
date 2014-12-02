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
#define ORDER_NUMBER 20
#endif

#define REPS 100000

inline double sec(struct timeval start, struct timeval end) {
  return ((double)(((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)))) / 1.0e6;
}


void run_A_square() {
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
    for (int n = 0; n < 9; n++) {
      for (int k = 0; k < ORDER_NUMBER; k++) {
#pragma simd

        for (int m = 0; m < ORDER_NUMBER; m++) {
          c_gold[(n * ORDER_NUMBER) + m] += a[(k * ORDER_NUMBER) + m] * b[(n * ORDER_NUMBER) + k];
        }
      }
    }
  }

  gettimeofday(&end, NULL);
  double total = sec(start, end);

  std::cout << total << "s for C" << std::endl;
  std::cout << ((double)(REPS * ORDER_NUMBER * ORDER_NUMBER) * 9.0 * 2.0) / (total * 1.0e9) << " GFLOPS for C" << std::endl;

  gettimeofday(&start, NULL);

  for (int t = 0; t < REPS; t++) {
    dense_test_square(a, b, c);
  }

  gettimeofday(&end, NULL);
  total = sec(start, end);

  std::cout << total << "s for initrinsics" << std::endl;
  std::cout << ((double)(REPS * ORDER_NUMBER * ORDER_NUMBER) * 9.0 * 2.0) / (total * 1.0e9) << " GFLOPS for initrinsics" << std::endl;

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
}

void run_A_rect() {
  // allocate
  double* a = (double*)_mm_malloc(ORDER_NUMBER * 9 * sizeof(double), 64);
  double* b = (double*)_mm_malloc(9 * 9 * sizeof(double), 64);
  double* c = (double*)_mm_malloc(ORDER_NUMBER * 9 * sizeof(double), 64);
  double* c_gold = (double*)_mm_malloc(ORDER_NUMBER * 9 * sizeof(double), 64);

  // touch
  for (int i = 0; i < ORDER_NUMBER; i++) {
    for (int j = 0; j < 9; j++) {
      a[(j * ORDER_NUMBER) + i] = (double)(i + (j * ORDER_NUMBER));
      c[(j * ORDER_NUMBER) + i] = 0.0;
      c_gold[(j * ORDER_NUMBER) + i] = 0.0;

      if (i < 9) b[(j * 9) + i] = (double)(i + (j * 9));
    }
  }

  // C routine
  struct timeval start, end;
  gettimeofday(&start, NULL);

  for (int t = 0; t < REPS; t++) {
    for (int n = 0; n < 9; n++) {
      for (int k = 0; k < 9; k++) {
#pragma simd

        for (int m = 0; m < ORDER_NUMBER; m++) {
          c_gold[(n * ORDER_NUMBER) + m] += a[(k * ORDER_NUMBER) + m] * b[(n * 9) + k];
        }
      }
    }
  }

  gettimeofday(&end, NULL);
  double total = sec(start, end);

  std::cout << total << "s for C" << std::endl;
  std::cout << ((double)(REPS * ORDER_NUMBER * 9.0) * 9.0 * 2.0) / (total * 1.0e9) << " GFLOPS for C" << std::endl;

  gettimeofday(&start, NULL);

  for (int t = 0; t < REPS; t++) {
    dense_test_rect(a, b, c);
  }

  gettimeofday(&end, NULL);
  total = sec(start, end);

  std::cout << total << "s for initrinsics" << std::endl;
  std::cout << ((double)(REPS * ORDER_NUMBER * 9.0) * 9.0 * 2.0) / (total * 1.0e9) << " GFLOPS for initrinsics" << std::endl;

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
}


int main(int argc, char* argv[]) {
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "RUNNING (" << ORDER_NUMBER << "x" << ORDER_NUMBER << ") X (" << ORDER_NUMBER << "x" << 9 << ") = (" << ORDER_NUMBER << "x" << 9 << ")"  << std::endl;
  std::cout << "------------------------------------------------" << std::endl;
  run_A_square();
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "RUNNING (" << ORDER_NUMBER << "x" << 9 << ") X (" << 9 << "x" << 9 << ") = (" << ORDER_NUMBER << "x" << 9 << ")"  << std::endl;
  std::cout << "------------------------------------------------" << std::endl;
  run_A_rect();
  std::cout << "------------------------------------------------" << std::endl;
  return 0;
}
