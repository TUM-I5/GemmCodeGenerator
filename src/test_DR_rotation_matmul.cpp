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

#ifndef ORDER_NUMBER
#define ORDER_NUMBER 56
#endif

#define REPS 100000

inline void dense_test_rect(double* A, double* B, double* C, double* A_prefetch = NULL, double* B_prefetch = NULL, double* C_prefetch = NULL)
{
__m256d c_0_0;
__m256d c_0_1;
__m256d c_0_2;
__m256d c_0_3;
__m256d c_1_0;
__m256d c_1_1;
__m256d c_1_2;
__m256d c_1_3;
__m256d b_0;
__m256d b_1;
__m256d b_2;
__m256d b_3;
__m256d a_0;
__m256d a_1;
__m256d a_2;
__m256d a_3;

double* c0 = C;
double* c1 = C+56;
double* c2 = C+112;
double* c3 = C+168;

  // 6x4 part of B
  for(int m = 0; m < 56; m+=8)
  {
    double* b0 = B;
    double* b1 = B+(1*9);
    double* b2 = B+(2*9);
    double* b3 = B+(3*9);
    double* a0 = A+m;
    c_0_0 = _mm256_load_pd(c0);
    c_0_1 = _mm256_load_pd(c1);
    c_0_2 = _mm256_load_pd(c2);
    c_0_3 = _mm256_load_pd(c3);

    c_1_0 = _mm256_load_pd(c0+4);
    c_1_1 = _mm256_load_pd(c1+4);
    c_1_2 = _mm256_load_pd(c2+4);
    c_1_3 = _mm256_load_pd(c3+4);

      // k=1
      b_0 = _mm256_broadcast_sd(b0);
      b_1 = _mm256_broadcast_sd(b1);
      b_2 = _mm256_broadcast_sd(b2);
      b_3 = _mm256_broadcast_sd(b3);

      b0++; b1++; b2++; b3++;

      a_0 = _mm256_load_pd(a0);
      a0+=4;
      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));
      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));
      c_0_2 = _mm256_add_pd(c_0_2, _mm256_mul_pd(a_0, b_2));
      c_0_3 = _mm256_add_pd(c_0_3, _mm256_mul_pd(a_0, b_3));

      a_1 = _mm256_load_pd(a0);
      a0+=52;
      c_1_0 = _mm256_add_pd(c_1_0, _mm256_mul_pd(a_1, b_0));
      c_1_1 = _mm256_add_pd(c_1_1, _mm256_mul_pd(a_1, b_1));
      c_1_2 = _mm256_add_pd(c_1_2, _mm256_mul_pd(a_1, b_2));
      c_1_3 = _mm256_add_pd(c_1_3, _mm256_mul_pd(a_1, b_3));

      // k=2
      b_0 = _mm256_broadcast_sd(b0);
      b_1 = _mm256_broadcast_sd(b1);
      b_2 = _mm256_broadcast_sd(b2);
      b_3 = _mm256_broadcast_sd(b3);

      b0++; b1++; b2++; b3++;

      a_0 = _mm256_load_pd(a0);
      a0+=4;
      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));
      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));
      c_0_2 = _mm256_add_pd(c_0_2, _mm256_mul_pd(a_0, b_2));
      c_0_3 = _mm256_add_pd(c_0_3, _mm256_mul_pd(a_0, b_3));

      a_1 = _mm256_load_pd(a0);
      a0+=52;
      c_1_0 = _mm256_add_pd(c_1_0, _mm256_mul_pd(a_1, b_0));
      c_1_1 = _mm256_add_pd(c_1_1, _mm256_mul_pd(a_1, b_1));
      c_1_2 = _mm256_add_pd(c_1_2, _mm256_mul_pd(a_1, b_2));
      c_1_3 = _mm256_add_pd(c_1_3, _mm256_mul_pd(a_1, b_3));

      // k=3
      b_0 = _mm256_broadcast_sd(b0);
      b_1 = _mm256_broadcast_sd(b1);
      b_2 = _mm256_broadcast_sd(b2);
      b_3 = _mm256_broadcast_sd(b3);

      b0++; b1++; b2++; b3++;

      a_0 = _mm256_load_pd(a0);
      a0+=4;
      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));
      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));
      c_0_2 = _mm256_add_pd(c_0_2, _mm256_mul_pd(a_0, b_2));
      c_0_3 = _mm256_add_pd(c_0_3, _mm256_mul_pd(a_0, b_3));

      a_1 = _mm256_load_pd(a0);
      a0+=52;
      c_1_0 = _mm256_add_pd(c_1_0, _mm256_mul_pd(a_1, b_0));
      c_1_1 = _mm256_add_pd(c_1_1, _mm256_mul_pd(a_1, b_1));
      c_1_2 = _mm256_add_pd(c_1_2, _mm256_mul_pd(a_1, b_2));
      c_1_3 = _mm256_add_pd(c_1_3, _mm256_mul_pd(a_1, b_3));

      // k=4
      b_0 = _mm256_broadcast_sd(b0);
      b_1 = _mm256_broadcast_sd(b1);
      b_2 = _mm256_broadcast_sd(b2);
      b_3 = _mm256_broadcast_sd(b3);

      b0++; b1++; b2++; b3++;

      a_0 = _mm256_load_pd(a0);
      a0+=4;
      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));
      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));
      c_0_2 = _mm256_add_pd(c_0_2, _mm256_mul_pd(a_0, b_2));
      c_0_3 = _mm256_add_pd(c_0_3, _mm256_mul_pd(a_0, b_3));

      a_1 = _mm256_load_pd(a0);
      a0+=52;
      c_1_0 = _mm256_add_pd(c_1_0, _mm256_mul_pd(a_1, b_0));
      c_1_1 = _mm256_add_pd(c_1_1, _mm256_mul_pd(a_1, b_1));
      c_1_2 = _mm256_add_pd(c_1_2, _mm256_mul_pd(a_1, b_2));
      c_1_3 = _mm256_add_pd(c_1_3, _mm256_mul_pd(a_1, b_3));

      // k=5
      b_0 = _mm256_broadcast_sd(b0);
      b_1 = _mm256_broadcast_sd(b1);
      b_2 = _mm256_broadcast_sd(b2);
      b_3 = _mm256_broadcast_sd(b3);

      b0++; b1++; b2++; b3++;

      a_0 = _mm256_load_pd(a0);
      a0+=4;
      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));
      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));
      c_0_2 = _mm256_add_pd(c_0_2, _mm256_mul_pd(a_0, b_2));
      c_0_3 = _mm256_add_pd(c_0_3, _mm256_mul_pd(a_0, b_3));

      a_1 = _mm256_load_pd(a0);
      a0+=52;
      c_1_0 = _mm256_add_pd(c_1_0, _mm256_mul_pd(a_1, b_0));
      c_1_1 = _mm256_add_pd(c_1_1, _mm256_mul_pd(a_1, b_1));
      c_1_2 = _mm256_add_pd(c_1_2, _mm256_mul_pd(a_1, b_2));
      c_1_3 = _mm256_add_pd(c_1_3, _mm256_mul_pd(a_1, b_3));

      // k=6
      b_0 = _mm256_broadcast_sd(b0);
      b_1 = _mm256_broadcast_sd(b1);
      b_2 = _mm256_broadcast_sd(b2);
      b_3 = _mm256_broadcast_sd(b3);

      b0++; b1++; b2++; b3++;

      a_0 = _mm256_load_pd(a0);
      a0+=4;
      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));
      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));
      c_0_2 = _mm256_add_pd(c_0_2, _mm256_mul_pd(a_0, b_2));
      c_0_3 = _mm256_add_pd(c_0_3, _mm256_mul_pd(a_0, b_3));

      a_1 = _mm256_load_pd(a0);
      a0+=52;
      c_1_0 = _mm256_add_pd(c_1_0, _mm256_mul_pd(a_1, b_0));
      c_1_1 = _mm256_add_pd(c_1_1, _mm256_mul_pd(a_1, b_1));
      c_1_2 = _mm256_add_pd(c_1_2, _mm256_mul_pd(a_1, b_2));
      c_1_3 = _mm256_add_pd(c_1_3, _mm256_mul_pd(a_1, b_3));


    _mm256_store_pd(c0, c_0_0);
    _mm256_store_pd(c1, c_0_1);
    _mm256_store_pd(c2, c_0_2);
    _mm256_store_pd(c3, c_0_3);

    _mm256_store_pd(c0+4, c_1_0);
    _mm256_store_pd(c1+4, c_1_1);
    _mm256_store_pd(c2+4, c_1_2);
    _mm256_store_pd(c3+4, c_1_3);

    c0+=8;
    c1+=8;
    c2+=8;
    c3+=8;
  }

  c0 = C + (4*56);
  c1 = C + (5*56);

  // 3x2 part of B
  for(int m = 0; m < 56; m+=8)
  {
    double* b0 = B+(4*9)+6;
    double* b1 = B+(5*9)+6;
    double* a0 = A+(6*56)+m;
    c_0_0 = _mm256_load_pd(c0);
    c_0_1 = _mm256_load_pd(c1);

    c_1_0 = _mm256_load_pd(c0+4);
    c_1_1 = _mm256_load_pd(c1+4);

      // k=1
      b_0 = _mm256_broadcast_sd(b0);
      b_1 = _mm256_broadcast_sd(b1);

      b0++; b1++;

      a_0 = _mm256_load_pd(a0);
      a0+=4;
      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));
      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));

      a_1 = _mm256_load_pd(a0);
      a0+=52;
      c_1_0 = _mm256_add_pd(c_1_0, _mm256_mul_pd(a_1, b_0));
      c_1_1 = _mm256_add_pd(c_1_1, _mm256_mul_pd(a_1, b_1));

      // k=2
      b_0 = _mm256_broadcast_sd(b0);
      b_1 = _mm256_broadcast_sd(b1);

      b0++; b1++;

      a_0 = _mm256_load_pd(a0);
      a0+=4;
      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));
      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));

      a_1 = _mm256_load_pd(a0);
      a0+=52;
      c_1_0 = _mm256_add_pd(c_1_0, _mm256_mul_pd(a_1, b_0));
      c_1_1 = _mm256_add_pd(c_1_1, _mm256_mul_pd(a_1, b_1));

      // k=3
      b_0 = _mm256_broadcast_sd(b0);
      b_1 = _mm256_broadcast_sd(b1);

      b0++; b1++;

      a_0 = _mm256_load_pd(a0);
      a0+=4;
      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));
      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));

      a_1 = _mm256_load_pd(a0);
      a0+=52;
      c_1_0 = _mm256_add_pd(c_1_0, _mm256_mul_pd(a_1, b_0));
      c_1_1 = _mm256_add_pd(c_1_1, _mm256_mul_pd(a_1, b_1));



    _mm256_store_pd(c0, c_0_0);
    _mm256_store_pd(c1, c_0_1);

    _mm256_store_pd(c0+4, c_1_0);
    _mm256_store_pd(c1+4, c_1_1);

    c0+=8;
    c1+=8;
  }
}

inline double sec(struct timeval start, struct timeval end) {
  return ((double)(((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)))) / 1.0e6;
}

void run_A_rect() {
  // allocate
  double* a = (double*)_mm_malloc(ORDER_NUMBER * 9 * sizeof(double), 64);
  double* b = (double*)_mm_malloc(9 * 6 * sizeof(double), 64);
  double* c = (double*)_mm_malloc(ORDER_NUMBER * 6 * sizeof(double), 64);
  double* c_gold = (double*)_mm_malloc(ORDER_NUMBER * 6 * sizeof(double), 64);

  // touch
  for (int i = 0; i < ORDER_NUMBER; i++) {
    for (int j = 0; j < 9; j++) {
      a[(j * ORDER_NUMBER) + i] = (double)(i + (j * ORDER_NUMBER));
    }
  }

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 6; j++) {
      b[(j * 9) + i] = (double)(i + (j * 9));
    }
  }
  
  // set zero blocks in B
  for (int i = 6; i < 9; i++) {
    for (int j = 0; j < 4; j++) {
      b[(j * 9) + i] = 0.0;
    }
  }

  for (int i = 0; i < 6; i++) {
    for (int j = 4; j < 6; j++) {
      b[(j * 9) + i] = 0.0;
    }
  }

#if 0
  // print B
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 6; j++) {
      std::cout << b[(j * 9) + i] << " ";
    }
    std::cout << std::endl;
  }
#endif

  for (int i = 0; i < ORDER_NUMBER; i++) {
    for (int j = 0; j < 6; j++) {
      c[(j * ORDER_NUMBER) + i] = 0.0;
      c_gold[(j * ORDER_NUMBER) + i] = 0.0;
    }
  }

  // C routine
  struct timeval start, end;
  gettimeofday(&start, NULL);

  for (int t = 0; t < REPS; t++) {
    for (int n = 0; n < 6; n++) {
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
  //std::cout << ((double)(REPS * ORDER_NUMBER * 9.0) * 6.0 * 2.0) / (total * 1.0e9) << " GFLOPS for C" << std::endl;

  gettimeofday(&start, NULL);

  for (int t = 0; t < REPS; t++) {
    dense_test_rect(a, b, c);
  }

  gettimeofday(&end, NULL);
  total = sec(start, end);

  std::cout << total << "s for initrinsics" << std::endl;
  //std::cout << ((double)(REPS * ORDER_NUMBER * 9.0) * 6.0 * 2.0) / (total * 1.0e9) << " GFLOPS for initrinsics" << std::endl;

  // check result
  double max_error = 0.0;

  for (int i = 0; i < ORDER_NUMBER; i++) {
    for (int j = 0; j < 6; j++) {
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
  std::cout << "RUNNING (" << ORDER_NUMBER << "x" << 9 << ") X (" << 9 << "x" << 6 << ") = (" << ORDER_NUMBER << "x" << 6 << ") (dense blocks)"  << std::endl;
  std::cout << "------------------------------------------------" << std::endl;
  run_A_rect();
  std::cout << "------------------------------------------------" << std::endl;
  return 0;
}

