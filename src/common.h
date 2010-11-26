// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#ifndef YAPS_COMMON_H
#define YAPS_COMMON_H

#include <vector>
using namespace std;

// Dimension of the simulation
extern int dimension;

// Parameters
struct Parameters
{
    // initial particle distribution
    float   particlesDistrib;
    // initial boundary particle distribution
    float   bparticlesDistrib;
    // speed of sound
    float   sos;
    // kernel to use in the calculations
    char    kernelType[20];
    // kernel smoothing length
    float   smoothR;
    // equation of state to calculate pressures
    char    eosType[20];
    // alpha factor to calculate viscosity
    float   viscAlpha;
    // beta factor to calculate viscosity
    float   viscBeta;
    // time step of integration
    float   timeStep;
    // number of steps to perform
    int     nsteps;
    // output frequence
    int     outFreq;
    // clipping volume (the area to render)
    float   clipVolume;
    // radius to draw particles
    float   particlesRadius;
};
extern Parameters parameters;

// Smoothing particles
struct Particle
{
    int no;              // material number
    float pos[3];        // position (x,y,z)
    float vel[3];        // velocity vector (Vx,Vy,Vz)
    float ivalVel[3];    // velocity vector (Vx,Vy,Vz) at (t-dt/2)
    float accel[3];      // acceleration (Ax,Ay,Az) of the particle
    float dens;          // density at the location of the partile
    float dens0;         // initial density at the location of the partile 
    float ivalDens;      // density at (t-dt/2)
    float dervDens;      // rate of change of the density (dro/dt)
    float press;         // pressure at the location of the particle
    float mass;          // mass carried by the particle
};
typedef vector<Particle> Particles;
extern Particles particles;

// Boundary particles
struct BParticle
{
    float pos[3];        // position (x,y,z)
};
typedef vector<BParticle> BParticles;
extern BParticles bparticles;

// Obstacles
struct Obstacle
{
    int no;             // material number
    float vrtx1[3];     // vertex 1
    float vrtx2[3];     // vertex 2
    float vrtx3[3];     // vertex 3
};
typedef vector<Obstacle> Obstacles;
extern Obstacles obstacles;

// RGB colors for rendering
//struct RendColors {
//};
//extern RendColors rendColors;

#endif /* YAPS_COMMON_H */