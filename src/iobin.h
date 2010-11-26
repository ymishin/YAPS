// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#ifndef YAPS_IOBIN_H
#define YAPS_IOBIN_H

class IOBin
{

public:
    // filename
    static const char* fname;
    // read and write transient data in binary form
    static int readData  ( int nfile);
    static int writeData ( int nfile);

};

#endif // YAPS_IOBIN_H