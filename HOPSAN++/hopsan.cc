#include "HopsanCore.h"
#include "TicToc.h"
#include "CoreUtilities/Delay.h"

void test1()
{
    HopsanEssentials Hopsan;
    TicToc totaltimer("totaltimer");

    //Create master component
    ComponentSystem simulationmodel("simulationmodel");
    //Create other components
    ComponentPressureSource psourceL("ps_left_side", 10e5);
    ComponentOrifice orificeL("orifice_left_side", 1e-12);
    ComponentVolume volumeC("volume_center");
    //ComponentTLMlossless volumeC("volume_center");
    ComponentOrifice orificeR("orifice_right_side", 1e-12);
    ComponentPressureSource psourceR("ps_right_side", 0e5);

    //Add components
    simulationmodel.addComponent(psourceL);
    simulationmodel.addComponent(orificeL);
    simulationmodel.addComponent(volumeC);
    simulationmodel.addComponent(orificeR);
    simulationmodel.addComponent(psourceR);
    //Connect components
    simulationmodel.connect(psourceL, "P1", orificeL, "P1");
    simulationmodel.connect(orificeL, "P2", volumeC, "P1");
    simulationmodel.connect(volumeC, "P2", orificeR, "P1");
    simulationmodel.connect(orificeR, "P2", psourceR, "P1");

    //Run simulation
    TicToc prealloctimer("prealloctimer");
    simulationmodel.preAllocateLogSpace(0, 10);
    prealloctimer.TocPrint();

    TicToc simutimer("simutimer");
    simulationmodel.simulate(0,10);
    simutimer.TocPrint();

    totaltimer.TocPrint();

    //Test write to file
    TicToc filewritetimer("filewritetimer");
    volumeC.getPort("P1").getNode().saveLogData("output.txt");
    filewritetimer.TocPrint();
    cout << "test1() Done!" << endl;
}


void test2() //Test of the Delay utillity class
{
	Delay d1(.15, .1); //delay .15 with sampletime .1
	for (int i=0; i < 11; ++i) {
		cout << "Value: " << i << "    Delayed value: " << d1.value() << endl;
		d1.update(i);
	}
}


void testTLM()
{
    HopsanEssentials Hopsan;
	/*   Exempelsystem:

	   q       T, Zc
	 ------>o=========o p

	 */
	ComponentSystem simulationmodel("simulationmodel");
    //Create other components
    ComponentFlowSourceQ qsourceL(    "qs_left_side",  1.0);
    ComponentTLMlossless lineC(       "line_center",   3.0, 0.2);
    ComponentPressureSourceQ psourceR("ps_right_side", 1.0);

    //Add components
    simulationmodel.addComponent(qsourceL);
    simulationmodel.addComponent(lineC);
    simulationmodel.addComponent(psourceR);

    //List and set parameters
    lineC.listParametersConsole();
    lineC.setParameter("Zc",  3.0);
    lineC.setParameter("TD", 0.1);
    lineC.listParametersConsole();

    //Connect components
    simulationmodel.connect(qsourceL, "P1", lineC, "P1");
    simulationmodel.connect(lineC, "P2", psourceR, "P1");

    //Run simulation
    simulationmodel.preAllocateLogSpace(0, 1.0);

    simulationmodel.simulate(0.0, 1.0);

    //Test write to file
    lineC.getPort("P1").getNode().saveLogData("output.txt");

	//Finished
    cout << "testTLM Done!" << endl;

}


void testTLMlumped()
{
    HopsanEssentials Hopsan;
	/*   Exempelsystem:

	   q         T, Zc
             v  v  v  v  v
	 ------>o=============o p
             ^  ^  ^  ^  ^
            R/8 R/4 ... R/8

	 */
    double R  = 1.0;
    double Zc = 3.0;
    double T  = 0.1;

	ComponentSystem simulationmodel("simulationmodel");
    //Create other components
    ComponentFlowSourceQ qsourceL("qs_left_side",  1.0);
    ComponentTLMRlineR lineL("line_left",     Zc, T/4.0, R/8.0, 0.0);
    ComponentOrifice orificeL("orifice_L", 4.0*R);
    ComponentTLMRlineR lineLC("line_lcenter", Zc, T/4.0, 0.0, 0.0);
    ComponentOrifice orificeC("orifice_C", 4.0*R);
    ComponentTLMRlineR lineRC("line_rcenter", Zc, T/4.0, 0.0, 0.0);
    ComponentOrifice orificeR("orifice_R", 4.0*R);
    ComponentTLMRlineR lineR("line_right",    Zc, T/4.0, 0.0, R/8.0);
    ComponentPressureSourceQ psourceR("ps_right_side", 1.0);

    //Add components
    simulationmodel.addComponent(qsourceL);
    simulationmodel.addComponent(lineL);
    simulationmodel.addComponent(orificeL);
    simulationmodel.addComponent(lineLC);
    simulationmodel.addComponent(orificeC);
    simulationmodel.addComponent(lineRC);
    simulationmodel.addComponent(orificeR);
    simulationmodel.addComponent(lineR);
    simulationmodel.addComponent(psourceR);

    //List and set parameters
    lineLC.listParametersConsole();

    //Connect components
    simulationmodel.connect(qsourceL, "P1", lineL, "P1");
    simulationmodel.connect(lineL, "P2", orificeL, "P1");
    simulationmodel.connect(orificeL, "P2", lineLC, "P1");
    simulationmodel.connect(lineLC, "P2", orificeC, "P1");
    simulationmodel.connect(orificeC, "P2", lineRC, "P1");
    simulationmodel.connect(lineRC, "P2", orificeR, "P1");
    simulationmodel.connect(orificeR, "P2", lineR, "P1");
    simulationmodel.connect(lineR, "P2", psourceR, "P1");

    //Run simulation
    simulationmodel.preAllocateLogSpace(0.0, 2.0);

    simulationmodel.simulate(0.0, 2.0);

    //Test write to file
    lineL.getPort("P1").getNode().saveLogData("output.txt");

	//Finished
    cout << "testTLMlumped() Done!" << endl;

}


void test3()
{
    HopsanEssentials Hopsan;
	/*   Exempelsystem:
					  Kc
	   q       T, Zc  v
	 ------>o=========----o p
	                  ^
    */
	ComponentSystem simulationmodel("simulationmodel");
    //Create other components
    ComponentFlowSourceQ qsourceL(   "qs_left_side",       1.0);
    ComponentTLMlossless lineC(      "line_center",        1.0, 0.1, 0.0);
    ComponentOrifice orificeR(       "orifice_right_side", 3.0);
    ComponentPressureSource psourceR("ps_right_side",      1.0);

    //Add components
    simulationmodel.addComponent(qsourceL);
    simulationmodel.addComponent(lineC);
    simulationmodel.addComponent(orificeR);
    simulationmodel.addComponent(psourceR);

    //Connect components
    simulationmodel.connect(qsourceL, "P1", lineC,    "P1");
    simulationmodel.connect(lineC,    "P2", orificeR, "P1");
    simulationmodel.connect(orificeR, "P2", psourceR, "P1");

    //List and set parameters
    qsourceL.listParametersConsole();
    lineC.listParametersConsole();
    orificeR.listParametersConsole();
    psourceR.listParametersConsole();
    lineC.setParameter("Zc", 1.0);
    lineC.setParameter("TD", 0.005);
    qsourceL.listParametersConsole();
    lineC.listParametersConsole();
    orificeR.listParametersConsole();
    psourceR.listParametersConsole();

    //Run simulation
    simulationmodel.preAllocateLogSpace(0.0, 1.0);

    simulationmodel.simulate(0.0, 1.0);

    //Test write to file
    lineC.getPort("P1").getNode().saveLogData("output.txt");
    lineC.getPort("P2").getNode().saveLogData("output2.txt");

	//Finished
    cout << "test3() Done!" << endl;
}


void test_external_lib()
{
    HopsanEssentials Hopsan;

    //Create master component
    ComponentSystem simulationmodel("simulationmodel");
    //Create other components
    //ComponentPressureSource psourceL("ps_left_side", 10e5);
    //ComponentOrifice orificeL("orifice_left_side", 1e-12);

    #ifdef WIN32
    Hopsan.externalLoader.load("./libHydraulic.dll");
    #elif defined MAC
    Hopsan.externalLoader.load("/Users/bjoer37/svn/HOPSAN++/bin/Debug/libHydraulic.dylib");
    #else
    Hopsan.externalLoader.load("./bin/Debug/libHydraulic.so");
    #endif

    cout << "afterload" << endl;

    Component* psourceL = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalPressureSource");
    Component* orificeL = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalOrifice");
    Component* volumeC = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalVolume");
    Component* orificeR = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalOrifice");
    Component* psourceR = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalPressureSource");
    psourceL->setParameter("P", 10e5);
    orificeR->setName("right orifice");
    orificeR->setParameter("Kc", 1e-12);
    psourceR->setParameter("P", 0e5);


    //ComponentOrifice orificeR("orifice_right_side", 1e-12);
    //ComponentPressureSource psourceR("ps_right_side", 0e5);

    //Add components
    simulationmodel.addComponent(*psourceL);
    simulationmodel.addComponent(*orificeL);
    simulationmodel.addComponent(*volumeC);
    simulationmodel.addComponent(*orificeR);
    simulationmodel.addComponent(*psourceR);
    //Connect components
    simulationmodel.connect(*psourceL, "P1", *orificeL, "P1");
    simulationmodel.connect(*orificeL, "P2", *volumeC, "P1");
    simulationmodel.connect(*volumeC, "P2", *orificeR, "P1");
    simulationmodel.connect(*orificeR, "P2", *psourceR, "P1");

    //list some stuff
    orificeR->listParametersConsole();

    //Run simulation
    simulationmodel.preAllocateLogSpace(0, 100);
    simulationmodel.simulate(0,100);

    //Test write to file
    volumeC->getPort("P1").getNode().saveLogData("output.txt");

    cout << "test_external_lib() Done!" << endl;

}


void test_fixed_pump()
{
    HopsanEssentials Hopsan;

    //Create master component
    ComponentSystem simulationmodel("simulationmodel");

    //Create other components

    #ifdef WIN32
    Hopsan.externalLoader.load("./libHydraulic.dll");
    #elif defined MAC
    Hopsan.externalLoader.load("/Users/bjoer37/svn/HOPSAN++/bin/Debug/libHydraulic.dylib");
    #else
    Hopsan.externalLoader.load("./bin/Debug/libHydraulic.so");
    #endif

    cout << "afterload" << endl;

    Component* psourceL = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalPressureSource");
    Component* pump = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalFixedDisplacementPump");
    Component* volumeC = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalVolume");
    Component* psourceR = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalPressureSourceQ");

    psourceL->setParameter("P", 10e5);
    //pump->setParameter("Kcp", 1e-7);
    psourceR->setParameter("P", 10e5);

    //Add components
    simulationmodel.addComponent(*psourceL);
    simulationmodel.addComponent(*pump);
    simulationmodel.addComponent(*volumeC);
    simulationmodel.addComponent(*psourceR);

    //Connect components
    simulationmodel.connect(*psourceL, "P1", *pump, "P1");
    simulationmodel.connect(*pump, "P2", *volumeC, "P1");
    simulationmodel.connect(*volumeC, "P2", *psourceR, "P1");

    //Run simulation
    simulationmodel.preAllocateLogSpace(0, 100);
    simulationmodel.simulate(0,100);

    //Test write to file
    pump->getPort("P2").getNode().saveLogData("output.txt");

    cout << "test_fixed_pump() Done!" << endl;

}


void testSignal()
{
	/*   Exempelsystem:

	 2   |\ 3
	 o===| >===o
	     |/
    */

    HopsanEssentials Hopsan;

    #ifdef WIN32
    Hopsan.externalLoader.load("./libHydraulic.dll");
    #elif defined MAC
    Hopsan.externalLoader.load("/Users/bjoer37/svn/HOPSAN++/bin/Debug/libHydraulic.dylib");
    #else
    Hopsan.externalLoader.load("./bin/Debug/libHydraulic.so");
    #endif

    cout << "afterload" << endl;

	ComponentSystem simulationmodel("simulationmodel");
    //Create other components
    ComponentSource sourceL("source_left", 1.0);
    ComponentGain gainC("gain_center", 1.0);
    ComponentSink sinkR("sink_right");

    //Add components
    simulationmodel.addComponent(sourceL);
    simulationmodel.addComponent(gainC);
    simulationmodel.addComponent(sinkR);

    //Connect components
    simulationmodel.connect(sourceL, "out", gainC, "in");
    simulationmodel.connect(gainC, "out", sinkR, "in");

    //List and set parameters
    sourceL.listParametersConsole();
    gainC.listParametersConsole();
    sourceL.setParameter("Value", 2.0);
    gainC.setParameter("Gain", 3.0);
    sourceL.listParametersConsole();
    gainC.listParametersConsole();

    //Run simulation
    simulationmodel.preAllocateLogSpace(0.0, 1.0);

    simulationmodel.simulate(0.0, 1.0);

    //Test write to file
    sinkR.getPort("in").getNode().saveLogData("output.txt");

	//Finished
    cout << "testSignal() Done!" << endl;
}


void testExternalSignal()
{
	/*   Exempelsystem:

	 2   |\ 3
	 o===| >===o
	     |/
    */

    HopsanEssentials Hopsan;

    #ifdef WIN32
    Hopsan.externalLoader.load("./libHydraulic.dll");
    #elif defined MAC
    Hopsan.externalLoader.load("/Users/bjoer37/svn/HOPSAN++/bin/Debug/libHydraulic.dylib");
    #else
    Hopsan.externalLoader.load("./bin/Debug/libHydraulic.so");
    #endif

    cout << "afterload" << endl;

	ComponentSystem simulationmodel("simulationmodel");
    //Create other components
    Component* sourceL = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalSource");
    Component* gainC = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalGain");
    Component* sinkR = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalSink");

    //Add components
    simulationmodel.addComponent(*sourceL);
    simulationmodel.addComponent(*gainC);
    simulationmodel.addComponent(*sinkR);

    //Connect components
    simulationmodel.connect(*sourceL, "out", *gainC, "in");
    simulationmodel.connect(*gainC, "out", *sinkR, "in");

    //List and set parameters
    sourceL->listParametersConsole();
    gainC->listParametersConsole();
    sourceL->setParameter("Value", 2.0);
    gainC->setParameter("Gain", 3.0);
    sourceL->listParametersConsole();
    gainC->listParametersConsole();

    //Run simulation
    simulationmodel.preAllocateLogSpace(0.0, 1.0);

    simulationmodel.simulate(0.0, 1.0);

    //Test write to file
    sinkR->getPort("in").getNode().saveLogData("output.txt");

	//Finished
    cout << "testSignal() Done!" << endl;
}


void testkarl()
{
    HopsanEssentials Hopsan;

	ComponentSystem simulationmodel("simulationmodel");

#ifdef WIN32
    Hopsan.externalLoader.load("./libHydraulic.dll");
    #elif defined MAC
    Hopsan.externalLoader.load("/Users/bjoer37/svn/HOPSAN++/bin/Debug/libHydraulic.dylib");
    #else
    Hopsan.externalLoader.load("./bin/Debug/libHydraulic.so");
    #endif

//    //Create other components
//    ComponentPressureSource psourceL(   "ps_left_side");
//    ComponentTurbOrifice orificeL(       "orifice_left_side");
//    ComponentVolume volume("volume_left");
//    ComponentTurbOrifice orificeR(       "orifice_right_side");
//    ComponentPressureSource psourceR("ps_right_side");
//
//    //Add components
//    simulationmodel.addComponent(psourceL);
//    simulationmodel.addComponent(orificeL);
//    simulationmodel.addComponent(volume);
//    simulationmodel.addComponent(orificeR);
//    simulationmodel.addComponent(psourceR);
//
//    //Connect components
//    simulationmodel.connect(psourceL, "P1", orificeL,    "P1");
//    simulationmodel.connect(orificeL,    "P2", volume, "P1");
//    simulationmodel.connect(volume, "P2", orificeR, "P1");
//    simulationmodel.connect(orificeR, "P2", psourceR, "P1");
//
//    //List and set parameters
//    psourceL.listParametersConsole();
//    orificeL.listParametersConsole();
//    volume.listParametersConsole();
//    orificeR.listParametersConsole();
//    psourceR.listParametersConsole();
//
//    psourceL.setParameter("P", 1.0e5);
//    psourceR.setParameter("P", 10.0e5);
//    orificeR.setParameter("A",0.00001);
//    volume.setParameter("V", 1.0e-1);
//    psourceL.listParametersConsole();
//    psourceR.listParametersConsole();

    //Create other components
    Component* psource = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalPressureSource");
    Component* tankT = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalPressureSource");
    Component* valve = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternal43Valve");
    Component* volumeA = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalVolume");
    Component* volumeB = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalVolume");
    Component* orificeA = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalOrifice");
    Component* orificeB = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalOrifice");
    Component* tankA = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalPressureSource");
    Component* tankB = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalPressureSource");
    ComponentSource constant("const");

    //Add components
    simulationmodel.addComponent(*psource);
    simulationmodel.addComponent(*tankT);
    simulationmodel.addComponent(*valve);
    simulationmodel.addComponent(*volumeA);
    simulationmodel.addComponent(*volumeB);
    simulationmodel.addComponent(*orificeA);
    simulationmodel.addComponent(*orificeB);
    simulationmodel.addComponent(*tankA);
    simulationmodel.addComponent(*tankB);
    simulationmodel.addComponent(constant);

    //Connect components
    simulationmodel.connect(*psource, "P1", *valve,    "PP");
    simulationmodel.connect(*tankT, "P1", *valve,    "PT");
    simulationmodel.connect(*volumeA, "P1", *valve,    "PA");
    simulationmodel.connect(*volumeB, "P1", *valve,    "PB");
    simulationmodel.connect(constant, "out", *valve,    "PX");
    simulationmodel.connect(*volumeA,    "P2", *orificeA, "P1");
    simulationmodel.connect(*volumeB,    "P2", *orificeB, "P1");
    simulationmodel.connect(*orificeA,    "P2", *tankA, "P1");
    simulationmodel.connect(*orificeB,    "P2", *tankB, "P1");

    //List and set parameters
    psource->setParameter("P", 10.0e5);
    tankT->setParameter("P", 1.0e5);
    tankA->setParameter("P", 1.0e5);
    tankB->setParameter("P", 1.0e5);
    volumeA->setParameter("V", 1.0e-1);
    volumeB->setParameter("V", 1.0e-1);
    valve->setParameter("overlap_pa", -0.0001);
    constant.setParameter("Value", 0.0);

    //Run simulation
    simulationmodel.preAllocateLogSpace(0.0, 20.0);

    simulationmodel.simulate(0.0, 20.0);

    //Test write to file
    volumeA->getPort("P1").getNode().saveLogData("output.txt");
    volumeA->getPort("P2").getNode().saveLogData("output2.txt");

	//Finished
    cout << "testkarl() Done!" << endl;
}


void testExternalSignalStep()
{
	/*   Exempelsystem:

	 2   |\ 3
	 o===| >===o
	     |/
    */

    HopsanEssentials Hopsan;

    #ifdef WIN32
    Hopsan.externalLoader.load("./libHydraulic.dll");
    #elif defined MAC
    Hopsan.externalLoader.load("/Users/bjoer37/svn/HOPSAN++/bin/Debug/libHydraulic.dylib");
    #else
    Hopsan.externalLoader.load("./bin/Debug/libHydraulic.so");
    #endif

    cout << "afterload" << endl;

	ComponentSystem simulationmodel("simulationmodel");
    //Create other components
    Component* stepL = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalStep");
    Component* gainC = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalGain");
    Component* sinkR = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalSink");

    //Add components
    simulationmodel.addComponent(*stepL);
    simulationmodel.addComponent(*gainC);
    simulationmodel.addComponent(*sinkR);

    //Connect components
    simulationmodel.connect(*stepL, "out", *gainC, "in");
    simulationmodel.connect(*gainC, "out", *sinkR, "in");

    //List and set parameters
    stepL->listParametersConsole();
    gainC->listParametersConsole();
    //sourceL->setParameter("Value", 2.0);
    gainC->setParameter("Gain", 3.0);
    gainC->listParametersConsole();

    //Run simulation
    simulationmodel.preAllocateLogSpace(0.0, 2.0);

    simulationmodel.simulate(0.0, 2.0);

    //Test write to file
    sinkR->getPort("in").getNode().saveLogData("output.txt");

	//Finished
    cout << "testExternalSignalStep() Done!" << endl;
}


void testExternalSineWave()
{
	/*   Exempelsystem:

	 2   |\ 3
	 o===| >===o
	     |/
    */

    HopsanEssentials Hopsan;

    #ifdef WIN32
    Hopsan.externalLoader.load("./libHydraulic.dll");
    #elif defined MAC
    Hopsan.externalLoader.load("/Users/bjoer37/svn/HOPSAN++/bin/Debug/libHydraulic.dylib");
    #else
    Hopsan.externalLoader.load("./bin/Debug/libHydraulic.so");
    #endif

    cout << "afterload" << endl;

	ComponentSystem simulationmodel("simulationmodel");
    //Create other components
    Component* sineL = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalSineWave");
    Component* gainC = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalGain");
    Component* sinkR = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalSink");

    //Add components
    simulationmodel.addComponent(*sineL);
    simulationmodel.addComponent(*gainC);
    simulationmodel.addComponent(*sinkR);

    //Connect components
    simulationmodel.connect(*sineL, "out", *gainC, "in");
    simulationmodel.connect(*gainC, "out", *sinkR, "in");

    //List and set parameters
    sineL->listParametersConsole();
    gainC->listParametersConsole();
    sineL->setParameter("StartTime", 1.0);
    sineL->setParameter("Frequency", 2.0);
    sineL->setParameter("Amplitude", 5);
    //sineL->setParameter("Offset", 0.5);
    //gainC->setParameter("Gain", 3.0);
    gainC->listParametersConsole();

    //Run simulation
    simulationmodel.preAllocateLogSpace(0.0, 10.0);

    simulationmodel.simulate(0.0, 10.0);

    //Test write to file
    sinkR->getPort("in").getNode().saveLogData("output.txt");

	//Finished
    cout << "testExternalSineWave() Done!" << endl;
}


void testExternalSquareWave()
{
	/*   Exempelsystem:

	 2   |\ 3
	 o===| >===o
	     |/
    */

    HopsanEssentials Hopsan;

    #ifdef WIN32
    Hopsan.externalLoader.load("./libHydraulic.dll");
    #elif defined MAC
    Hopsan.externalLoader.load("/Users/bjoer37/svn/HOPSAN++/bin/Debug/libHydraulic.dylib");
    #else
    Hopsan.externalLoader.load("./bin/Debug/libHydraulic.so");
    #endif

    cout << "afterload" << endl;

	ComponentSystem simulationmodel("simulationmodel");
    //Create other components
    Component* squareL = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalSquareWave");
    Component* gainC = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalGain");
    Component* sinkR = Hopsan.getComponentFactoryPtr()->CreateInstance("ComponentExternalSink");

    //Add components
    simulationmodel.addComponent(*squareL);
    simulationmodel.addComponent(*gainC);
    simulationmodel.addComponent(*sinkR);

    //Connect components
    simulationmodel.connect(*squareL, "out", *gainC, "in");
    simulationmodel.connect(*gainC, "out", *sinkR, "in");

    //List and set parameters
    squareL->listParametersConsole();
    gainC->listParametersConsole();
    squareL->setParameter("StartTime", 1.0);
    squareL->setParameter("Frequency", 2.0);
    squareL->setParameter("Amplitude", 5);
    squareL->setParameter("BaseValue", 2);
    squareL->listParametersConsole();
    //gainC->setParameter("Gain", 3.0);
    gainC->listParametersConsole();

    //Run simulation
    simulationmodel.preAllocateLogSpace(0.0, 10.0);

    simulationmodel.simulate(0.0, 10.0);

    //Test write to file
    sinkR->getPort("in").getNode().saveLogData("output.txt");

	//Finished
    cout << "testExternalSquareWave() Done!" << endl;
}


int main()
{

    //test2();

    //cfact_ptr->RegisterCreatorFunction("ComponentExternalSink", ComponentExternalSink::Creator);
    //test_external_lib();


    //testExternalSquareWave();


    testkarl();


    //test1();


    //test_fixed_pump();


    // testSignal();
    return 0;
}
