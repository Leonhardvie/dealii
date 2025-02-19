// ---------------------------------------------------------------------
//
// Copyright (C) 2001 - 2018 by the deal.II authors
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



#include <deal.II/base/numbers.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/grid_tools.h>
#include <deal.II/grid/tria.h>

#include "../tests.h"


// check GridTools::diameter
template <int dim>
void
test1()
{
  // test 1: hypercube
  if (true)
    {
      Triangulation<dim> tria;
      GridGenerator::hyper_cube(tria);

      for (unsigned int i = 0; i < 2; ++i)
        {
          tria.refine_global(2);
          deallog << dim << "d, "
                  << "hypercube diameter, " << i * 2
                  << " refinements: " << GridTools::diameter(tria) << std::endl;
        };
    };

  // test 2: hyperball
  if (dim == 2)
    {
      Triangulation<dim> tria;
      GridGenerator::hyper_ball(tria, Point<dim>(), 1);

      for (unsigned int i = 0; i < 2; ++i)
        {
          tria.refine_global(2);
          deallog << dim << "d, "
                  << "hyperball diameter, " << i * 2
                  << " refinements: " << GridTools::diameter(tria) << std::endl;
        };
    };
}


// GridTools::transform
void
test2()
{
  std::ostream &   logfile = deallog.get_file_stream();
  Triangulation<2> tria;
  GridGenerator::hyper_cube(tria);

  logfile << "Unchanged grid:" << std::endl;
  GridOut().write_gnuplot(tria, logfile);

  logfile << "Shifted grid:" << std::endl;
  const Point<2> shift(1, 2);
  GridTools::shift(shift, tria);
  GridOut().write_gnuplot(tria, logfile);

  logfile << "Rotated grid:" << std::endl;
  GridTools::rotate(numbers::PI_4, tria);
  GridOut().write_gnuplot(tria, logfile);
}


int
main()
{
  initlog();
  deallog << std::setprecision(4);
  deallog.get_file_stream() << std::setprecision(4);

  test1<1>();
  test1<2>();
  test1<3>();

  test2();

  return 0;
}
