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

#ifndef GENERATORCSC_HPP
#define GENERATORCSC_HPP

#include <vector>

#include "Generator.hpp"

namespace seissolgen {

  /**
   * FIXME add description
   */
  class GeneratorCSC : public Generator {
    private:
      bool bGenerateExitForCK_;
      bool bAdd_;
      std::vector<int> BasisfunctionsCounter_;

      void generate_code_left_innerloop_scalar(std::stringstream& codestream, int ldc, int l, int z, int* rowidx, int* colidx);

      void generate_code_left_innerloop_2vector(std::stringstream& codestream, int ldc, int l, int z, int* rowidx, int* colidx);

      void generate_code_left_innerloop_3vector(std::stringstream& codestream, int ldc, int l, int z, int* rowidx, int* colidx);

      void generate_code_left_innerloop_4vector(std::stringstream& codestream, int ldc, int l, int z, int* rowidx, int* colidx);

    public:
      GeneratorCSC();

      GeneratorCSC(bool bGenerateExitForCK, int nMaxOrder, bool bAdd);

      virtual std::string generate_code_right(std::string tFilename, int nRows, int nCols, bool bIsRowMajor, int lda, int ldc);

      virtual std::string generate_code_left(std::string tFilename, int nRows, int nCols, bool bIsRowMajor, int ldb, int ldc);
  };

}

#endif /* GENERATORCSC_HPP */

