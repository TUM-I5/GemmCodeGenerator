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
#include <immintrin.h>

#include "GeneratorCSC.hpp"
#include "ReaderCSC.hpp"

#define C_UNROLLING_FACTOR_RIGHT 1
//#define USE_THREE_ELEMENT_AVX_VECTORIZATION

namespace seissolgen {

  GeneratorCSC::GeneratorCSC() : bGenerateExitForCK_(false), bAdd_(true) {
  }

  GeneratorCSC::GeneratorCSC(bool bGenerateExitForCK, int nMaxOrder, bool bAdd) : bGenerateExitForCK_(bGenerateExitForCK), bAdd_(bAdd) {
    for (int i = 2; i < nMaxOrder; i++) {
      int numBasisfunctions = ((i) * (i + 1) * (i + 2)) / 6;
      BasisfunctionsCounter_.push_back(numBasisfunctions);
    }
  }

  void GeneratorCSC::generate_code_left_innerloop_scalar(std::stringstream& codestream, int ldc, int l, int z, int* rowidx, int* colidx) {
    codestream << "__m128d c" << l << "_" << z << " = _mm_load_sd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "]);" << std::endl;
    codestream << "__m128d a" << l << "_" << z << " = _mm_load_sd(&values[" << colidx[l] + z << "]);" << std::endl;
    codestream << "#if defined(__SSE3__) && defined(__AVX256__)" << std::endl;
    codestream << "c" << l << "_" << z << " = _mm_add_sd(c" << l << "_" << z << ", _mm_mul_sd(a" << l << "_" << z << ", _mm256_castpd256_pd128(b" << l << ")));" << std::endl;
    codestream << "#endif" << std::endl;
    codestream << "#if defined(__SSE3__) && !defined(__AVX256__)" << std::endl;
    codestream << "c" << l << "_" << z << " = _mm_add_sd(c" << l << "_" << z << ", _mm_mul_sd(a" << l << "_" << z << ", b" << l << "));" << std::endl;
    codestream << "#endif" << std::endl;
    codestream << "_mm_store_sd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "], c" << l << "_" << z << ");" << std::endl;
  }

  void GeneratorCSC::generate_code_left_innerloop_2vector(std::stringstream& codestream, int ldc, int l, int z, int* rowidx, int* colidx) {
    codestream << "__m128d c" << l << "_" << z << " = _mm_loadu_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "]);" << std::endl;
    codestream << "__m128d a" << l << "_" << z << " = _mm_loadu_pd(&values[" << colidx[l] + z << "]);" << std::endl;
    codestream << "#if defined(__SSE3__) && defined(__AVX256__)" << std::endl;
    codestream << "c" << l << "_" << z << " = _mm_add_pd(c" << l << "_" << z << ", _mm_mul_pd(a" << l << "_" << z << ", _mm256_castpd256_pd128(b" << l << ")));" << std::endl;
    codestream << "#endif" << std::endl;
    codestream << "#if defined(__SSE3__) && !defined(__AVX256__)" << std::endl;
    codestream << "c" << l << "_" << z << " = _mm_add_pd(c" << l << "_" << z << ", _mm_mul_pd(a" << l << "_" << z << ", b" << l << "));" << std::endl;
    codestream << "#endif" << std::endl;
    codestream << "_mm_storeu_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "], c" << l << "_" << z << ");" << std::endl;
  }

  void GeneratorCSC::generate_code_left_innerloop_3vector(std::stringstream& codestream, int ldc, int l, int z, int* rowidx, int* colidx) {
    codestream << "#if defined(__SSE3__) && defined(__AVX256__)" << std::endl;
    codestream << "__m256d c" << l << "_" << z << " = _mm256_maskload_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "], three_mask);" << std::endl;
    codestream << "__m256d a" << l << "_" << z << " = _mm256_maskload_pd(&values[" << colidx[l] + z << "], three_mask);" << std::endl;
    codestream << "c" << l << "_" << z << " = _mm256_add_pd(c" << l << "_" << z << ", _mm256_mul_pd(a" << l << "_" << z << ", b" << l << "));" << std::endl;
    codestream << "_mm256_maskstore_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "], three_mask, c" << l << "_" << z << ");" << std::endl;
    codestream << "#endif" << std::endl;
    codestream << "#if defined(__SSE3__) && !defined(__AVX256__)" << std::endl;
    codestream << "__m128d c" << l << "_" << z << " = _mm_loadu_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "]);" << std::endl;
    codestream << "__m128d a" << l << "_" << z << " = _mm_loadu_pd(&values[" << colidx[l] + z << "]);" << std::endl;
    codestream << "c" << l << "_" << z << " = _mm_add_pd(c" << l << "_" << z << ", _mm_mul_pd(a" << l << "_" << z << ", b" << l << "));" << std::endl;
    codestream << "_mm_storeu_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "], c" << l << "_" << z << ");" << std::endl;
    codestream << "__m128d c" << l << "_" << z + 2 << " = _mm_load_sd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z + 2] << "]);" << std::endl;
    codestream << "__m128d a" << l << "_" << z + 2 << " = _mm_load_sd(&values[" << colidx[l] + z + 2 << "]);" << std::endl;
    codestream << "c" << l << "_" << z + 2 << " = _mm_add_sd(c" << l << "_" << z + 2 << ", _mm_mul_sd(a" << l << "_" << z + 2 << ", b" << l << "));" << std::endl;
    codestream << "_mm_store_sd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z + 2] << "], c" << l << "_" << z + 2 << ");" << std::endl;
    codestream << "#endif" << std::endl;
  }

  void GeneratorCSC::generate_code_left_innerloop_4vector(std::stringstream& codestream, int ldc, int l, int z, int* rowidx, int* colidx) {
    codestream << "#if defined(__SSE3__) && defined(__AVX256__)" << std::endl;
    codestream << "__m256d c" << l << "_" << z << " = _mm256_loadu_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "]);" << std::endl;
    codestream << "__m256d a" << l << "_" << z << " = _mm256_loadu_pd(&values[" << colidx[l] + z << "]);" << std::endl;
    codestream << "c" << l << "_" << z << " = _mm256_add_pd(c" << l << "_" << z << ", _mm256_mul_pd(a" << l << "_" << z << ", b" << l << "));" << std::endl;
    codestream << "_mm256_storeu_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "], c" << l << "_" << z << ");" << std::endl;
    codestream << "#endif" << std::endl;
    codestream << "#if defined(__SSE3__) && !defined(__AVX256__)" << std::endl;
    codestream << "__m128d c" << l << "_" << z << " = _mm_loadu_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "]);" << std::endl;
    codestream << "__m128d a" << l << "_" << z << " = _mm_loadu_pd(&values[" << colidx[l] + z << "]);" << std::endl;
    codestream << "c" << l << "_" << z << " = _mm_add_pd(c" << l << "_" << z << ", _mm_mul_pd(a" << l << "_" << z << ", b" << l << "));" << std::endl;
    codestream << "_mm_storeu_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "], c" << l << "_" << z << ");" << std::endl;
    codestream << "__m128d c" << l << "_" << z + 2 << " = _mm_loadu_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z + 2] << "]);" << std::endl;
    codestream << "__m128d a" << l << "_" << z + 2 << " = _mm_loadu_pd(&values[" << colidx[l] + z + 2 << "]);" << std::endl;
    codestream << "c" << l << "_" << z + 2 << " = _mm_add_pd(c" << l << "_" << z + 2 << ", _mm_mul_pd(a" << l << "_" << z + 2 << ", b" << l << "));" << std::endl;
    codestream << "_mm_storeu_pd(&C[(i*" << ldc << ")+" << rowidx[colidx[l] + z + 2] << "], c" << l << "_" << z + 2 << ");" << std::endl;
    codestream << "#endif" << std::endl;
  }

  std::string GeneratorCSC::generate_code_right(std::string tFilename, int nRows, int nCols, bool bIsRowMajor, int lda, int ldc) {
    int* ptr_rowidx = NULL;
    int* ptr_colidx = NULL;
    double* ptr_values = NULL;
    int numRows = 0;
    int numCols = 0;
    int numElems = 0;
    std::stringstream codestream;

    ReaderCSC myReader;
    myReader.parse_file(tFilename, ptr_rowidx, ptr_colidx, ptr_values, numRows, numCols, numElems);

    int* rowidx = ptr_rowidx;
    int* colidx = ptr_colidx;
    double* values = ptr_values;
    size_t num_local_flops = 0;

#ifdef DEBUG
    std::cout << numRows << " " << numCols << " " << numElems << std::endl;
    std::cout << ptr_rowidx << " " << ptr_colidx << " " << ptr_values << std::endl;
#endif

    if (bAdd_ == false) {
      codestream << "for (int n = 0; n < " << numCols << "; n++) {" << std::endl;
      codestream << "  #pragma simd" << std::endl;

      if (this->bGenerateExitForCK_ == true) {
        codestream << "  for (int m = 0; m < exit_row; m++) { C[(n*" << ldc << ")+m] = 0.0; }" << std::endl;
      } else {
        codestream << "  for (int m = 0; m < " << nRows << "; m++) { C[(n*" << ldc << ")+m] = 0.0; }" << std::endl;
      }

      codestream << "}" << std::endl;
    }

    codestream << "#pragma simd" << std::endl;

    // we can exit this routine during Taylor Series calculation
    if (this->bGenerateExitForCK_ == true) {
      //codestream << "if ( __builtin_expect(i == exit_row, false) ) { return; }" << std::endl;
      codestream << "for (int i = 0; i < exit_row; i+=" << C_UNROLLING_FACTOR_RIGHT << ")" << std::endl;
    } else {
      codestream << "for (int i = 0; i < " << nRows << "; i+=" << C_UNROLLING_FACTOR_RIGHT << ")" << std::endl;
    }

    codestream << "{" << std::endl;

    // generate code
    for (int t = 0; t < numCols; t++) {
      int lcl_colElems = colidx[t + 1] - colidx[t];

      for (int z = 0; z < lcl_colElems; z++) {
        // loop over all rows in A
        for (int l = 0; l < C_UNROLLING_FACTOR_RIGHT; l++) {
          codestream << "C[(i)+(" << t* ldc + l << ")] += A[(i)+(" << l + (rowidx[colidx[t] + z]*lda) << ")] * ";
          codestream << "values[" << colidx[t] + z << "];" << std::endl;
          num_local_flops += 2;
        }
      }
    }

    codestream << "}" << std::endl;

    codestream << "#ifndef NDEBUG" << std::endl;

    if (this->bGenerateExitForCK_ == true) {
      codestream << "num_flops += " << num_local_flops << "*exit_row;" << std::endl;
    } else {
      codestream << "num_flops += " << num_local_flops* nRows << ";" << std::endl;
    }

    codestream << "#endif" << std::endl << std::endl;

    _mm_free(ptr_rowidx);
    _mm_free(ptr_colidx);
    _mm_free(ptr_values);

    return codestream.str();
  }

  std::string GeneratorCSC::generate_code_left(std::string tFilename, int nRows, int nCols, bool bIsRowMajor, int ldb, int ldc) {
    int* ptr_rowidx = NULL;
    int* ptr_colidx = NULL;
    double* ptr_values = NULL;
    int numRows = 0;
    int numCols = 0;
    int numElems = 0;
    std::stringstream codestream;

    ReaderCSC myReader;
    myReader.parse_file(tFilename, ptr_rowidx, ptr_colidx, ptr_values, numRows, numCols, numElems);

    int* rowidx = ptr_rowidx;
    int* colidx = ptr_colidx;
    double* values = ptr_values;
    size_t num_local_flops = 0;

#ifdef DEBUG
    std::cout << numRows << " " << numCols << " " << numElems << std::endl;
    std::cout << ptr_rowidx << " " << ptr_colidx << " " << ptr_values << std::endl;
#endif

    codestream << "#pragma nounroll" << std::endl;
    codestream << "for (int i = 0; i < " << nCols << "; i++)" << std::endl;
    codestream << "{" << std::endl;
#ifdef USE_THREE_ELEMENT_AVX_VECTORIZATION
    codestream << "#if defined(__SSE3__) && defined(__AVX256__)" << std::endl;
    codestream << "__m256i three_mask = _mm256_set_epi64x(0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);" << std::endl;
    codestream << "#endif" << std::endl;
#endif

    // loop over columns in A, rows in B
    for (int l = 0; l < nRows; l++) {

      if (bAdd_ == false) {
        // set C column to zero
        if (l == 0) {
          // we can exit this routine during Taylor Series calculation
          if (this->bGenerateExitForCK_ == true) {
            codestream << "  #pragma simd" << std::endl;
            codestream << "  for (int m = 0; m < exit_col; m++) {" << std::endl;
          } else {
            codestream << "  #pragma simd" << std::endl;
            codestream << "  for (int m = 0; m < " << nRows << "; m++) {" << std::endl;
          }

          codestream << "    C[(i*" << ldc << ")+m] = 0.0;" << std::endl;
          codestream << "  }" << std::endl;
        }
      }

      // we can exit this routine during Taylor Series calculation
      if (this->bGenerateExitForCK_ == true) {
        codestream << "#ifndef NDEBUG" << std::endl;
        codestream << "num_flops += " << num_local_flops << ";" << std::endl;
        num_local_flops = 0;
        codestream << "#endif" << std::endl;

        for (int o = 0; o < this->BasisfunctionsCounter_.size(); o++) {
          if (l == this->BasisfunctionsCounter_[o]) {
            codestream << "if ( __builtin_expect(exit_col == " << l << ", false) ) { continue; }" << std::endl;
          }
        }
      }

      int lcl_colElems = colidx[l + 1] - colidx[l];
      codestream << "#if defined(__SSE3__) || defined(__AVX256__)" << std::endl;

      if (lcl_colElems > 0) {
        codestream << "#if defined(__SSE3__) && defined(__AVX256__)" << std::endl;
        codestream << "__m256d b" << l << " = _mm256_broadcast_sd(&B[(i*" << ldb << ")+" << l << "]);" << std::endl;
        codestream << "#endif" << std::endl;

        codestream << "#if defined(__SSE3__) && !defined(__AVX256__)" << std::endl;
        codestream << "__m128d b" << l << " = _mm_loaddup_pd(&B[(i*" << ldb << ")+" << l << "]);" << std::endl;
        codestream << "#endif" << std::endl;
      }

      // loop over all rows in A
      int z;
#pragma novector

      for (z = 0; z < lcl_colElems; z++) {
        // 4 element vector might be possible
        if (z < (lcl_colElems - 3)) {
          // generate 256bit vector instruction
          if ((rowidx[colidx[l] + z] + 1 == rowidx[colidx[l] + (z + 1)]) &&
              (rowidx[colidx[l] + z] + 2 == rowidx[colidx[l] + (z + 2)]) &&
              (rowidx[colidx[l] + z] + 3 == rowidx[colidx[l] + (z + 3)]) ) {
            generate_code_left_innerloop_4vector(codestream, ldc, l, z, rowidx, colidx);
            //num_local_flops += 8;
            z += 3;
          }

#ifdef USE_THREE_ELEMENT_AVX_VECTORIZATION
          // generate 256bit vector instruction (but using only the 3 lower elements)
          else if ((rowidx[colidx[l] + z] + 1 == rowidx[colidx[l] + (z + 1)]) &&
                   (rowidx[colidx[l] + z] + 2 == rowidx[colidx[l] + (z + 2)]) ) {
            generate_code_left_innerloop_3vector(codestream, ldc, l, z, rowidx, colidx);
            z += 2;
            //num_local_flops += 6;
          }

#endif
          // generate 128bit vector instruction
          else if (rowidx[colidx[l] + z] + 1 == rowidx[colidx[l] + (z + 1)]) {
            generate_code_left_innerloop_2vector(codestream, ldc, l, z, rowidx, colidx);
            //num_local_flops += 4;
            z++;
          }
          //scalar
          else {
            generate_code_left_innerloop_scalar(codestream, ldc, l, z, rowidx, colidx);
            //num_local_flops += 2;
          }
        }
        // 2 element vector might be possible
        else if (z < (lcl_colElems - 1)) {
          // generate 128bit vector instruction
          if (rowidx[colidx[l] + z] + 1 == rowidx[colidx[l] + (z + 1)]) {
            generate_code_left_innerloop_2vector(codestream, ldc, l, z, rowidx, colidx);
            //num_local_flops += 4;
            z++;
          }
          //scalar
          else {
            generate_code_left_innerloop_scalar(codestream, ldc, l, z, rowidx, colidx);
            //num_local_flops += 2;
          }
        }

        // scalar anyway
        else {
          generate_code_left_innerloop_scalar(codestream, ldc, l, z, rowidx, colidx);
          //num_local_flops += 2;
        }

      }


      codestream << "#else" << std::endl;

      for (z = 0; z < lcl_colElems; z++) {
        codestream << "C[(i*" << ldc << ")+" << rowidx[colidx[l] + z] << "] += ";
        codestream << "values[" << colidx[l] + z << "]";
        codestream << " * B[(i*" << ldb << ")+" << l << "];" << std::endl;
        num_local_flops += 2;
      }

      codestream << "#endif" << std::endl;

      if (this->bGenerateExitForCK_ == true) {
        codestream << "#ifndef NDEBUG" << std::endl;
        codestream << "num_flops += " << num_local_flops << ";" << std::endl;
        num_local_flops = 0;
        codestream << "#endif" << std::endl;
      }
    }

    codestream << std::endl;
    codestream << "}" << std::endl << std::endl;

    codestream << "#ifndef NDEBUG" << std::endl;
    codestream << "num_flops += " << num_local_flops* nCols << ";" << std::endl;
    codestream << "#endif" << std::endl << std::endl;

    _mm_free(ptr_rowidx);
    _mm_free(ptr_colidx);
    _mm_free(ptr_values);

    return codestream.str();
  }

}

