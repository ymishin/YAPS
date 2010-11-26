// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#include "vec.h"
#include "common.h"
#include <cmath>
using namespace std;

// The function adds vector 'vec2' to vector 'vec1' and 
// stores the result in vector 'resVec': 'resVec' = 'vec1' + 'vec2'.
void
vectorAddition( float *resVec,   // resulting vector
                float *vec1,     // vector 1
                float *vec2)     // vector 2
{
    int d;
    
    for ( d = 0; d < dimension; d++ )
        resVec[d] = vec1[d] + vec2[d];
    
    return;
}

// The function substracts vector 'vec2' from vector 'vec1' and 
// stores the result in vector 'resVec': 'resVec' = 'vec1' - 'vec2'.
void
vectorSubstraction( float *resVec,   // resulting vector
                    float *vec1,     // vector 1
                    float *vec2)     // vector 2
{
    int d;
    
    for ( d = 0; d < dimension; d++ )
        resVec[d] = vec1[d] - vec2[d];
    
    return;
}

// The function computes and returns the innerproduct 
// of two vectors - ('vec1', 'vec2').
float
vectorInnerproduct( float *vec1,   // vector 1
                    float *vec2)   // vector 2
{
    float res;
    int d;

    res = 0;
    for ( d = 0; d < dimension; d++ )
        res += vec1[d] * vec2[d];

    return res;
}

// The function computes and returns 
// the norm of the vector - |<vec>|.
float
vectorNorm( float *vec)   // vector
{
    float res;
    int d;

    res = 0;
    for ( d = 0; d < dimension; d++ )
        res += vec[d] * vec[d];

    res = sqrt( res );

    return res;
}
