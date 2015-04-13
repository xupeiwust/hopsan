#ifndef HYDRAULICCYLDAMP_HPP_INCLUDED
#define HYDRAULICCYLDAMP_HPP_INCLUDED

#include <iostream>
#include "ComponentEssentials.h"
#include "ComponentUtilities.h"
#include "math.h"

//!
//! @file HydraulicCyldamp.hpp
//! @author Petter Krus <petter.krus@liu.se>
//! @co-author **This component has not been audited by a second person**
//! @date Fri 10 Apr 2015 10:19:24
//! @brief A load sensing hydraulic directional valve
//! @ingroup HydraulicComponents
//!
//==This code has been autogenerated using Compgen==
//from 
/*{, C:, HopsanTrunk, componentLibraries, defaultLibrary, Hydraulic, \
Special}/HydraulicCyldamp0.nb*/

using namespace hopsan;

class HydraulicCyldamp : public ComponentQ
{
private:
     double rho;
     double Cq;
     double x0;
     double d0;
     double d1;
     double Acb;
     double Ac0;
     double Kcs;
     double plam;
     Port *mpPp;
     Port *mpPc;
     double delayParts1[9];
     double delayParts2[9];
     double delayParts3[9];
     Matrix jacobianMatrix;
     Vec systemEquations;
     Matrix delayedPart;
     int i;
     int iter;
     int mNoiter;
     double jsyseqnweight[4];
     int order[3];
     int mNstep;
     //Port Pp variable
     double pp;
     double qp;
     double Tp;
     double dEp;
     double cp;
     double Zcp;
     //Port Pc variable
     double pc;
     double qc;
     double Tc;
     double dEc;
     double cc;
     double Zcc;
//==This code has been autogenerated using Compgen==
     //inputVariables
     double xp;
     //outputVariables
     double ac;
     //InitialExpressions variables
     double pc0;
     //LocalExpressions variables
     double Ks;
     double Ksb;
     double Ks0;
     //Expressions variables
     //Port Pp pointer
     double *mpND_pp;
     double *mpND_qp;
     double *mpND_Tp;
     double *mpND_dEp;
     double *mpND_cp;
     double *mpND_Zcp;
     //Port Pc pointer
     double *mpND_pc;
     double *mpND_qc;
     double *mpND_Tc;
     double *mpND_dEc;
     double *mpND_cc;
     double *mpND_Zcc;
     //Delay declarations
//==This code has been autogenerated using Compgen==
     //inputVariables pointers
     double *mpxp;
     //inputParameters pointers
     double *mprho;
     double *mpCq;
     double *mpx0;
     double *mpd0;
     double *mpd1;
     double *mpAcb;
     double *mpAc0;
     double *mpKcs;
     double *mpplam;
     //outputVariables pointers
     double *mpac;
     Delay mDelayedPart10;
     EquationSystemSolver *mpSolver;

public:
     static Component *Creator()
     {
        return new HydraulicCyldamp();
     }

     void configure()
     {
//==This code has been autogenerated using Compgen==

        mNstep=9;
        jacobianMatrix.create(3,3);
        systemEquations.create(3);
        delayedPart.create(4,6);
        mNoiter=2;
        jsyseqnweight[0]=1;
        jsyseqnweight[1]=0.67;
        jsyseqnweight[2]=0.5;
        jsyseqnweight[3]=0.5;


        //Add ports to the component
        mpPp=addPowerPort("Pp","NodeHydraulic");
        mpPc=addPowerPort("Pc","NodeHydraulic");
        //Add inputVariables to the component
            addInputVariable("xp","Piston position","m",0.,&mpxp);

        //Add inputParammeters to the component
            addInputVariable("rho", "oil density", "kg/m3", 860.,&mprho);
            addInputVariable("Cq", "Flow coefficient.", "", 0.67,&mpCq);
            addInputVariable("x0", "damp distance", "m", 0.05,&mpx0);
            addInputVariable("d0", "min cone diameter", "m", 0.097,&mpd0);
            addInputVariable("d1", "outer diameter", "m", 0.1,&mpd1);
            addInputVariable("Acb", "back valve area", "m2", 0.0001,&mpAcb);
            addInputVariable("Ac0", "series valve area", "m2", \
0.00001,&mpAc0);
            addInputVariable("Kcs", "Fix flow pressure coeff", "m3/Pa", \
1.e-9,&mpKcs);
            addInputVariable("plam", "Turbulence onset pressure", "Pa", \
10000.,&mpplam);
        //Add outputVariables to the component
            addOutputVariable("ac","valve area","m",0.,&mpac);

//==This code has been autogenerated using Compgen==
        //Add constantParameters
        mpSolver = new EquationSystemSolver(this,3);
     }

    void initialize()
     {
        //Read port variable pointers from nodes
        //Port Pp
        mpND_pp=getSafeNodeDataPtr(mpPp, NodeHydraulic::Pressure);
        mpND_qp=getSafeNodeDataPtr(mpPp, NodeHydraulic::Flow);
        mpND_Tp=getSafeNodeDataPtr(mpPp, NodeHydraulic::Temperature);
        mpND_dEp=getSafeNodeDataPtr(mpPp, NodeHydraulic::HeatFlow);
        mpND_cp=getSafeNodeDataPtr(mpPp, NodeHydraulic::WaveVariable);
        mpND_Zcp=getSafeNodeDataPtr(mpPp, NodeHydraulic::CharImpedance);
        //Port Pc
        mpND_pc=getSafeNodeDataPtr(mpPc, NodeHydraulic::Pressure);
        mpND_qc=getSafeNodeDataPtr(mpPc, NodeHydraulic::Flow);
        mpND_Tc=getSafeNodeDataPtr(mpPc, NodeHydraulic::Temperature);
        mpND_dEc=getSafeNodeDataPtr(mpPc, NodeHydraulic::HeatFlow);
        mpND_cc=getSafeNodeDataPtr(mpPc, NodeHydraulic::WaveVariable);
        mpND_Zcc=getSafeNodeDataPtr(mpPc, NodeHydraulic::CharImpedance);

        //Read variables from nodes
        //Port Pp
        pp = (*mpND_pp);
        qp = (*mpND_qp);
        Tp = (*mpND_Tp);
        dEp = (*mpND_dEp);
        cp = (*mpND_cp);
        Zcp = (*mpND_Zcp);
        //Port Pc
        pc = (*mpND_pc);
        qc = (*mpND_qc);
        Tc = (*mpND_Tc);
        dEc = (*mpND_dEc);
        cc = (*mpND_cc);
        Zcc = (*mpND_Zcc);

        //Read inputVariables from nodes
        xp = (*mpxp);

        //Read inputParameters from nodes
        rho = (*mprho);
        Cq = (*mpCq);
        x0 = (*mpx0);
        d0 = (*mpd0);
        d1 = (*mpd1);
        Acb = (*mpAcb);
        Ac0 = (*mpAc0);
        Kcs = (*mpKcs);
        plam = (*mpplam);

        //Read outputVariables from nodes
        ac = (*mpac);

//==This code has been autogenerated using Compgen==
        //InitialExpressions
        pc0 = pc;

        //LocalExpressions
        Ks = (1.4142135623730951*Cq*(3.14159*(Power(d1,2)/4. - Power(d0 + \
((-d0 + d1)*(x0 - xp))/x0,2)/4.)*onNegative(-x0 + xp) + \
0.785398*Power(d1,2)*onPositive(-x0 + xp)))/Sqrt(rho);
        Ksb = (1.4142135623730951*Acb*Cq)/Sqrt(rho);
        Ks0 = (1.4142135623730951*Ac0*Cq)/Sqrt(rho);

        //Initialize delays

        delayedPart[1][1] = delayParts1[1];
        delayedPart[2][1] = delayParts2[1];
        delayedPart[3][1] = delayParts3[1];

        simulateOneTimestep();

     }
    void simulateOneTimestep()
     {
        Vec stateVar(3);
        Vec stateVark(3);
        Vec deltaStateVar(3);

        //Read variables from nodes
        //Port Pp
        Tp = (*mpND_Tp);
        cp = (*mpND_cp);
        Zcp = (*mpND_Zcp);
        //Port Pc
        Tc = (*mpND_Tc);
        cc = (*mpND_cc);
        Zcc = (*mpND_Zcc);

        //Read inputVariables from nodes
        xp = (*mpxp);

        //Read inputParameters from nodes
        rho = (*mprho);
        Cq = (*mpCq);
        x0 = (*mpx0);
        d0 = (*mpd0);
        d1 = (*mpd1);
        Acb = (*mpAcb);
        Ac0 = (*mpAc0);
        Kcs = (*mpKcs);
        plam = (*mpplam);

        //LocalExpressions
        Ks = (1.4142135623730951*Cq*(3.14159*(Power(d1,2)/4. - Power(d0 + \
((-d0 + d1)*(x0 - xp))/x0,2)/4.)*onNegative(-x0 + xp) + \
0.785398*Power(d1,2)*onPositive(-x0 + xp)))/Sqrt(rho);
        Ksb = (1.4142135623730951*Acb*Cq)/Sqrt(rho);
        Ks0 = (1.4142135623730951*Ac0*Cq)/Sqrt(rho);

        //Initializing variable vector for Newton-Raphson
        stateVark[0] = qp;
        stateVark[1] = pp;
        stateVark[2] = pc;

        //Iterative solution using Newton-Rapshson
        for(iter=1;iter<=mNoiter;iter++)
        {
         //Cyldamp
         //Differential-algebraic system of equation parts

          //Assemble differential-algebraic equations
          systemEquations[0] =qp - Sqrt((Power(Ks0,2)*Power(Ksb*onNegative(pc \
- pp) + Ks*onPositive(pc - pp),2))/(Power(Ks0,2) + Power(Ksb*onNegative(pc - \
pp) + Ks*onPositive(pc - pp),2)))*signedSquareL(pc - pp,plam);
          systemEquations[1] =pp - lowLimit(cp - qc*Zcp,0);
          systemEquations[2] =pc - lowLimit(cc + qc*Zcc,0);

          //Jacobian matrix
          jacobianMatrix[0][0] = 1;
          jacobianMatrix[0][1] = dxSignedSquareL(pc - \
pp,plam)*Sqrt((Power(Ks0,2)*Power(Ksb*onNegative(pc - pp) + Ks*onPositive(pc \
- pp),2))/(Power(Ks0,2) + Power(Ksb*onNegative(pc - pp) + Ks*onPositive(pc - \
pp),2)));
          jacobianMatrix[0][2] = -(dxSignedSquareL(pc - \
pp,plam)*Sqrt((Power(Ks0,2)*Power(Ksb*onNegative(pc - pp) + Ks*onPositive(pc \
- pp),2))/(Power(Ks0,2) + Power(Ksb*onNegative(pc - pp) + Ks*onPositive(pc - \
pp),2))));
          jacobianMatrix[1][0] = 0;
          jacobianMatrix[1][1] = 1;
          jacobianMatrix[1][2] = 0;
          jacobianMatrix[2][0] = 0;
          jacobianMatrix[2][1] = 0;
          jacobianMatrix[2][2] = 1;
//==This code has been autogenerated using Compgen==

          //Solving equation using LU-faktorisation
          mpSolver->solve(jacobianMatrix, systemEquations, stateVark, iter);
          qp=stateVark[0];
          pp=stateVark[1];
          pc=stateVark[2];
          //Expressions
          qc = -qp;
          ac = Sqrt((Power(Ks0,2)*Power(Ksb*onNegative(pc - pp) + \
Ks*onPositive(pc - pp),2))/(Power(Ks0,2) + Power(Ksb*onNegative(pc - pp) + \
Ks*onPositive(pc - pp),2)));
        }

        //Calculate the delayed parts

        delayedPart[1][1] = delayParts1[1];
        delayedPart[2][1] = delayParts2[1];
        delayedPart[3][1] = delayParts3[1];

        //Write new values to nodes
        //Port Pp
        (*mpND_pp)=pp;
        (*mpND_qp)=qp;
        (*mpND_dEp)=dEp;
        //Port Pc
        (*mpND_pc)=pc;
        (*mpND_qc)=qc;
        (*mpND_dEc)=dEc;
        //outputVariables
        (*mpac)=ac;

        //Update the delayed variabels

     }
    void deconfigure()
    {
        delete mpSolver;
    }
};
#endif // HYDRAULICCYLDAMP_HPP_INCLUDED
