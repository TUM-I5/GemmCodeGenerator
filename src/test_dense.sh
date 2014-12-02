#!/bin/sh
##
# @file
# This file is part of GemmCodeGenerator.
#
# @author Alexander Heinecke (alexander.heinecke AT mytum.de, http://www5.in.tum.de/wiki/index.php/Alexander_Heinecke,_M.Sc.,_M.Sc._with_honors)
#
# @section LICENSE
# Copyright (c) 2012-2014, Technische Universitaet Muenchen
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# @section DESCRIPTION
# <DESCRIPTION>
#
VEC=avx
START=1
END=84
ORDERS="4 10 20 35 56 84"
#ORDERS="56"
#START=56
#END=56

make clean
make

#for i in `seq $START 1 $END`
for i in ${ORDERS}
do
        echo "START TEST WITH MATRIX DIMENISON $i"
	rm -rf gen_matmul_dense.hpp
	./generator.exe dense gen_matmul_dense.hpp dense_test_square $i 9 $i $i $i $i 1 > /dev/null
        ./generator.exe dense gen_matmul_dense.hpp dense_test_rect $i 9 9 $i 9 $i 1 > /dev/null
	icpc -O3 -m$VEC -ansi-alias -DNDEBUG -DORDER_NUMBER=$i dgemm_test.cpp
#       g++ -O3 -mavx2 -mfma -funroll-loops -DNDEBUG -DORDER_NUMBER=$i dgemm_test.cpp
#	icpc -O2 -mavx -D__AVX2__ -fma -unroll-loops -DNDEBUG -DORDER_NUMBER=$i dgemm_test.cpp
	./a.out
	echo "END TEST WITH MATRIX DIMENISON $i"
done
