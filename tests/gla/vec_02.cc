// ---------------------------------------------------------------------
//
// Copyright (C) 2004 - 2020 by the deal.II authors
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



// assignment of ghost vectors

#include <deal.II/base/index_set.h>

#include <deal.II/lac/generic_linear_algebra.h>

#include <iostream>
#include <vector>

#include "../tests.h"

#include "gla.h"

template <class LA>
void
test()
{
  unsigned int myid    = Utilities::MPI::this_mpi_process(MPI_COMM_WORLD);
  unsigned int numproc = Utilities::MPI::n_mpi_processes(MPI_COMM_WORLD);

  if (myid == 0)
    deallog << "numproc=" << numproc << std::endl;

  // each processor owns 2 indices and all
  // are ghosting Element 1 (the second)

  IndexSet local_active(numproc * 2);
  local_active.add_range(myid * 2, myid * 2 + 2);
  IndexSet local_relevant(numproc * 2);
  local_relevant.add_range(1, 2);

  typename LA::MPI::Vector vb(local_active, MPI_COMM_WORLD);
  typename LA::MPI::Vector v(local_active, local_relevant, MPI_COMM_WORLD);
  typename LA::MPI::Vector v2(local_active, local_relevant, MPI_COMM_WORLD);

  vb = 1.0;
  v2 = vb;

  // set local values
  vb(myid * 2)     = myid * 2.0;
  vb(myid * 2 + 1) = myid * 2.0 + 1.0;

  vb.compress(VectorOperation::insert);
  vb *= 2.0;
  v = vb;

  Assert(vb.size() == numproc * 2, ExcInternalError());
  Assert(v.size() == numproc * 2, ExcInternalError());

  Assert(!vb.has_ghost_elements(), ExcInternalError());
  Assert(v.has_ghost_elements(), ExcInternalError());

  // check local values
  if (Utilities::MPI::this_mpi_process(MPI_COMM_WORLD) == 0)
    {
      deallog << myid * 2 << ':' << get_real_assert_zero_imag(v(myid * 2))
              << std::endl;
      deallog << myid * 2 + 1 << ':'
              << get_real_assert_zero_imag(v(myid * 2 + 1)) << std::endl;
    }

  // assignment from ghosted to ghosted
  v2 = v;
  Assert(get_real_assert_zero_imag(v2(1)) == 2.0, ExcInternalError());
  Assert(get_real_assert_zero_imag(v2(myid * 2)) == myid * 4.0,
         ExcInternalError());
  Assert(get_real_assert_zero_imag(v2(myid * 2 + 1)) == myid * 4.0 + 2.0,
         ExcInternalError());



  Assert(get_real_assert_zero_imag(v(myid * 2)) == myid * 4.0,
         ExcInternalError());
  Assert(get_real_assert_zero_imag(v(myid * 2 + 1)) == myid * 4.0 + 2.0,
         ExcInternalError());


  // check ghost values
  if (Utilities::MPI::this_mpi_process(MPI_COMM_WORLD) == 0)
    deallog << "ghost: " << get_real_assert_zero_imag(v(1)) << std::endl;
  Assert(get_real_assert_zero_imag(v(1)) == 2.0, ExcInternalError());

  // done
  if (myid == 0)
    deallog << "OK" << std::endl;
}



int
main(int argc, char **argv)
{
  Utilities::MPI::MPI_InitFinalize mpi_initialization(argc, argv, 1);
  unsigned int myid = Utilities::MPI::this_mpi_process(MPI_COMM_WORLD);

  deallog.push(Utilities::int_to_string(myid));

  if (myid == 0)
    {
      initlog();
      deallog << std::setprecision(4);

      {
        deallog.push("PETSc");
        test<LA_PETSc>();
        deallog.pop();
        deallog.push("Trilinos");
        test<LA_Trilinos>();
        deallog.pop();
      }
    }
  else
    {
      deallog.push("PETSc");
      test<LA_PETSc>();
      deallog.pop();
      deallog.push("Trilinos");
      test<LA_Trilinos>();
      deallog.pop();
    }

  if (myid == 9999)
    test<LA_Dummy>();
}
