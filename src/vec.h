// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#ifndef YAPS_VECTOR_H
#define YAPS_VECTOR_H

// 'resVec' = 'vec1' + 'vec2'
extern void  vectorAddition     ( float *resVec, 
                                  float *vec1, 
                                  float *vec2);

// 'resVec' = 'vec1' - 'vec2'
extern void  vectorSubstraction ( float *resVec, 
                                  float *vec1, 
                                  float *vec2);

// ('vec1', 'vec2') is returned
extern float vectorInnerproduct ( float *vec1, 
                                  float *vec2);

// |'vec'| is returned
extern float vectorNorm         ( float *vec);

#endif // YAPS_VECTOR_H
