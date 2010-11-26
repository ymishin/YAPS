// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#include "render.h"
#include "opengl.h"
#include "iobin.h"
#include "common.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

// RGB colors for rendering
// smoothing particles
const float Render::particleColor[2][3]  = { { 0.0f, 0.4f, 0.6f },
                                             { 0.0f, 0.2f, 0.4f } };
// background
const float Render::backgroundColor[3]   = { 0.7f, 0.7f, 0.7f };
// segments
const float Render::segmentColor[3]      = { 0.0f, 0.0f, 0.0f };
// triangles' edges
const float Render::triangleLineColor[3] = { 0.0f, 0.0f, 0.0f };
// triangles' interiors
const float Render::triangleFillColor[4] = { 0.6f, 0.6f, 0.6f, 0.7f };

// display list(s) numbers
const int Render::obstacles_list = 1;

// default size of the window
const int Render::windowWidth  = 800;
const int Render::windowHeight = 800;

// current time step
int Render::nfile = 0;

// Constructor.
Render::Render( int argc, char **argv)
{
    // initialize GLUT
    glutInit( &argc, argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    
    // create window
    glutInitWindowSize( windowWidth, windowHeight);
    glutCreateWindow( argv[0]);
    char title[20];
    sprintf( title, "%s - %05d", "YAPS", nfile);
    glutSetWindowTitle( title);
    
    // initialize GL capabilities
    initGLCapabilities();
    
    // initialize display lists
    initDisplayLists();
    
    // register callbacks for current window
    glutDisplayFunc( displayCallback);
    glutReshapeFunc( reshapeCallback);
    glutKeyboardFunc( keyboardCallback);
    glutSpecialFunc( specialFuncCallback);
    glutMouseFunc( mouseFuncCallback);
    glutMotionFunc( mouseMotionCallback);
} // Render

// Run renderer.
void 
Render::run()
{
    // main loop
    glutMainLoop();
} // run

// Initialize OpenGL capabilities.
void
Render::initGLCapabilities()
{
    // turn on antialiasing for lines
    glEnable( GL_LINE_SMOOTH);
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    // turn on Z-Buffer
    glEnable( GL_DEPTH_TEST);
    glDepthFunc( GL_LEQUAL);

    // turn on blending
    glEnable( GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return;
} // initGLCapabilities

// Initialize display list(s).
void
Render::initDisplayLists( void)
{
    // initialize obstacles_list
    glNewList( obstacles_list, GL_COMPILE);

    if ( dimension == 2 )
    {
        // draw segments
        glBegin( GL_LINES);
        for ( int i = 0; i < (int)obstacles.size(); i++ )
        {
            glColor3fv( segmentColor);
            glVertex3fv( obstacles[i].vrtx1);
            glVertex3fv( obstacles[i].vrtx2);
        }
        glEnd();
    }
    else if ( dimension == 3 )
    {
        // draw triangles' edges
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
        glBegin( GL_TRIANGLES);
        for ( int i = 0; i < (int)obstacles.size(); i++ )
        {
            glColor3fv( triangleLineColor);
            glVertex3fv( obstacles[i].vrtx1);
            glVertex3fv( obstacles[i].vrtx2);
            glVertex3fv( obstacles[i].vrtx3);
        }
        glEnd();

        // fill triangles' interiors
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
        // if it is transparent - turn off depth test
        if ( triangleFillColor[3] < 1.0f )
            glDepthMask( GL_FALSE);
        glBegin( GL_TRIANGLES);
        for ( int i = 0; i < (int)obstacles.size(); i++ )
        {
            glColor4fv( triangleFillColor);
            glVertex3fv( obstacles[i].vrtx1);
            glVertex3fv( obstacles[i].vrtx2);
            glVertex3fv( obstacles[i].vrtx3);
        }
        glEnd();
        // turn on depth test if it was turned off
        if ( triangleFillColor[3] < 1.0f )
            glDepthMask( GL_TRUE);
    }

    // end of obstacles_list
    glEndList();
    
    return;
} // initDisplayLists

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                   GLUT CALLBACKS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// GLUT display callback.
void
Render::displayCallback()
{
    // clear the buffers
    glClearColor( backgroundColor[0],
                  backgroundColor[1],
                  backgroundColor[2],
                  1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the particles
    for ( int i = 0; i < (int)particles.size(); i++ )
    {
        glPushMatrix();
        glTranslatef( particles[i].pos[0], 
                      particles[i].pos[1], 
                      particles[i].pos[2]);
        glColor3fv( particleColor[particles[i].no-1]);
        glutSolidSphere( parameters.particlesRadius, 20, 20);
        glPopMatrix();
    }

    // draw the obstacles
    glCallList( obstacles_list);

    // swap the buffers
    glutSwapBuffers();
    
    return;
} // displayCallback

// GLUT reshape callback.
void
Render::reshapeCallback( int width,   // window's width
                         int height)  // window's height
{
    float clipVolume = parameters.clipVolume;

    // it's not allowed to change the size of the window
    glutReshapeWindow( windowWidth, windowHeight);
    glViewport( 0, 0, windowWidth, windowHeight);

    // set projection matrix
    glMatrixMode( GL_PROJECTION);
    glLoadIdentity();
    glOrtho( 0.0f, clipVolume, 0.0f, clipVolume, 
             -clipVolume * 4.0f, clipVolume * 3.0f);
    glMatrixMode( GL_MODELVIEW);
    
    return;
} // reshapeCallback

// GLUT keyboard callback.
void
Render::keyboardCallback( unsigned char key,   // key's code
                          int x,               // position of
                          int y)               // the mouse
{
    char title[20];

    switch (key) {
      case 'n':
          // read next file and redisplay
          if ( IOBin().readData( nfile + 1) == 0 )
          {
              nfile++;
              sprintf( title, "%s - %05d", "YAPS", nfile);
              glutSetWindowTitle( title);
              glutPostRedisplay();
          }
          break;
      case 'p':
          // read previous file and redisplay
          if ( IOBin().readData( nfile - 1) == 0 )
          {
              nfile--;
              sprintf( title, "%s - %05d", "YAPS", nfile);
              glutSetWindowTitle( title);
              glutPostRedisplay();
          }
          break;
      case 'q':
          // exit the program
          exit( 0);
      default:
        break;
    }

    return;
} // keyboardCallback

// scaling/rotation steps
const float Render::scaleStep    = 0.05f;
const float Render::xRotateStep  = 1.0f;
const float Render::yRotateStep  = 1.0f;
const float Render::xyRotateMult = 0.1f;
// current scaling/rotation factors
float Render::scaleFactor   = 1.0f;
float Render::xRotateFactor = 0.0f;
float Render::yRotateFactor = 0.0f;

// GLUT special functions callback.
void
Render::specialFuncCallback( int key,   // key's code
                             int x,     // position of
                             int y)     // the mouse
{

    switch (key) {
      case GLUT_KEY_UP:
          xRotateFactor += xRotateStep;
          break;
      case GLUT_KEY_DOWN:
          xRotateFactor -= xRotateStep;
          break;
      case GLUT_KEY_LEFT:
          yRotateFactor += yRotateStep;
          break;
      case GLUT_KEY_RIGHT:
          yRotateFactor -= yRotateStep;
          break;
      case GLUT_KEY_PAGE_UP:
          scaleFactor += scaleStep;
          break;
      case GLUT_KEY_PAGE_DOWN:
          scaleFactor -= scaleStep;
          break;
      default:
          return;
          break;
    }

    scaleRotateFunc();

    return;
} // specialFuncCallback

// current mouse button and position
int Render::x0 = -1;
int Render::y0 = -1;
int Render::curButton = -1;

// GLUT mouse button callback.
void
Render::mouseFuncCallback( int button, int state, int x, int y)
{

    // save pressed button and position
    curButton = button;
    x0 = x;
    y0 = y;

    // scaling
    switch (curButton) {
      case 3:
          scaleFactor += scaleStep;
          break;
      case 4:
          scaleFactor -= scaleStep;
          break;
      default:
          return;
          break;
    }

    // redraw
    scaleRotateFunc();

    return;
} // mouseFuncCallback

// GLUT mouse motion callback.
void
Render::mouseMotionCallback( int x, int y)
{
    // rotation
    switch (curButton) {
      case GLUT_LEFT_BUTTON:
          yRotateFactor -= xyRotateMult * (x0 - x);
          xRotateFactor -= xyRotateMult * (y0 - y);
          x0 = x;
          y0 = y;
          break;
      default:
          return;
          break;
    }
     
    // redraw
    scaleRotateFunc();

    return;
} // mouseMotionCallback

// Rotate and scale and redraw.
void
Render::scaleRotateFunc()
{
    glLoadIdentity();

    float clipVolume = parameters.clipVolume;

    // set the center of rotation
    glTranslatef( clipVolume / 2.0f, clipVolume / 2.0f, clipVolume / 2.0f);
    // rotate the scene along X-axis
    glRotatef( xRotateFactor, 1.0, 0.0, 0.0);
    // rotate the scene along Y-axis
    glRotatef( yRotateFactor, 0.0, 1.0, 0.0);
    // scale the scene
    if ( scaleFactor < scaleStep )
        scaleFactor = scaleStep;
    glScalef( scaleFactor, scaleFactor, scaleFactor);
    // restore the origin
    glTranslatef( -clipVolume / 2.0f, -clipVolume / 2.0f, -clipVolume / 2.0f);
    
    // redisplay the scene
    glutPostRedisplay();

    return;
} // scaleRotateFunc