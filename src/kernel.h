// Copyright (c) 2005,2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#ifndef YAPS_KERNEL_H
#define YAPS_KERNEL_H

class KernelBase
{
public:
    // calculate the kernel's gradient
    virtual int getGrad( float *grad, float *Rij) = 0;
protected:
    // factor to calculate the gradient
    float gradFactor;
    static const float PI;
};

class KernelSpline : public KernelBase {
public:
    // constructor
    KernelSpline();
    // calculate the kernel's gradient
    int getGrad( float *grad, float *Rij);
};

class KernelSpiky : public KernelBase {
public:
    // constructor
    KernelSpiky();
    // calculate the kernel's gradient
    int getGrad( float *grad, float *Rij);
};

#endif /* YAPS_KERNEL_H */