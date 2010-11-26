// Copyright (c) 2008-2010 Yury Mishin <yury.mishin@gmail.com>
// See the file COPYING for copying permission.
//
// $Id$

#ifndef YAPS_EOS_H
#define YAPS_EOS_H

class EOSBase {
public:
    // calculate particles' pressures
    virtual void calcPress() = 0;
};

class EOSBatchelor : public EOSBase {
public:
    // calculate particles' pressures
    void calcPress();
};

class EOSDesbrun : public EOSBase {
public:
    // calculate particles' pressures
    void calcPress();
};

#endif /* YAPS_EOS_H */
