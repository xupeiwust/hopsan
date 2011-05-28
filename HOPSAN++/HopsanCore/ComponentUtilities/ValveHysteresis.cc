/*-----------------------------------------------------------------------------
 This source file is part of Hopsan NG

 Copyright (c) 2011 
    Mikael Axin, Robert Braun, Alessandro Dell'Amico, Bj�rn Eriksson,
    Peter Nordin, Karl Pettersson, Petter Krus, Ingo Staack

 This file is provided "as is", with no guarantee or warranty for the
 functionality or reliability of the contents. All contents in this file is
 the original work of the copyright holders at the Division of Fluid and
 Mechatronic Systems (Flumes) at Link�ping University. Modifying, using or
 redistributing any part of this file is prohibited without explicit
 permission from the copyright holders.
-----------------------------------------------------------------------------*/

//!
//! @file   ValveHysteresis.cc
//! @author Robert Braun <robert.braun@liu.se>
//! @date   2010-01-13
//!
//! @brief Contains a hysteresis function for valves and signals
//!
//$Id$

#include <math.h>
#include "ValveHysteresis.h"

using namespace hopsan;

ValveHysteresis::ValveHysteresis()
{
}

double ValveHysteresis::getValue(double xs, double xh, double xd)
{
    if (xd < xs-xh/2)
    {
        return xs-xh/2;
    }
    else if (xd > xs+xh/2)
    {
        return xs+xh/2;
    }
    else
    {
        return xd;
    }
}
