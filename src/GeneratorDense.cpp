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

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <assert.h>

#include "GeneratorDense.hpp"

#define FULL_UNROLL

namespace seissolgen {

  GeneratorDense::GeneratorDense() : bGenerateExitForCK_(false), bAdd_(true) {
  }

  GeneratorDense::GeneratorDense(bool bGenerateExitForCK, int nMaxOrder, bool bAdd) : bGenerateExitForCK_(bGenerateExitForCK), bAdd_(bAdd) {
    for (int i = 2; i < nMaxOrder; i++) {
      int numBasisfunctions = ((i) * (i + 1) * (i + 2)) / 6;
      BasisfunctionsCounter_.push_back(numBasisfunctions);
    }
  }

  void sse_inner_blocked_kernel_6(std::stringstream& codestream, int lda, bool alignA) {
    codestream << "      b_0 = _mm_loaddup_pd(b0);" << std::endl;
    codestream << "      b_1 = _mm_loaddup_pd(b1);" << std::endl;
    codestream << "      b_2 = _mm_loaddup_pd(b2);" << std::endl << std::endl;
    codestream << "      b0++; b1++; b2++;" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_0 = _mm_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_0 = _mm_loadu_pd(a0);" << std::endl;
    }

    codestream << "      a0+=" << 2 << ";" << std::endl;
    codestream << "      c_0_0 = _mm_add_pd(c_0_0, _mm_mul_pd(a_0, b_0));" << std::endl;
    codestream << "      c_0_1 = _mm_add_pd(c_0_1, _mm_mul_pd(a_0, b_1));" << std::endl;
    codestream << "      c_0_2 = _mm_add_pd(c_0_2, _mm_mul_pd(a_0, b_2));" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_1 = _mm_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_1 = _mm_loadu_pd(a0);" << std::endl;
    }

    codestream << "      a0+=" << 2 << ";" << std::endl;
    codestream << "      c_1_0 = _mm_add_pd(c_1_0, _mm_mul_pd(a_1, b_0));" << std::endl;
    codestream << "      c_1_1 = _mm_add_pd(c_1_1, _mm_mul_pd(a_1, b_1));" << std::endl;
    codestream << "      c_1_2 = _mm_add_pd(c_1_2, _mm_mul_pd(a_1, b_2));" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_2 = _mm_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_2 = _mm_loadu_pd(a0);" << std::endl;
    }

    codestream << "      a0+=" << lda - 4 << ";" << std::endl;
    codestream << "      c_2_0 = _mm_add_pd(c_2_0, _mm_mul_pd(a_2, b_0));" << std::endl;
    codestream << "      c_2_1 = _mm_add_pd(c_2_1, _mm_mul_pd(a_2, b_1));" << std::endl;
    codestream << "      c_2_2 = _mm_add_pd(c_2_2, _mm_mul_pd(a_2, b_2));" << std::endl << std::endl;
  }

  void sse_inner_blocked_kernel_4(std::stringstream& codestream, int lda, bool alignA) {
    codestream << "      b_0 = _mm_loaddup_pd(b0);" << std::endl;
    codestream << "      b_1 = _mm_loaddup_pd(b1);" << std::endl;
    codestream << "      b_2 = _mm_loaddup_pd(b2);" << std::endl << std::endl;
    codestream << "      b0++; b1++; b2++;" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_0 = _mm_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_0 = _mm_loadu_pd(a0);" << std::endl;
    }

    codestream << "      a0+=" << 2 << ";" << std::endl;
    codestream << "      c_0_0 = _mm_add_pd(c_0_0, _mm_mul_pd(a_0, b_0));" << std::endl;
    codestream << "      c_0_1 = _mm_add_pd(c_0_1, _mm_mul_pd(a_0, b_1));" << std::endl;
    codestream << "      c_0_2 = _mm_add_pd(c_0_2, _mm_mul_pd(a_0, b_2));" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_1 = _mm_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_1 = _mm_loadu_pd(a0);" << std::endl;
    }

    codestream << "      a0+=" << lda - 2 << ";" << std::endl;
    codestream << "      c_1_0 = _mm_add_pd(c_1_0, _mm_mul_pd(a_1, b_0));" << std::endl;
    codestream << "      c_1_1 = _mm_add_pd(c_1_1, _mm_mul_pd(a_1, b_1));" << std::endl;
    codestream << "      c_1_2 = _mm_add_pd(c_1_2, _mm_mul_pd(a_1, b_2));" << std::endl << std::endl;
  }

  void sse_inner_blocked_kernel_2(std::stringstream& codestream, int lda, bool alignA) {
    codestream << "      b_0 = _mm_loaddup_pd(b0);" << std::endl;
    codestream << "      b_1 = _mm_loaddup_pd(b1);" << std::endl;
    codestream << "      b_2 = _mm_loaddup_pd(b2);" << std::endl << std::endl;
    codestream << "      b0++; b1++; b2++;" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_0 = _mm_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_0 = _mm_loadu_pd(a0);" << std::endl;
    }

    codestream << "      a0+=" << lda << ";" << std::endl;
    codestream << "      c_0_0 = _mm_add_pd(c_0_0, _mm_mul_pd(a_0, b_0));" << std::endl;
    codestream << "      c_0_1 = _mm_add_pd(c_0_1, _mm_mul_pd(a_0, b_1));" << std::endl;
    codestream << "      c_0_2 = _mm_add_pd(c_0_2, _mm_mul_pd(a_0, b_2));" << std::endl << std::endl;
  }

  void sse_inner_blocked_kernel_1(std::stringstream& codestream, int lda, bool alignA) {
    codestream << "      b_0 = _mm_load_sd(b0);" << std::endl;
    codestream << "      b_1 = _mm_load_sd(b1);" << std::endl;
    codestream << "      b_2 = _mm_load_sd(b2);" << std::endl << std::endl;
    codestream << "      b0++; b1++; b2++;" << std::endl << std::endl;

    codestream << "      a_0 = _mm_load_sd(a0);" << std::endl;

    codestream << "      a0+=" << lda << ";" << std::endl;
    codestream << "      c_0_0 = _mm_add_sd(c_0_0, _mm_mul_sd(a_0, b_0));" << std::endl;
    codestream << "      c_0_1 = _mm_add_sd(c_0_1, _mm_mul_sd(a_0, b_1));" << std::endl;
    codestream << "      c_0_2 = _mm_add_sd(c_0_2, _mm_mul_sd(a_0, b_2));" << std::endl << std::endl;
  }

  void avx2_inner_blocked_kernel_16(std::stringstream& codestream, int lda, bool alignA, bool preC) {
    codestream << "      b_0 = _mm256_broadcast_sd(b0);" << std::endl;
    codestream << "      b_1 = _mm256_broadcast_sd(b1);" << std::endl;
    codestream << "      b_2 = _mm256_broadcast_sd(b2);" << std::endl << std::endl;
    codestream << "      b0++; b1++; b2++;" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_0 = _mm256_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_0 = _mm256_loadu_pd(a0);" << std::endl;
    }
    
    if (preC) {
      codestream << "      _mm_prefetch((const char*)c0, _MM_HINT_T0);" << std::endl;
      codestream << "      _mm_prefetch((const char*)c1, _MM_HINT_T0);" << std::endl;
      codestream << "      _mm_prefetch((const char*)c2, _MM_HINT_T0);" << std::endl;
    }
    //codestream << "      _mm_prefetch((const char*)a0+" << lda*2 << ", _MM_HINT_T0);" << std::endl;
    codestream << "      a0+=" << 4 << ";" << std::endl;
    codestream << "      c_0_0 = _mm256_fmadd_pd(a_0, b_0, c_0_0);" << std::endl;
    codestream << "      c_0_1 = _mm256_fmadd_pd(a_0, b_1, c_0_1);" << std::endl;
    codestream << "      c_0_2 = _mm256_fmadd_pd(a_0, b_2, c_0_2);" << std::endl;

    if (alignA == true) {
      codestream << "      a_1 = _mm256_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_1 = _mm256_loadu_pd(a0);" << std::endl;
    }

    if (preC) {
      codestream << "      _mm_prefetch((const char*)c0+4, _MM_HINT_T0);" << std::endl;
      codestream << "      _mm_prefetch((const char*)c1+4, _MM_HINT_T0);" << std::endl;
      codestream << "      _mm_prefetch((const char*)c2+4, _MM_HINT_T0);" << std::endl;
    }
    codestream << "      a0+=" << 4 << ";" << std::endl;
    //codestream << "      _mm_prefetch((const char*)a0+" << lda*2 << ", _MM_HINT_T0);" << std::endl;
    codestream << "      c_1_0 = _mm256_fmadd_pd(a_1, b_0, c_1_0);" << std::endl;
    codestream << "      c_1_1 = _mm256_fmadd_pd(a_1, b_1, c_1_1);" << std::endl;
    codestream << "      c_1_2 = _mm256_fmadd_pd(a_1, b_2, c_1_2);" << std::endl;

    if (alignA == true) {
      codestream << "      a_2 = _mm256_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_2 = _mm256_loadu_pd(a0);" << std::endl;
    }

    if (preC) {
      codestream << "      _mm_prefetch((const char*)c0+8, _MM_HINT_T0);" << std::endl;
      codestream << "      _mm_prefetch((const char*)c1+8, _MM_HINT_T0);" << std::endl;
      codestream << "      _mm_prefetch((const char*)c2+8, _MM_HINT_T0);" << std::endl;
    }
    codestream << "      a0+=" << 4 << ";" << std::endl;
    codestream << "      c_2_0 = _mm256_fmadd_pd(a_2, b_0, c_2_0);" << std::endl;
    codestream << "      c_2_1 = _mm256_fmadd_pd(a_2, b_1, c_2_1);" << std::endl;
    codestream << "      c_2_2 = _mm256_fmadd_pd(a_2, b_2, c_2_2);" << std::endl;

    if (alignA == true) {
      codestream << "      a_3 = _mm256_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_3 = _mm256_loadu_pd(a0);" << std::endl;
    }

    if (preC) {
      codestream << "      _mm_prefetch((const char*)c0+12, _MM_HINT_T0);" << std::endl;
      codestream << "      _mm_prefetch((const char*)c1+12, _MM_HINT_T0);" << std::endl;
      codestream << "      _mm_prefetch((const char*)c2+12, _MM_HINT_T0);" << std::endl;
    }
    codestream << "      a0+=" << lda - 12 << ";" << std::endl;
    codestream << "      c_3_0 = _mm256_fmadd_pd(a_3, b_0, c_3_0);" << std::endl;
    codestream << "      c_3_1 = _mm256_fmadd_pd(a_3, b_1, c_3_1);" << std::endl;
    codestream << "      c_3_2 = _mm256_fmadd_pd(a_3, b_2, c_3_2);" << std::endl;

  }


  void avx_inner_blocked_kernel_12(std::stringstream& codestream, int lda, bool alignA) {
    codestream << "      b_0 = _mm256_broadcast_sd(b0);" << std::endl;
    codestream << "      b_1 = _mm256_broadcast_sd(b1);" << std::endl;
    codestream << "      b_2 = _mm256_broadcast_sd(b2);" << std::endl << std::endl;
    codestream << "      b0++; b1++; b2++;" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_0 = _mm256_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_0 = _mm256_loadu_pd(a0);" << std::endl;
    }

    codestream << "      a0+=" << 4 << ";" << std::endl;
    codestream << "#ifdef __AVX2__" << std::endl;
    codestream << "      c_0_0 = _mm256_fmadd_pd(a_0, b_0, c_0_0);" << std::endl;
    codestream << "      c_0_1 = _mm256_fmadd_pd(a_0, b_1, c_0_1);" << std::endl;
    codestream << "      c_0_2 = _mm256_fmadd_pd(a_0, b_2, c_0_2);" << std::endl;
    codestream << "#else" << std::endl;
    codestream << "      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));" << std::endl;
    codestream << "      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));" << std::endl;
    codestream << "      c_0_2 = _mm256_add_pd(c_0_2, _mm256_mul_pd(a_0, b_2));" << std::endl;
    codestream << "#endif" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_1 = _mm256_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_1 = _mm256_loadu_pd(a0);" << std::endl;
    }

    codestream << "      a0+=" << 4 << ";" << std::endl;
    codestream << "#ifdef __AVX2__" << std::endl;
    codestream << "      c_1_0 = _mm256_fmadd_pd(a_1, b_0, c_1_0);" << std::endl;
    codestream << "      c_1_1 = _mm256_fmadd_pd(a_1, b_1, c_1_1);" << std::endl;
    codestream << "      c_1_2 = _mm256_fmadd_pd(a_1, b_2, c_1_2);" << std::endl;
    codestream << "#else" << std::endl;
    codestream << "      c_1_0 = _mm256_add_pd(c_1_0, _mm256_mul_pd(a_1, b_0));" << std::endl;
    codestream << "      c_1_1 = _mm256_add_pd(c_1_1, _mm256_mul_pd(a_1, b_1));" << std::endl;
    codestream << "      c_1_2 = _mm256_add_pd(c_1_2, _mm256_mul_pd(a_1, b_2));" << std::endl;
    codestream << "#endif" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_2 = _mm256_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_2 = _mm256_loadu_pd(a0);" << std::endl;
    }

    codestream << "      a0+=" << lda - 8 << ";" << std::endl;
    codestream << "#ifdef __AVX2__" << std::endl;
    codestream << "      c_2_0 = _mm256_fmadd_pd(a_2, b_0, c_2_0);" << std::endl;
    codestream << "      c_2_1 = _mm256_fmadd_pd(a_2, b_1, c_2_1);" << std::endl;
    codestream << "      c_2_2 = _mm256_fmadd_pd(a_2, b_2, c_2_2);" << std::endl;
    codestream << "#else" << std::endl;
    codestream << "      c_2_0 = _mm256_add_pd(c_2_0, _mm256_mul_pd(a_2, b_0));" << std::endl;
    codestream << "      c_2_1 = _mm256_add_pd(c_2_1, _mm256_mul_pd(a_2, b_1));" << std::endl;
    codestream << "      c_2_2 = _mm256_add_pd(c_2_2, _mm256_mul_pd(a_2, b_2));" << std::endl;
    codestream << "#endif" << std::endl << std::endl;
  }

  void avx_inner_blocked_kernel_8(std::stringstream& codestream, int lda, bool alignA) {
    codestream << "      b_0 = _mm256_broadcast_sd(b0);" << std::endl;
    codestream << "      b_1 = _mm256_broadcast_sd(b1);" << std::endl;
    codestream << "      b_2 = _mm256_broadcast_sd(b2);" << std::endl << std::endl;
    codestream << "      b0++; b1++; b2++;" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_0 = _mm256_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_0 = _mm256_loadu_pd(a0);" << std::endl;
    }

    //codestream << "      _mm_prefetch((const char*)a0+" << lda*2 << ", _MM_HINT_T0);" << std::endl;
    codestream << "      a0+=" << 4 << ";" << std::endl;
    codestream << "#ifdef __AVX2__" << std::endl;
    codestream << "      c_0_0 = _mm256_fmadd_pd(a_0, b_0, c_0_0);" << std::endl;
    codestream << "      c_0_1 = _mm256_fmadd_pd(a_0, b_1, c_0_1);" << std::endl;
    codestream << "      c_0_2 = _mm256_fmadd_pd(a_0, b_2, c_0_2);" << std::endl;
    codestream << "#else" << std::endl;
    codestream << "      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));" << std::endl;
    codestream << "      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));" << std::endl;
    codestream << "      c_0_2 = _mm256_add_pd(c_0_2, _mm256_mul_pd(a_0, b_2));" << std::endl;
    codestream << "#endif" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_1 = _mm256_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_1 = _mm256_loadu_pd(a0);" << std::endl;
    }

    codestream << "      a0+=" << lda - 4  << ";" << std::endl;
    codestream << "#ifdef __AVX2__" << std::endl;
    codestream << "      c_1_0 = _mm256_fmadd_pd(a_1, b_0, c_1_0);" << std::endl;
    codestream << "      c_1_1 = _mm256_fmadd_pd(a_1, b_1, c_1_1);" << std::endl;
    codestream << "      c_1_2 = _mm256_fmadd_pd(a_1, b_2, c_1_2);" << std::endl;
    codestream << "#else" << std::endl;
    codestream << "      c_1_0 = _mm256_add_pd(c_1_0, _mm256_mul_pd(a_1, b_0));" << std::endl;
    codestream << "      c_1_1 = _mm256_add_pd(c_1_1, _mm256_mul_pd(a_1, b_1));" << std::endl;
    codestream << "      c_1_2 = _mm256_add_pd(c_1_2, _mm256_mul_pd(a_1, b_2));" << std::endl;
    codestream << "#endif" << std::endl << std::endl;
  }

  void avx_inner_blocked_kernel_4(std::stringstream& codestream, int lda, bool alignA) {
    codestream << "      b_0 = _mm256_broadcast_sd(b0);" << std::endl;
    codestream << "      b_1 = _mm256_broadcast_sd(b1);" << std::endl;
    codestream << "      b_2 = _mm256_broadcast_sd(b2);" << std::endl << std::endl;
    codestream << "      b0++; b1++; b2++;" << std::endl << std::endl;

    if (alignA == true) {
      codestream << "      a_0 = _mm256_load_pd(a0);" << std::endl;
    } else {
      codestream << "      a_0 = _mm256_loadu_pd(a0);" << std::endl;
    }

    codestream << "      a0+=" << lda << ";" << std::endl;
    codestream << "#ifdef __AVX2__" << std::endl;
    codestream << "      c_0_0 = _mm256_fmadd_pd(a_0, b_0, c_0_0);" << std::endl;
    codestream << "      c_0_1 = _mm256_fmadd_pd(a_0, b_1, c_0_1);" << std::endl;
    codestream << "      c_0_2 = _mm256_fmadd_pd(a_0, b_2, c_0_2);" << std::endl;
    codestream << "#else" << std::endl;
    codestream << "      c_0_0 = _mm256_add_pd(c_0_0, _mm256_mul_pd(a_0, b_0));" << std::endl;
    codestream << "      c_0_1 = _mm256_add_pd(c_0_1, _mm256_mul_pd(a_0, b_1));" << std::endl;
    codestream << "      c_0_2 = _mm256_add_pd(c_0_2, _mm256_mul_pd(a_0, b_2));" << std::endl;
    codestream << "#endif" << std::endl << std::endl;
  }

  void avx_inner_blocked_kernel_1(std::stringstream& codestream, int lda, bool alignA) {
    codestream << "      b_0_128 = _mm_load_sd(b0);" << std::endl;
    codestream << "      b_1_128 = _mm_load_sd(b1);" << std::endl;
    codestream << "      b_2_128 = _mm_load_sd(b2);" << std::endl << std::endl;
    codestream << "      b0++; b1++; b2++;" << std::endl << std::endl;

    codestream << "      a_0_128 = _mm_load_sd(a0);" << std::endl;

    codestream << "      a0+=" << lda << ";" << std::endl;
    codestream << "#ifdef __AVX2__" << std::endl;
    codestream << "      c_0_0_128 = _mm_fmadd_sd(a_0_128, b_0_128, c_0_0_128);" << std::endl;
    codestream << "      c_0_1_128 = _mm_fmadd_sd(a_0_128, b_1_128, c_0_1_128);" << std::endl;
    codestream << "      c_0_2_128 = _mm_fmadd_sd(a_0_128, b_2_128, c_0_2_128);" << std::endl;
    codestream << "#else" << std::endl;
    codestream << "      c_0_0_128 = _mm_add_sd(c_0_0_128, _mm_mul_sd(a_0_128, b_0_128));" << std::endl;
    codestream << "      c_0_1_128 = _mm_add_sd(c_0_1_128, _mm_mul_sd(a_0_128, b_1_128));" << std::endl;
    codestream << "      c_0_2_128 = _mm_add_sd(c_0_2_128, _mm_mul_sd(a_0_128, b_2_128));" << std::endl;
    codestream << "#endif" << std::endl << std::endl;
  }

  void mic_inner_blocked_kernel_8_L1(std::stringstream& codestream, int lda, int ldb, int cnt) {
    if (cnt > 7) {
      std::cout << "MIC inner kernel unroll is greater than 7 --> Exit! Please fix Code Generator" << std::endl;
      exit(-1);
    }

    codestream << "    _mm_prefetch((const char*) (cur_A + 8), _MM_HINT_T1);" << std::endl;
    codestream << "    a_0 = _mm512_load_pd(cur_A);" << std::endl;
    codestream << "    cur_A += " << lda << ";" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (0*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_0 = _mm512_fmadd_pd(a_0, b_bcst, c_0);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (1*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    _mm_prefetch((const char*)cur_A, _MM_HINT_T0);" << std::endl;
    codestream << "    c_1 = _mm512_fmadd_pd(a_0, b_bcst, c_1);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (2*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_2 = _mm512_fmadd_pd(a_0, b_bcst, c_2);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (3*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_3 = _mm512_fmadd_pd(a_0, b_bcst, c_3);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (4*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_4 = _mm512_fmadd_pd(a_0, b_bcst, c_4);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (5*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    _mm_prefetch((const char*)pre_B_L1 + (" << cnt << "*" << ldb << "), _MM_HINT_T0);" << std::endl;
    codestream << "    c_5 = _mm512_fmadd_pd(a_0, b_bcst, c_5);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (6*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_6 = _mm512_fmadd_pd(a_0, b_bcst, c_6);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (7*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_7 = _mm512_fmadd_pd(a_0, b_bcst, c_7);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (8*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_8 = _mm512_fmadd_pd(a_0, b_bcst, c_8);" << std::endl;

    if (cnt == 7) {
      codestream << "    _mm_prefetch((const char*)pre_B_L1 + (" << cnt + 1 << "*" << ldb << "), _MM_HINT_T0);" << std::endl;
      codestream << "    pre_B_L1+=8;" << std::endl;
    }

    codestream << "    cur_B++;" << std::endl << std::endl;
  }

  void mic_inner_blocked_kernel_8_noprefetch_B(std::stringstream& codestream, int lda, int ldb, int cnt) {
    codestream << "    _mm_prefetch((const char*) (cur_A + 8), _MM_HINT_T1);" << std::endl;
    codestream << "    a_0 = _mm512_load_pd(cur_A);" << std::endl;
    codestream << "    cur_A += " << lda << ";" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (0*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_0 = _mm512_fmadd_pd(a_0, b_bcst, c_0);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (1*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    _mm_prefetch((const char*)cur_A, _MM_HINT_T0);" << std::endl;
    codestream << "    c_1 = _mm512_fmadd_pd(a_0, b_bcst, c_1);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (2*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_2 = _mm512_fmadd_pd(a_0, b_bcst, c_2);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (3*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_3 = _mm512_fmadd_pd(a_0, b_bcst, c_3);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (4*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_4 = _mm512_fmadd_pd(a_0, b_bcst, c_4);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (5*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_5 = _mm512_fmadd_pd(a_0, b_bcst, c_5);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (6*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_6 = _mm512_fmadd_pd(a_0, b_bcst, c_6);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (7*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_7 = _mm512_fmadd_pd(a_0, b_bcst, c_7);" << std::endl;
    codestream << "    b_bcst = _mm512_extload_pd(cur_B + (8*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "    c_8 = _mm512_fmadd_pd(a_0, b_bcst, c_8);" << std::endl;
    codestream << "    cur_B++;" << std::endl << std::endl;
  }

  void mic_inner_blocked_kernel_8_L2(std::stringstream& codestream, int lda, int ldb, int cnt) {
    if (cnt > 7) {
      std::cout << "MIC inner kernel unroll is greater than 7 --> Exit! Please fix Code Generator" << std::endl;
      exit(-1);
    }

    codestream << "  _mm_prefetch((const char*)pre_A_L2, _MM_HINT_T2);" << std::endl;
    codestream << "  a_0 = _mm512_load_pd(cur_A);" << std::endl;
    codestream << "  cur_A += " << lda << ";" << std::endl;
    codestream << "  pre_A_L2 += " << lda << ";" << std::endl;
    codestream << "  b_bcst = _mm512_extload_pd(cur_B + (0*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "  c_0 = _mm512_fmadd_pd(a_0, b_bcst, c_0);" << std::endl;
    codestream << "  b_bcst = _mm512_extload_pd(cur_B + (1*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "  _mm_prefetch((const char*)cur_A, _MM_HINT_T0);" << std::endl;
    codestream << "  c_1 = _mm512_fmadd_pd(a_0, b_bcst, c_1);" << std::endl;
    codestream << "  b_bcst = _mm512_extload_pd(cur_B + (2*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "  c_2 = _mm512_fmadd_pd(a_0, b_bcst, c_2);" << std::endl;
    codestream << "  b_bcst = _mm512_extload_pd(cur_B + (3*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "  _mm_prefetch((const char*)pre_B_L2 + (" << cnt << "*56), _MM_HINT_T1);" << std::endl;
    codestream << "  c_3 = _mm512_fmadd_pd(a_0, b_bcst, c_3);" << std::endl;
    codestream << "  b_bcst = _mm512_extload_pd(cur_B + (4*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "  c_4 = _mm512_fmadd_pd(a_0, b_bcst, c_4);" << std::endl;
    codestream << "  b_bcst = _mm512_extload_pd(cur_B + (5*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "  //_mm_prefetch((const char*)pre_B_L1 + (" << cnt << "*" << ldb << "), _MM_HINT_T0);" << std::endl;
    codestream << "  c_5 = _mm512_fmadd_pd(a_0, b_bcst, c_5);" << std::endl;
    codestream << "  b_bcst = _mm512_extload_pd(cur_B + (6*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "  c_6 = _mm512_fmadd_pd(a_0, b_bcst, c_6);" << std::endl;
    codestream << "  b_bcst = _mm512_extload_pd(cur_B + (7*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "  c_7 = _mm512_fmadd_pd(a_0, b_bcst, c_7);" << std::endl;

    if (cnt == 7) codestream << "  _mm_prefetch((const char*)pre_B_L2 + (" << cnt + 1 << "*56), _MM_HINT_T1);" << std::endl;

    codestream << "  b_bcst = _mm512_extload_pd(cur_B + (8*" << ldb << "), _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, _MM_HINT_NONE);" << std::endl;
    codestream << "  c_8 = _mm512_fmadd_pd(a_0, b_bcst, c_8);" << std::endl;

    if (cnt == 7) {
      codestream << "  //_mm_prefetch((const char*)pre_B_L1 + (" << cnt + 1 << "*" << ldb << "), _MM_HINT_T0);" << std::endl;
      codestream << "  //pre_B_L1+=8;" << std::endl;
      codestream << "  pre_B_L2+=8;" << std::endl;
    }

    codestream << "  cur_B++;" << std::endl << std::endl;
  }

  std::string GeneratorDense::generate_dense(bool bIsColMajor, int M, int N, int K, int lda, int ldb, int ldc) {
    std::stringstream codestream;
    int mDone = 0;
    int mDone_old = 0;
    int mDoneAVX2 = 0;
    int mDoneAVX2_old = 0;
    int remainder = 0;
    int block_end = 0;
    int kb = 0;
    bool alignA = false;
    bool alignB = false;
    bool alignC = false;

#ifdef DEBUG
    std::cout << "Generating dense matrix multiplication" << std::endl;
    std::cout << "M=" << M << " N=" << N << " K=" << K << std::endl;
    std::cout << "lda=" << lda << " ldb=" << ldb << " ldc=" << ldc << std::endl;
#endif

    assert(N % 3 == 0);
    assert(bIsColMajor == true);

    // generating SSE3 code
    codestream << "#if defined(__SSE3__) && !defined(__AVX__)" << std::endl;
    codestream << "__m128d c_0_0;" << std::endl;
    codestream << "__m128d c_0_1;" << std::endl;
    codestream << "__m128d c_0_2;" << std::endl;
    codestream << "__m128d c_1_0;" << std::endl;
    codestream << "__m128d c_1_1;" << std::endl;
    codestream << "__m128d c_1_2;" << std::endl;
    codestream << "__m128d c_2_0;" << std::endl;
    codestream << "__m128d c_2_1;" << std::endl;
    codestream << "__m128d c_2_2;" << std::endl;
    codestream << "__m128d b_0;" << std::endl;
    codestream << "__m128d b_1;" << std::endl;
    codestream << "__m128d b_2;" << std::endl;
    codestream << "__m128d a_0;" << std::endl;
    codestream << "__m128d a_1;" << std::endl;
    codestream << "__m128d a_2;" << std::endl;
    codestream << "#endif" << std::endl << std::endl;
    // generating AVX code
    codestream << "#if defined(__SSE3__) && defined(__AVX__)" << std::endl;
    codestream << "__m256d c_0_0;" << std::endl;
    codestream << "__m256d c_0_1;" << std::endl;
    codestream << "__m256d c_0_2;" << std::endl;
    codestream << "__m256d c_1_0;" << std::endl;
    codestream << "__m256d c_1_1;" << std::endl;
    codestream << "__m256d c_1_2;" << std::endl;
    codestream << "__m256d c_2_0;" << std::endl;
    codestream << "__m256d c_2_1;" << std::endl;
    codestream << "__m256d c_2_2;" << std::endl;
    codestream << "__m256d c_3_0;" << std::endl;
    codestream << "__m256d c_3_1;" << std::endl;
    codestream << "__m256d c_3_2;" << std::endl;
    codestream << "__m256d b_0;" << std::endl;
    codestream << "__m256d b_1;" << std::endl;
    codestream << "__m256d b_2;" << std::endl;
    codestream << "__m256d a_0;" << std::endl;
    codestream << "__m256d a_1;" << std::endl;
    codestream << "__m256d a_2;" << std::endl;
    codestream << "__m256d a_3;" << std::endl;
    codestream << "__m128d b_0_128;" << std::endl;
    codestream << "__m128d b_1_128;" << std::endl;
    codestream << "__m128d b_2_128;" << std::endl;
    codestream << "__m128d a_0_128;" << std::endl;
    codestream << "__m128d a_1_128;" << std::endl;
    codestream << "__m128d a_2_128;" << std::endl;
    codestream << "__m128d c_0_0_128;" << std::endl;
    codestream << "__m128d c_0_1_128;" << std::endl;
    codestream << "__m128d c_0_2_128;" << std::endl;
    codestream << "#endif" << std::endl << std::endl;

    /////////////////////////
    /////////////////////////
    // generating SSE code //
    /////////////////////////
    /////////////////////////

    if (lda % 2 == 0)
      alignA = true;

    if (ldb % 2 == 0)
      alignB = true;

    if (ldc % 2 == 0)
      alignC = true;

    codestream << "#if defined(__SSE3__) && !defined(__AVX__)" << std::endl;
    codestream << "double* c0 = C;" << std::endl;
    codestream << "double* c1 = C+" << ldc << ";" << std::endl;
    codestream << "double* c2 = C+" << 2 * ldc << ";" << std::endl;

    if (this->bGenerateExitForCK_ == true) {
      codestream << "int M = 2;" << std::endl;
      codestream << "switch(exit_col)" << std::endl;
      codestream << "{" << std::endl;
      codestream << "  case 84:" << std::endl;
      codestream << "    M = 56;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 56:" << std::endl;
      codestream << "    M = 36;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 35:" << std::endl;
      codestream << "    M = 20;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 20:" << std::endl;
      codestream << "    M = 10;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 10:" << std::endl;
      codestream << "    M = 4;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 4:" << std::endl;
      codestream << "    M = 2;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "}" << std::endl;
    }

    codestream << "for(int n = 0; n < " << N << "; n+=3)" << std::endl;
    codestream << "{" << std::endl;

    // calculate the maximum number of row
    // we can process with max. blocking
    if (this->bGenerateExitForCK_ == true) {
      codestream << "  int mDone, mDone_old;" << std::endl;
      codestream << "  mDone = (M/6)*6;" << std::endl;
      codestream << "  for(int m = 0; m < mDone; m+=6)" << std::endl;
    } else {
      mDone = (M / 6) * 6;
      codestream << "  for(int m = 0; m < " << mDone << "; m+=6)" << std::endl;
    }

    codestream << "  {" << std::endl;
    codestream << "    double* b0 = B+(n*" << ldb << ");" << std::endl;
    codestream << "    double* b1 = B+((n+1)*" << ldb << ");" << std::endl;
    codestream << "    double* b2 = B+((n+2)*" << ldb << ");" << std::endl;
    codestream << "    double* a0 = A+m;" << std::endl;

    if (bAdd_) {
      if (alignC == true) {
        codestream << "    c_0_0 = _mm_load_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm_load_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm_load_pd(c2);" << std::endl;
        codestream << "    c_1_0 = _mm_load_pd(c0+2);" << std::endl;
        codestream << "    c_1_1 = _mm_load_pd(c1+2);" << std::endl;
        codestream << "    c_1_2 = _mm_load_pd(c2+2);" << std::endl;
        codestream << "    c_2_0 = _mm_load_pd(c0+4);" << std::endl;
        codestream << "    c_2_1 = _mm_load_pd(c1+4);" << std::endl;
        codestream << "    c_2_2 = _mm_load_pd(c2+4);" << std::endl << std::endl;
      } else {
        codestream << "    c_0_0 = _mm_loadu_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm_loadu_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm_loadu_pd(c2);" << std::endl;
        codestream << "    c_1_0 = _mm_loadu_pd(c0+2);" << std::endl;
        codestream << "    c_1_1 = _mm_loadu_pd(c1+2);" << std::endl;
        codestream << "    c_1_2 = _mm_loadu_pd(c2+2);" << std::endl;
        codestream << "    c_2_0 = _mm_loadu_pd(c0+4);" << std::endl;
        codestream << "    c_2_1 = _mm_loadu_pd(c1+4);" << std::endl;
        codestream << "    c_2_2 = _mm_loadu_pd(c2+4);" << std::endl << std::endl;
      }
    } else {
      codestream << "    c_0_0 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_0_1 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_0_2 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_1_0 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_1_1 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_1_2 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_2_0 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_2_1 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_2_2 = _mm_setzero_pd();" << std::endl << std::endl;
    }

#ifndef FULL_UNROLL

    if (this->bGenerateExitForCK_ == true) {
      codestream << "    for (int k = 0; k < exit_col; k++)" << std::endl;
    } else {
      codestream << "    for (int k = 0; k < " << K << "; k++)" << std::endl;
    }

    codestream << "    {" << std::endl;
#else

    for (int k = 0; k < K; k++) {
      if (this->bGenerateExitForCK_ == true) {
        for (int o = 0; o < this->BasisfunctionsCounter_.size(); o++) {
          if (k == this->BasisfunctionsCounter_[o]) {
            codestream << "      if ( __builtin_expect(exit_col == " << k << ", false) ) { goto sse_six_end; }" << std::endl;
          }
        }
      }

#endif
    sse_inner_blocked_kernel_6(codestream, lda, alignA);
#ifndef FULL_UNROLL
    codestream << "    }" << std::endl;
#else
  }

  if (this->bGenerateExitForCK_ == true && K > 4) {
    codestream << "sse_six_end:" << std::endl;
  }

#endif

    if (alignC == true) {
      codestream << "    _mm_store_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm_store_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm_store_pd(c2, c_0_2);" << std::endl;
      codestream << "    _mm_store_pd(c0+2, c_1_0);" << std::endl;
      codestream << "    _mm_store_pd(c1+2, c_1_1);" << std::endl;
      codestream << "    _mm_store_pd(c2+2, c_1_2);" << std::endl;
      codestream << "    _mm_store_pd(c0+4, c_2_0);" << std::endl;
      codestream << "    _mm_store_pd(c1+4, c_2_1);" << std::endl;
      codestream << "    _mm_store_pd(c2+4, c_2_2);" << std::endl;
    } else {
      codestream << "    _mm_storeu_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm_storeu_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm_storeu_pd(c2, c_0_2);" << std::endl;
      codestream << "    _mm_storeu_pd(c0+2, c_1_0);" << std::endl;
      codestream << "    _mm_storeu_pd(c1+2, c_1_1);" << std::endl;
      codestream << "    _mm_storeu_pd(c2+2, c_1_2);" << std::endl;
      codestream << "    _mm_storeu_pd(c0+4, c_2_0);" << std::endl;
      codestream << "    _mm_storeu_pd(c1+4, c_2_1);" << std::endl;
      codestream << "    _mm_storeu_pd(c2+4, c_2_2);" << std::endl;
    }

    codestream << "    c0+=6;" << std::endl;
    codestream << "    c1+=6;" << std::endl;
    codestream << "    c2+=6;" << std::endl;
    codestream << "  }" << std::endl;

    if (this->bGenerateExitForCK_ == true) {
      codestream << "  mDone_old = mDone;" << std::endl;
      codestream << "  mDone = mDone_old+(((M-mDone_old)/4)*4);" << std::endl;
      codestream << "  for(int m = mDone_old; m < mDone; m+=4)" << std::endl;
    } else {
      mDone_old = mDone;
      mDone = mDone_old + (((M - mDone_old) / 4) * 4);
      codestream << "  for(int m = " << mDone_old << "; m < " << mDone << "; m+=4)" << std::endl;
    }

    codestream << "  {" << std::endl;
    codestream << "    double* b0 = B+(n*" << ldb << ");" << std::endl;
    codestream << "    double* b1 = B+((n+1)*" << ldb << ");" << std::endl;
    codestream << "    double* b2 = B+((n+2)*" << ldb << ");" << std::endl;
    codestream << "    double* a0 = A+m;" << std::endl;

    if (bAdd_) {
      if (alignC == true) {
        codestream << "    c_0_0 = _mm_load_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm_load_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm_load_pd(c2);" << std::endl;
        codestream << "    c_1_0 = _mm_load_pd(c0+2);" << std::endl;
        codestream << "    c_1_1 = _mm_load_pd(c1+2);" << std::endl;
        codestream << "    c_1_2 = _mm_load_pd(c2+2);" << std::endl;
      } else {
        codestream << "    c_0_0 = _mm_loadu_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm_loadu_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm_loadu_pd(c2);" << std::endl;
        codestream << "    c_1_0 = _mm_loadu_pd(c0+2);" << std::endl;
        codestream << "    c_1_1 = _mm_loadu_pd(c1+2);" << std::endl;
        codestream << "    c_1_2 = _mm_loadu_pd(c2+2);" << std::endl;
      }
    } else {
      codestream << "    c_0_0 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_0_1 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_0_2 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_1_0 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_1_1 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_1_2 = _mm_setzero_pd();" << std::endl;
    }

#ifndef FULL_UNROLL

    if (this->bGenerateExitForCK_ == true) {
      codestream << "    for (int k = 0; k < exit_col; k++)" << std::endl;
    } else {
      codestream << "    for (int k = 0; k < " << K << "; k++)" << std::endl;
    }

    codestream << "    {" << std::endl;
#else

    for (int k = 0; k < K; k++) {
      if (this->bGenerateExitForCK_ == true) {
        for (int o = 0; o < this->BasisfunctionsCounter_.size(); o++) {
          if (k == this->BasisfunctionsCounter_[o]) {
            codestream << "      if ( __builtin_expect(exit_col == " << k << ", false) ) { goto sse_four_end; }" << std::endl;
          }
        }
      }

#endif
    sse_inner_blocked_kernel_4(codestream, lda, alignA);
#ifndef FULL_UNROLL
    codestream << "    }" << std::endl;
#else
  }

  if (this->bGenerateExitForCK_ == true && K > 4) {
    codestream << "sse_four_end:" << std::endl;
  }

#endif

    if (alignC == true) {
      codestream << "    _mm_store_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm_store_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm_store_pd(c2, c_0_2);" << std::endl;
      codestream << "    _mm_store_pd(c0+2, c_1_0);" << std::endl;
      codestream << "    _mm_store_pd(c1+2, c_1_1);" << std::endl;
      codestream << "    _mm_store_pd(c2+2, c_1_2);" << std::endl;
    } else {
      codestream << "    _mm_storeu_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm_storeu_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm_storeu_pd(c2, c_0_2);" << std::endl;
      codestream << "    _mm_storeu_pd(c0+2, c_1_0);" << std::endl;
      codestream << "    _mm_storeu_pd(c1+2, c_1_1);" << std::endl;
      codestream << "    _mm_storeu_pd(c2+2, c_1_2);" << std::endl;
    }

    codestream << "    c0+=4;" << std::endl;
    codestream << "    c1+=4;" << std::endl;
    codestream << "    c2+=4;" << std::endl;
    codestream << "  }" << std::endl;

    if (this->bGenerateExitForCK_ == true) {
      codestream << "  mDone_old = mDone;" << std::endl;
      codestream << "  mDone = mDone_old+(((M-mDone_old)/2)*2);" << std::endl;
      codestream << "  for(int m = mDone_old; m < mDone; m+=2)" << std::endl;
    } else {
      mDone_old = mDone;
      mDone = mDone_old + (((M - mDone_old) / 2) * 2);
      codestream << "  for(int m = " << mDone_old << "; m < " << mDone << "; m+=2)" << std::endl;
    }

    codestream << "  {" << std::endl;
    codestream << "    double* b0 = B+(n*" << ldb << ");" << std::endl;
    codestream << "    double* b1 = B+((n+1)*" << ldb << ");" << std::endl;
    codestream << "    double* b2 = B+((n+2)*" << ldb << ");" << std::endl;
    codestream << "    double* a0 = A+m;" << std::endl;

    if (bAdd_) {
      if (alignC == true) {
        codestream << "    c_0_0 = _mm_load_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm_load_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm_load_pd(c2);" << std::endl;
      } else {
        codestream << "    c_0_0 = _mm_loadu_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm_loadu_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm_loadu_pd(c2);" << std::endl;
      }
    } else {
      codestream << "    c_0_0 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_0_1 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_0_2 = _mm_setzero_pd();" << std::endl;
    }

#ifndef FULL_UNROLL

    if (this->bGenerateExitForCK_ == true) {
      codestream << "    for (int k = 0; k < exit_col; k++)" << std::endl;
    } else {
      codestream << "    for (int k = 0; k < " << K << "; k++)" << std::endl;
    }

    codestream << "    {" << std::endl;
#else

    for (int k = 0; k < K; k++) {
      if (this->bGenerateExitForCK_ == true) {
        for (int o = 0; o < this->BasisfunctionsCounter_.size(); o++) {
          if (k == this->BasisfunctionsCounter_[o]) {
            codestream << "      if ( __builtin_expect(exit_col == " << k << ", false) ) { goto sse_two_end; }" << std::endl;
          }
        }
      }

#endif
    sse_inner_blocked_kernel_2(codestream, lda, alignA);
#ifndef FULL_UNROLL
    codestream << "    }" << std::endl;
#else
  }

  if (this->bGenerateExitForCK_ == true && K > 4) {
    codestream << "sse_two_end:" << std::endl;
  }

#endif

    if (alignC == true) {
      codestream << "    _mm_store_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm_store_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm_store_pd(c2, c_0_2);" << std::endl;
    } else {
      codestream << "    _mm_storeu_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm_storeu_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm_storeu_pd(c2, c_0_2);" << std::endl;
    }

    codestream << "    c0+=2;" << std::endl;
    codestream << "    c1+=2;" << std::endl;
    codestream << "    c2+=2;" << std::endl;
    codestream << "  }" << std::endl;

    if (this->bGenerateExitForCK_ == true) {
      codestream << "  for(int m = mDone; m < M; m++)" << std::endl;
    } else {
      codestream << "  for(int m = " << mDone << "; m < " << M << "; m++)" << std::endl;
    }

    codestream << "  {" << std::endl;
    codestream << "    double* b0 = B+(n*" << ldb << ");" << std::endl;
    codestream << "    double* b1 = B+((n+1)*" << ldb << ");" << std::endl;
    codestream << "    double* b2 = B+((n+2)*" << ldb << ");" << std::endl;
    codestream << "    double* a0 = A+m;" << std::endl;

    if (bAdd_) {
      codestream << "    c_0_0 = _mm_load_sd(c0);" << std::endl;
      codestream << "    c_0_1 = _mm_load_sd(c1);" << std::endl;
      codestream << "    c_0_2 = _mm_load_sd(c2);" << std::endl;
    } else {
      codestream << "    c_0_0 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_0_1 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_0_2 = _mm_setzero_pd();" << std::endl;
    }

#ifndef FULL_UNROLL

    if (this->bGenerateExitForCK_ == true) {
      codestream << "    for (int k = 0; k < exit_col; k++)" << std::endl;
    } else {
      codestream << "    for (int k = 0; k < " << K << "; k++)" << std::endl;
    }

    codestream << "    {" << std::endl;
#else

    for (int k = 0; k < K; k++) {
      if (this->bGenerateExitForCK_ == true) {
        for (int o = 0; o < this->BasisfunctionsCounter_.size(); o++) {
          if (k == this->BasisfunctionsCounter_[o]) {
            codestream << "      if ( __builtin_expect(exit_col == " << k << ", false) ) { goto sse_one_end; }" << std::endl;
          }
        }
      }

#endif
    sse_inner_blocked_kernel_1(codestream, lda, alignA);
#ifndef FULL_UNROLL
    codestream << "    }" << std::endl;
#else
  }

  if (this->bGenerateExitForCK_ == true && K > 4) {
    codestream << "sse_one_end:" << std::endl;
  }

#endif

    codestream << "    _mm_store_sd(c0, c_0_0);" << std::endl;
    codestream << "    _mm_store_sd(c1, c_0_1);" << std::endl;
    codestream << "    _mm_store_sd(c2, c_0_2);" << std::endl;

    codestream << "    c0++;" << std::endl;
    codestream << "    c1++;" << std::endl;
    codestream << "    c2++;" << std::endl;
    codestream << "  }" << std::endl;

    if (this->bGenerateExitForCK_ == true) {
      codestream << "  c0+=(" << (2 * ldc) << "+(" << ldc << "-M));" << std::endl;
      codestream << "  c1+=(" << (2 * ldc) << "+(" << ldc << "-M));" << std::endl;
      codestream << "  c2+=(" << (2 * ldc) << "+(" << ldc << "-M));" << std::endl;
    } else {
      codestream << "  c0+=" << (2 * ldc) << ";" << std::endl;
      codestream << "  c1+=" << (2 * ldc) << ";" << std::endl;
      codestream << "  c2+=" << (2 * ldc) << ";" << std::endl;
    }

    codestream << "}" << std::endl << std::endl;

    codestream << "#endif" << std::endl << std::endl;

    /////////////////////////
    // generating AVX code //
    /////////////////////////

    if (lda % 4 == 0)
      alignA = true;

    if (ldb % 4 == 0)
      alignB = true;

    if (ldc % 4 == 0)
      alignC = true;

    codestream << "#if defined(__SSE3__) && defined(__AVX__)" << std::endl;
    codestream << "double* c0 = C;" << std::endl;
    codestream << "double* c1 = C+" << ldc << ";" << std::endl;
    codestream << "double* c2 = C+" << 2 * ldc << ";" << std::endl;

    if (this->bGenerateExitForCK_ == true) {
      codestream << "int M = 4;" << std::endl;
      codestream << "switch(exit_col)" << std::endl;
      codestream << "{" << std::endl;
      codestream << "  case 84:" << std::endl;
      codestream << "    M = 56;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 56:" << std::endl;
      codestream << "    M = 36;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 35:" << std::endl;
      codestream << "    M = 20;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 20:" << std::endl;
      codestream << "    M = 12;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 10:" << std::endl;
      codestream << "    M = 4;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 4:" << std::endl;
      codestream << "    M = 4;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "}" << std::endl;
    }

    codestream << "for(int n = 0; n < " << N << "; n+=3)" << std::endl;
    codestream << "{" << std::endl;

    // calculate the maximum number of row
    // we can process with max. blocking
    // In case of FMA, we can add another blocking stage of 16x3
    codestream << "#ifdef __AVX2__" << std::endl;
    if (this->bGenerateExitForCK_ == true) {
      codestream << "  int mDone, mDone_old;" << std::endl;
      codestream << "  mDone = (M/16)*16;" << std::endl;
      codestream << "  for(int m = 0; m < mDone; m+=16)" << std::endl;
    } else {
      mDoneAVX2 = (M / 16) * 16;
      codestream << "  for(int m = 0; m < " << mDoneAVX2 << "; m+=16)" << std::endl;
    }

    codestream << "  {" << std::endl;
    codestream << "    double* b0 = B+(n*" << ldb << ");" << std::endl;
    codestream << "    double* b1 = B+((n+1)*" << ldb << ");" << std::endl;
    codestream << "    double* b2 = B+((n+2)*" << ldb << ");" << std::endl;
    codestream << "    double* a0 = A+m;" << std::endl;

    if (bAdd_) {
      if (alignC == true) {
        codestream << "    c_0_0 = _mm256_load_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm256_load_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm256_load_pd(c2);" << std::endl;
        codestream << "    c_1_0 = _mm256_load_pd(c0+4);" << std::endl;
        codestream << "    c_1_1 = _mm256_load_pd(c1+4);" << std::endl;
        codestream << "    c_1_2 = _mm256_load_pd(c2+4);" << std::endl;
        codestream << "    c_2_0 = _mm256_load_pd(c0+8);" << std::endl;
        codestream << "    c_2_1 = _mm256_load_pd(c1+8);" << std::endl;
        codestream << "    c_2_2 = _mm256_load_pd(c2+8);" << std::endl;
        codestream << "    c_3_0 = _mm256_load_pd(c0+12);" << std::endl;
        codestream << "    c_3_1 = _mm256_load_pd(c1+12);" << std::endl;
        codestream << "    c_3_2 = _mm256_load_pd(c2+12);" << std::endl << std::endl;
      } else {
        codestream << "    c_0_0 = _mm256_loadu_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm256_loadu_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm256_loadu_pd(c2);" << std::endl;
        codestream << "    c_1_0 = _mm256_loadu_pd(c0+4);" << std::endl;
        codestream << "    c_1_1 = _mm256_loadu_pd(c1+4);" << std::endl;
        codestream << "    c_1_2 = _mm256_loadu_pd(c2+4);" << std::endl;
        codestream << "    c_2_0 = _mm256_loadu_pd(c0+8);" << std::endl;
        codestream << "    c_2_1 = _mm256_loadu_pd(c1+8);" << std::endl;
        codestream << "    c_2_2 = _mm256_loadu_pd(c2+8);" << std::endl;
        codestream << "    c_3_0 = _mm256_loadu_pd(c0+12);" << std::endl;
        codestream << "    c_3_1 = _mm256_loadu_pd(c1+12);" << std::endl;
        codestream << "    c_3_2 = _mm256_loadu_pd(c2+12);" << std::endl << std::endl;
      }
    } else {
      codestream << "    c_0_0 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_0_1 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_0_2 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_1_0 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_1_1 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_1_2 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_2_0 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_2_1 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_2_2 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_3_0 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_3_1 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_3_2 = _mm256_setzero_pd();" << std::endl << std::endl;
    }

#ifndef FULL_UNROLL

    if (this->bGenerateExitForCK_ == true) {
      codestream << "    for (int k = 0; k < exit_col; k++)" << std::endl;
    } else {
      codestream << "    for (int k = 0; k < " << K << "; k++)" << std::endl;
    }

    codestream << "    {" << std::endl;
    avx2_inner_blocked_kernel_16(codestream, lda, alignA, false);
#else

    for (int k = 0; k < K; k++) {
      if (this->bGenerateExitForCK_ == true) {
        for (int o = 0; o < this->BasisfunctionsCounter_.size(); o++) {
          if (k == this->BasisfunctionsCounter_[o]) {
            codestream << "      if ( __builtin_expect(exit_col == " << k << ", false) ) { goto avx_sixteen_end; }" << std::endl;
          }
        }
      }
     //if (k == K-3)
     //  avx2_inner_blocked_kernel_16(codestream, lda, alignA, true);
     //else
       avx2_inner_blocked_kernel_16(codestream, lda, alignA, false);
#endif
#ifndef FULL_UNROLL
    codestream << "    }" << std::endl;
#else
  }

  if (this->bGenerateExitForCK_ == true && K > 4) {
    codestream << "avx_sixteen_end:" << std::endl;
  }

#endif

    if (alignC == true) {
      codestream << "    _mm256_store_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm256_store_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm256_store_pd(c2, c_0_2);" << std::endl;
      codestream << "    _mm256_store_pd(c0+4, c_1_0);" << std::endl;
      codestream << "    _mm256_store_pd(c1+4, c_1_1);" << std::endl;
      codestream << "    _mm256_store_pd(c2+4, c_1_2);" << std::endl;
      codestream << "    _mm256_store_pd(c0+8, c_2_0);" << std::endl;
      codestream << "    _mm256_store_pd(c1+8, c_2_1);" << std::endl;
      codestream << "    _mm256_store_pd(c2+8, c_2_2);" << std::endl;
      codestream << "    _mm256_store_pd(c0+12, c_3_0);" << std::endl;
      codestream << "    _mm256_store_pd(c1+12, c_3_1);" << std::endl;
      codestream << "    _mm256_store_pd(c2+12, c_3_2);" << std::endl;
    } else {
      codestream << "    _mm256_storeu_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm256_storeu_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm256_storeu_pd(c2, c_0_2);" << std::endl;
      codestream << "    _mm256_storeu_pd(c0+4, c_1_0);" << std::endl;
      codestream << "    _mm256_storeu_pd(c1+4, c_1_1);" << std::endl;
      codestream << "    _mm256_storeu_pd(c2+4, c_1_2);" << std::endl;
      codestream << "    _mm256_storeu_pd(c0+8, c_2_0);" << std::endl;
      codestream << "    _mm256_storeu_pd(c1+8, c_2_1);" << std::endl;
      codestream << "    _mm256_storeu_pd(c2+8, c_2_2);" << std::endl;
      codestream << "    _mm256_storeu_pd(c0+12, c_3_0);" << std::endl;
      codestream << "    _mm256_storeu_pd(c1+12, c_3_1);" << std::endl;
      codestream << "    _mm256_storeu_pd(c2+12, c_3_2);" << std::endl;
    }

    codestream << "    c0+=16;" << std::endl;
    codestream << "    c1+=16;" << std::endl;
    codestream << "    c2+=16;" << std::endl;
    codestream << "  }" << std::endl;
    codestream << "#endif" << std::endl;


    if (this->bGenerateExitForCK_ == true) {
      codestream << "#ifdef __AVX2__" << std::endl;
      codestream << "  mDone_old = mDone;" << std::endl;
      codestream << "  mDone = mDone_old+(((M-mDone_old)/12)*12);" << std::endl;
      codestream << "  for(int m = mDone_old; m < mDone; m+=12)" << std::endl;
      codestream << "#else" << std::endl;
      codestream << "  int mDone, mDone_old;" << std::endl;
      codestream << "  mDone = (M/12)*12;" << std::endl;
      codestream << "  for(int m = 0; m < mDone; m+=12)" << std::endl;
      codestream << "#endif" << std::endl;
    } else {
      codestream << "#ifdef __AVX2__" << std::endl;
      mDoneAVX2_old = mDoneAVX2;
      mDoneAVX2 = mDoneAVX2_old + (((M - mDoneAVX2_old) / 12) * 12);
      codestream << "  for(int m = " << mDoneAVX2_old << "; m < " << mDoneAVX2 << "; m+=12)" << std::endl;
      codestream << "#else" << std::endl;
      mDone = (M / 12) * 12;
      codestream << "  for(int m = 0; m < " << mDone << "; m+=12)" << std::endl;
      codestream << "#endif" << std::endl;
    }

    codestream << "  {" << std::endl;
    codestream << "    double* b0 = B+(n*" << ldb << ");" << std::endl;
    codestream << "    double* b1 = B+((n+1)*" << ldb << ");" << std::endl;
    codestream << "    double* b2 = B+((n+2)*" << ldb << ");" << std::endl;
    codestream << "    double* a0 = A+m;" << std::endl;

    if (bAdd_) {
      if (alignC == true) {
        codestream << "    c_0_0 = _mm256_load_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm256_load_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm256_load_pd(c2);" << std::endl;
        codestream << "    c_1_0 = _mm256_load_pd(c0+4);" << std::endl;
        codestream << "    c_1_1 = _mm256_load_pd(c1+4);" << std::endl;
        codestream << "    c_1_2 = _mm256_load_pd(c2+4);" << std::endl;
        codestream << "    c_2_0 = _mm256_load_pd(c0+8);" << std::endl;
        codestream << "    c_2_1 = _mm256_load_pd(c1+8);" << std::endl;
        codestream << "    c_2_2 = _mm256_load_pd(c2+8);" << std::endl << std::endl;
      } else {
        codestream << "    c_0_0 = _mm256_loadu_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm256_loadu_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm256_loadu_pd(c2);" << std::endl;
        codestream << "    c_1_0 = _mm256_loadu_pd(c0+4);" << std::endl;
        codestream << "    c_1_1 = _mm256_loadu_pd(c1+4);" << std::endl;
        codestream << "    c_1_2 = _mm256_loadu_pd(c2+4);" << std::endl;
        codestream << "    c_2_0 = _mm256_loadu_pd(c0+8);" << std::endl;
        codestream << "    c_2_1 = _mm256_loadu_pd(c1+8);" << std::endl;
        codestream << "    c_2_2 = _mm256_loadu_pd(c2+8);" << std::endl << std::endl;
      }
    } else {
      codestream << "    c_0_0 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_0_1 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_0_2 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_1_0 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_1_1 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_1_2 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_2_0 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_2_1 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_2_2 = _mm256_setzero_pd();" << std::endl << std::endl;
    }

#ifndef FULL_UNROLL

    if (this->bGenerateExitForCK_ == true) {
      codestream << "    for (int k = 0; k < exit_col; k++)" << std::endl;
    } else {
      codestream << "    for (int k = 0; k < " << K << "; k++)" << std::endl;
    }

    codestream << "    {" << std::endl;
#else

    for (int k = 0; k < K; k++) {
      if (this->bGenerateExitForCK_ == true) {
        for (int o = 0; o < this->BasisfunctionsCounter_.size(); o++) {
          if (k == this->BasisfunctionsCounter_[o]) {
            codestream << "      if ( __builtin_expect(exit_col == " << k << ", false) ) { goto avx_twelve_end; }" << std::endl;
          }
        }
      }

#endif
    avx_inner_blocked_kernel_12(codestream, lda, alignA);
#ifndef FULL_UNROLL
    codestream << "    }" << std::endl;
#else
  }

  if (this->bGenerateExitForCK_ == true && K > 4) {
    codestream << "avx_twelve_end:" << std::endl;
  }

#endif

    if (alignC == true) {
      codestream << "    _mm256_store_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm256_store_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm256_store_pd(c2, c_0_2);" << std::endl;
      codestream << "    _mm256_store_pd(c0+4, c_1_0);" << std::endl;
      codestream << "    _mm256_store_pd(c1+4, c_1_1);" << std::endl;
      codestream << "    _mm256_store_pd(c2+4, c_1_2);" << std::endl;
      codestream << "    _mm256_store_pd(c0+8, c_2_0);" << std::endl;
      codestream << "    _mm256_store_pd(c1+8, c_2_1);" << std::endl;
      codestream << "    _mm256_store_pd(c2+8, c_2_2);" << std::endl;
    } else {
      codestream << "    _mm256_storeu_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm256_storeu_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm256_storeu_pd(c2, c_0_2);" << std::endl;
      codestream << "    _mm256_storeu_pd(c0+4, c_1_0);" << std::endl;
      codestream << "    _mm256_storeu_pd(c1+4, c_1_1);" << std::endl;
      codestream << "    _mm256_storeu_pd(c2+4, c_1_2);" << std::endl;
      codestream << "    _mm256_storeu_pd(c0+8, c_2_0);" << std::endl;
      codestream << "    _mm256_storeu_pd(c1+8, c_2_1);" << std::endl;
      codestream << "    _mm256_storeu_pd(c2+8, c_2_2);" << std::endl;
    }

    codestream << "    c0+=12;" << std::endl;
    codestream << "    c1+=12;" << std::endl;
    codestream << "    c2+=12;" << std::endl;
    codestream << "  }" << std::endl;


    // calculate the maximum number of row
    // we can process with max. blocking
    if (this->bGenerateExitForCK_ == true) {
      codestream << "  mDone_old = mDone;" << std::endl;
      codestream << "  mDone = mDone_old+(((M-mDone_old)/8)*8);" << std::endl;
      codestream << "  for(int m = mDone_old; m < mDone; m+=8)" << std::endl;
    } else {
      codestream << "#ifdef __AVX2__" << std::endl;
      mDoneAVX2_old = mDoneAVX2;
      mDoneAVX2 = mDoneAVX2_old + (((M - mDoneAVX2_old) / 8) * 8);
      codestream << "  for(int m = " << mDoneAVX2_old << "; m < " << mDoneAVX2 << "; m+=8)" << std::endl;
      codestream << "#else" << std::endl;
      mDone_old = mDone;
      mDone = mDone_old + (((M - mDone_old) / 8) * 8);
      codestream << "  for(int m = " << mDone_old << "; m < " << mDone << "; m+=8)" << std::endl;
      codestream << "#endif" << std::endl;
    }

    codestream << "  {" << std::endl;
    codestream << "    double* b0 = B+(n*" << ldb << ");" << std::endl;
    codestream << "    double* b1 = B+((n+1)*" << ldb << ");" << std::endl;
    codestream << "    double* b2 = B+((n+2)*" << ldb << ");" << std::endl;
    codestream << "    double* a0 = A+m;" << std::endl;

    if (bAdd_) {
      if (alignC == true) {
        codestream << "    c_0_0 = _mm256_load_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm256_load_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm256_load_pd(c2);" << std::endl;
        codestream << "    c_1_0 = _mm256_load_pd(c0+4);" << std::endl;
        codestream << "    c_1_1 = _mm256_load_pd(c1+4);" << std::endl;
        codestream << "    c_1_2 = _mm256_load_pd(c2+4);" << std::endl;
      } else {
        codestream << "    c_0_0 = _mm256_loadu_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm256_loadu_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm256_loadu_pd(c2);" << std::endl;
        codestream << "    c_1_0 = _mm256_loadu_pd(c0+4);" << std::endl;
        codestream << "    c_1_1 = _mm256_loadu_pd(c1+4);" << std::endl;
        codestream << "    c_1_2 = _mm256_loadu_pd(c2+4);" << std::endl;
      }
    } else {
      codestream << "    c_0_0 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_0_1 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_0_2 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_1_0 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_1_1 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_1_2 = _mm256_setzero_pd();" << std::endl;
    }

#ifndef FULL_UNROLL

    if (this->bGenerateExitForCK_ == true) {
      codestream << "    for (int k = 0; k < exit_col; k++)" << std::endl;
    } else {
      codestream << "    for (int k = 0; k < " << K << "; k++)" << std::endl;
    }

    codestream << "    {" << std::endl;
#else

    for (int k = 0; k < K; k++) {
      if (this->bGenerateExitForCK_ == true) {
        for (int o = 0; o < this->BasisfunctionsCounter_.size(); o++) {
          if (k == this->BasisfunctionsCounter_[o]) {
            codestream << "      if ( __builtin_expect(exit_col == " << k << ", false) ) { goto avx_eight_end; }" << std::endl;
          }
        }
      }

#endif
    avx_inner_blocked_kernel_8(codestream, lda, alignA);
#ifndef FULL_UNROLL
    codestream << "    }" << std::endl;
#else
  }

  if (this->bGenerateExitForCK_ == true && K > 4) {
    codestream << "avx_eight_end:" << std::endl;
  }

#endif

    if (alignC == true) {
      codestream << "    _mm256_store_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm256_store_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm256_store_pd(c2, c_0_2);" << std::endl;
      codestream << "    _mm256_store_pd(c0+4, c_1_0);" << std::endl;
      codestream << "    _mm256_store_pd(c1+4, c_1_1);" << std::endl;
      codestream << "    _mm256_store_pd(c2+4, c_1_2);" << std::endl;
    } else {
      codestream << "    _mm256_storeu_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm256_storeu_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm256_storeu_pd(c2, c_0_2);" << std::endl;
      codestream << "    _mm256_storeu_pd(c0+4, c_1_0);" << std::endl;
      codestream << "    _mm256_storeu_pd(c1+4, c_1_1);" << std::endl;
      codestream << "    _mm256_storeu_pd(c2+4, c_1_2);" << std::endl;
    }

    codestream << "    c0+=8;" << std::endl;
    codestream << "    c1+=8;" << std::endl;
    codestream << "    c2+=8;" << std::endl;
    codestream << "  }" << std::endl;


    // calculate the maximum number of row
    // we can process with max. blocking
    if (this->bGenerateExitForCK_ == true) {
      codestream << "  mDone_old = mDone;" << std::endl;
      codestream << "  mDone = mDone_old+(((M-mDone_old)/4)*4);" << std::endl;
      codestream << "  for(int m = mDone_old; m < mDone; m+=4)" << std::endl;
    } else {
      codestream << "#ifdef __AVX2__" << std::endl;
      mDoneAVX2_old = mDoneAVX2;
      mDoneAVX2 = mDoneAVX2_old + (((M - mDoneAVX2_old) / 4) * 4);
      codestream << "  for(int m = " << mDoneAVX2_old << "; m < " << mDoneAVX2 << "; m+=4)" << std::endl;
      codestream << "#else" << std::endl;
      mDone_old = mDone;
      mDone = mDone_old + (((M - mDone_old) / 4) * 4);
      codestream << "  for(int m = " << mDone_old << "; m < " << mDone << "; m+=4)" << std::endl;
      codestream << "#endif" << std::endl;
    }

    codestream << "  {" << std::endl;
    codestream << "    double* b0 = B+(n*" << ldb << ");" << std::endl;
    codestream << "    double* b1 = B+((n+1)*" << ldb << ");" << std::endl;
    codestream << "    double* b2 = B+((n+2)*" << ldb << ");" << std::endl;
    codestream << "    double* a0 = A+m;" << std::endl;

    if (bAdd_) {
      if (alignC == true) {
        codestream << "    c_0_0 = _mm256_load_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm256_load_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm256_load_pd(c2);" << std::endl;
      } else {
        codestream << "    c_0_0 = _mm256_loadu_pd(c0);" << std::endl;
        codestream << "    c_0_1 = _mm256_loadu_pd(c1);" << std::endl;
        codestream << "    c_0_2 = _mm256_loadu_pd(c2);" << std::endl;
      }
    } else {
      codestream << "    c_0_0 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_0_1 = _mm256_setzero_pd();" << std::endl;
      codestream << "    c_0_2 = _mm256_setzero_pd();" << std::endl;
    }

#ifndef FULL_UNROLL

    if (this->bGenerateExitForCK_ == true) {
      codestream << "    for (int k = 0; k < exit_col; k++)" << std::endl;
    } else {
      codestream << "    for (int k = 0; k < " << K << "; k++)" << std::endl;
    }

    codestream << "    {" << std::endl;
#else

    for (int k = 0; k < K; k++) {
      if (this->bGenerateExitForCK_ == true) {
        for (int o = 0; o < this->BasisfunctionsCounter_.size(); o++) {
          if (k == this->BasisfunctionsCounter_[o]) {
            codestream << "      if ( __builtin_expect(exit_col == " << k << ", false) ) { goto avx_four_end; }" << std::endl;
          }
        }
      }

#endif
    avx_inner_blocked_kernel_4(codestream, lda, alignA);
#ifndef FULL_UNROLL
    codestream << "    }" << std::endl;
#else
  }

  if (this->bGenerateExitForCK_ == true && K > 4) {
    codestream << "avx_four_end:" << std::endl;
  }

#endif

    if (alignC == true) {
      codestream << "    _mm256_store_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm256_store_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm256_store_pd(c2, c_0_2);" << std::endl;
    } else {
      codestream << "    _mm256_storeu_pd(c0, c_0_0);" << std::endl;
      codestream << "    _mm256_storeu_pd(c1, c_0_1);" << std::endl;
      codestream << "    _mm256_storeu_pd(c2, c_0_2);" << std::endl;
    }

    codestream << "    c0+=4;" << std::endl;
    codestream << "    c1+=4;" << std::endl;
    codestream << "    c2+=4;" << std::endl;
    codestream << "  }" << std::endl;

    if (this->bGenerateExitForCK_ == true) {
      codestream << "  for(int m = mDone; m < M; m++)" << std::endl;
    } else {
      codestream << "#ifdef __AVX2__" << std::endl;
      codestream << "  for(int m = " << mDoneAVX2 << "; m < " << M << "; m++)" << std::endl;
      codestream << "#else" << std::endl;
      codestream << "  for(int m = " << mDone << "; m < " << M << "; m++)" << std::endl;
      codestream << "#endif" << std::endl;
    }

    codestream << "  {" << std::endl;
    codestream << "    double* b0 = B+(n*" << ldb << ");" << std::endl;
    codestream << "    double* b1 = B+((n+1)*" << ldb << ");" << std::endl;
    codestream << "    double* b2 = B+((n+2)*" << ldb << ");" << std::endl;
    codestream << "    double* a0 = A+m;" << std::endl;

    if (bAdd_) {
      codestream << "    c_0_0_128 = _mm_load_sd(c0);" << std::endl;
      codestream << "    c_0_1_128 = _mm_load_sd(c1);" << std::endl;
      codestream << "    c_0_2_128 = _mm_load_sd(c2);" << std::endl;
    } else {
      codestream << "    c_0_0_128 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_0_1_128 = _mm_setzero_pd();" << std::endl;
      codestream << "    c_0_2_128 = _mm_setzero_pd();" << std::endl;
    }

#ifndef FULL_UNROLL

    if (this->bGenerateExitForCK_ == true) {
      codestream << "    for (int k = 0; k < exit_col; k++)" << std::endl;
    } else {
      codestream << "    for (int k = 0; k < " << K << "; k++)" << std::endl;
    }

    codestream << "    {" << std::endl;
#else

    for (int k = 0; k < K; k++) {
      if (this->bGenerateExitForCK_ == true) {
        for (int o = 0; o < this->BasisfunctionsCounter_.size(); o++) {
          if (k == this->BasisfunctionsCounter_[o]) {
            codestream << "      if ( __builtin_expect(exit_col == " << k << ", false) ) { goto avx_one_end; }" << std::endl;
          }
        }
      }

#endif
    avx_inner_blocked_kernel_1(codestream, lda, alignA);
#ifndef FULL_UNROLL
    codestream << "    }" << std::endl;
#else
  }

  if (this->bGenerateExitForCK_ == true && K > 4) {
    codestream << "avx_one_end:" << std::endl;
  }

#endif

    codestream << "    _mm_store_sd(c0, c_0_0_128);" << std::endl;
    codestream << "    _mm_store_sd(c1, c_0_1_128);" << std::endl;
    codestream << "    _mm_store_sd(c2, c_0_2_128);" << std::endl;

    codestream << "    c0+=1;" << std::endl;
    codestream << "    c1+=1;" << std::endl;
    codestream << "    c2+=1;" << std::endl;
    codestream << "  }" << std::endl;

    if (this->bGenerateExitForCK_ == true) {
      codestream << "  c0+=(" << (2 * ldc) << "+(" << ldc << "-M));" << std::endl;
      codestream << "  c1+=(" << (2 * ldc) << "+(" << ldc << "-M));" << std::endl;
      codestream << "  c2+=(" << (2 * ldc) << "+(" << ldc << "-M));" << std::endl;
    } else {
      codestream << "  c0+=" << (2 * ldc) << ";" << std::endl;
      codestream << "  c1+=" << (2 * ldc) << ";" << std::endl;
      codestream << "  c2+=" << (2 * ldc) << ";" << std::endl;
    }

    codestream << "}" << std::endl << std::endl;
    codestream << "#endif" << std::endl << std::endl;

    /////////////////////////
    // generating MIC code //
    /////////////////////////

    if ( (M == 56) ) {
      codestream << "#if defined(__MIC__)" << std::endl;

      if (this->bGenerateExitForCK_ == true) {
        codestream << "int M = 8;" << std::endl;
        codestream << "int K = 8;" << std::endl;
        codestream << "switch(exit_col)" << std::endl;
        codestream << "{" << std::endl;
        codestream << "  case 84:" << std::endl;
        codestream << "    M = 56;" << std::endl;
        codestream << "    K = 84;" << std::endl;
        codestream << "    break;" << std::endl;
        codestream << "  case 56:" << std::endl;
        codestream << "    M = 40;" << std::endl;
        codestream << "    K = 56;" << std::endl;
        codestream << "    break;" << std::endl;
        codestream << "  case 35:" << std::endl;
        codestream << "    M = 24;" << std::endl;
        codestream << "    K = 40;" << std::endl;
        codestream << "    break;" << std::endl;
        codestream << "  case 20:" << std::endl;
        codestream << "    M = 16;" << std::endl;
        codestream << "    K = 24;" << std::endl;
        codestream << "    break;" << std::endl;
        codestream << "  case 10:" << std::endl;
        codestream << "    M = 8;" << std::endl;
        codestream << "    K = 16;" << std::endl;
        codestream << "    break;" << std::endl;
        codestream << "  case 4:" << std::endl;
        codestream << "    M = 8;" << std::endl;
        codestream << "    K = 8;" << std::endl;
        codestream << "    break;" << std::endl;
        codestream << "}" << std::endl;
        codestream << "exit_col = K;" << std::endl << std::endl;
      }

      codestream << "A_prefetch = A;" << std::endl;
      codestream << "B_prefetch = B;" << std::endl;
      codestream << "C_prefetch = C;" << std::endl << std::endl;

      codestream << "__m512d c_0;" << std::endl;
      codestream << "__m512d c_1;" << std::endl;
      codestream << "__m512d c_2;" << std::endl;
      codestream << "__m512d c_3;" << std::endl;
      codestream << "__m512d c_4;" << std::endl;
      codestream << "__m512d c_5;" << std::endl;
      codestream << "__m512d c_6;" << std::endl;
      codestream << "__m512d c_7;" << std::endl;
      codestream << "__m512d c_8;" << std::endl << std::endl;

      codestream << "__m512d b_bcst;" << std::endl << std::endl;

      codestream << "__m512d a_0;" << std::endl << std::endl;

      codestream << "#pragma prefetch C" << std::endl;

      if (this->bGenerateExitForCK_ == true) {
        codestream << "for (int m = 0; m < M; m += 8)" << std::endl;
      } else {
        codestream << "for (int m = 0; m < 48; m += 8)" << std::endl;
      }

      codestream << "{" << std::endl;

      if (bAdd_) {
        codestream << "  c_0 = _mm512_load_pd(C + m + 0*" << ldc << ");" << std::endl;
        codestream << "  c_1 = _mm512_load_pd(C + m + 1*" << ldc << ");" << std::endl;
        codestream << "  c_2 = _mm512_load_pd(C + m + 2*" << ldc << ");" << std::endl;
        codestream << "  c_3 = _mm512_load_pd(C + m + 3*" << ldc << ");" << std::endl;
        codestream << "  c_4 = _mm512_load_pd(C + m + 4*" << ldc << ");" << std::endl;
        codestream << "  c_5 = _mm512_load_pd(C + m + 5*" << ldc << ");" << std::endl;
        codestream << "  c_6 = _mm512_load_pd(C + m + 6*" << ldc << ");" << std::endl;
        codestream << "  c_7 = _mm512_load_pd(C + m + 7*" << ldc << ");" << std::endl;
        codestream << "  c_8 = _mm512_load_pd(C + m + 8*" << ldc << ");" << std::endl << std::endl;
      } else {
        codestream << "  c_0 = _mm512_setzero_pd();" << std::endl;
        codestream << "  c_1 = _mm512_setzero_pd();" << std::endl;
        codestream << "  c_2 = _mm512_setzero_pd();" << std::endl;
        codestream << "  c_3 = _mm512_setzero_pd();" << std::endl;
        codestream << "  c_4 = _mm512_setzero_pd();" << std::endl;
        codestream << "  c_5 = _mm512_setzero_pd();" << std::endl;
        codestream << "  c_6 = _mm512_setzero_pd();" << std::endl;
        codestream << "  c_7 = _mm512_setzero_pd();" << std::endl;
        codestream << "  c_8 = _mm512_setzero_pd();" << std::endl << std::endl;
      }

      codestream << "  double* cur_A = A + m;" << std::endl;
      codestream << "  double* cur_B = B;" << std::endl;
      codestream << "  double* pre_B_L1 = B+8;" << std::endl << std::endl;

      if (K == 9) {
        for (int i = 0; i < 9; i++) {
          mic_inner_blocked_kernel_8_noprefetch_B(codestream, lda, ldb, i);
        }
      } else {
        codestream << "  #pragma noprefetch" << std::endl;

        if (this->bGenerateExitForCK_ == true) {
          codestream << "  for (int k = 0; k < exit_col; k+=8)" << std::endl;
        } else {
          codestream << "  for (int k = 0; k < 56; k+=8)" << std::endl;
        }

        codestream << "  {" << std::endl;

        for (int i = 0; i < 8; i++) {
          mic_inner_blocked_kernel_8_L1(codestream, lda, ldb, i);
        }

        codestream << "  }" << std::endl << std::endl;
      }

      codestream << "  _mm512_store_pd(C + m + 0*" << ldc << ", c_0);" << std::endl;
      codestream << "  _mm512_store_pd(C + m + 1*" << ldc << ", c_1);" << std::endl;
      codestream << "  _mm512_store_pd(C + m + 2*" << ldc << ", c_2);" << std::endl;
      codestream << "  _mm512_store_pd(C + m + 3*" << ldc << ", c_3);" << std::endl;
      codestream << "  _mm512_store_pd(C + m + 4*" << ldc << ", c_4);" << std::endl;
      codestream << "  _mm512_store_pd(C + m + 5*" << ldc << ", c_5);" << std::endl;
      codestream << "  _mm512_store_pd(C + m + 6*" << ldc << ", c_6);" << std::endl;
      codestream << "  _mm512_store_pd(C + m + 7*" << ldc << ", c_7);" << std::endl;
      codestream << "  _mm512_store_pd(C + m + 8*" << ldc << ", c_8);" << std::endl;
      codestream << "}" << std::endl << std::endl;

      if (this->bGenerateExitForCK_ == false) {
        codestream << "int m = 48;" << std::endl << std::endl;

        codestream << "_mm_prefetch((const char*)C_prefetch + (0*" << ldc << "), _MM_HINT_T1);" << std::endl;

        if (bAdd_) {
          codestream << "c_0 = _mm512_load_pd(C + m + 0*" << ldc << ");" << std::endl;
        } else {
          codestream << "c_0 = _mm512_setzero_pd();" << std::endl;
        }

        codestream << "_mm_prefetch((const char*)C_prefetch + (1*" << ldc << "), _MM_HINT_T1);" << std::endl;

        if (bAdd_) {
          codestream << "c_1 = _mm512_load_pd(C + m + 1*" << ldc << ");" << std::endl;
        } else {
          codestream << "c_1 = _mm512_setzero_pd();" << std::endl;
        }

        codestream << "_mm_prefetch((const char*)C_prefetch + (2*" << ldc << "), _MM_HINT_T1);" << std::endl;

        if (bAdd_) {
          codestream << "c_2 = _mm512_load_pd(C + m + 2*" << ldc << ");" << std::endl;
        } else {
          codestream << "c_2 = _mm512_setzero_pd();" << std::endl;
        }

        codestream << "_mm_prefetch((const char*)C_prefetch + (3*" << ldc << "), _MM_HINT_T1);" << std::endl;

        if (bAdd_) {
          codestream << "c_3 = _mm512_load_pd(C + m + 3*" << ldc << ");" << std::endl;
        } else {
          codestream << "c_3 = _mm512_setzero_pd();" << std::endl;
        }

        codestream << "_mm_prefetch((const char*)C_prefetch + (4*" << ldc << "), _MM_HINT_T1);" << std::endl;

        if (bAdd_) {
          codestream << "c_4 = _mm512_load_pd(C + m + 4*" << ldc << ");" << std::endl;
        } else {
          codestream << "c_4 = _mm512_setzero_pd();" << std::endl;
        }

        codestream << "_mm_prefetch((const char*)C_prefetch + (5*" << ldc << "), _MM_HINT_T1);" << std::endl;

        if (bAdd_) {
          codestream << "c_5 = _mm512_load_pd(C + m + 5*" << ldc << ");" << std::endl;
        } else {
          codestream << "c_5 = _mm512_setzero_pd();" << std::endl;
        }

        codestream << "_mm_prefetch((const char*)C_prefetch + (6*" << ldc << "), _MM_HINT_T1);" << std::endl;

        if (bAdd_) {
          codestream << "c_6 = _mm512_load_pd(C + m + 6*" << ldc << ");" << std::endl;
        } else {
          codestream << "c_6 = _mm512_setzero_pd();" << std::endl;
        }

        codestream << "_mm_prefetch((const char*)C_prefetch + (7*" << ldc << "), _MM_HINT_T1);" << std::endl;

        if (bAdd_) {
          codestream << "c_7 = _mm512_load_pd(C + m + 7*" << ldc << ");" << std::endl;
        } else {
          codestream << "c_7 = _mm512_setzero_pd();" << std::endl;
        }

        codestream << "_mm_prefetch((const char*)C_prefetch + (8*" << ldc << "), _MM_HINT_T1);" << std::endl;

        if (bAdd_) {
          codestream << "c_8 = _mm512_load_pd(C + m + 8*" << ldc << ");" << std::endl << std::endl;
        } else {
          codestream << "c_8 = _mm512_setzero_pd();" << std::endl << std::endl;
        }

        codestream << "double* cur_A = A + m;" << std::endl;
        codestream << "double* cur_B = B;" << std::endl;
        codestream << "double* pre_B_L1 = B+8;" << std::endl;
        codestream << "double* pre_B_L2 = B_prefetch;" << std::endl;
        codestream << "double* pre_A_L2 = A_prefetch;" << std::endl << std::endl;

        if (K == 9) {
          for (int i = 0; i < 8; i++) {
            mic_inner_blocked_kernel_8_L2(codestream, lda, ldb, i);
          }

          mic_inner_blocked_kernel_8_L2(codestream, lda, ldb, 0);
        } else {
          codestream << "#pragma noprefetch" << std::endl;
          codestream << "for (int k = 0; k < 56; k+=8)" << std::endl;
          codestream << "{" << std::endl;

          for (int i = 0; i < 8; i++) {
            mic_inner_blocked_kernel_8_L2(codestream, lda, ldb, i);
          }

          codestream << "}" << std::endl << std::endl;
        }

        codestream << "//_mm_prefetch((const char*)C_prefetch + (0*" << ldc << "), _MM_HINT_T0);" << std::endl;
        codestream << "_mm512_store_pd(C + m + 0*" << ldc << ", c_0);" << std::endl;
        codestream << "//_mm_prefetch((const char*)C_prefetch + (1*" << ldc << "), _MM_HINT_T0);" << std::endl;
        codestream << "_mm512_store_pd(C + m + 1*" << ldc << ", c_1);" << std::endl;
        codestream << "//_mm_prefetch((const char*)C_prefetch + (2*" << ldc << "), _MM_HINT_T0);" << std::endl;
        codestream << "_mm512_store_pd(C + m + 2*" << ldc << ", c_2);" << std::endl;
        codestream << "//_mm_prefetch((const char*)C_prefetch + (3*" << ldc << "), _MM_HINT_T0);" << std::endl;
        codestream << "_mm512_store_pd(C + m + 3*" << ldc << ", c_3);" << std::endl;
        codestream << "//_mm_prefetch((const char*)C_prefetch + (4*" << ldc << "), _MM_HINT_T0);" << std::endl;
        codestream << "_mm512_store_pd(C + m + 4*" << ldc << ", c_4);" << std::endl;
        codestream << "//_mm_prefetch((const char*)C_prefetch + (5*" << ldc << "), _MM_HINT_T0);" << std::endl;
        codestream << "_mm512_store_pd(C + m + 5*" << ldc << ", c_5);" << std::endl;
        codestream << "//_mm_prefetch((const char*)C_prefetch + (6*" << ldc << "), _MM_HINT_T0);" << std::endl;
        codestream << "_mm512_store_pd(C + m + 6*" << ldc << ", c_6);" << std::endl;
        codestream << "//_mm_prefetch((const char*)C_prefetch + (7*" << ldc << "), _MM_HINT_T0);" << std::endl;
        codestream << "_mm512_store_pd(C + m + 7*" << ldc << ", c_7);" << std::endl;
        codestream << "//_mm_prefetch((const char*)C_prefetch + (8*" << ldc << "), _MM_HINT_T0);" << std::endl;
        codestream << "_mm512_store_pd(C + m + 8*" << ldc << ", c_8);" << std::endl << std::endl;
      }

      codestream << "#endif" << std::endl << std::endl;
    }

    // generate fallback c code
    if ( (M == 56) ) {
      codestream << "#if !defined(__SSE3__) && !defined(__AVX__) && !defined(__MIC__)" << std::endl;
    } else {
      codestream << "#if !defined(__SSE3__) && !defined(__AVX__)" << std::endl;
    }

    if (this->bGenerateExitForCK_ == true) {
      codestream << "int M = 4;" << std::endl;
      codestream << "switch(exit_col)" << std::endl;
      codestream << "{" << std::endl;
      codestream << "  case 84:" << std::endl;
      codestream << "    M = 56;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 56:" << std::endl;
      codestream << "    M = 35;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 35:" << std::endl;
      codestream << "    M = 20;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 20:" << std::endl;
      codestream << "    M = 10;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 10:" << std::endl;
      codestream << "    M = 4;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "  case 4:" << std::endl;
      codestream << "    M = 4;" << std::endl;
      codestream << "    break;" << std::endl;
      codestream << "}" << std::endl;
    }


    codestream << "for (int n = 0; n < " << N << "; n++)" << std::endl;
    codestream << "{" << std::endl;

    if (this->bAdd_ == false) {
      codestream << "  for(int m = 0; m < " << M << "; m++) { C[(n*" << ldc << ")+m] = 0.0; }" << std::endl << std::endl;
    }

    if (this->bGenerateExitForCK_ == true) {
      codestream << "  for (int k = 0; k < exit_col; k++)" << std::endl;
    } else {
      codestream << "  for (int k = 0; k < " << K << "; k++)" << std::endl;
    }

    codestream << "  {" << std::endl;

    if (this->bGenerateExitForCK_ == true) {
      codestream << "    for(int m = 0; m < M; m++)" << std::endl;
    } else {
      codestream << "    for(int m = 0; m < " << M << "; m++)" << std::endl;
    }

    codestream << "    {" << std::endl;
    codestream << "      C[(n*" << ldc << ")+m] += A[(k*" << lda << ")+m] * B[(n*" << ldb << ")+k];" << std::endl;
    codestream << "    }" << std::endl;
    codestream << "  }" << std::endl;
    codestream << "}" << std::endl;
    codestream << "#endif" << std::endl << std::endl;

    codestream << "#ifndef NDEBUG" << std::endl;

    if (this->bGenerateExitForCK_ == true) {
      codestream << "num_flops += " << 2 * N << "*M*exit_col;" << std::endl;
    } else {
      codestream << "num_flops += " << 2 * N* M* K << ";" << std::endl;
    }

    codestream << "#endif" << std::endl << std::endl;

    return codestream.str();
  }

}

