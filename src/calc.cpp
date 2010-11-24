// Copyright (c) 2005,2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#include "calc.h"
#include "vec.h"
#include "kernel.h"
#include "eos.h"
#include "iobin.h"
#include "common.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <omp.h>
using namespace std;

// external force field
const float Calc::externalForce[] = { 0.0f, -0.00981f, 0.0f };
// D factor to calculate repulsive Lennard-Jones forces
const float Calc::LenJonD  = 10.0f;
// P1 power to calculate repulsive Lennard-Jones forces
const float Calc::LenJonP1 = 4.0f;
// P2 power to calculate repulsive Lennard-Jones forces
const float Calc::LenJonP2 = 2.0f;

// kernel and equation of state 
KernelBase* Calc::kernel = NULL;
EOSBase* Calc::eos = NULL;

// Constructor.
// Initialize calculation module - choose appropriate 
// kernel, equation of state, etc. according to parameters.
Calc::Calc()
{
    // search for the required kernel
    char *kernelType = parameters.kernelType;
    if ( !strcmp( kernelType, "SPLINE") )
        kernel = new KernelSpline();
    else if ( !strcmp( kernelType, "SPIKY") )
        kernel = new KernelSpiky();

    // search for the required EOS
    char *eosType = parameters.eosType;
    if ( !strcmp( eosType, "BATCHELOR") )
        eos = new EOSBatchelor();
    else if ( !strcmp( eosType, "DESBRUN") )
        eos = new EOSDesbrun();
}

// Destructor.
Calc::~Calc()
{
    // deallocate
    delete kernel;
    delete eos;
}

// Run simulator.
void 
Calc::run()
{
    int nfile = 1;
    time_t t1, t2;

#ifdef YAPS_TIME
    time( &t1);
#endif

    for ( int i = 0; i < parameters.nsteps; i++ )
    {
        doCalcStep();

        if ( !(i % parameters.outFreq) )
        {
            IOBin().writeData( nfile++);

#ifdef YAPS_TIME
            time( &t2);
            printf( "timing : %6.0f seconds\n", difftime( t2, t1));
            time( &t1);
#endif
        }
    }
}

// Do one calculation step.
void
Calc::doCalcStep()
{
    float gradKernel[3];
    float pressTerm;
    float viscTerm;
    float viscNu;
    float Vij[3];
    float Rij[3];
    float tmp1, tmp2;
    int   i, j, d;

    // parameters
    float sos               = parameters.sos;
    float smoothR           = parameters.smoothR;
    float viscAlpha         = parameters.viscAlpha;
    float viscBeta          = parameters.viscBeta;
    float particlesDistrib  = parameters.particlesDistrib;
    
    // Nu factor to calculate viscosity
    viscNu = 0.01f * smoothR * smoothR;
    
    // calculate the particles' pressures
    eos->calcPress();

    // calculate the rates of change of velocities and the 
    // rates of change of densities for all the particles
    // J.J.Monaghan, Simulating Free Surface Flows with SPH, 
    // J.Comput.Phys., 110, 399-406, 1994.
#pragma omp parallel for schedule(dynamic,50) private(gradKernel,pressTerm,viscTerm,Vij,Rij,tmp1,tmp2,j,d)
    for ( i = 0; i < (int)particles.size(); i++ )
    {
        // take into account the external force field
        memcpy( particles[i].accel, externalForce, sizeof(externalForce));
        
        particles[i].dervDens = 0.0f;

        // calculate forces between smoothing particles 
        // and update the rate of change of the density
        for ( j = 0; j < (int)particles.size(); j++ )
        {
            if ( j == i )
                continue;

            vectorSubstraction( Rij, particles[i].pos, particles[j].pos);

            // get the kernel's gradient at the point Rij
            if ( kernel->getGrad( gradKernel, Rij) )
                continue;
            
            // take into account the viscocity of the medium
            vectorSubstraction( Vij, particles[i].vel, particles[j].vel);
            tmp1 = vectorInnerproduct( Rij, Vij);
            if ( tmp1 < 0.0f )
            {
                tmp2 = vectorInnerproduct( Rij, Rij);
                tmp1 = smoothR * tmp1 / (tmp2 + viscNu);
                viscTerm = 2.0f * tmp1 * (-viscAlpha * sos + viscBeta * tmp1) / 
                          (particles[i].dens + particles[j].dens);
            }
            else
            {
                viscTerm = 0.0f;
            }

            // take into account the difference of the particles' pressures
            pressTerm = 
                particles[i].press / (particles[i].dens * particles[i].dens) +
                particles[j].press / (particles[j].dens * particles[j].dens);
            
            // update the acceleration of the particle
            tmp1 = particles[j].mass * (pressTerm + viscTerm);
            for ( d = 0; d < dimension; d++ )
                particles[i].accel[d] -= tmp1 * gradKernel[d];

            // update the rate of change of the density for the particle
            tmp1 = vectorInnerproduct( Vij, gradKernel);
            particles[i].dervDens += particles[j].mass * tmp1;
        }

        // calculate the Lennard-Jones forces between 
        // the particle and the boundary particles
        for ( j = 0; j < (int)bparticles.size(); j++ )
        {
            vectorSubstraction( Rij, particles[i].pos, bparticles[j].pos);
            tmp1 = vectorInnerproduct( Rij, Rij);
            tmp2 = particlesDistrib / sqrt( tmp1);
            // only repulsive forces are taken into account
            if ( tmp2 > 1.0f )
            {
                tmp1 = (pow( tmp2, LenJonP1) - pow( tmp2, LenJonP2)) * 
                       LenJonD / tmp1;
                for ( d = 0; d < dimension; d++ )
                    particles[i].accel[d] += Rij[d] * tmp1;
            }
        }
    }

    // time integration
    leapfrogIntegration();
    
    return;
} // doCalcStep

// 'leap-frog' integration scheme
// M.P.Allen and D.J.Tildesley, Computer Simulation 
// of Liquids, Oxford Univ.Press, 1987.
void
Calc::leapfrogIntegration()
{
    int i;
    int d;

    float timeStep = parameters.timeStep;
    
    // calculate new positions, velocities and densities for all the particles
    for ( i = 0; i < (int)particles.size(); i++ )
    {
        for ( d = 0; d < dimension; d++ )
        {
            // new interval velocity (t+dt/2)
            particles[i].ivalVel[d] += particles[i].accel[d] * timeStep;
            // new position (t+dt)
            particles[i].pos[d] += particles[i].ivalVel[d] * timeStep;
            // new velocity (t+dt)
            particles[i].vel[d] = particles[i].ivalVel[d] + 
                                  particles[i].accel[d] * timeStep / 2.0f;
        }
        // new interval density (t+dt/2)
        particles[i].ivalDens += particles[i].dervDens * timeStep;
        // new density (t+dt)
        particles[i].dens = particles[i].ivalDens +
                            particles[i].dervDens * timeStep / 2.0f;
    }
    
    return;
} // leapfrogIntegration