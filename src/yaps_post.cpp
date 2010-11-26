// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#include "io.h"
#include "iobin.h"
#include "render.h"
#include "common.h"
#include <cstdio>
using namespace std;

int
main( int argc, char **argv)
{
    // read input
    IO::doReadParticles = 0;
    IO::doReadBParticles = 0;
    IO().readInput();

    // read initial state
    IOBin().readData( 0);

    printf( "Numbers of particles (smooth / boundary / total) : %d / %d / %d\n", 
        (int)particles.size(), (int)bparticles.size(), 
        (int)particles.size() + (int)bparticles.size());

    // run renderer
    Render( argc, argv).run();

    return 0;
}