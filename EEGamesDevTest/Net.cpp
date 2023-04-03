#include "Net.h"
#include <random>
#include <thread>
#include <iostream>

Net::Net(EventProbabilities Probabilities) : Probabilities(Probabilities) {}

void Net::StartSimulation(long long TickDuration)
{
	std::thread thread(&Net::StartSimulationThread, this, TickDuration);
	thread.detach();
}

void Net::Update()
{
	if(Nodes.size()>0)
	{
		for (int i = 0; i < Nodes.size(); ++i)
		{
			if (Nodes[i]->GetNeighbours().size() <= 0)
				Nodes.erase(Nodes.begin() + i);
		}
		if (Nodes.size() <= 0) StopSimulation();
	}
	else
	{
		StopSimulation();
	}
}

void Net::StopSimulation()
{
	SimulationRunning = false;
	std::cout << "Simulation Stopped\n";
}

void Net::FillRandomNodes(int Amount)
{
	//std::random_device dev;
	//std::mt19937 rng(dev());
	//std::uniform_int_distribution<std::mt19937::result_type> distribution(0, Amount-1);

	for (size_t i = 0; i < Amount; i++)
	{
		//Nodes.push_back(new Node(distribution(rng)));
		Nodes.push_back(new Node(GetRandom(0, 1000)));
	}

	for (size_t i = 0; i < Amount; i++)
	{
		std::cout << "ID: " << Nodes[i]->GetID() << " Name: " << Nodes[i]->GetName() << std::endl;
	}
}

void Net::StartSimulationThread(long long TickDuration)
{
	SimulationRunning = true;
	int cnt = 0;
	while (SimulationRunning)
	{
		for (int i = 0; i < Nodes.size(); ++i)
		{
			int Action = GetRandom(0, 100);
			if (Action <= Probabilities.pEvent)
			{
				Nodes[i]->MakeEvent();
				std::cout << Nodes[i]->GetName() << " Made an Event\n";
			}
			else if (Action <= (Probabilities.pNewNode + Probabilities.pEvent))
			{
				//std::random_device dev;
				//std::mt19937 rng(dev());
				//std::uniform_int_distribution<std::mt19937::result_type> IDdistribution(0, 1000);
				//auto NewNode = Nodes[i]->CreateNewNode(IDdistribution(rng));
				auto NewNode = Nodes[i]->CreateNewNode(GetRandom(0, 1000));
				Nodes.push_back(NewNode);
				std::cout << Nodes[i]->GetName() << " Created new node\n";
			}
			else if (Action <= (Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent))
			{
				//auto Neighbours = Nodes[i]->GetNeighbours();
				//auto Neighbour = Neighbours.begin();
				//if (Neighbours.size() > 0)
				//{
				//	std::advance(Neighbour, rand() % Neighbours.size());
				//	auto NeighboursOfNeighbours = Neighbour->second->GetNeighbours();
				//	auto NeighbourOfNeighbour = NeighboursOfNeighbours.begin();
				//	if (NeighboursOfNeighbours.size() > 0)
				//	{
				//		std::advance(NeighbourOfNeighbour, rand() % NeighboursOfNeighbours.size());
				//		Nodes[i]->SubscribeTo(NeighbourOfNeighbour->second);
				//		std::cout << Nodes[i]->GetName() << " Subscribed to " << NeighbourOfNeighbour->second->GetName() << std::endl;
				//	}
				//}

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
				}
			}
			else if (Action <= (Probabilities.pUnsubscription + Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent))
			{
				auto Subscriptions = Nodes[i]->GetSubscriptions();
				if (Subscriptions.size() > 0)
				{
					auto Subscription = Subscriptions.begin();
					//srand((unsigned)time(NULL));
					//std::advance(Subscription, rand() % Subscriptions.size());
					std::advance(Subscription, GetRandom(0, Subscriptions.size() - 1));
					Nodes[i]->UnsubscribeFrom(Subscription->second);
					std::cout << Nodes[i]->GetName() << " Unsubscribed from " << Subscription->second->GetName() << std::endl;
				}
			}
			else
			{
				std::cout << Nodes[i]->GetName() << " Did nothing\n";
			}
		}
		std::cout << std::endl;
		cnt++;
		if (cnt == 10)
		{
			Update();
			cnt = 0;
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
