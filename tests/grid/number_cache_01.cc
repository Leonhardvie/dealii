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



// test that we correctly compute the number of lines, quads, hexes, etc and
// store them in the number cache of the Triangulation class. use a simple
// grid for this

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_in.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/manifold_lib.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include "../tests.h"


template <int dim>
void
output(const Triangulation<dim> &tria)
{
  deallog << "  " << tria.n_active_cells() << std::endl;
  deallog << "  " << tria.n_cells() << std::endl;
  deallog << "  " << tria.n_active_lines() << std::endl;
  deallog << "  " << tria.n_lines() << std::endl;
  deallog << "  " << tria.n_active_quads() << std::endl;
  deallog << "  " << tria.n_quads() << std::endl;
  deallog << "  " << tria.n_active_hexs() << std::endl;
  deallog << "  " << tria.n_hexs() << std::endl;

  for (unsigned int i = 0; i < tria.n_levels(); ++i)
    {
      deallog << "  " << tria.n_active_cells(i) << std::endl;
      deallog << "  " << tria.n_cells(i) << std::endl;
      if (dim == 1)
        {
          deallog << "  " << tria.n_active_lines(i) << std::endl;
          deallog << "  " << tria.n_lines(i) << std::endl;
        }

      if (dim == 2)
        {
          deallog << "  " << tria.n_active_quads(i) << std::endl;
          deallog << "  " << tria.n_quads(i) << std::endl;
        }

      if (dim == 3)
        {
          deallog << "  " << tria.n_active_hexs(i) << std::endl;
          deallog << "  " << tria.n_hexs(i) << std::endl;
        }
    }
}


template <int dim>
void
test()
{
  deallog << dim << 'd' << std::endl;

  Triangulation<dim> tria;
  GridGenerator::hyper_cube(tria);

  output(tria);

  deallog << "Refining..." << std::endl;

  tria.begin_active()->set_refine_flag();
  tria.execute_coarsening_and_refinement();

  output(tria);
}


int
main()
{
  initlog();
  deallog << std::setprecision(2);

  test<1>();
  test<2>();
  test<3>();
}
