// ---------------------------------------------------------------------
//
// Copyright (C) 2021 - 2021 by the deal.II authors
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

/*
 * Test the FaceQuadratureGenerator class, by setting up simple cuts over a face
 * of the unit box and writing the generated quadrature rules to the output
 * file.
 *
 * Each function beginning with "test_" sets up a level set function and then
 * calls the function create_and_print_quadratures() to generate the
 * quadratures.
 */

#include <deal.II/base/function_level_set.h>
#include <deal.II/base/geometry_info.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/non_matching/quadrature_generator.h>

#include <vector>

#include "../tests.h"

#include "quadrature_printing.h"


/*
 * Create immersed quadrature rules over the face_index:th unit-box-face for the
 * incoming level set function. Print the constructed quadrature rules to
 * deallog.
 */
template <int dim>
void
create_and_print_quadratures(const Function<dim> &levelset,
                             const unsigned int   face_index)
{
  const hp::QCollection<1> quadratures1D(QGauss<1>(2));
  const BoundingBox<dim>   box = create_unit_bounding_box<dim>();

  NonMatching::FaceQuadratureGenerator<dim> quadrature_generator(quadratures1D);
  quadrature_generator.generate(levelset, box, face_index);

  deallog << "Inside quadrature" << std::endl;
  print_quadrature(quadrature_generator.get_inside_quadrature());
  deallog << "Outside quadrature" << std::endl;
  print_quadrature(quadrature_generator.get_outside_quadrature());
  deallog << "Surface quadrature" << std::endl;
  print_surface_quadrature(quadrature_generator.get_surface_quadrature());
}



/*
 * For each coordinate direction, set up a level set function corresponding to a
 * plane cutting through the unit cell, having the normal aligned with the
 * coordinate direction. Create and print the constructed quadratures for the
 * two faces that are intersected. We expect that the constructed quadrature has
 * the same number of points in the inside/outside region and that they are
 * tensor products.
 */
template <int dim>
void
test_plane_cuts_through_center()
{
  deallog << "test_vertical_cuts_through_center" << std::endl;
  deallog << "dim=" << dim << std::endl;

  Point<dim> center;
  for (int d = 0; d < dim; ++d)
    center(d) = .5;

  // For each coordinate direction set up a plane throught the center.
  for (int plane_direction = 0; plane_direction < dim; ++plane_direction)
    {
      const Tensor<1, dim> normal = Point<dim>::unit_vector(plane_direction);
      const Functions::LevelSet::Plane<dim> levelset(center, normal);

      // Test all faces that are intersected by the plane.
      for (unsigned int f = 0; f < GeometryInfo<dim>::faces_per_cell; ++f)
        {
          const int normal_direction =
            GeometryInfo<dim>::unit_normal_direction[f];
          if (plane_direction != normal_direction)
            {
              deallog << "plane direction = " << plane_direction << ", ";
              deallog << "face = " << f << std::endl;
              create_and_print_quadratures(levelset, f);
              deallog << std::endl;
            }
        }
    }
}



int
main()
{
  initlog();

  test_plane_cuts_through_center<2>();
  test_plane_cuts_through_center<3>();
}
