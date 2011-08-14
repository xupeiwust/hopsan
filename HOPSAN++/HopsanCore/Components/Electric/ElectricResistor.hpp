#ifndef ELECTRICRESISTOR_HPP_INCLUDED
#define ELECTRICRESISTOR_HPP_INCLUDED

#include <iostream>
#include <Qt/qdebug.h>
#include "../../HopsanCore/ComponentEssentials.h"
#include "../../HopsanCore/ComponentUtilities.h"
#include <math.h>
#include "matrix.h"

//!
//! @file ElectricResistor.hpp
//! @author Petter Krus <petter.krus@liu.se>
//! @date Sun 12 Jun 2011 13:16:14
//! @brief Resistor
//! @ingroup ElectricComponents
//!
//This component is generated by COMPGEN for HOPSAN-NG simulation 
//from 
/*{, C:, Documents and Settings, petkr14, My Documents, \
CompgenNG}/ElectricNG2.nb*/

using namespace hopsan;

class ElectricResistor : public ComponentQ
{
private:
     double mConduct;
     Port *mpPel1;
     Port *mpPel2;
     double delayParts1[9];
     double delayParts2[9];
     double delayParts3[9];
     Matrix jacobianMatrix;
     Vec systemEquations;
     Matrix delayedPart;
     int i;
     int iter;
     int mNoiter;
     int jsyseqnweight[4];
     int order[3];
     int mNstep;
     //Port Pel1 variable
     double uel1;
     double iel1;
     double cel1;
     double Zcel1;
     //Port Pel2 variable
     double uel2;
     double iel2;
     double cel2;
     double Zcel2;
     //Port Pel1 pointer
     double *mpND_uel1;
     double *mpND_iel1;
     double *mpND_cel1;
     double *mpND_Zcel1;
     //Port Pel2 pointer
     double *mpND_uel2;
     double *mpND_iel2;
     double *mpND_cel2;
     double *mpND_Zcel2;
     //Delay declarations
     Delay mDelayedPart10;

public:
     static Component *Creator()
     {
        std::cout << "running ElectricResistor creator" << std::endl;
        return new ElectricResistor("Resistor");
     }

     ElectricResistor(const std::string name = "Resistor"
                             ,const double Conduct = 0.1
                             )
        : ComponentQ(name)
     {
        mNstep=9;
        jacobianMatrix.create(3,3);
        systemEquations.create(3);
        delayedPart.create(4,6);
        mNoiter=2;
        jsyseqnweight[0]=1;
        jsyseqnweight[1]=0.67;
        jsyseqnweight[2]=0.5;
        jsyseqnweight[3]=0.5;

        mConduct = Conduct;

        //Add ports to the component
        mpPel1=addPowerPort("Pel1","NodeElectric");
        mpPel2=addPowerPort("Pel2","NodeElectric");

        //Register changable parameters to the HOPSAN++ core
        registerParameter("Conduct", "Conductivity (1/resistance)", "A/V", \
mConduct);
     }

    void initialize()
     {
        //Read variable pointers from nodes
        //Port Pel1
        mpND_uel1=getSafeNodeDataPtr(mpPel1, NodeElectric::VOLTAGE);
        mpND_iel1=getSafeNodeDataPtr(mpPel1, NodeElectric::CURRENT);
        mpND_cel1=getSafeNodeDataPtr(mpPel1, NodeElectric::WAVEVARIABLE);
        mpND_Zcel1=getSafeNodeDataPtr(mpPel1, NodeElectric::CHARIMP);
        //Port Pel2
        mpND_uel2=getSafeNodeDataPtr(mpPel2, NodeElectric::VOLTAGE);
        mpND_iel2=getSafeNodeDataPtr(mpPel2, NodeElectric::CURRENT);
        mpND_cel2=getSafeNodeDataPtr(mpPel2, NodeElectric::WAVEVARIABLE);
        mpND_Zcel2=getSafeNodeDataPtr(mpPel2, NodeElectric::CHARIMP);
        //Read variables from nodes
        //Port Pel1
        uel1 = (*mpND_uel1);
        iel1 = (*mpND_iel1);
        cel1 = (*mpND_cel1);
        Zcel1 = (*mpND_Zcel1);
        //Port Pel2
        uel2 = (*mpND_uel2);
        iel2 = (*mpND_iel2);
        cel2 = (*mpND_cel2);
        Zcel2 = (*mpND_Zcel2);



        //Initialize delays
     }
    void simulateOneTimestep()
     {
        Vec stateVar(3);
        Vec stateVark(3);
        Vec deltaStateVar(3);

        //Read variables from nodes
        //Port Pel1
        uel1 = (*mpND_uel1);
        iel1 = (*mpND_iel1);
        cel1 = (*mpND_cel1);
        Zcel1 = (*mpND_Zcel1);
        //Port Pel2
        uel2 = (*mpND_uel2);
        iel2 = (*mpND_iel2);
        cel2 = (*mpND_cel2);
        Zcel2 = (*mpND_Zcel2);

        //LocalExpressions

        //Initializing variable vector for Newton-Raphson
        stateVark[0] = iel2;
        stateVark[1] = uel1;
        stateVark[2] = uel2;

        //Iterative solution using Newton-Rapshson
        for(iter=1;iter<=mNoiter;iter++)
        {
         //Resistor
         //Differential-algebraic system of equation parts

          delayedPart[1][1] = delayParts1[1];
          delayedPart[2][1] = delayParts2[1];
          delayedPart[3][1] = delayParts3[1];

          //Assemble differential-algebraic equations
          systemEquations[0] =iel2 + mConduct*(-uel1 + uel2);
          systemEquations[1] =-cel1 + uel1 + iel2*Zcel1;
          systemEquations[2] =-cel2 + uel2 - iel2*Zcel2;

          //Jacobian matrix
          jacobianMatrix[0][0] = 1;
          jacobianMatrix[0][1] = -mConduct;
          jacobianMatrix[0][2] = mConduct;
          jacobianMatrix[1][0] = Zcel1;
          jacobianMatrix[1][1] = 1;
          jacobianMatrix[1][2] = 0;
          jacobianMatrix[2][0] = -Zcel2;
          jacobianMatrix[2][1] = 0;
          jacobianMatrix[2][2] = 1;

          //Solving equation using LU-faktorisation
          ludcmp(jacobianMatrix, order);
          solvlu(jacobianMatrix,systemEquations,deltaStateVar,order);

        for(i=0;i<3;i++)
          {
          stateVar[i] = stateVark[i] - 
          jsyseqnweight[iter - 1] * deltaStateVar[i];
          }
        for(i=0;i<3;i++)
          {
          stateVark[i] = stateVar[i];
          }
        }
        iel2=stateVark[0];
        uel1=stateVark[1];
        uel2=stateVark[2];
        //Expressions
        double iel1 = -iel2;

        //Write new values to nodes
        //Port Pel1
        (*mpND_uel1)=uel1;
        (*mpND_iel1)=iel1;
        //Port Pel2
        (*mpND_uel2)=uel2;
        (*mpND_iel2)=iel2;

        //Update the delayed variabels

     }
};
#endif // ELECTRICRESISTOR_HPP_INCLUDED
