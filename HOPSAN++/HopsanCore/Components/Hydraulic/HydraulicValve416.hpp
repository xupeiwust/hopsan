#ifndef HYDRAULICVALVE416_HPP_INCLUDED
#define HYDRAULICVALVE416_HPP_INCLUDED

#include <iostream>
#include "ComponentEssentials.h"
#include "ComponentUtilities.h"
#include "ComponentUtilities/matrix.h"
#include "math.h"

//!
//! @file HydraulicValve416.hpp
//! @author Petter Krus <petter.krus@liu.se>
//! @date Sun 18 Dec 2011 00:40:13
//! @brief A hydraulic valve with separate orifices
//! @ingroup HydraulicComponents
//!
//This component is generated by COMPGEN for HOPSAN-NG simulation 
//from 
/*{, C:, Documents and Settings, petkr14, My Documents, \
CompgenNG}/HydraulicComponentsNG.nb*/

using namespace hopsan;

class HydraulicValve416 : public ComponentQ
{
private:
     double mrho;
     double mCq;
     double mSd;
     double mFrpa;
     double mFrta;
     double mFrpb;
     double mFrtb;
     double mXpa0;
     double mXta0;
     double mXpb0;
     double mXtb0;
     double mmmmmXvmax;
     double mplam;
     double mxvpa;
     double mxvta;
     double mxvpb;
     double mxvtb;
     Port *mpPp;
     Port *mpPt;
     Port *mpPa;
     Port *mpPb;
     Port *mpPxvpa;
     Port *mpPxvta;
     Port *mpPxvpb;
     Port *mpPxvtb;
     double delayParts1[9];
     double delayParts2[9];
     double delayParts3[9];
     double delayParts4[9];
     double delayParts5[9];
     double delayParts6[9];
     double delayParts7[9];
     double delayParts8[9];
     Matrix jacobianMatrix;
     Vec systemEquations;
     Matrix delayedPart;
     int i;
     int iter;
     int mNoiter;
     double jsyseqnweight[4];
     int order[8];
     int mNstep;
     //Port Pp variable
     double pp;
     double qp;
     double cp;
     double Zcp;
     //Port Pt variable
     double pt;
     double qt;
     double ct;
     double Zct;
     //Port Pa variable
     double pa;
     double qa;
     double ca;
     double Zca;
     //Port Pb variable
     double pb;
     double qb;
     double cb;
     double Zcb;
     //inputVariables
     double xvpa;
     double xvta;
     double xvpb;
     double xvtb;
     //outputVariables
     //Port Pp pointer
     double *mpND_pp;
     double *mpND_qp;
     double *mpND_cp;
     double *mpND_Zcp;
     //Port Pt pointer
     double *mpND_pt;
     double *mpND_qt;
     double *mpND_ct;
     double *mpND_Zct;
     //Port Pa pointer
     double *mpND_pa;
     double *mpND_qa;
     double *mpND_ca;
     double *mpND_Zca;
     //Port Pb pointer
     double *mpND_pb;
     double *mpND_qb;
     double *mpND_cb;
     double *mpND_Zcb;
     //Delay declarations
     //inputVariables pointers
     double *mpND_xvpa;
     double *mpND_xvta;
     double *mpND_xvpb;
     double *mpND_xvtb;
     //outputVariables pointers
     Delay mDelayedPart10;
     Delay mDelayedPart20;
     Delay mDelayedPart30;
     Delay mDelayedPart40;

public:
     static Component *Creator()
     {
        return new HydraulicValve416();
     }

     HydraulicValve416() : ComponentQ()
     {
        const double rho = 860.;
        const double Cq = 0.67;
        const double Sd = 0.001;
        const double Frpa = 1.;
        const double Frta = 1.;
        const double Frpb = 1.;
        const double Frtb = 1.;
        const double Xpa0 = 0.;
        const double Xta0 = 0.;
        const double Xpb0 = 0.;
        const double Xtb0 = 0.;
        const double Xvmax = 0.01;
        const double plam = 10000.;
        const double xvpa = 0.;
        const double xvta = 0.;
        const double xvpb = 0.;
        const double xvtb = 0.;

        mNstep=9;
        jacobianMatrix.create(8,8);
        systemEquations.create(8);
        delayedPart.create(9,6);
        mNoiter=2;
        jsyseqnweight[0]=1;
        jsyseqnweight[1]=0.67;
        jsyseqnweight[2]=0.5;
        jsyseqnweight[3]=0.5;

        mrho = rho;
        mCq = Cq;
        mSd = Sd;
        mFrpa = Frpa;
        mFrta = Frta;
        mFrpb = Frpb;
        mFrtb = Frtb;
        mXpa0 = Xpa0;
        mXta0 = Xta0;
        mXpb0 = Xpb0;
        mXtb0 = Xtb0;
        mmmmmXvmax = Xvmax;
        mplam = plam;
        mxvpa = xvpa;
        mxvta = xvta;
        mxvpb = xvpb;
        mxvtb = xvtb;

        //Add ports to the component
        mpPp=addPowerPort("Pp","NodeHydraulic");
        mpPt=addPowerPort("Pt","NodeHydraulic");
        mpPa=addPowerPort("Pa","NodeHydraulic");
        mpPb=addPowerPort("Pb","NodeHydraulic");

        //Add inputVariables ports to the component
        mpPxvpa=addReadPort("Pxvpa","NodeSignal", Port::NOTREQUIRED);
        mpPxvta=addReadPort("Pxvta","NodeSignal", Port::NOTREQUIRED);
        mpPxvpb=addReadPort("Pxvpb","NodeSignal", Port::NOTREQUIRED);
        mpPxvtb=addReadPort("Pxvtb","NodeSignal", Port::NOTREQUIRED);

        //Add outputVariables ports to the component

        //Register changable parameters to the HOPSAN++ core
        registerParameter("rho", "oil density", "kg/m3", mrho);
        registerParameter("Cq", "Flow coefficient.", "", mCq);
        registerParameter("Sd", "spool diameter", "m", mSd);
        registerParameter("Frpa", "Spool cricle fraction(P-A)", "", mFrpa);
        registerParameter("Frta", "Spool cricle fraction(A-T)", "", mFrta);
        registerParameter("Frpb", "Spool cricle fraction(P-B)", "", mFrpb);
        registerParameter("Frtb", "Spool cricle fraction(B-T)", "", mFrtb);
        registerParameter("Xpa0", "Underlap", "m", mXpa0);
        registerParameter("Xta0", "Underlap", "m", mXta0);
        registerParameter("Xpb0", "Underlap", "m", mXpb0);
        registerParameter("Xtb0", "Underlap", "m", mXtb0);
        registerParameter("Xvmax", "Max opening", "m", mmmmmXvmax);
        registerParameter("plam", "Turbulence onset pressure", "Pa", mplam);
        registerParameter("xvpa", "Spool position", "m", mxvpa);
        registerParameter("xvta", "Spool position", "m", mxvta);
        registerParameter("xvpb", "Spool position", "m", mxvpb);
        registerParameter("xvtb", "Spool position", "m", mxvtb);
     }

    void initialize()
     {
        //Read port variable pointers from nodes
        //Port Pp
        mpND_pp=getSafeNodeDataPtr(mpPp, NodeHydraulic::PRESSURE);
        mpND_qp=getSafeNodeDataPtr(mpPp, NodeHydraulic::FLOW);
        mpND_cp=getSafeNodeDataPtr(mpPp, NodeHydraulic::WAVEVARIABLE);
        mpND_Zcp=getSafeNodeDataPtr(mpPp, NodeHydraulic::CHARIMP);
        //Port Pt
        mpND_pt=getSafeNodeDataPtr(mpPt, NodeHydraulic::PRESSURE);
        mpND_qt=getSafeNodeDataPtr(mpPt, NodeHydraulic::FLOW);
        mpND_ct=getSafeNodeDataPtr(mpPt, NodeHydraulic::WAVEVARIABLE);
        mpND_Zct=getSafeNodeDataPtr(mpPt, NodeHydraulic::CHARIMP);
        //Port Pa
        mpND_pa=getSafeNodeDataPtr(mpPa, NodeHydraulic::PRESSURE);
        mpND_qa=getSafeNodeDataPtr(mpPa, NodeHydraulic::FLOW);
        mpND_ca=getSafeNodeDataPtr(mpPa, NodeHydraulic::WAVEVARIABLE);
        mpND_Zca=getSafeNodeDataPtr(mpPa, NodeHydraulic::CHARIMP);
        //Port Pb
        mpND_pb=getSafeNodeDataPtr(mpPb, NodeHydraulic::PRESSURE);
        mpND_qb=getSafeNodeDataPtr(mpPb, NodeHydraulic::FLOW);
        mpND_cb=getSafeNodeDataPtr(mpPb, NodeHydraulic::WAVEVARIABLE);
        mpND_Zcb=getSafeNodeDataPtr(mpPb, NodeHydraulic::CHARIMP);
        //Read inputVariables pointers from nodes
        mpND_xvpa=getSafeNodeDataPtr(mpPxvpa, NodeSignal::VALUE,mxvpa);
        mpND_xvta=getSafeNodeDataPtr(mpPxvta, NodeSignal::VALUE,mxvta);
        mpND_xvpb=getSafeNodeDataPtr(mpPxvpb, NodeSignal::VALUE,mxvpb);
        mpND_xvtb=getSafeNodeDataPtr(mpPxvtb, NodeSignal::VALUE,mxvtb);
        //Read outputVariable pointers from nodes

        //Read variables from nodes
        //Port Pp
        pp = (*mpND_pp);
        qp = (*mpND_qp);
        cp = (*mpND_cp);
        Zcp = (*mpND_Zcp);
        //Port Pt
        pt = (*mpND_pt);
        qt = (*mpND_qt);
        ct = (*mpND_ct);
        Zct = (*mpND_Zct);
        //Port Pa
        pa = (*mpND_pa);
        qa = (*mpND_qa);
        ca = (*mpND_ca);
        Zca = (*mpND_Zca);
        //Port Pb
        pb = (*mpND_pb);
        qb = (*mpND_qb);
        cb = (*mpND_cb);
        Zcb = (*mpND_Zcb);

        //Read inputVariables from nodes
        xvpa = (*mpND_xvpa);
        xvta = (*mpND_xvta);
        xvpb = (*mpND_xvpb);
        xvtb = (*mpND_xvtb);

        //Read outputVariables from nodes


        //LocalExpressions
        double Ks = (1.4142135623730951*mCq)/Sqrt(mrho);
        double Kspa = 3.14159*Ks*mFrpa*mSd*limit(mXpa0 + xvpa,0.,mmmmmXvmax + \
mXpa0);
        double Ksta = 3.14159*Ks*mFrta*mSd*limit(mXta0 + xvta,0.,mmmmmXvmax + \
mXta0);
        double Kspb = 3.14159*Ks*mFrpb*mSd*limit(mXpb0 + xvpb,0.,mmmmmXvmax + \
mXpb0);
        double Kstb = 3.14159*Ks*mFrtb*mSd*limit(mXtb0 + xvtb,0.,mmmmmXvmax + \
mXtb0);

        //Initialize delays

        delayedPart[1][1] = delayParts1[1];
        delayedPart[2][1] = delayParts2[1];
        delayedPart[3][1] = delayParts3[1];
        delayedPart[4][1] = delayParts4[1];
        delayedPart[5][1] = delayParts5[1];
        delayedPart[6][1] = delayParts6[1];
        delayedPart[7][1] = delayParts7[1];
        delayedPart[8][1] = delayParts8[1];
     }
    void simulateOneTimestep()
     {
        Vec stateVar(8);
        Vec stateVark(8);
        Vec deltaStateVar(8);

        //Read variables from nodes
        //Port Pp
        cp = (*mpND_cp);
        Zcp = (*mpND_Zcp);
        //Port Pt
        ct = (*mpND_ct);
        Zct = (*mpND_Zct);
        //Port Pa
        ca = (*mpND_ca);
        Zca = (*mpND_Zca);
        //Port Pb
        cb = (*mpND_cb);
        Zcb = (*mpND_Zcb);

        //Read inputVariables from nodes
        xvpa = (*mpND_xvpa);
        xvta = (*mpND_xvta);
        xvpb = (*mpND_xvpb);
        xvtb = (*mpND_xvtb);

        //LocalExpressions
        double Ks = (1.4142135623730951*mCq)/Sqrt(mrho);
        double Kspa = 3.14159*Ks*mFrpa*mSd*limit(mXpa0 + xvpa,0.,mmmmmXvmax + \
mXpa0);
        double Ksta = 3.14159*Ks*mFrta*mSd*limit(mXta0 + xvta,0.,mmmmmXvmax + \
mXta0);
        double Kspb = 3.14159*Ks*mFrpb*mSd*limit(mXpb0 + xvpb,0.,mmmmmXvmax + \
mXpb0);
        double Kstb = 3.14159*Ks*mFrtb*mSd*limit(mXtb0 + xvtb,0.,mmmmmXvmax + \
mXtb0);

        //Initializing variable vector for Newton-Raphson
        stateVark[0] = qp;
        stateVark[1] = qt;
        stateVark[2] = qa;
        stateVark[3] = qb;
        stateVark[4] = pp;
        stateVark[5] = pt;
        stateVark[6] = pa;
        stateVark[7] = pb;

        //Iterative solution using Newton-Rapshson
        for(iter=1;iter<=mNoiter;iter++)
        {
         //Valve416
         //Differential-algebraic system of equation parts

          //Assemble differential-algebraic equations
          systemEquations[0] =qp + Kspa*signedSquareL(-pa + pp,mplam) + \
Kspb*signedSquareL(-pb + pp,mplam);
          systemEquations[1] =qt + Ksta*signedSquareL(-pa + pt,mplam) + \
Kstb*signedSquareL(-pb + pt,mplam);
          systemEquations[2] =qa - Kspa*signedSquareL(-pa + pp,mplam) - \
Ksta*signedSquareL(-pa + pt,mplam);
          systemEquations[3] =qb - Kspb*signedSquareL(-pb + pp,mplam) - \
Kstb*signedSquareL(-pb + pt,mplam);
          systemEquations[4] =pp - (cp + qp*Zcp)*onPositive(pp);
          systemEquations[5] =pt - (ct + qt*Zct)*onPositive(pt);
          systemEquations[6] =pa - (ca + qa*Zca)*onPositive(pa);
          systemEquations[7] =pb - (cb + qb*Zcb)*onPositive(pb);

          //Jacobian matrix
          jacobianMatrix[0][0] = 1;
          jacobianMatrix[0][1] = 0;
          jacobianMatrix[0][2] = 0;
          jacobianMatrix[0][3] = 0;
          jacobianMatrix[0][4] = Kspa*dxSignedSquareL(-pa + pp,mplam) + \
Kspb*dxSignedSquareL(-pb + pp,mplam);
          jacobianMatrix[0][5] = 0;
          jacobianMatrix[0][6] = -(Kspa*dxSignedSquareL(-pa + pp,mplam));
          jacobianMatrix[0][7] = -(Kspb*dxSignedSquareL(-pb + pp,mplam));
          jacobianMatrix[1][0] = 0;
          jacobianMatrix[1][1] = 1;
          jacobianMatrix[1][2] = 0;
          jacobianMatrix[1][3] = 0;
          jacobianMatrix[1][4] = 0;
          jacobianMatrix[1][5] = Ksta*dxSignedSquareL(-pa + pt,mplam) + \
Kstb*dxSignedSquareL(-pb + pt,mplam);
          jacobianMatrix[1][6] = -(Ksta*dxSignedSquareL(-pa + pt,mplam));
          jacobianMatrix[1][7] = -(Kstb*dxSignedSquareL(-pb + pt,mplam));
          jacobianMatrix[2][0] = 0;
          jacobianMatrix[2][1] = 0;
          jacobianMatrix[2][2] = 1;
          jacobianMatrix[2][3] = 0;
          jacobianMatrix[2][4] = -(Kspa*dxSignedSquareL(-pa + pp,mplam));
          jacobianMatrix[2][5] = -(Ksta*dxSignedSquareL(-pa + pt,mplam));
          jacobianMatrix[2][6] = Kspa*dxSignedSquareL(-pa + pp,mplam) + \
Ksta*dxSignedSquareL(-pa + pt,mplam);
          jacobianMatrix[2][7] = 0;
          jacobianMatrix[3][0] = 0;
          jacobianMatrix[3][1] = 0;
          jacobianMatrix[3][2] = 0;
          jacobianMatrix[3][3] = 1;
          jacobianMatrix[3][4] = -(Kspb*dxSignedSquareL(-pb + pp,mplam));
          jacobianMatrix[3][5] = -(Kstb*dxSignedSquareL(-pb + pt,mplam));
          jacobianMatrix[3][6] = 0;
          jacobianMatrix[3][7] = Kspb*dxSignedSquareL(-pb + pp,mplam) + \
Kstb*dxSignedSquareL(-pb + pt,mplam);
          jacobianMatrix[4][0] = -(Zcp*onPositive(pp));
          jacobianMatrix[4][1] = 0;
          jacobianMatrix[4][2] = 0;
          jacobianMatrix[4][3] = 0;
          jacobianMatrix[4][4] = 1;
          jacobianMatrix[4][5] = 0;
          jacobianMatrix[4][6] = 0;
          jacobianMatrix[4][7] = 0;
          jacobianMatrix[5][0] = 0;
          jacobianMatrix[5][1] = -(Zct*onPositive(pt));
          jacobianMatrix[5][2] = 0;
          jacobianMatrix[5][3] = 0;
          jacobianMatrix[5][4] = 0;
          jacobianMatrix[5][5] = 1;
          jacobianMatrix[5][6] = 0;
          jacobianMatrix[5][7] = 0;
          jacobianMatrix[6][0] = 0;
          jacobianMatrix[6][1] = 0;
          jacobianMatrix[6][2] = -(Zca*onPositive(pa));
          jacobianMatrix[6][3] = 0;
          jacobianMatrix[6][4] = 0;
          jacobianMatrix[6][5] = 0;
          jacobianMatrix[6][6] = 1;
          jacobianMatrix[6][7] = 0;
          jacobianMatrix[7][0] = 0;
          jacobianMatrix[7][1] = 0;
          jacobianMatrix[7][2] = 0;
          jacobianMatrix[7][3] = -(Zcb*onPositive(pb));
          jacobianMatrix[7][4] = 0;
          jacobianMatrix[7][5] = 0;
          jacobianMatrix[7][6] = 0;
          jacobianMatrix[7][7] = 1;

          //Solving equation using LU-faktorisation
          ludcmp(jacobianMatrix, order);
          solvlu(jacobianMatrix,systemEquations,deltaStateVar,order);

        for(i=0;i<8;i++)
          {
          stateVar[i] = stateVark[i] - 
          jsyseqnweight[iter - 1] * deltaStateVar[i];
          }
        for(i=0;i<8;i++)
          {
          stateVark[i] = stateVar[i];
          }
          qp=stateVark[0];
          qt=stateVark[1];
          qa=stateVark[2];
          qb=stateVark[3];
          pp=stateVark[4];
          pt=stateVark[5];
          pa=stateVark[6];
          pb=stateVark[7];
        }

        //Calculate the delayed parts

        delayedPart[1][1] = delayParts1[1];
        delayedPart[2][1] = delayParts2[1];
        delayedPart[3][1] = delayParts3[1];
        delayedPart[4][1] = delayParts4[1];
        delayedPart[5][1] = delayParts5[1];
        delayedPart[6][1] = delayParts6[1];
        delayedPart[7][1] = delayParts7[1];
        delayedPart[8][1] = delayParts8[1];

        //Write new values to nodes
        //Port Pp
        (*mpND_pp)=pp;
        (*mpND_qp)=qp;
        //Port Pt
        (*mpND_pt)=pt;
        (*mpND_qt)=qt;
        //Port Pa
        (*mpND_pa)=pa;
        (*mpND_qa)=qa;
        //Port Pb
        (*mpND_pb)=pb;
        (*mpND_qb)=qb;
        //outputVariables

        //Update the delayed variabels

     }
};
#endif // HYDRAULICVALVE416_HPP_INCLUDED
