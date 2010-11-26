// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#include "io.h"
#include "common.h"
#include "vec.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
using namespace std;

// name of the input file
const char* IO::inputFileName       = "input";
// maximum length of the file's string (with line feed)
const int   IO::fileLineLength      = 1024;
// maximum length of a section's name
const int   IO::sectionNameLength   = 16;
// keyword to end a section
const char* IO::sectionEnd          = "$END";
// maximum length of a parameter-string
const int   IO::stringParamLength   = 16;
// maximum length of a parameter's name
const int   IO::paramNameLength     = 16;

// read everythig by default
char IO::doReadParticles  = 1;
char IO::doReadBParticles = 1;
char IO::doReadObstacles  = 1;

// Section info
struct IO::Section
{
    const char *name;               // name in the input file
    int firstLine;                  // first line of the section
    int endLine;                    // end line of the section
    int (*func)( char **input, 
                 Section *info);    // function to read the section
};

// Read the description file and initialize
// all objects used in simulation and rendering.
void
IO::readInput()
{
    FILE *file;
    char **input;
    char str[fileLineLength + 1];
    char name[sectionNameLength + 1];
    char fmt[6];
    int linesNum;
    char searchEnd;
    int i, j, n;

    // open the input file
    file = fopen( inputFileName, "r");
    if ( file == NULL )
        return;

    // read the input file
    linesNum = 0;
    input = NULL;
    while ( feof( file) == 0 )
    {
        // read next string
        fgets( str, fileLineLength, file);
        n = (int)strlen( str);

        // empty strings or strings with comments 
        // aren't stored (comments begin with '#')
        for ( i = 0; i < n; i++ )
        {
            if ( isgraph( str[i]) )
                break;
        }
        if ( i == n || str[i] == '#' )
            continue;
            
        // allocate memory and store the string
        i = linesNum++;
        input = (char **)realloc( input, linesNum * sizeof(char *));
        input[i] = (char *)malloc( (n + 1) * sizeof(char));
        strcpy( input[i], str);
    }

    // possible sections in the scene description file, 
    // they are processed in the order they follow here
    Section sections[] =
    {
        // parameters section - contains simulator's parameters
        "$PARAMS",    -1, -1, readParamsSection,
        // clouds section - set up clouds of particles
        "$CLOUDS",    -1, -1, readCloudsSection,
        // obstacles section - set up obstacles in the scene
        "$OBSTACLES", -1, -1, readObstaclesSection,
    };
    // number of sections
    int sectsNum = sizeof(sections) / sizeof(sections[0]);

    // find sections in the input file
    searchEnd = 0;
    sprintf( fmt, "%%%ds", sectionNameLength);
    for ( i = 0; i < linesNum; i++ )
    {
        // read next string
        sscanf( input[i], fmt, name);

        // section starts/ends with '$'
        if ( name[0] != '$' )
            continue;

        if ( searchEnd )
        {
            // first line has been found already -
            // it's necessary to find the section's end
            if ( strcmp( sectionEnd, name) )
                continue;
            sections[j].endLine = i;
            searchEnd = 0;
            continue;
        }
        
        for ( j = 0; j < sectsNum; j++ )
        {
            // some section has been found in the 
            // input file - check if it is valid
            if ( strcmp( sections[j].name, name) )
                continue;
            sections[j].firstLine = i + 1;
            // now it's necessary to find the end
            searchEnd = 1;
            break;
        }
    }
    
    // read and process the sections of the input file
    for ( i = 0; i < sectsNum; i++ )
        sections[i].func( input, &sections[i]);

    // close the file and free the memory
    if ( file != NULL )
    {
        fclose( file);
        for ( i = 0; i < linesNum; i++ )
            free( input[i]);
        free( input);
    }

    return;
} // readInput

// Read obstacles section which is specified by 'info' from array with 
// input data 'input', initialize corresponding data structures
// and create boundary particles. The function returns 0 if succeeded 
// and the number of string containing an error otherwise.
//
int
IO::readObstaclesSection( char **input,     // array with input data
                          Section *info)    // section's info
{
    if (doReadObstacles == 0 && doReadBParticles == 0)
        return 0;

    Obstacle obstacle;
    float **pnts;
    int pntsNum;
    int res;
    int i, n;
    
    pnts = NULL;
    pntsNum = 0;
    res = 0;

    // obstacle's specification occupies one string
    int obstaclesNumber = info->endLine - info->firstLine;

    if ( dimension == 2 )
    {
        float vec[2];
        
        for ( i = 0; i < obstaclesNumber; i++ )
        {
            // in 2D simulation segment-obstacles are used
            memset( &obstacle, 0, sizeof(struct Obstacle));
            n = sscanf( input[info->firstLine + i], "%d %f %f %f %f", 
                        &(obstacle.no),
                        &(obstacle.vrtx1[0]), &(obstacle.vrtx1[1]), 
                        &(obstacle.vrtx2[0]), &(obstacle.vrtx2[1]));
            // error has occured
            if ( n != 5 )
                break;
            // store
            obstacles.push_back( obstacle);
            // fill segment with points
            vectorSubstraction( vec, obstacle.vrtx2, obstacle.vrtx1);
            fillSegmentWithPoints( obstacle.vrtx1, vec, &pnts, 
                                   &pntsNum, parameters.bparticlesDistrib);
        }
    }
    else if ( dimension == 3 )
    {
        for ( i = 0; i < obstaclesNumber; i++ )
        {
            // in 3D simulation triangle-obstacles are used
            memset( &obstacle, 0, sizeof(struct Obstacle));
            n = sscanf( input[info->firstLine + i], 
                        "%d %f %f %f %f %f %f %f %f %f", &(obstacle.no),
                        &(obstacle.vrtx1[0]), &(obstacle.vrtx1[1]), 
                        &(obstacle.vrtx1[2]), &(obstacle.vrtx2[0]), 
                        &(obstacle.vrtx2[1]), &(obstacle.vrtx2[2]), 
                        &(obstacle.vrtx3[0]), &(obstacle.vrtx3[1]), 
                        &(obstacle.vrtx3[2]));
            // error has occured
            if ( n != 10 )
                break;
            // store
            obstacles.push_back( obstacle);
            // fill triangle with points
            fillTriangleWithPoints( obstacle.vrtx1, obstacle.vrtx2, 
                                    obstacle.vrtx3, &pnts, &pntsNum, 
                                    parameters.bparticlesDistrib);
        }
    }

    if ( i != obstaclesNumber )
    {
        // error has occured
        res = i;
    }
    else if (doReadBParticles)
    {
        // create boundary particles using coordinates of 
        // points which the obstacles have been filled with
        BParticle bparticle;
        unifyPoints( 0, &pnts, &pntsNum);
        for ( i = 0; i < pntsNum; i++ )
        {
            memset( &bparticle, 0, sizeof(struct BParticle));
            // boundary particle's position
            memcpy( bparticle.pos, pnts[i], dimension * sizeof(float));
            // store
            bparticles.push_back( bparticle);
        }
    }

    // delete points
    for ( i = 0; i < pntsNum; i++ )
        free( pnts[i]);
    free( pnts);

    // clear obstacles
    if (doReadObstacles == 0)
        obstacles.clear();

    return res;
} // readObstaclesSection

// Read clouds section which is specified by 'info' from array with 
// input data 'input', initialize corresponding data structures
// and create smoothing particles. The function returns 0 if succeeded 
// and the number of string containing an error otherwise.
int
IO::readCloudsSection( char **input,    // array with input data
                       Section *info)   // section's info
{
    if (doReadParticles == 0)
        return 0;

    Particle particle;
    float **pnts;
    int pntsNum;
    int res;
    int i, j, n, no;
    
    pnts = NULL;
    pntsNum = 0;
    res = 0;
    
    if ( dimension == 2 )
    {
        float vrtx1[2], vrtx2[2], vrtx3[2];
        float dens0, vel[2];
        
        for ( i = info->firstLine; i < info->endLine; i++ )
        {
            // in 2D simulation cloud of particle 
            // has the form of a parallelogram
            n = sscanf( input[i], "%d %f  %f %f  %f %f  %f %f  %f %f", 
                                   &no, &dens0,
                                   &vrtx1[0], &vrtx1[1], 
                                   &vrtx2[0], &vrtx2[1], 
                                   &vrtx3[0], &vrtx3[1], 
                                   &vel[0], &vel[1]);
            // error has occured
            if ( n != 10 )
                break;
            // fill parallelogram with points
            n = pntsNum;
            fillParlgramWithPoints( vrtx1, vrtx2, vrtx3, &pnts, 
                                    &pntsNum, parameters.particlesDistrib);
            unifyPoints( n, &pnts, &pntsNum);
            // create new particles using coordinates of points 
            // which the parallelogram has been filled with
            for ( j = n; j < pntsNum; j++ )
            {
                memset( &particle, 0, sizeof(struct Particle));
                // material number
                particle.no = no;
                // position
                memcpy( particle.pos, pnts[j], dimension * sizeof(float));
                // velocity
                memcpy( particle.vel, vel, dimension * sizeof(float));
                memcpy( particle.ivalVel, vel, dimension * sizeof(float));
                // density
                particle.dens = dens0;
                particle.dens0 = dens0;
                particle.ivalDens = dens0;
                // mass
                particle.mass = pow( parameters.particlesDistrib, 3) * dens0;
                // store
                particles.push_back( particle);
            }
        }
    }
    else if ( dimension == 3 )
    {
        float vrtx1[3], vrtx2[3], vrtx3[3], vrtx4[3];
        float dens0, vel[3];
        
        for ( i = info->firstLine; i < info->endLine; i++ )
        {
            // in 3D simulation cloud of particle 
            // has the form of a parallelepiped
            n = sscanf( input[i], "%d %f  %f %f %f  %f %f %f \
                                   %f %f %f  %f %f %f  %f %f %f", 
                                   &no, &dens0,
                                   &vrtx1[0], &vrtx1[1], &vrtx1[2], 
                                   &vrtx2[0], &vrtx2[1], &vrtx2[2], 
                                   &vrtx3[0], &vrtx3[1], &vrtx3[2], 
                                   &vrtx4[0], &vrtx4[1], &vrtx4[2], 
                                   &vel[0], &vel[1], &vel[2]);
            // error has occured
            if ( n != 17 )
                break;
            // fill parallelepiped with points
            n = pntsNum;
            fillParlpipedWithPoints( vrtx1, vrtx2, vrtx3, vrtx4, &pnts, 
                                     &pntsNum, parameters.particlesDistrib);
            unifyPoints( n, &pnts, &pntsNum);
            // create new particles using coordinates of points 
            // which the parallelepiped has been filled with
            for ( j = n; j < pntsNum; j++ )
            {
                memset( &particle, 0, sizeof(struct Particle));
                // material number
                particle.no = no;
                // position
                memcpy( particle.pos, pnts[j], dimension * sizeof(float));
                // velocity
                memcpy( particle.vel, vel, dimension * sizeof(float));
                memcpy( particle.ivalVel, vel, dimension * sizeof(float));
                // density
                particle.dens = dens0;
                particle.dens0 = dens0;
                particle.ivalDens = dens0;
                // mass
                particle.mass = pow( parameters.particlesDistrib, 3) * dens0;
                // store
                particles.push_back( particle);
            }
        }
    }

    // error has occured
    if ( i != info->endLine )
        res = i;

    // delete points
    for ( i = 0; i < pntsNum; i++ )
        free( pnts[i]);
    free( pnts);

    return res;
} // readCloudsSection

// Read parameters section which is specified by 'info' from array with 
// input data 'input', and initialize corresponding variables and 
// data structures. The function returns 0 if succeeded and the number 
// of string containing an error otherwise.
int
IO::readParamsSection( char **input,    // array with input data
                       Section *info)   // section's info
{
    char name[paramNameLength + 1];
    char fmt[10];
    int i, j, n;
    int res = 0;

    // parameter's info
    struct Param
    {
        const char *name; // name of the parameter
        int  type;        // type of the parameter
        void *var;        // pointer to a var the parameter's value to store in
    };

    // possible types of parameters
    enum { INT_PARAM, FLOAT_PARAM, STRING_PARAM };

    // possible parameters in the parameters section
    Param params[] =
    {
        // dimension of the simulation
        "DIM",          INT_PARAM,    (void *)(&dimension),
        // initial particle distribution
        "PRTS_DISTR",   FLOAT_PARAM,  (void *)(&parameters.particlesDistrib),
        // initial boundary particle distribution
        "BPRTS_DISTR",  FLOAT_PARAM,  (void *)(&parameters.bparticlesDistrib),
        // speed of sound
        "SOS",          FLOAT_PARAM,  (void *)(&parameters.sos),
        // kernel to use in the calculations
        "KERNEL",       STRING_PARAM, (void *)(parameters.kernelType),
        // kernel's smoothing length
        "SMOOTH_LEN",   FLOAT_PARAM,  (void *)(&parameters.smoothR),
        // equation of state to calculate pressures
        "EOS",          STRING_PARAM, (void *)(parameters.eosType),
        // alpha factor to calculate viscosity
        "VISC_ALPHA",   FLOAT_PARAM,  (void *)(&parameters.viscAlpha),
        // beta factor to calculate viscosity
        "VISC_BETA",    FLOAT_PARAM,  (void *)(&parameters.viscBeta),
        // time step of integration
        "TIME_STEP",    FLOAT_PARAM,  (void *)(&parameters.timeStep),
        // number of steps to perform
        "NSTEPS",       INT_PARAM,    (void *)(&parameters.nsteps),
        // output frequence
        "OUT_FREQ",     INT_PARAM,    (void *)(&parameters.outFreq),
        // clipping volume (the area to render)
        "CLIP_VOL",     FLOAT_PARAM,  (void *)(&parameters.clipVolume),
        // radius to draw particles
        "PRADIUS",      FLOAT_PARAM,  (void *)(&parameters.particlesRadius),
    };

    // number of parameters
    int paramsNum = sizeof(params) / sizeof(params[0]);

    // read parameters from the parameters section
    for ( i = info->firstLine; i < info->endLine; i++ )
    {
        sprintf( fmt, "%%%ds %%n", paramNameLength);
        sscanf( input[i], fmt, name, &n);

        for ( j = 0; j < paramsNum; j++ )
        {
            if ( strcmp( params[j].name, name) )
                continue;

            // some parameter has been found - read and store its value
            if ( params[j].type == INT_PARAM )
            {
                // type of the parameter is integer
                sscanf( input[i] + n, "%d", (int *)params[j].var);
                break;
            }
            else if ( params[j].type == FLOAT_PARAM )
            {
                // type of the parameter is float
                sscanf( input[i] + n, "%f", (float *)params[j].var);
                break;
            }
            else if ( params[j].type == STRING_PARAM )
            {
                // type of the parameter is string (char *)
                sprintf( fmt, "%%%ds", stringParamLength);
                sscanf( input[i] + n, fmt, (char *)params[j].var);
                break;
            }
        }
        
        if ( j == paramsNum )
        {
            // parameter isn't valid
            res = i;
            break;
        }
    }

    return res;
} // readParamsSection

// Unification of array of points - the function searches for identical 
// points in the array 'pnts' of the size 'pntsNum', eliminates all of 
// them but one, and shift the array. New size of the array is returned 
// through 'pntsNum'. The size of the part of the array which is already 
// unified is set through 'unifiedPart'.
void
IO::unifyPoints( int unifiedPart, // unified part of the array
                 float ***pnts,   // array of points
                 int *pntsNum)    // size of the array
{
    float **p;
    int i, j, k;
    int n;

    // array of points
    p = *pnts;
    // current size of the array
    n = *pntsNum;
    
    // unify the array
    for ( i = 0; i < n - 1; i++ )
    {
        // search from current position for point which is identical to p[i]
        for ( j = ((unifiedPart > i) ? unifiedPart : i) + 1; j < n; j++ )
        {
            // is point identical to p[i]?
            if ( memcmp( p[i], p[j], dimension * sizeof(float)) )
                continue;
            // eliminate p[j]
            free (p[j]);
            n--;
            // search from the end of the array for point to replace p[j]
            for ( k = n; k > j; k-- )
            {
                // is point identical to p[j]?
                if ( memcmp( p[i], p[k], dimension * sizeof(float)) )
                {
                    // replace
                    p[j] = p[k];
                    p[k] = NULL;
                    break;
                }
                // eliminate p[k]
                free (p[k]);
                n--;
            }
        }
    }
    
    // new size of the array
    *pntsNum = n;
    // reallocate the array
    p = (float **)realloc( p, n * sizeof(float *));
    *pnts = p;

    return;
} // unifyPoints

// The function fills triangle given by 'vrtx1', 'vrtx2' and 'vrtx3' with 
// points, the interval between points is set by 'ival'. At the moment of 
// function invocation, the array 'pnts' could already contain some points 
// and in this case 'pntsNum' has to contain its current size; if the 
// array is empty, 'pnts' has to be set to NULL, and 'pntsNum' has to be 
// set to 0. New points are stored in the end of reallocated array 'pnts', 
// new size of the array is returned through 'pntsNum'.
void
IO::fillTriangleWithPoints( float *vrtx1,   // vertex 1
                            float *vrtx2,   // vertex 2
                            float *vrtx3,   // vertex 3
                            float ***pnts,  // array of points
                            int *pntsNum,   // size of the array
                            float ival)     // interval between points
{
    float vec[3], vec1[3], vec2[3];
    float pnt1[3], pnt2[3];
    float offset;
    float param;
    float r;
    int i, j;
    
    // reference vectors
    vectorSubstraction( vec1, vrtx2, vrtx1);
    vectorSubstraction( vec2, vrtx3, vrtx1);
    // length of the triangle's side
    r = vectorNorm( vec1);
    // number of points the triangle's side can be filled with
    j = (int)(r / ival);
    // fill triangle with points
    offset = (r - (float)(j - 1) * ival) / 2;
    for ( i = 0; i < j; i++ )
    {
        // get point1 on one triangle's side and point2 on another
        param = r ? ((ival * (float)i + offset) / r) : 0;
        getPointOnSegmentByParam( vrtx1, vec1, pnt1, param);
        getPointOnSegmentByParam( vrtx1, vec2, pnt2, param);
        // fill the resulting segment with points
        vectorSubstraction( vec, pnt2, pnt1);
        fillSegmentWithPoints( pnt1, vec, pnts, pntsNum, ival);
    }
    
    return;
} /* fillTriangleWithPoints */

// The function fills parallelepiped given by origin 'vrtx' and reference 
// vectors 'vec1', 'vec2' and 'vec3' with with points, the interval between 
// points is set by 'ival'. At the moment of function invocation, the array 
// 'pnts' could already contain some points and in this case 'pntsNum' has 
// to contain its current size; if the array is empty, 'pnts' has to be 
// set to NULL, and 'pntsNum' has to be set to 0. New points are stored in 
// the end of reallocated array 'pnts', new size of the array is returned 
// through 'pntsNum'.
void
IO::fillParlpipedWithPoints( float *vrtx,    // origin
                             float *vec1,    // vector 1
                             float *vec2,    // vector 2
                             float *vec3,    // vector 3
                             float ***pnts,  // array of points
                             int *pntsNum,   // size of the array
                             float ival)     // interval between points
{
    float pnt[3];
    float param;
    float offset;
    float r;
    int i, j;
    
    // length of the parallelepiped's edge
    r = vectorNorm( vec1);
    // number of points the parallelepiped's edge can be filled with
    j = (int)(r / ival);
    // fill parallelepiped with points
    offset = (r - (float)(j - 1) * ival) / 2;
    for ( i = 0; i < j; i++ )
    {
        // get point on the parallelepiped's edge
        param = r ? ((ival * (float)i + offset) / r) : 0;
        getPointOnSegmentByParam( vrtx, vec1, pnt, param);
        // fill the parallelogram with points
        fillParlgramWithPoints( pnt, vec2, vec3, pnts, pntsNum, ival);
    }
    
    return;
} // fillParlpipedWithPoints

// The function fills parallelogram given by origin 'vrtx' and reference 
// vectors 'vec1' and 'vec2' with points, the interval between points is 
// set by 'ival'. At the moment of function invocation, the array 'pnts' 
// could already contain some points and in this case 'pntsNum' has to 
// contain its current size; if the array is empty, 'pnts' has to be set 
// to NULL, and 'pntsNum' has to be set to 0. New points are stored in the 
// end of reallocated array 'pnts', new size of the array is returned 
// through 'pntsNum'.
void
IO::fillParlgramWithPoints( float *vrtx,    // origin
                            float *vec1,    // vector 1
                            float *vec2,    // vector 2
                            float ***pnts,  // array of points
                            int *pntsNum,   // size of the array
                            float ival)     // interval between points
{
    float pnt[3];
    float param;
    float offset;
    float r;
    int i, j;
    
    // length of the parallelogram's side
    r = vectorNorm( vec1);
    // number of points the parallelogram's side can be filled with
    j = (int)(r / ival);
    // fill parallelogram with points
    offset = (r - (float)(j - 1) * ival) / 2;
    for ( i = 0; i < j; i++ )
    {
        // get point on the parallelogram's side
        param = r ? ((ival * (float)i + offset) / r) : 0;
        getPointOnSegmentByParam( vrtx, vec1, pnt, param);
        // fill the segment with points
        fillSegmentWithPoints( pnt, vec2, pnts, pntsNum, ival);
    }
    
    return;
} // fillParlgramWithPoints

// The function fills segment given by origin 'vrtx' and reference 
// vector 'vec' with points, the interval between points is set by 
// 'ival'. At the moment of function invocation, the array 'pnts' 
// could already contain some points and in this case 'pntsNum' has 
// to contain its current size; if the array is empty, 'pnts' has 
// to be set to NULL, and 'pntsNum' has to be set to 0. New points 
// are stored in the end of reallocated array 'pnts', new size of 
// the array is returned through 'pntsNum'. 
void
IO::fillSegmentWithPoints( float *vrtx,    // origin
                           float *vec,     // vector
                           float ***pnts,  // array of points
                           int *pntsNum,   // size of the array
                           float ival)     // interval between points
{
    float param;
    float offset;
    float r;
    int i, j, n;
    
    // length of the segment
    r = vectorNorm( vec);
    // current size of the array
    n = *pntsNum;
    // number of points the segment can be filled with
    j = (int)(r / ival);
    // new size of the array
    *pntsNum += j;
    // reallocate the array
    *pnts = (float **)realloc( *pnts, *pntsNum * sizeof(float *));
    // fill segment with points
    offset = (r - (float)(j - 1) * ival) / 2;
    for ( i = 0; i < j; i++ )
    {
        // allocate memory for new point
        (*pnts)[i + n] = (float *)malloc( dimension * sizeof(float));
        // get next point on the segment by parameter and store it
        param = r ? ((ival * (float)i + offset) / r): 0;
        getPointOnSegmentByParam( vrtx, vec, (*pnts)[i + n], param);
    }

    return;
} // fillSegmentWithPoints

// Returns point 'pnt' belonging to a segment specified by 
// origin 'vrtx' and reference vector 'vec', the offset of 
// point from 'vrtx' is determined by 'param' (should be [0..1]).
void
IO::getPointOnSegmentByParam( float *vrtx,    // origin
                              float *vec,     // vector
                              float *pnt,     // point
                              float param)    // parameter
{
    int d;
    
    for ( d = 0; d < dimension; d++ )
        pnt[d] = param * vec[d] + vrtx[d];

    return;
} // getPointOnSegmentByParam