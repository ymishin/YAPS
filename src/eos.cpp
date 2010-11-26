// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#include "eos.h"
#include "common.h"
#include <cmath>
using namespace std;

// Calculate pressures at particles' positions using
// the equation of state suggested by Batchelor: 
// G.K.Batchelor, An Introduction to Fluid Dynamics, 
// Cambridge Univ.Press, 2000.
// In fact, the function implements a modified version
// of this equation suggested by Monaghan:
// J.J.Monaghan, Simulating Free Surface Flows with SPH, 
// J.Comput.Phys., 110, 399-406, 1994.
void
EOSBatchelor::calcPress()
{
    // speed of sound
    float sos = parameters.sos;

    // calculate pressures for all particles (Monaghan'94)
    for ( int i = 0; i < (int)particles.size(); i++ )
    {
        particles[i].press = (particles[i].dens0 * sos * sos / 7.0f) * 
            (pow( particles[i].dens / particles[i].dens0, 7.0f) - 1.0f);
    }

    return;
} // calcPress

// Calculate pressures at particles' positions using
// the equation of state suggested by Desbrun and Gascuel:
// M.Desbrun and M.Gascuel, Smoothed Particles: A new paradigm 
// for animating highly deformable bodies, Proceedings of 6th 
// Eurographics Workshop on Animation and Simulation, 61-76, 1996.
void
EOSDesbrun::calcPress()
{
    // stiffness parameter
    float k = 30.0f;

    // calculate pressures for all particles
    for ( int i = 0; i < (int)particles.size(); i++ )
    {
        particles[i].press = k * ( particles[i].dens - particles[i].dens0);
    }

    return;
} // calcPress