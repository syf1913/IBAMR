## ---------------------------------------------------------------------
##
## Copyright (c) 2021 - 2021 by the IBAMR developers
## All rights reserved.
##
## This file is part of IBAMR.
##
## IBAMR is free software and is distributed under the 3-clause BSD
## license. The full text of the license can be found in the file
## COPYRIGHT at the top level directory of IBAMR.
##
## ---------------------------------------------------------------------

FOREACH(_dir ex0 ex1 ex2 ex3 ex4 ex5 ex6 ex7 ex8 ex9)
  ADD_SUBDIRECTORY(${_dir})
ENDFOREACH()
