#include <iostream>
#include "Net.h"
#include <thread>

int main()
{
    EventProbabilities Probabilities{ 10,1,50,1 };
    Net net(Probabilities);
    net.FillRandomNodes(20);

    std::thread thread(&Net::StartSimulation, &net);
    thread.detach();
    std::string InputCommand;
    do
    {
        std::cin >> InputCommand;

    } while (InputCommand != "stop");
    net.StopSimulation();

    //net.StartSimulation();
}
