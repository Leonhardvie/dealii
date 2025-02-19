// ---------------------------------------------------------------------
//
// Copyright (C) 2002 - 2018 by the deal.II authors
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



// ParameterHandler could not deal with missing endline at end of file
// or can it?
// http://code.google.com/p/dealii/issues/detail?id=126

// this is a variant of parameter_handler_15 but in fact reads data
// from a file

#include <deal.II/base/parameter_handler.h>

#include "../tests.h"

void
test()
{
  ParameterHandler foo;
  foo.enter_subsection("bar");
  foo.declare_entry("val", "1.0", dealii::Patterns::Double(), "");
  foo.leave_subsection();

  foo.parse_input(SOURCE_DIR "/parameter_handler_18_in.prm");

  foo.enter_subsection("bar");
  deallog << foo.get("val") << std::endl;
  foo.leave_subsection();
}

int
main()
{
  initlog();

  test();

  return 0;
}
