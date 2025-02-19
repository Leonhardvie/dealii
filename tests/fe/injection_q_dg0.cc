// ---------------------------------------------------------------------
//
// Copyright (C) 2006 - 2020 by the deal.II authors
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



char logname[] = "output";

#include <deal.II/base/quadrature_lib.h>

#include "injection_common.h"


template <int dim>
void
test()
{
  deallog << std::setprecision(10);
  for (unsigned int i = 1; i < 4; ++i)
    for (unsigned int j = i; j < 4; ++j)
      {
        deallog << "inj " << i << ' ' << j << ':' << std::endl;
        do_check(FE_Q_DG0<dim>(QIterated<1>(QTrapezoid<1>(), i)),
                 FE_Q_DG0<dim>(QIterated<1>(QTrapezoid<1>(), j)));
      }
}
