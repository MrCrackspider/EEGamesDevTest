#include "Net.h"
#include <random>
#include <thread>
#include <iostream>
#include <algorithm>

Net::Net(EventProbabilities Probabilities)
{
	// Normalization
	SimulationRunning = false;
	float ProbabilitiesSumm = Probabilities.pUnsubscription + Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent;
	if (ProbabilitiesSumm > 100)
	{
		this->Probabilities.pUnsubscription = 100 * Probabilities.pUnsubscription / ProbabilitiesSumm;
		this->Probabilities.pSubscription = 100 * Probabilities.pSubscription / ProbabilitiesSumm;
		this->Probabilities.pNewNode = 100 * Probabilities.pNewNode / ProbabilitiesSumm;
		this->Probabilities.pEvent = 100 * Probabilities.pEvent / ProbabilitiesSumm;
	}
	else this->Probabilities = Probabilities;
}

void Net::StartSimulation()
{
	std::thread thread(&Net::StartSimulationThread, this);
	thread.detach();
}

int Net::Update()
{
	int NodesErased = 0;
	if (!Nodes.empty())
	{
		for (int i = 0; i < Nodes.size();)
		{
			if ((Nodes[i]->GetNeighbours().empty()))
			{
				delete Nodes[i];
				Nodes.erase(Nodes.begin() + i);
				NodesErased++;
			}
			else i++;
		}
		if (Nodes.empty()) StopSimulation();
		return NodesErased;
	}
}

void Net::StopSimulation()
{
	if (SimulationRunning)
	{
		SimulationRunning = false;
		std::cout << "==================\nSimulation Stopped\n==================\n";
	}
}

void Net::FillRandomNodes(int AmountOfNodes, int AmountOfSubscriptions)
{
	for (size_t i = 0; i < AmountOfNodes; i++)
	{
		Nodes.push_back(new Node(GenerateNewID(100)));
	}
	// Amount of result subscriptions may not match the requirement
	for (size_t i = 0; i < AmountOfSubscriptions; i++)
	{
		Nodes[GetRandom(0, static_cast<int>(Nodes.size()) - 1)]->SubscribeTo((Nodes[GetRandom(0, static_cast<int>(Nodes.size()) - 1)]));
	}
	Update();
	std::cout << "Initial nodes(" << Nodes.size() << "):\n";
}

void Net::StartSimulationThread()
{
	SimulationRunning = true;
	std::cout << "==================\nSimulation Started\n==================\n";

	// Iteration stats
	int Iteration = 1;
	int NewUnsubscriptionAmount = 0;
	int NewSubscriptionAmount = 0;
	int NewNodesAmount = 0;
	int EventsAmount = 0;

	while (SimulationRunning)
	{
		NewUnsubscriptionAmount = 0;
		NewSubscriptionAmount = 0;
		NewNodesAmount = 0;
		EventsAmount = 0;

		std::cout << "-----------------------------------------------\nIteration: " << Iteration << std::endl;
		for (int i = 0; i < Nodes.size(); ++i)
		{
			int Action = GetRandom(0, 100);
			// Make an event
			if (Action <= Probabilities.pEvent)
			{
				int EventValue = Nodes[i]->MakeEvent();
				EventsAmount++;
				std::cout << Nodes[i]->GetName() << " Made an Event with value: " << EventValue << "\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
					<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;
			}
			// Make a new node
			else if (Action <= (Probabilities.pNewNode + Probabilities.pEvent))
			{
				Node* NewNode = Nodes[i]->CreateNewNode(GenerateNewID(100));
				Nodes.push_back(NewNode);
				NewNodesAmount++;
				NewSubscriptionAmount++;
				std::cout << Nodes[i]->GetName() << " Created new node " << NewNode->GetID() << "\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
					<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;
			}
			// Subscription
			else if (Action <= (Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent))
			{
				auto Neighbours = Nodes[i]->GetNeighbours();
				if (Neighbours.size() > 0)
				{
					auto Neighbour = Neighbours.begin();
					std::advance(Neighbour, GetRandom(0, static_cast<int>(Neighbours.size()) - 1));
					Neighbours = Neighbour->second->GetNeighbours();
					if (Neighbours.size() > 0)
					{
						Neighbour = Neighbours.begin();
						std::advance(Neighbour, GetRandom(0, static_cast<int>(Neighbours.size()) - 1));
						Nodes[i]->SubscribeTo(Neighbour->second);
						NewSubscriptionAmount++;
						std::cout << Nodes[i]->GetName() << " Subscribed to " << Neighbour->second->GetName() << "\t\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
							<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;
					}
					else std::cout << Nodes[i]->GetName() << " Failed to subscribe\n";
				}
				else std::cout << Nodes[i]->GetName() << " Failed to subscribe\n";
			}
			// Unsubscription
			else if (Action <= (Probabilities.pUnsubscription + Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent))
			{
				auto Subscriptions = Nodes[i]->GetSubscriptions();
				if (Subscriptions.size() > 0)
				{
					auto Subscription = Subscriptions.begin();
					std::advance(Subscription, GetRandom(0, static_cast<int>(Subscriptions.size()) - 1));
					Nodes[i]->UnsubscribeFrom(Subscription->second);
					NewUnsubscriptionAmount++;
					std::cout << Nodes[i]->GetName() << " Unsubscribed from " << Subscription->second->GetName() << "\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
						<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;
				}
				else std::cout << Nodes[i]->GetName() << " Failed to unsubscribe\t\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
					<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;
			}
			// None of the above
			else
			{
				std::cout << Nodes[i]->GetName() << " Did nothing\t\t\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
					<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;
			}
		}
		Iteration++;

		std::cout << "Iteration stats:\nEvents generated: " << EventsAmount << std::endl;
		std::cout << "New nodes created: " << NewNodesAmount << std::endl;
		std::cout << "New subscriptions amount: " << NewSubscriptionAmount << std::endl;
		std::cout << "Unsubscriptions amount: " << NewUnsubscriptionAmount << std::endl;

		size_t NodesSize = Nodes.size();
		int NodesErased = 0;
		NodesErased = Update();
		std::cout << "Nodes erased: " << NodesErased << "/" << NodesSize << "\n-----------------------------------------------\n\n";

	}
}

int Net::GetRandom(int Min, int Max)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distribution(Min, Max);
	return distribution(rng);
}

bool Net::IsIDExists(int ID)
{
	for (auto node : Nodes)
	{
		if (node->GetID() == ID)
		{
			return true;
		}
	}
	return false;
}

int Net::GenerateNewID(int Efforts)
{
	int Counter = 0;
	int NewID = 0;
	do
	{
		NewID = GetRandom(0, 1000);
		Counter++;
	} while (IsIDExists(NewID) && (Counter <= Efforts));

	if (Counter > Efforts)
	{
		return -1;
	}
	else return NewID;
}

