#ifndef ELECTRICBATTERY_HPP_INCLUDED
#define ELECTRICBATTERY_HPP_INCLUDED

#include <iostream>
#include <Qt/qdebug.h>
#include "../../HopsanCore/ComponentEssentials.h"
#include "../../HopsanCore/ComponentUtilities.h"
#include <math.h>
#include "matrix.h"

//!
//! @file ElectricBattery.hpp
//! @author Petter Krus <petter.krus@liu.se>
//! @date Tue 21 Jun 2011 13:32:15
//! @brief Battery with static behaviour
//! @ingroup ElectricComponents
//!
//This component is generated by COMPGEN for HOPSAN-NG simulation 
//from 
/*{, C:, Documents and Settings, petkr14, My Documents, \
CompgenNG}/ElectricNG3.nb*/

using namespace hopsan;

class ElectricBattery : public ComponentQ
{
private:
     double mcond;
     double munom;
     double mcapacity;
     double mkappa;
     Port *mpPel1;
     Port *mpPsoc;
     Port *mpPubatt;
     double delayParts1[9];
     double delayParts2[9];
     double delayParts3[9];
     double delayParts4[9];
     Matrix jacobianMatrix;
     Vec systemEquations;
     Matrix delayedPart;
     int i;
     int iter;
     int mNoiter;
     int jsyseqnweight[4];
     int order[4];
     int mNstep;
     //Port Pel1 variable
     double uel1;
     double iel1;
     double cel1;
     double Zcel1;
     //Port Psoc variable
     double soc;
     //Port Pubatt variable
     double ubatt;
     //Port Pel1 pointer
     double *mpND_uel1;
     double *mpND_iel1;
     double *mpND_cel1;
     double *mpND_Zcel1;
     //Port Psoc pointer
     double *mpND_soc;
     //Port Pubatt pointer
     double *mpND_ubatt;
     //Delay declarations
     Delay mDelayedPart10;
     Delay mDelayedPart11;
     Delay mDelayedPart20;
     Delay mDelayedPart30;

public:
     static Component *Creator()
     {
        std::cout << "running ElectricBattery creator" << std::endl;
        return new ElectricBattery("Battery");
     }

     ElectricBattery(const std::string name = "Battery"
                             ,const double cond = 0.01
                             ,const double unom = 12
                             ,const double capacity = 41
                             ,const double kappa = 0.1
                             )
        : ComponentQ(name)
     {
        mNstep=9;
        jacobianMatrix.create(4,4);
        systemEquations.create(4);
        delayedPart.create(5,6);
        mNoiter=2;
        jsyseqnweight[0]=1;
        jsyseqnweight[1]=0.67;
        jsyseqnweight[2]=0.5;
        jsyseqnweight[3]=0.5;

        mcond = cond;
        munom = unom;
        mcapacity = capacity;
        mkappa = kappa;

        //Add ports to the component
        mpPel1=addPowerPort("Pel1","NodeElectric");
        mpPsoc=addWritePort("Psoc","NodeSignal", Port::NOTREQUIRED);
        mpPubatt=addWritePort("Pubatt","NodeSignal", Port::NOTREQUIRED);

        //Register changable parameters to the HOPSAN++ core
        registerParameter("cond", "conductance (at 1)", "1/ohm", mcond);
        registerParameter("unom", "nominal voltage of battery", "V", munom);
        registerParameter("capacity", "capacity", "Ah", mcapacity);
        registerParameter("kappa", "exponent of discharge function", "", \
mkappa);
     }

    void initialize()
     {
        //Read variable pointers from nodes
        //Port Pel1
        mpND_uel1=getSafeNodeDataPtr(mpPel1, NodeElectric::VOLTAGE);
        mpND_iel1=getSafeNodeDataPtr(mpPel1, NodeElectric::CURRENT);
        mpND_cel1=getSafeNodeDataPtr(mpPel1, NodeElectric::WAVEVARIABLE);
        mpND_Zcel1=getSafeNodeDataPtr(mpPel1, NodeElectric::CHARIMP);
        //Port Psoc
        mpND_soc=getSafeNodeDataPtr(mpPsoc, NodeSignal::VALUE);
        //Port Pubatt
        mpND_ubatt=getSafeNodeDataPtr(mpPubatt, NodeSignal::VALUE);

        //Read variables from nodes
        //Port Pel1
        uel1 = (*mpND_uel1);
        iel1 = (*mpND_iel1);
        cel1 = (*mpND_cel1);
        Zcel1 = (*mpND_Zcel1);
        //Port Psoc
        soc = mpPsoc->getStartValue(NodeSignal::VALUE);
        //Port Pubatt
        ubatt = mpPubatt->getStartValue(NodeSignal::VALUE);


        //Initialize delays
        delayParts1[1] = (iel1*mTimestep - \
7200*mcapacity*soc)/(7200.*mcapacity);
        mDelayedPart11.initialize(mNstep,delayParts1[0]);
     }
    void simulateOneTimestep()
     {
        Vec stateVar(4);
        Vec stateVark(4);
        Vec deltaStateVar(4);

        //Read variables from nodes
        //Port Pel1
        cel1 = (*mpND_cel1);
        Zcel1 = (*mpND_Zcel1);
        //Port Psoc
        //Port Pubatt

        //LocalExpressions

        //Initializing variable vector for Newton-Raphson
        stateVark[0] = soc;
        stateVark[1] = iel1;
        stateVark[2] = ubatt;
        stateVark[3] = uel1;

        //Iterative solution using Newton-Rapshson
        for(iter=1;iter<=mNoiter;iter++)
        {
         //Battery
         //Differential-algebraic system of equation parts
          delayParts1[1] = (iel1*mTimestep - \
7200*mcapacity*soc)/(7200.*mcapacity);

          delayedPart[1][1] = delayParts1[1];
          delayedPart[2][1] = delayParts2[1];
          delayedPart[3][1] = delayParts3[1];
          delayedPart[4][1] = delayParts4[1];

          //Assemble differential-algebraic equations
          systemEquations[0] =(iel1*mTimestep)/(7200.*mcapacity) + soc + \
delayedPart[1][1];
          systemEquations[1] =iel1 + mcond*(-ubatt + uel1);
          systemEquations[2] =ubatt - \
Power(2,mkappa)*munom*Power(asin(limit(soc,1.e-9,0.9999)),mkappa);
          systemEquations[3] =-cel1 + uel1 - iel1*Zcel1;

          //Jacobian matrix
          jacobianMatrix[0][0] = 1;
          jacobianMatrix[0][1] = mTimestep/(7200.*mcapacity);
          jacobianMatrix[0][2] = 0;
          jacobianMatrix[0][3] = 0;
          jacobianMatrix[1][0] = 0;
          jacobianMatrix[1][1] = 1;
          jacobianMatrix[1][2] = -mcond;
          jacobianMatrix[1][3] = mcond;
          jacobianMatrix[2][0] = \
-((Power(2,mkappa)*mkappa*munom*Power(asin(limit(soc,1.e-9,0.9999)),-1 + \
mkappa)*dxLimit(soc,1.e-9,0.9999))/Sqrt(1 - \
Power(limit(soc,1.e-9,0.9999),2)));
          jacobianMatrix[2][1] = 0;
          jacobianMatrix[2][2] = 1;
          jacobianMatrix[2][3] = 0;
          jacobianMatrix[3][0] = 0;
          jacobianMatrix[3][1] = -Zcel1;
          jacobianMatrix[3][2] = 0;
          jacobianMatrix[3][3] = 1;

          //Solving equation using LU-faktorisation
          ludcmp(jacobianMatrix, order);
          solvlu(jacobianMatrix,systemEquations,deltaStateVar,order);

        for(i=0;i<4;i++)
          {
          stateVar[i] = stateVark[i] - 
          jsyseqnweight[iter - 1] * deltaStateVar[i];
          }
        for(i=0;i<4;i++)
          {
          stateVark[i] = stateVar[i];
          }
        }
        soc=stateVark[0];
        iel1=stateVark[1];
        ubatt=stateVark[2];
        uel1=stateVark[3];

        //Write new values to nodes
        //Port Pel1
        (*mpND_uel1)=uel1;
        (*mpND_iel1)=iel1;
        //Port Psoc
        (*mpND_soc)=soc;
        //Port Pubatt
        (*mpND_ubatt)=ubatt;

        //Update the delayed variabels
        mDelayedPart11.update(delayParts1[1]);

     }
};
#endif // ELECTRICBATTERY_HPP_INCLUDED
