// ---------------------------------------------------------------------
//
// Copyright (C) 2003 - 2020 by the deal.II authors
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



// generate two cubes that are attached to each other in a way so that
// the edges are all ok, but the normals of the common face don't
// match up for the standard orientation of the normals. we thus have
// to store the face orientation in each cell
//
// for this grid, check that vertex numbers still match up

#include <deal.II/grid/grid_reordering.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include "../tests.h"

#include "mesh_3d.h"



int
main()
{
  initlog();

  Triangulation<3> coarse_grid;
  create_two_cubes(coarse_grid);

  const Triangulation<3>::active_cell_iterator cells[2] = {
    coarse_grid.begin_active(), std::next(coarse_grid.begin_active())};

  // output all vertices
  for (unsigned int c = 0; c < 2; ++c)
    for (const unsigned int v : GeometryInfo<3>::vertex_indices())
      deallog << "Cell " << c << ", vertex " << v << ": "
              << cells[c]->vertex_index(v) << "  @  " << cells[c]->vertex(v)
              << std::endl;

  // make sure by hand that certain
  // vertices match up
  AssertThrow(cells[0]->vertex(1) == cells[1]->vertex(4), ExcInternalError());
  AssertThrow(cells[0]->vertex(3) == cells[1]->vertex(6), ExcInternalError());
  AssertThrow(cells[0]->vertex(5) == cells[1]->vertex(5), ExcInternalError());
  AssertThrow(cells[0]->vertex(7) == cells[1]->vertex(7), ExcInternalError());
}
