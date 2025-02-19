// ---------------------------------------------------------------------
//
// Copyright (C) 2007 - 2020 by the deal.II authors
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



// test the FEValues views and extractor classes. this test is for
// get_function_hessians for vector components and a non-primitive element

#include <deal.II/base/function.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_dgq.h>
#include <deal.II/fe/fe_nedelec.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_raviart_thomas.h>
#include <deal.II/fe/fe_system.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/mapping_q1.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/manifold_lib.h>

#include <deal.II/lac/vector.h>

#include "../tests.h"



template <int dim>
void
test(const Triangulation<dim> &tr, const FiniteElement<dim> &fe)
{
  deallog << "FE=" << fe.get_name() << std::endl;

  DoFHandler<dim> dof(tr);
  dof.distribute_dofs(fe);

  Vector<double> fe_function(dof.n_dofs());
  for (unsigned int i = 0; i < dof.n_dofs(); ++i)
    fe_function(i) = i + 1;

  const QGauss<dim> quadrature(2);
  FEValues<dim>     fe_values(fe,
                          quadrature,
                          update_values | update_gradients | update_hessians);
  fe_values.reinit(dof.begin_active());

  std::vector<Tensor<3, dim>> selected_vector_values(quadrature.size());
  std::vector<std::vector<Tensor<2, dim>>> vector_values(
    quadrature.size(), std::vector<Tensor<2, dim>>(fe.n_components()));

  fe_values.get_function_hessians(fe_function, vector_values);

  for (unsigned int c = 0; c < fe.n_components(); ++c)
    // use a vector extractor if there
    // are sufficiently many components
    // left after the current component
    // 'c'
    if (c + dim <= fe.n_components())
      {
        FEValuesExtractors::Vector vector_components(c);
        fe_values[vector_components].get_function_hessians(
          fe_function, selected_vector_values);
        deallog << "component=" << c << std::endl;

        for (const auto q : fe_values.quadrature_point_indices())
          for (unsigned int d = 0; d < dim; ++d)
            {
              for (unsigned int e = 0; e < dim; ++e)
                for (unsigned int f = 0; f < dim; ++f)
                  deallog << selected_vector_values[q][d][e][f] << ' ';
              deallog << std::endl;
              Assert((selected_vector_values[q][d] - vector_values[q][c + d])
                         .norm() <= 1e-12 * selected_vector_values[q][d].norm(),
                     ExcInternalError());
            }
      }
}



template <int dim>
void
test_hyper_sphere()
{
  Triangulation<dim> tr;
  GridGenerator::hyper_ball(tr);

  static const SphericalManifold<dim> boundary;
  tr.set_manifold(0, boundary);

  FESystem<dim> fe(
    FE_Q<dim>(1), 1, FE_RaviartThomas<dim>(1), 1, FE_Nedelec<dim>(0), 1);
  test(tr, fe);
}


int
main()
{
  initlog();

  test_hyper_sphere<2>();
  test_hyper_sphere<3>();
}
