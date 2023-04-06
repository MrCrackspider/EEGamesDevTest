#include <iostream>
#include "Net.h"
#include <conio.h>

int main()
{
    struct EventProbabilities Probabilities{ 25,10,50,10 };
    Net net(100, 400, Probabilities);
    char InputCommand;
    bool exit = false;
    bool run = true;
    std::cout << "s - start simulation auto\nn - perform iteration\nr - reset\ne - exit\n";
    while (!exit)
    {
        std::cout << "\nInput: ";
        std::cin >> InputCommand;
        switch (InputCommand)
        {
        case 's':
            net.StartSimulation();
            break;
        case 'n':
            net.PerformIteration();
            break;
        case 'r':
            net.Reset();
            break;
        case 'e':
            exit = true;
            break;
        default:
            break;
        }
    }
}
