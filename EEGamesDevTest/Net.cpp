#include "Net.h"
#include <random>
#include <thread>
#include <iostream>

Net::Net(EventProbabilities Probabilities) : Probabilities(Probabilities) {}

void Net::StartSimulation(long long TickDuration, int UpdateRate)
{
	std::thread thread(&Net::StartSimulationThread, this, TickDuration, UpdateRate);
	thread.detach();
}

int Net::Update()
{
	if(Nodes.size()>0)
	{
		int NodesErased = 0;
		for (int i = 0; i < Nodes.size(); ++i)
		{
			if ((Nodes[i]->GetNeighbours().size() <= 0) && (ExceptionList.find(Nodes[i]->GetID()) != ExceptionList.end()))
			{
				Nodes.erase(Nodes.begin() + i);
				NodesErased++;
			}
		}
		if (Nodes.size() <= 0) StopSimulation();
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

void Net::StartSimulationThread(long long TickDuration, int UpdateRate)
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
			if (Action <= Probabilities.pEvent)
			{
				int EventValue = Nodes[i]->MakeEvent();
				std::cout << Nodes[i]->GetName() << " Made an Event with value: " << EventValue << std::endl;
			}
			else if (Action <= (Probabilities.pNewNode + Probabilities.pEvent))
			{
				auto NewNode = Nodes[i]->CreateNewNode(GetRandom(0, 1000));
				Nodes.push_back(NewNode);
				ExceptionList.insert(std::make_pair(NewNode->GetID(), NewNode));
				std::cout << Nodes[i]->GetName() << " Created new node\n";
			}
			else if (Action <= (Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent))
			{
				auto Neighbours1 = Nodes[i]->GetNeighbours();
				if (Neighbours1.size() > 0)
				{
					auto Neighbour1 = Neighbours1.begin();
					std::advance(Neighbour1, GetRandom(0, Neighbours1.size()-1));
					auto Neighbours2 = Neighbour1->second->GetNeighbours();
					if (Neighbours2.size() > 0)
					{
						auto Neighbour2 = Neighbours2.begin();
						std::advance(Neighbour2, GetRandom(0, Neighbours2.size()-1));
						Nodes[i]->SubscribeTo(Neighbour2->second);
						std::cout << Nodes[i]->GetName() << " Subscribed to " << Neighbour2->second->GetName() << std::endl;
					}
					else std::cout << Nodes[i]->GetName() << " Failed to subscribe\n";
				}
				else std::cout << Nodes[i]->GetName() << " Failed to subscribe\n";
			}
			else if (Action <= (Probabilities.pUnsubscription + Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent))
			{
				auto Subscriptions = Nodes[i]->GetSubscriptions();
				if (Subscriptions.size() > 0)
				{
					auto Subscription = Subscriptions.begin();
					std::advance(Subscription, GetRandom(0, Subscriptions.size() - 1));
					Nodes[i]->UnsubscribeFrom(Subscription->second);
					std::cout << Nodes[i]->GetName() << " Unsubscribed from " << Subscription->second->GetName() << std::endl;
				}
				else std::cout << Nodes[i]->GetName() << " Failed to unsubscribe\n";
			}
			else
			{
				std::cout << Nodes[i]->GetName() << " Did nothing\n";
			}
		}
		std::cout << std::endl;
		Iteration++;
		if (Iteration % UpdateRate == 0)
		{
			int NodesErased = 0;
			size_t NodesSize = Nodes.size();
			NodesErased = Update();
			std::cout << "Nodes erased: " << NodesErased << "/" << NodesSize << std::endl;
		}
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