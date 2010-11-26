// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#ifndef YAPS_CALC_H
#define YAPS_CALC_H

#include "kernel.h"
#include "eos.h"

class Calc
{

public:
    // constructor and destructor
    Calc();
    ~Calc();
    // run simulator
    static void run();

private:

    // external force field
    static const float externalForce[];
    // D factor to calculate repulsive Lennard-Jones forces
    static const float LenJonD;
    // P1 power to calculate repulsive Lennard-Jones forces
    static const float LenJonP1;
    // P2 power to calculate repulsive Lennard-Jones forces
    static const float LenJonP2;
    // kernel
    static KernelBase *kernel;
    // equation of state
    static EOSBase *eos;    
    // do one calculation step
    static void doCalcStep();
    // 'leap-frog' integration scheme
    static void leapfrogIntegration();

};

#endif // YAPS_CALC_H
