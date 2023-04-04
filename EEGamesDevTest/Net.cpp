#include "Net.h"
#include <random>
#include <thread>
#include <iostream>
#include <algorithm>

#define DEBUG

#ifdef DEBUG
#define debug(str) std::cout << str;
#else
#define debug(str)
#endif

Net::Net(EventProbabilities Probabilities) : Probabilities(Probabilities) {}

void Net::StartSimulation(long long TickDuration, int UpdatePeriod)
{
	std::thread thread(&Net::StartSimulationThread, this, TickDuration, UpdatePeriod);
	thread.detach();
}

int Net::Update()
{
	if(Nodes.size()>0)
	{
		int NodesErased = 0;
		for (int i = 0; i < Nodes.size(); ++i)
		{
			if ((Nodes[i]->GetNeighbours().size() <= 0) && (std::find(ExceptionList.begin(), ExceptionList.end(), Nodes[i]->GetID()) != ExceptionList.end()))
			{
				delete Nodes[i];
				Nodes.erase(Nodes.begin() + i);
				NodesErased++;
			}
		}
		if (Nodes.size() <= 0) StopSimulation();
		ExceptionList.clear();
		return NodesErased;
	}
	else
	{
		StopSimulation();
		return 0;
	}
}

void Net::StopSimulation()
{
	SimulationRunning = false;
	std::cout << "Simulation Stopped\n";
}

void Net::FillRandomNodes(int AmountOfNodes, int AmountOfSubscriptions)
{
	for (size_t i = 0; i < AmountOfNodes; i++)
	{
		Nodes.push_back(new Node(GetRandom(0, 1000)));
	}

	for (size_t i = 0; i < AmountOfSubscriptions; i++)
	{
		Nodes[GetRandom(0, Nodes.size() - 1)]->SubscribeTo(Nodes[GetRandom(0, Nodes.size() - 1)]);
	}
	Update();
}

void Net::StartSimulationThread(long long TickDuration, int UpdatePeriod)
{
	SimulationRunning = true;
	int Iteration = 1;
	//std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while (SimulationRunning)
	{
		std::cout << "Iteration: " << Iteration << std::endl;
		for (int i = 0; i < Nodes.size(); ++i)
		{
			int Action = GetRandom(0, 100);
			// Make an event
			if (Action <= Probabilities.pEvent)
			{
				int EventValue = Nodes[i]->MakeEvent();
				debug(Nodes[i]->GetName() << " Made an Event with value: " << EventValue << std::endl);
			}
			// Make a new node
			else if (Action <= (Probabilities.pNewNode + Probabilities.pEvent))
			{
				auto NewNode = Nodes[i]->CreateNewNode(GetRandom(0, 1000));
				Nodes.push_back(NewNode);
				ExceptionList.push_back(NewNode->GetID());
				debug (Nodes[i]->GetName() << " Created new node\n");
			}
			// Subscription
			else if (Action <= (Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent))
			{
				auto Neighbours = Nodes[i]->GetNeighbours();
				if (Neighbours.size() > 0)
				{
					auto Neighbour = Neighbours.begin();
					std::advance(Neighbour, GetRandom(0, Neighbours.size()-1));
					Neighbours = Neighbour->second->GetNeighbours();
					if (Neighbours.size() > 0)
					{
						Neighbour = Neighbours.begin();
						std::advance(Neighbour, GetRandom(0, Neighbours.size()-1));
						Nodes[i]->SubscribeTo(Neighbour->second);
						debug (Nodes[i]->GetName() << " Subscribed to " << Neighbour->second->GetName() << std::endl);
					}
					else debug (Nodes[i]->GetName() << " Failed to subscribe\n");
				}
				else debug (Nodes[i]->GetName() << " Failed to subscribe\n");
			}
			// Unsubscription
			else if (Action <= (Probabilities.pUnsubscription + Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent))
			{
				auto Subscriptions = Nodes[i]->GetSubscriptions();
				if (Subscriptions.size() > 0)
				{
					auto Subscription = Subscriptions.begin();
					std::advance(Subscription, GetRandom(0, Subscriptions.size() - 1));
					Nodes[i]->UnsubscribeFrom(Subscription->second);
					debug (Nodes[i]->GetName() << " Unsubscribed from " << Subscription->second->GetName() << std::endl);
				}
				else debug (Nodes[i]->GetName() << " Failed to unsubscribe\n");
			}
			// None of the above
			else
			{
				debug (Nodes[i]->GetName() << " Did nothing\n");
			}
		}
		// Calls Update each UpdatePeriod
		if (Iteration % UpdatePeriod == 0)
		{
			int NodesErased = 0;
			size_t NodesSize = Nodes.size();
			NodesErased = Update();
			std::cout << "Nodes erased: " << NodesErased << "/" << NodesSize << std::endl;
		}
		Iteration++;
		std::cout << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(TickDuration));
	}
}

int Net::GetRandom(int Min, int Max)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distribution(Min, Max);
	return distribution(rng);
}