// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#include "kernel.h"
#include "common.h"
#include "vec.h"

const float KernelBase::PI = 3.1415926535f;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Cubic spline kernel
// J.J.Monaghan, Smoothed Particle Hydrodynamics,
// Annu.Rev.Astron.Astrophys., 30, 543-574, 1992.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Constructor.
KernelSpline::KernelSpline()
{
    float normFactor;
    float smoothR = parameters.smoothR;

    // kernel's normalization factor
    if ( dimension == 2 )
        normFactor = 10.0f / (7.0f * PI * smoothR * smoothR);
    else if ( dimension == 3 )
        normFactor = 1.0f / (PI * smoothR * smoothR * smoothR);

    // factor to calculate the kernel's gradient
    gradFactor = normFactor / (smoothR * smoothR);

    return;
} // KernelSpline

// Calculate the kernel's gradient at the point 'Rij' with 
// respect to Ri, the resulting gradient vector is return 
// through 'grad'. The function returns -1 if the gradient 
// vector is equal to zero and 0 if it's meaning.
int
KernelSpline::getGrad( float *grad,   // result (gradient vector)
                       float *Rij)    // vector Rij = Ri - Rj
{
    float s = vectorNorm( Rij) / parameters.smoothR;

    if ( s > 2.0f )
    {
        return -1;
    }
    else if ( s > 1.0f )
    {
        for ( int d = 0; d < dimension; d++ )
            grad[d] = gradFactor * Rij[d] * 
            -0.75f * (2.0f - s) * (2.0f - s) / s;
    }
    else
    {
        for ( int d = 0; d < dimension; d++ )
            grad[d] = gradFactor * Rij[d] * 
            (2.25f * s - 3.0f);
    }

    return 0;
} // getGrad

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Spiky kernel
// M.Desbrun and M.Gascuel, Smoothed Particles: A new paradigm
// for animating highly deformable bodies, Proceedings of 6th
// Eurographics Workshop on Animation and Simulation, 61-76, 1996.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Constructor.
KernelSpiky::KernelSpiky()
{
    float normFactor;
    float smoothR = parameters.smoothR;

    // kernel's normalization factor
    if ( dimension == 2 )
        normFactor = 5.0f / (16.0f * PI * smoothR * smoothR);
    else if ( dimension == 3 )
        normFactor = 15.0f / (64.0f * PI * smoothR * smoothR * smoothR);

    // factor to calculate the kernel's gradient
    gradFactor = normFactor * (-3.0f / (smoothR * smoothR));

    return;
} // KernelSpiky

// Calculate the kernel's gradient at the point 'Rij' with 
// respect to Ri, the resulting gradient vector is return 
// through 'grad'. The function returns -1 if the gradient 
// vector is equal to zero and 0 if it's meaning.
int
KernelSpiky::getGrad( float *grad,   // result (gradient vector)
                      float *Rij)    // vector Rij = Ri - Rj
{
    float s = vectorNorm( Rij) / parameters.smoothR;

    if ( s > 2.0f )
    {
        return -1;
    }
    else
    {
        for ( int d = 0; d < dimension; d++ )
            grad[d] = gradFactor * Rij[d] * 
            (2.0f - s) * (2.0f - s) / s;
    }

    return 0;
} // getGrad