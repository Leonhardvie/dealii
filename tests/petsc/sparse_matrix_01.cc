// ---------------------------------------------------------------------
//
// Copyright (C) 2004 - 2018 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------



// check SparseMatrix::add(factor, other)

#include <deal.II/lac/petsc_sparse_matrix.h>

#include <iostream>
#include <vector>

#include "../tests.h"


void
test()
{
  const unsigned int          s = 10;
  PETScWrappers::SparseMatrix m(s, s, s);
  for (unsigned int i = 0; i < m.m(); ++i)
    for (unsigned int j = 0; j <= i; ++j)
      m.set(i, j, i + 2 * j);

  m.compress(VectorOperation::insert);

  PETScWrappers::SparseMatrix m2(s, s, s);
  m2.set(0, 1, 5.0);
  for (unsigned int i = 0; i < m2.m(); ++i)
    m2.set(i, i, 1.0 + i);
  m2.compress(VectorOperation::insert);

  // we now print the matrix m,
  // print after adding m2, and then subtract m2 again
  // to get the original matrix back.

  deallog << "before: " << m(0, 1) << std::endl;
  for (unsigned int i = 0; i < s; ++i)
    deallog << m(i, i) << ' ';
  deallog << std::endl;

  m.add(1.0, m2);

  deallog << "after: " << m(0, 1) << std::endl;
  for (unsigned int i = 0; i < s; ++i)
    deallog << m(i, i) << ' ';
  deallog << std::endl;

  m.add(-1.0, m2);

  deallog << "back to original: " << m(0, 1) << std::endl;
  for (unsigned int i = 0; i < s; ++i)
    deallog << m(i, i) << ' ';
  deallog << std::endl;

  deallog << "OK" << std::endl;
}



int
main(int argc, char **argv)
{
  initlog();

  Utilities::MPI::MPI_InitFinalize mpi_initialization(argc, argv, 1);
  test();
}
