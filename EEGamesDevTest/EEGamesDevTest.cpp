#include <iostream>
#include "Net.h"
#include <conio.h>

int main()
{
    struct EventProbabilities Probabilities{ 25,1,50,1 };
    Net net(Probabilities);
    net.FillRandomNodes(20, 100);
    net.StartSimulation();
    while (true)
    {
        if (_kbhit())
        {
            net.StopSimulation();
        }
    }
}
