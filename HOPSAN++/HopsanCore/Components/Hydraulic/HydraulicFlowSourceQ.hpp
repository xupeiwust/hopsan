/*-----------------------------------------------------------------------------
 This source file is part of Hopsan NG

 Copyright (c) 2011 
    Mikael Axin, Robert Braun, Alessandro Dell'Amico, Björn Eriksson,
    Peter Nordin, Karl Pettersson, Petter Krus, Ingo Staack

 This file is provided "as is", with no guarantee or warranty for the
 functionality or reliability of the contents. All contents in this file is
 the original work of the copyright holders at the Division of Fluid and
 Mechatronic Systems (Flumes) at Linköping University. Modifying, using or
 redistributing any part of this file is prohibited without explicit
 permission from the copyright holders.
-----------------------------------------------------------------------------*/

//!
//! @file   HydraulicFlowSourceQ.hpp
//! @author FluMeS
//! @date   2009-12-21
//!
//! @brief Contains a Hydraulic Flow Source Component of Q-type
//!
//$Id$

#ifndef HYDRAULICFLOWSOURCEQ_HPP_INCLUDED
#define HYDRAULICFLOWSOURCEQ_HPP_INCLUDED

#include <iostream>
#include "../../ComponentEssentials.h"

namespace hopsan {

    //!
    //! @brief
    //! @ingroup HydraulicComponents
    //!
    class HydraulicFlowSourceQ : public ComponentQ
    {
    private:
        double mFlow;
        double *mpND_in, *mpND_p, *mpND_q, *mpND_c, *mpND_Zc;

        Port *mpIn, *mpP1;

    public:
        static Component *Creator()
        {
            return new HydraulicFlowSourceQ("FlowSourceQ");
        }

        HydraulicFlowSourceQ(const std::string name) : ComponentQ(name)
        {
            mFlow = 1.0e-3;

            mpIn = addReadPort("in", "NodeSignal",  Port::NOTREQUIRED);
            mpP1 = addPowerPort("P1", "NodeHydraulic");

            registerParameter("q", "Flow", "[m^3/s]", mFlow);
        }


        void initialize()
        {
            mpND_in = getSafeNodeDataPtr(mpIn, NodeSignal::VALUE, mFlow);
            mpND_p = getSafeNodeDataPtr(mpP1, NodeHydraulic::PRESSURE);
            mpND_q = getSafeNodeDataPtr(mpP1, NodeHydraulic::FLOW);
            mpND_c = getSafeNodeDataPtr(mpP1, NodeHydraulic::WAVEVARIABLE);
            mpND_Zc = getSafeNodeDataPtr(mpP1, NodeHydraulic::CHARIMP);
        }


        void simulateOneTimestep()
        {
            //Declare local variables
            double in, p, q, c, Zc;

            //Read variables from nodes
            in = (*mpND_in);
            c = (*mpND_c);
            Zc = (*mpND_Zc);

            //Flow source equations
            q = in;
            p = c + q*Zc;

            if(p<0)
            {
                p=0;
            }

            (*mpND_p) = p;
            (*mpND_q) = q;
        }

        void finalize()
        {

        }
    };
}

#endif // HYDRAULICFLOWSOURCEQ_HPP_INCLUDED
