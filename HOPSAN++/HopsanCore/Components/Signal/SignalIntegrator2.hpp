//!
//! @file   SignalIntegrator2.hpp
//! @author Björn Eriksson <bjorn.eriksson@liu.se>
//! @date   2010-01-22
//!
//! @brief Contains a Signal Integrator Component using CoreUtilities
//!
//$Id$

#ifndef SIGNALINTEGRATOR2_HPP_INCLUDED
#define SIGNALINTEGRATOR2_HPP_INCLUDED

#include "HopsanCore.h"
#include "CoreUtilities/Integrator.h"

class SignalIntegrator2 : public ComponentSignal
{

private:
    Integrator mIntegrator;
    double mStartY;
    enum {in, out};

public:
    static Component *Creator()
    {
        std::cout << "running Integrator creator" << std::endl;
        return new SignalIntegrator2("DefaultIntegratorName");
    }

    SignalIntegrator2(const string name,
                     const double timestep = 0.001)
	: ComponentSignal(name, timestep)
    {
        mStartY = 0.0;

 //       mIntegrator.initializeValues(0.0, mStartY, mTimestep, mTime);

        addReadPort("in", "NodeSignal", in);
        addWritePort("out", "NodeSignal", out);
    }


	void initialize()
	{
	    double u0 = mPortPtrs[in]->readNode(NodeSignal::VALUE);
	    mIntegrator.initialize(mTime, mTimestep, u0, mStartY);
	    ///TODO: Write out values into node as well? (I think so) This is true for all components
	}


    void simulateOneTimestep()
    {
        //Get variable values from nodes
        double u = mPortPtrs[in]->readNode(NodeSignal::VALUE);

        //Filter equation

        //Write new values to nodes
        mPortPtrs[out]->writeNode(NodeSignal::VALUE, mIntegrator.value(u));

    }
};

#endif // SIGNALINTEGRATOR_HPP_INCLUDED


