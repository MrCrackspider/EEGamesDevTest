#include <iostream>
#include "Net.h"

int main()
{
    struct EventProbabilities Probabilities{ 25,1,50,1 };
    Net net(Probabilities);
    net.FillRandomNodes(20, 100);
    net.StartSimulation(1000, 10);
    std::string InputCommand;
    do
    {
        std::cin >> InputCommand;

    } while (InputCommand != "stop");
    net.StopSimulation();
}
