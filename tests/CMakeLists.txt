## ---------------------------------------------------------------------
##
## Copyright (C) 2013 - 2021 by the deal.II authors
##
## This file is part of the deal.II library.
##
## The deal.II library is free software; you can use it, redistribute
## it, and/or modify it under the terms of the GNU Lesser General
## Public License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
## The full text of the license can be found in the file LICENSE.md at
## the top level directory of deal.II.
##
## ---------------------------------------------------------------------

#
# Set up the testsuite.
#
# We define toplevel targets:
#    setup_tests    - set up testsuite subprojects
#    prune_tests    - remove all testsuite subprojects
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.3.0)

MACRO(SET_IF_EMPTY _variable)
  IF("${${_variable}}" STREQUAL "")
    SET(${_variable} ${ARGN})
  ENDIF()
ENDMACRO()

SET_IF_EMPTY(MAKEOPTS $ENV{MAKEOPTS})
SET_IF_EMPTY(DEAL_II_DIR $ENV{DEAL_II_DIR})

#
# A bit of necessary setup if tests are configured as a stand-alone
# project:
#

IF(NOT DEFINED DEAL_II_HAVE_TESTS_DIRECTORY)
  FIND_PACKAGE(deal.II 9.3.0 REQUIRED HINTS ${DEAL_II_DIR})
  DEAL_II_INITIALIZE_CACHED_VARIABLES()
  PROJECT(TESTSUITE CXX)
  FILE(WRITE ${CMAKE_BINARY_DIR}/detailed.log
    "#        CMAKE_CXX_COMPILER:     ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} on platform ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}"
    )
ENDIF()

#
# A bit of necessary setup if tests are included via ADD_SUBDIRECTORY from
# the top-level CMake project:
#

IF(DEFINED DEAL_II_HAVE_TESTS_DIRECTORY)

  #
  # We have to repeat the policy statement here because the new
  # CMAKE_MINIMUM_REQUIRED call resets our previous policy set in the main
  # CMakeLists.txt file.
  #
  IF("${CMAKE_VERSION}" VERSION_LESS "3.11" AND POLICY CMP0037)
    # allow to override "test" target for quick tests
    CMAKE_POLICY(SET CMP0037 OLD)
  ENDIF()

  #
  # If this CMakeLists.txt file is called from within the deal.II build
  # system, set up quick tests as well:
  #
  ADD_SUBDIRECTORY(quick_tests)

  MESSAGE(STATUS "Setting up testsuite")

  #
  # Write minimalistic CTestTestfile.cmake files to CMAKE_BINARY_DIR and
  # CMAKE_BINARY_DIR/tests:
  #
  FILE(WRITE ${CMAKE_BINARY_DIR}/CTestTestfile.cmake "SUBDIRS(tests)\n")

  SET(_options "-DDEAL_II_DIR=${CMAKE_BINARY_DIR}")

ELSE()

  MESSAGE(STATUS "This is CMake ${CMAKE_VERSION}")
  MESSAGE(STATUS "")

  IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    MESSAGE(FATAL_ERROR "The testsuite cannot be configured in-source. "
      "Please create a separate build directory!"
      )
  ENDIF()

  FIND_PACKAGE(deal.II 9.3.0 REQUIRED HINTS ${DEAL_II_DIR} $ENV{DEAL_II_DIR})
  PROJECT(testsuite NONE)
  SET(_options "-DDEAL_II_DIR=${DEAL_II_PATH}")
ENDIF()

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake "")

#
# Always undefine the following variables in the setup_tests target:
#
FOREACH(_var
    DIFF_DIR NUMDIFF_DIR TEST_PICKUP_REGEX TEST_TIME_LIMIT
    TEST_MPI_RANK_LIMIT TEST_THREAD_LIMIT ENABLE_PERFORMANCE_TESTS
    TESTING_ENVIRONMENT
    )
  LIST(APPEND _options "-U${_var}")
  IF(NOT "${${_var}}" STREQUAL "")
    LIST(APPEND _options "-D${_var}=${${_var}}")
  ENDIF()
ENDFOREACH()

#
# Find all testsuite subprojects, i.e., every directory that contains a
# CMakeLists.txt file (with the exception of "quick_tests").
#
SET(_categories)
FILE(GLOB _dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/*
  )
FOREACH(_dir ${_dirs})
  IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_dir}/CMakeLists.txt AND
      NOT ${_dir} MATCHES quick_tests)
    LIST(APPEND _categories ${_dir})
  ENDIF()
ENDFOREACH()

#
# Custom targets for the testsuite:
#

# Setup tests:
ADD_CUSTOM_TARGET(setup_tests)

# Remove all tests:
ADD_CUSTOM_TARGET(prune_tests)

FOREACH(_category ${_categories})
  SET(_category_dir ${CMAKE_CURRENT_SOURCE_DIR}/${_category})

  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_category})

  IF(DEAL_II_MSVC)
    SET(_command ${CMAKE_COMMAND} -G${CMAKE_GENERATOR} ${_options} ${_category_dir})
  ELSE()
    # Do not pass the generator with -G so that we use make instead of ninja
    # for the test projects. This is because calling ninja several times in
    # parallel for the same project will break the configuration.
    SET(_command ${CMAKE_COMMAND} ${_options} ${_category_dir} > /dev/null)
  ENDIF()

  ADD_CUSTOM_TARGET(setup_tests_${_category}
    COMMAND ${_command}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_category}
    COMMENT "Processing tests/${_category}"
    )
  ADD_DEPENDENCIES(setup_tests setup_tests_${_category})

  ADD_CUSTOM_TARGET(prune_tests_${_category}
    COMMAND ${CMAKE_COMMAND} -E remove_directory
      ${CMAKE_CURRENT_BINARY_DIR}/${_category}
    COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_CURRENT_BINARY_DIR}/${_category}
    COMMENT "Processing tests/${_category}"
    )
  ADD_DEPENDENCIES(prune_tests prune_tests_${_category})

  FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    "SUBDIRS(${_category})\n"
    )
ENDFOREACH()


IF(DEFINED DEAL_II_HAVE_TESTS_DIRECTORY)
  #
  # Add a dummy target to make files known to IDEs like qtcreator
  #

  FILE(GLOB _misc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*/*.h
  )

  ADD_CUSTOM_TARGET(dummy_tests_files
    SOURCES ${_misc}
  )

  MESSAGE(STATUS "Setting up testsuite - Done")
ENDIF()
