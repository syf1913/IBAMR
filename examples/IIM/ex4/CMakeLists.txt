## ---------------------------------------------------------------------
##
## Copyright (c) 2020 - 2021 by the IBAMR developers
## All rights reserved.
##
## This file is part of IBAMR.
##
## IBAMR is free software and is distributed under the 3-clause BSD
## license. The full text of the license can be found in the file
## COPYRIGHT at the top level directory of IBAMR.
##
## ---------------------------------------------------------------------

IBAMR_ADD_EXAMPLE(
  TARGET_NAME
    "IIM-ex4-2d"
  OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/examples/IIM/ex4"
  OUTPUT_NAME
    main2d
  EXAMPLE_GROUP
    examples-IIM
  SOURCES
    example.cpp
  REQUIRES
    IBAMR_HAVE_LIBMESH
  LINK_TARGETS
    IBAMR2d
  INPUT_FILES
    input2d.LMred
    input2d.SMred
  EXTRA_FILES
    compute_St.m
    )

