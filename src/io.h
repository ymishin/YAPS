// Copyright (c) 2005,2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#ifndef YAPS_IO_H
#define YAPS_IO_H

class IO
{

public:

    // read input file
    static void readInput();

    // flags
    static char doReadParticles;
    static char doReadBParticles;
    static char doReadObstacles;

private:

    // name of the input file
    static const char* inputFileName;
    // maximum length of the file's string (with line feed)
    static const int fileLineLength;
    // maximum length of a section's name
    static const int sectionNameLength;
    // keyword to end a section
    static const char* sectionEnd;
    // maximum length of a parameter-string
    static const int stringParamLength;
    // maximum length of a parameter's name
    static const int paramNameLength;

    // section info
    struct Section;

    // read and process obstacles section
    static int   readObstaclesSection       ( char **input, 
                                              Section *info);
    // read and process clouds section
    static int   readCloudsSection          ( char **input, 
                                              Section *info);
    // read and process the section containing parameters
    static int   readParamsSection          ( char **input, 
                                              Section *Info);
    // unification of array of points
    static void  unifyPoints                ( int unifiedPart,
                                              float ***pnts, 
                                              int *pntsNum);
    // fill triangle with points
    static void  fillTriangleWithPoints     ( float *vrtx1, float *vrtx2, 
                                              float *vrtx3, float ***pnts, 
                                              int *pntsNum, float ival);
    // fill parallelepiped with points
    static void  fillParlpipedWithPoints    ( float *vrtx, float *vec1, 
                                              float *vec2, float *vec3, 
                                              float ***pnts, int *pntsNum, 
                                              float ival);
    // fill parallelogram with points
    static void  fillParlgramWithPoints     ( float *vrtx, float *vec1, 
                                              float *vec2, float ***pnts, 
                                              int *pntsNum, float ival);
    // fill segment with points
    static void  fillSegmentWithPoints      ( float *vrtx, float *vec, 
                                              float ***pnts, int *pntsNum, 
                                              float ival);
    // get point on segment
    static void  getPointOnSegmentByParam   ( float *vrtx, float *vec, 
                                              float *Pnt, float Param);

};

#endif // YAPS_IO_H