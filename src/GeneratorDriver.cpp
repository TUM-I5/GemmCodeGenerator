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

#define MAX_CK_ORDER 9

#include <iostream>
#include <cstdlib>
#include <fstream>

#include "GeneratorCSC.hpp"
#include "GeneratorDense.hpp"

void generate_custom_signature_right(std::ofstream& out, std::string tRoutineName) {
  //out << "inline void " << tRoutineName << "(double* A, double* values, double* C)" << std::endl << "{" << std::endl;
  out << "inline void " << tRoutineName << "(double* A, double* values, double* C, double* A_prefetch = NULL, double* B_prefetch = NULL, double* C_prefetch = NULL)" << std::endl << "{" << std::endl;
}

void generate_custom_signature_right_CK(std::ofstream& out, std::string tRoutineName) {
  //out << "inline void " << tRoutineName << "(double* A, double* values, double* C, int exit_row)" << std::endl << "{" << std::endl;
  out << "inline void " << tRoutineName << "(double* A, double* values, double* C, int exit_row, double* A_prefetch = NULL, double* B_prefetch = NULL, double* C_prefetch = NULL)" << std::endl << "{" << std::endl;
}

void generate_custom_signature_left(std::ofstream& out, std::string tRoutineName) {
  //out << "inline void " << tRoutineName << "(double* values, double* B, double* C)" << std::endl << "{" << std::endl;
  out << "inline void " << tRoutineName << "(double* values, double* B, double* C, double* A_prefetch = NULL, double* B_prefetch = NULL, double* C_prefetch = NULL)" << std::endl << "{" << std::endl;
}

void generate_custom_signature_left_CK(std::ofstream& out, std::string tRoutineName) {
  //out << "inline void " << tRoutineName << "(double* values, double* B, double* C, int exit_col)" << std::endl << "{" << std::endl;
  out << "inline void " << tRoutineName << "(double* values, double* B, double* C, int exit_col, double* A_prefetch = NULL, double* B_prefetch = NULL, double* C_prefetch = NULL)" << std::endl << "{" << std::endl;
}

void generate_custom_signature_dense(std::ofstream& out, std::string tRoutineName) {
  //out << "inline void " << tRoutineName << "(double* A, double* B, double* C)" << std::endl << "{" << std::endl;
  out << "inline void " << tRoutineName << "(double* A, double* B, double* C, double* A_prefetch = NULL, double* B_prefetch = NULL, double* C_prefetch = NULL)" << std::endl << "{" << std::endl;
}

void generate_custom_signature_dense_CK(std::ofstream& out, std::string tRoutineName) {
  //out << "inline void " << tRoutineName << "(double* A, double* B, double* C, int exit_col)" << std::endl << "{" << std::endl;
  out << "inline void " << tRoutineName << "(double* A, double* B, double* C, int exit_col, double* A_prefetch = NULL, double* B_prefetch = NULL, double* C_prefetch = NULL)" << std::endl << "{" << std::endl;
}

void generate_epilogue(std::ofstream& out) {
  out << "}" << std::endl << std::endl;
}

void generator_seissol_sparse(std::string tFileOut, std::string tRoutineName, std::string tFileIn, int nLeft, int nDenseRows, int nDenseCols, int nDenseLD, bool generateCK, bool bAdd) {
  std::string generated_code;
  std::ofstream out;

  out.open(tFileOut.c_str(), std::ios::app);

  seissolgen::GeneratorCSC* gen;

  gen = new seissolgen::GeneratorCSC(generateCK, MAX_CK_ORDER, bAdd);

  if (nLeft == 0) {
    if (generateCK == true)
      generate_custom_signature_right_CK(out, tRoutineName);
    else
      generate_custom_signature_right(out, tRoutineName);

    generated_code = gen->generate_code_right(tFileIn, nDenseRows, nDenseCols, false, nDenseLD, nDenseLD);
    out << generated_code;
  } else {
    if (generateCK == true)
      generate_custom_signature_left_CK(out, tRoutineName);
    else
      generate_custom_signature_left(out, tRoutineName);

    generated_code = gen->generate_code_left(tFileIn, nDenseRows, nDenseCols, false, nDenseLD, nDenseLD);
    out << generated_code;
  }

#ifdef DEBUG
  std::cout << "code was generated and exported to " << tFileOut << ":" << std::endl;
//  std::cout << generated_code << std::endl << std::endl;
#endif
  generate_epilogue(out);
  out.close();

  delete gen;
}

void generator_seissol_dense(std::string tFileOut, std::string tRoutineName, int nDenseM, int nDenseN, int nDenseK, int nDenseLDA, int nDenseLDB, int nDenseLDC, bool generateCK, bool bAdd) {
  std::string generated_code;
  std::ofstream out;
  seissolgen::GeneratorDense* gen;

  gen = new seissolgen::GeneratorDense(generateCK, MAX_CK_ORDER, bAdd);

  out.open(tFileOut.c_str(), std::ios::app);
  // generate code
  generated_code = gen->generate_dense(true, nDenseM, nDenseN, nDenseK, nDenseLDA, nDenseLDB, nDenseLDC);

  // generate function
  if (generateCK == true)
    generate_custom_signature_dense_CK(out, tRoutineName);
  else
    generate_custom_signature_dense(out, tRoutineName);

  // write code to file
  out << generated_code;

  // close function
  generate_epilogue(out);

  out.close();

#ifdef DEBUG
  std::cout << "code was generated and exported to " << tFileOut << ":" << std::endl;
//  std::cout << generated_code << std::endl << std::endl;
#endif

  delete gen;
}

void print_help() {
  std::cout << std::endl << "wrong usage -> exit!" << std::endl;
  std::cout << std::endl << std::endl;
  std::cout << "Usage (sparse*dense=dense, dense*sparse=dense):" << std::endl;
  std::cout << "    sparse" << std::endl;
  std::cout << "    filename to append" << std::endl;
  std::cout << "    rountine name" << std::endl;
  std::cout << "    matrix input" << std::endl;
  std::cout << "    sparse left / sparse right" << std::endl;
  std::cout << "    nDenseRows" << std::endl;
  std::cout << "    nDenseCols" << std::endl;
  std::cout << "    nDenseLD" << std::endl;
  std::cout << "    0: no add, otherwise: add" << std::endl;
  std::cout << std::endl << std::endl;
  std::cout << "Usage (sparse*dense=dense, dense*sparse=dense) for CK:" << std::endl;
  std::cout << "    sparseCK" << std::endl;
  std::cout << "    filename to append" << std::endl;
  std::cout << "    rountine name" << std::endl;
  std::cout << "    matrix input" << std::endl;
  std::cout << "    sparse left / sparse right" << std::endl;
  std::cout << "    nDenseRows" << std::endl;
  std::cout << "    nDenseCols" << std::endl;
  std::cout << "    nDenseLD" << std::endl;
  std::cout << "    0: no add, otherwise: add" << std::endl;
  std::cout << std::endl << std::endl;
  std::cout << "Usage (dense*dense=dense):" << std::endl;
  std::cout << "    dense" << std::endl;
  std::cout << "    filename to append" << std::endl;
  std::cout << "    rountine name" << std::endl;
  std::cout << "    nDenseM" << std::endl;
  std::cout << "    nDenseN" << std::endl;
  std::cout << "    nDenseK" << std::endl;
  std::cout << "    nDenseLDA" << std::endl;
  std::cout << "    nDenseLDB" << std::endl;
  std::cout << "    nDenseLDC" << std::endl;
  std::cout << "    0: no add, otherwise: add" << std::endl;
  std::cout << std::endl << std::endl;
  std::cout << "Usage (dense*dense=dense) for CK:" << std::endl;
  std::cout << "    denseCK" << std::endl;
  std::cout << "    filename to append" << std::endl;
  std::cout << "    rountine name" << std::endl;
  std::cout << "    nDenseM" << std::endl;
  std::cout << "    nDenseN" << std::endl;
  std::cout << "    nDenseK" << std::endl;
  std::cout << "    nDenseLDA" << std::endl;
  std::cout << "    nDenseLDB" << std::endl;
  std::cout << "    nDenseLDC" << std::endl;
  std::cout << "    0: no add, otherwise: add" << std::endl;
  std::cout << std::endl << std::endl;
}

int main(int argc, char* argv []) {
  if (argc != 11 && argc != 10) {
    print_help();
    return -1;
  }

  std::string tFileOut;
  std::string tRoutineName;
  std::string tType;

  if (argc == 10) {
    int nDenseRows = 0;
    int nDenseCols = 0;
    int nDenseLD = 0;
    int nLeft;
    int nAdd;
    std::string tFileIn;

    tType.assign(argv[1]);

    if (tType != "sparse" && tType != "sparseCK") {
      print_help();
      return -1;
    }

    bool generateCK = false;
    bool bAdd = true;

    if (tType == "sparseCK")
      generateCK = true;

    tFileOut.assign(argv[2]);
    tRoutineName.assign(argv[3]);
    tFileIn.assign(argv[4]);
    nLeft = atoi(argv[5]);
    nDenseRows = atoi(argv[6]);
    nDenseCols = atoi(argv[7]);
    nDenseLD = atoi(argv[8]);
    nAdd = atoi(argv[9]);

    if (nAdd == 0)
      bAdd = false;

    generator_seissol_sparse(tFileOut, tRoutineName, tFileIn, nLeft, nDenseRows, nDenseCols, nDenseLD, generateCK, bAdd);
  }

  if (argc == 11) {
    int nDenseM = 0;
    int nDenseN = 0;
    int nDenseK = 0;
    int nDenseLDA = 0;
    int nDenseLDB = 0;
    int nDenseLDC = 0;
    int nAdd = 0;

    tType.assign(argv[1]);

    if (tType != "dense" && tType != "denseCK") {
      print_help();
      return -1;
    }

    bool generateCK = false;
    bool bAdd = true;

    if (tType == "denseCK")
      generateCK = true;

    tFileOut.assign(argv[2]);
    tRoutineName.assign(argv[3]);
    nDenseM = atoi(argv[4]);
    nDenseN = atoi(argv[5]);
    nDenseK = atoi(argv[6]);
    nDenseLDA = atoi(argv[7]);
    nDenseLDB = atoi(argv[8]);
    nDenseLDC = atoi(argv[9]);
    nAdd = atoi(argv[10]);

    if (nAdd == 0)
      bAdd = false;

    generator_seissol_dense(tFileOut, tRoutineName, nDenseM, nDenseN, nDenseK, nDenseLDA, nDenseLDB, nDenseLDC, generateCK, bAdd);
  }
}

