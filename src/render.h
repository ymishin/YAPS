// Copyright (c) 2005,2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#ifndef YAPS_RENDER_H
#define YAPS_RENDER_H

class Render
{

public:
    
    // constructor
    Render( int argc, char **argv);
    // run renderer
    static void run();

private:

    // RGB colors for rendering
    static const float particleColor[];
    static const float backgroundColor[];
    static const float segmentColor[];
    static const float triangleLineColor[];
    static const float triangleFillColor[];
    // display list(s)
    static const int obstacles_list;
    // default size of the window
    static const int windowWidth;
    static const int windowHeight;
    // current file
    static int nfile;

    // initialize display list(s)
    static void initDisplayLists();
    // initialize GL capabilities
    static void initGLCapabilities();
    // GLUT callbacks
    static void displayCallback();
    static void reshapeCallback( int width, int height);
    static void keyboardCallback( unsigned char key, int x, int y);
    static void specialFuncCallback( int key, int x, int y);

};

#endif /* YAPS_RENDER_H */