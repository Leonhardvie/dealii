// ---------------------------------------------------------------------
//
// Copyright (C) 2008 - 2018 by the deal.II authors
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


// create a shared tria mesh and distribute multigrid
// Unlike tria_multigrid_01, this test does not need p4est library

#include <deal.II/base/tensor.h>

#include <deal.II/distributed/shared_tria.h>
#include <deal.II/distributed/tria.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include "../tests.h"


template <int dim>
void
test()
{
  parallel::shared::Triangulation<dim> shared_tria(
    MPI_COMM_WORLD,
    typename Triangulation<dim>::MeshSmoothing(
      Triangulation<dim>::limit_level_difference_at_vertices),
    true,
    typename parallel::shared::Triangulation<dim>::Settings(
      parallel::shared::Triangulation<dim>::partition_zorder |
      parallel::shared::Triangulation<dim>::construct_multigrid_hierarchy));

  unsigned int refinements = 2;
  GridGenerator::subdivided_hyper_cube(shared_tria, 2, -1, 1);
  shared_tria.refine_global(refinements);
  for (typename Triangulation<dim>::active_cell_iterator cell =
         shared_tria.begin_active();
       cell != shared_tria.end();
       ++cell)
    if (cell->center().norm() < 0.55)
      cell->set_refine_flag();
  shared_tria.execute_coarsening_and_refinement();
  for (typename Triangulation<dim>::active_cell_iterator cell =
         shared_tria.begin_active();
       cell != shared_tria.end();
       ++cell)
    if (cell->center().norm() > 0.3 && cell->center().norm() < 0.42)
      cell->set_refine_flag();
  shared_tria.execute_coarsening_and_refinement();
  if (dim != 1)
    {
      for (typename Triangulation<dim>::active_cell_iterator cell =
             shared_tria.begin_active();
           cell != shared_tria.end();
           ++cell)
        if (cell->at_boundary() &&
            (cell->center()[0] < 0 || cell->center()[1] < 0))
          cell->set_refine_flag();
      shared_tria.execute_coarsening_and_refinement();
    }

  deallog << "(CellId,level_subdomain_id) for each active cell:" << std::endl;
  for (unsigned int lvl = 0; lvl < shared_tria.n_levels(); ++lvl)
    {
      typename Triangulation<dim>::cell_iterator cell = shared_tria.begin(lvl),
                                                 endc = shared_tria.end(lvl);
      for (; cell != endc; ++cell)
        if (cell->level_subdomain_id() != numbers::artificial_subdomain_id)
          deallog << '(' << cell->id().to_string() << ','
                  << cell->level_subdomain_id() << ')' << std::endl;
    }
}

int
main(int argc, char *argv[])
{
  Utilities::MPI::MPI_InitFinalize mpi_initialization(argc, argv, 1);
  MPILogInitAll                    all;

  deallog.push("1d");
  test<1>();
  deallog.pop();
  deallog.push("2d");
  test<2>();
  deallog.pop();
  deallog.push("3d");
  test<3>();
  deallog.pop();

  deallog << "OK" << std::endl;
}
