#include "Components.h"

//Constructor
Component::Component(string name, double timestep)
{
    mName = name;
    //this->mName = name;
    mTimestep = timestep;
}

void Component::simulate(const double startT, const double Ts)
{
//TODO: adjust self.timestep or simulation depending on Ts from system above (self.timestep should be multipla of Ts)
    double stopT = startT+Ts;
    double time = startT;
    while (time < stopT)
    {
        simulateOneTimestep();
        time += mTimestep;
    }
}

void Component::setName(string &rName)
{
    mName = rName;
}

string &Component::getName()
{
    return mName;
}

void Component::setTimestep(const double timestep)
{
    mTimestep = timestep;
}

double Component::getTimestep()
{
    return mTimestep;
}

//constructor ComponentC
ComponentC::ComponentC(string name, double timestep) : Component(name, timestep)
{

}

//Constructor ComponentQ
ComponentQ::ComponentQ(string name, double timestep) : Component(name, timestep)
{

}
