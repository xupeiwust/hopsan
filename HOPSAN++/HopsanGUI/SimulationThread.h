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
//! @file   SimulationThread.h
//! @author Bj�rn Eriksson <bjorn.eriksson@liu.se>
//! @date   2010-03-09
//!
//! @brief Contains a class for simulation in a separate class
//!
//$Id$


#ifndef SIMULATIONTHREAD_H
#define SIMULATIONTHREAD_H

#include <QThread>

//Forward Declaration
class CoreSystemAccess;

class SimulationThread : public QThread
{
public:
    SimulationThread(CoreSystemAccess *pGUIRootSystem, double startTime, double finishTime, QObject *parent);

protected:
    void run();

private:
    CoreSystemAccess *mpCoreSystemAccess;
    double mStartTime;
    double mFinishTime;

};

#endif // SIMULATIONTHREAD_H
