#include <iostream>
#include "Net.h"
#include <conio.h>

int main()
{
    struct EventProbabilities Probabilities { 25, 10, 50, 10 };
    Net* net = new Net(100, 200, Probabilities);
    char InputCommand;
    bool exit = false;
    bool run = true;
    std::cout << "\ns - Start simulation auto (any key to pause)\nn - Perform iteration\nr - Reset\ne - Exit\n";
    while (!exit)
    {
        std::cout << "\nInput: ";
        std::cin >> InputCommand;
        switch (InputCommand)
        {
        case 's':
            net->StartSimulation();
            while (run)
            {
                if (_kbhit())
                {
                    net->StopSimulation();
                    run = false;
                }
            }
            break;
        case 'n':
            net->PerformIteration();
            break;
        case 'r':
            net->Reset();
            break;
        case 'e':
            exit = true;
            break;
        default:
            break;
        }
    }
    delete net;
}
