// ---------------------------------------------------------------------
//
// Copyright (C) 2017 - 2019 by the deal.II authors
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

// Test Utilities::MPI::create_ascending_partitioning()

#include <deal.II/base/mpi.h>
#include <deal.II/base/point.h>

#include <vector>

#include "../tests.h"

void
test()
{
  MPI_Comm   comm(MPI_COMM_WORLD);
  const auto my_proc = Utilities::MPI::this_mpi_process(comm);
  const auto n_proc  = Utilities::MPI::n_mpi_processes(comm);

  const auto my_size = (my_proc == 2 ? 0 : my_proc * 2 + 5);
  const auto vec = Utilities::MPI::create_ascending_partitioning(comm, my_size);

  // correct number:
  AssertThrow(vec.size() == n_proc, ExcInternalError());

  // ascending one-to-one
  AssertThrow(vec[my_proc].is_ascending_and_one_to_one(comm),
              ExcInternalError());

  // same size:
  const auto size = vec[0].size();
  for (unsigned int i = 1; i < vec.size(); ++i)
    AssertThrow(size == vec[i].size(), ExcInternalError());

  // 1:1
  for (unsigned int i = 0; i < vec.size(); ++i)
    for (unsigned int j = i + 1; j < vec.size(); ++j)
      {
        const IndexSet intersection = vec[i] & vec[j];
        AssertThrow(intersection.is_empty(), ExcInternalError());
      }

  // union of all
  IndexSet all(size);
  for (unsigned int i = 0; i < vec.size(); ++i)
    all.add_indices(vec[i]);

  AssertThrow(all == complete_index_set(size), ExcInternalError());

  // finally print:
  deallog << "Total size=" << size << std::endl;
  for (unsigned int p = 0; p < vec.size(); ++p)
    {
      deallog << "p=" << p << ':' << std::endl;
      vec[p].print(deallog.get_file_stream());
    }
}

int
main(int argc, char *argv[])
{
  Utilities::MPI::MPI_InitFinalize mpi_initialization(argc, argv, 1);

  MPILogInitAll log;

  test();
  return 0;
}
