// ---------------------------------------------------------------------
//
// Copyright (C) 2013 - 2020 by the deal.II authors
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

#include <deal.II/base/geometry_info.h>

#include <bitset>

#include "../tests.h"


//
// Test GeometryInfo<dim>::face_to_cell_vertices
// for correct behavior under face_orientation face_flip and face_rotation
//


template <int dim>
void
test_vertices()
{
  deallog << dim << "D:" << std::endl;

  for (const unsigned int i : GeometryInfo<dim>::face_indices())
    {
      deallog << "face " << i << ':' << std::endl;

      for (unsigned int o = 0; o < 8; ++o)
        {
          const std::bitset<3> orientation = o;

          deallog << "orientation " << orientation[0] << ", flip "
                  << orientation[1] << ", rotation " << orientation[2] << ':'
                  << std::endl
                  << "    ";

          for (unsigned int j = 0; j < GeometryInfo<dim>::vertices_per_face;
               ++j)
            {
              deallog << " (" << j << " -> "
                      << GeometryInfo<dim>::face_to_cell_vertices(
                           i, j, orientation[0], orientation[1], orientation[2])
                      << " )";
            }
          deallog << std::endl;
        }
    }
}


template <int dim>
void
test_lines()
{
  deallog << dim << "D:" << std::endl;

  for (const unsigned int i : GeometryInfo<dim>::face_indices())
    {
      deallog << "face " << i << ':' << std::endl;

      for (unsigned int o = 0; o < 8; ++o)
        {
          const std::bitset<3> orientation = o;

          deallog << "orientation " << orientation[0] << ", flip "
                  << orientation[1] << ", rotation " << orientation[2] << ':'
                  << std::endl
                  << "    ";

          for (unsigned int j = 0; j < GeometryInfo<dim>::lines_per_face; ++j)
            {
              deallog << " (" << j << " -> "
                      << GeometryInfo<dim>::face_to_cell_lines(
                           i, j, orientation[0], orientation[1], orientation[2])
                      << " )";
            }
          deallog << std::endl;
        }
    }
}


int
main()
{
  initlog();

  deallog << std::endl
          << "GeometryInfo<dim>::face_to_cell_vertices:" << std::endl;

  test_vertices<1>();
  test_vertices<2>();
  test_vertices<3>();

  deallog << std::endl
          << std::endl
          << "GeometryInfo<dim>::face_to_cell_lines:" << std::endl;

  test_lines<2>();
  test_lines<3>();

  return 0;
}
