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

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <string>

namespace seissolgen {

  /**
   * Abstract definition of the a code generator for sparse-dense and dense-sparse
   * matrix multiplications.
   */
  class Generator {
    public:

      /**
       * Generates dense x sparse multiplication code
       *
       * @param tFilename file that contains sparse matrix definition
       * @param nRows number of rows of the left dense matrix A
       * @param nCols number of cols of the left dense matrix A
       * @param bIsRowMajor specifies whethter A and C are row-major or column-major
       * @param lda lda of matrix A
       * @param ldc ldc of matrix C
       */
      virtual std::string generate_code_right(std::string tFilename, int nRows, int nCols, bool bIsRowMajor, int lda, int ldc) = 0;

      /**
       * Generates sparse x dense multiplication code
       *
       * @param tFilename file that contains sparse matrix definition
       * @param nRows number of rows of the right dense matrix B
       * @param nCols number of cols of the right dense matrix B
       * @param bIsRowMajor specifies whethter B and C are row-major or column-major
       * @param ldb ldb of matrix B
       * @param ldc ldc of matrix C
       */
      virtual std::string generate_code_left(std::string tFilename, int nRows, int nCols, bool bIsRowMajor, int ldb, int ldc) = 0;
  };

}

#endif /* GENERATOR_HPP */

