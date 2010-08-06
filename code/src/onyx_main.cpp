// -*- mode: c++; c-basic-offset: 4; -*-

#include "onyx/application.h"

int main(int argc, char* argv[])
{
  onyx::g_onyx_application->initialize(argc, argv);
  return onyx::g_onyx_application->onyxExec();
}
