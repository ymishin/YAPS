// Copyright (c) 2005,2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#include "io.h"
#include "iobin.h"
#include "calc.h"
#include "common.h"
#include <cstdio>
using namespace std;

int
main( int argc, char **argv)
{
    // read input
    IO::doReadObstacles = 0;
    IO().readInput();

    // write initial state
    IOBin().writeData( 0);

    printf( "Numbers of particles (smooth / boundary / total) : %d / %d / %d\n", 
        (int)particles.size(), (int)bparticles.size(), 
        (int)particles.size() + (int)bparticles.size());

    // run simulator
    Calc().run();

    return 0;
}