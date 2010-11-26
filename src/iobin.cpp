// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#include "iobin.h"
#include "common.h"
#include <cstdio>
using namespace std;

// filename
const char* IOBin::fname = "output";

// Read data in binary form
int
IOBin::readData(int nfile)
{
    // open file for reading
    char ffname[20];
    sprintf( ffname, "%s_%05d.bin", fname, nfile);
    FILE *file = fopen( ffname, "rb");
    if ( file == NULL )
        return 1;
 
    // read particles data
    Particle particle;
    particles.clear();
    while ( fread( (void *)&particle, sizeof(struct Particle), 1, file) )
        particles.push_back( particle);

    // close the file
    fclose( file);

    return 0;
} // readData

// Write data in binary form
int
IOBin::writeData(int nfile)
{
    // open file for writing
    char ffname[20];
    sprintf( ffname, "%s_%05d.bin", fname, nfile);
    FILE *file = fopen( ffname, "wb");
    if ( file == NULL )
        return 1;
 
    // write particles data
    for ( int i = 0; i < (int)particles.size(); i++ )
        fwrite( &(particles[i]), sizeof(struct Particle), 1, file);

    // close the file
    fclose( file);

    return 0;

} // writeData