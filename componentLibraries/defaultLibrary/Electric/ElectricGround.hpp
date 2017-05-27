/*-----------------------------------------------------------------------------

 Copyright 2017 Hopsan Group

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.


 The full license is available in the file LICENSE.
 For details about the 'Hopsan Group' or information about Authors and
 Contributors see the HOPSANGROUP and AUTHORS files that are located in
 the Hopsan source code root directory.

-----------------------------------------------------------------------------*/

#ifndef ELECTRICGROUND_HPP_INCLUDED
#define ELECTRICGROUND_HPP_INCLUDED

#include <iostream>
#include "ComponentEssentials.h"
#include "ComponentUtilities.h"
#include "math.h"

//!
//! @file ElectricGround.hpp
//! @author Petter Krus <petter.krus@liu.se>
//! @date Mon 7 Apr 2014 13:06:55
//! @brief Electric ground
//! @ingroup ElectricComponents
//!
//==This code has been autogenerated using Compgen==
//from 
/*{, C:, HopsanTrunk, ComponentLibraries, defaultLibrary, \
Electric}/ElectricGround.nb*/

using namespace hopsan;

class ElectricGround : public ComponentC
{
private:
     Port *mpPel1;
     int mNstep;
     //Port Pel1 variable
     double uel1;
     double iel1;
     double cel1;
     double Zcel1;
//==This code has been autogenerated using Compgen==
     //inputVariables
     //outputVariables
     //Expressions variables
     //Port Pel1 pointer
     double *mpND_uel1;
     double *mpND_iel1;
     double *mpND_cel1;
     double *mpND_Zcel1;
     //Delay declarations
//==This code has been autogenerated using Compgen==
     //inputVariables pointers
     //inputParameters pointers
     //outputVariables pointers
     EquationSystemSolver *mpSolver;

public:
     static Component *Creator()
     {
        return new ElectricGround();
     }

     void configure()
     {
//==This code has been autogenerated using Compgen==

        mNstep=9;

        //Add ports to the component
        mpPel1=addPowerPort("Pel1","NodeElectric");
        //Add inputVariables to the component

        //Add inputParammeters to the component
        //Add outputVariables to the component

//==This code has been autogenerated using Compgen==
        //Add constantParameters
     }

    void initialize()
     {
        //Read port variable pointers from nodes
        //Port Pel1
        mpND_uel1=getSafeNodeDataPtr(mpPel1, NodeElectric::Voltage);
        mpND_iel1=getSafeNodeDataPtr(mpPel1, NodeElectric::Current);
        mpND_cel1=getSafeNodeDataPtr(mpPel1, NodeElectric::WaveVariable);
        mpND_Zcel1=getSafeNodeDataPtr(mpPel1, NodeElectric::CharImpedance);

        //Read variables from nodes
        //Port Pel1
        uel1 = (*mpND_uel1);
        iel1 = (*mpND_iel1);
        cel1 = (*mpND_cel1);
        Zcel1 = (*mpND_Zcel1);

        //Read inputVariables from nodes

        //Read inputParameters from nodes

        //Read outputVariables from nodes

//==This code has been autogenerated using Compgen==


        //Initialize delays

     }
    void simulateOneTimestep()
     {
        //Read variables from nodes
        //Port Pel1
        uel1 = (*mpND_uel1);
        iel1 = (*mpND_iel1);

        //Read inputVariables from nodes

        //LocalExpressions

          //Expressions
          cel1 = 0;
          Zcel1 = 0.;

        //Calculate the delayed parts


        //Write new values to nodes
        //Port Pel1
        (*mpND_cel1)=cel1;
        (*mpND_Zcel1)=Zcel1;
        //outputVariables

        //Update the delayed variabels

     }
    void deconfigure()
    {
        delete mpSolver;
    }
};
#endif // ELECTRICGROUND_HPP_INCLUDED
