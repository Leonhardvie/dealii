/* ---------------------------------------------------------------------
 *
 * Copyright (C) 2021 by the deal.II authors
 *
 * This file is part of the deal.II library.
 *
 * The deal.II library is free software; you can use it, redistribute
 * it, and/or modify it under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * The full text of the license can be found in the file LICENSE.md at
 * the top level directory of deal.II.
 *
 * ---------------------------------------------------------------------
 */

// Check that ScratchData returns the correct solution values, gradients, etc.
// - Scalar valued FE
// - hp variant

#include <deal.II/base/function_lib.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/fe_values_extractors.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria.h>

#include <deal.II/hp/fe_collection.h>
#include <deal.II/hp/q_collection.h>

#include <deal.II/meshworker/scratch_data.h>

#include <deal.II/numerics/vector_tools.h>

#include "../tests.h"


template <int dim,
          int spacedim        = dim,
          typename NumberType = double,
          typename ExtractorType>
void
run(const ExtractorType &extractor)
{
  LogStream::Prefix prefix("Dim " + Utilities::to_string(dim));
  std::cout << "Dim: " << dim << std::endl;

  hp::FECollection<dim, spacedim> fe(FE_Q<dim, spacedim>(1),
                                     FE_Q<dim, spacedim>(3));
  hp::QCollection<dim>            qf_cell(QGauss<dim>(fe[0].degree + 1),
                               QGauss<dim>(fe[1].degree + 1));
  hp::QCollection<dim - 1>        qf_face(QGauss<dim - 1>(fe[0].degree + 1),
                                   QGauss<dim - 1>(fe[1].degree + 1));

  Triangulation<dim, spacedim> triangulation;
  GridGenerator::hyper_cube(triangulation);

  DoFHandler<dim, spacedim> dof_handler(triangulation);
  dof_handler.begin_active()->set_active_fe_index(1);
  dof_handler.distribute_dofs(fe);

  Vector<double> solution(dof_handler.n_dofs());
  VectorTools::interpolate(dof_handler,
                           Functions::CosineFunction<spacedim>(
                             fe.n_components()),
                           solution);

  const UpdateFlags update_flags =
    update_values | update_gradients | update_hessians | update_3rd_derivatives;
  MeshWorker::ScratchData<dim, spacedim> scratch_data(fe,
                                                      qf_cell,
                                                      update_flags);

  const auto cell = dof_handler.begin_active();
  scratch_data.reinit(cell);
  scratch_data.extract_local_dof_values("solution", solution);

  deallog << "Value: " << scratch_data.get_values("solution", extractor)[0]
          << std::endl;
  deallog << "Gradient: "
          << scratch_data.get_gradients("solution", extractor)[0] << std::endl;
  deallog << "Hessian: " << scratch_data.get_hessians("solution", extractor)[0]
          << std::endl;
  deallog << "Laplacian: "
          << scratch_data.get_laplacians("solution", extractor)[0] << std::endl;
  deallog << "Third derivative: "
          << scratch_data.get_third_derivatives("solution", extractor)[0]
          << std::endl;

  deallog << "OK" << std::endl;
}


int
main(int argc, char *argv[])
{
  initlog();
  Utilities::MPI::MPI_InitFinalize mpi_initialization(
    argc, argv, testing_max_num_threads());

  FEValuesExtractors::Scalar extractor(0);

  run<2>(extractor);
  run<3>(extractor);

  deallog << "OK" << std::endl;
}
