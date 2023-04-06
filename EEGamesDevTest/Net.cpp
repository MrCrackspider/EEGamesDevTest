#include "Net.h"
#include <random>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>

Net::Net(int AmountOfNodes, int AmountOfSubscriptions, EventProbabilities Probabilities)
{
	// Normalization
	float ProbabilitiesSumm = Probabilities.pUnsubscription + Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent;
	if (ProbabilitiesSumm > 100)
	{
		this->Probabilities.pUnsubscription = 100 * Probabilities.pUnsubscription / ProbabilitiesSumm;
		this->Probabilities.pSubscription = 100 * Probabilities.pSubscription / ProbabilitiesSumm;
		this->Probabilities.pNewNode = 100 * Probabilities.pNewNode / ProbabilitiesSumm;
		this->Probabilities.pEvent = 100 * Probabilities.pEvent / ProbabilitiesSumm;
	}
	else this->Probabilities = Probabilities;

	SimulationRunning = false;
	this->AmountOfNodes = AmountOfNodes;
	this->AmountOfSubscriptions = AmountOfSubscriptions;
	FillRandomNodes(AmountOfNodes, AmountOfSubscriptions);
	std::thread thread(&Net::StartSimulationThread, this);
	thread.detach();
}

void Net::StartSimulation()
{
	if (Nodes.empty())
	{
		std::cout << "No nodes remaining\n";
		SimulationRunning = false;
		return;
	}
	SimulationRunning = true;
}

void Net::StartSimulationThread()
{
	while (true)
	{
		while (SimulationRunning)
		{
			PerformIteration();
		}
	}
}

int Net::Update()
{
	int NodesErased = 0;
	Nodes.erase(std::remove_if(Nodes.begin(), Nodes.end(), [&NodesErased](Node* node) {
		bool ReturnValue = node->GetNeighbours().empty();
		if (ReturnValue)
		{
			delete node;
			NodesErased++;
		}
		return ReturnValue; }), Nodes.end());
	if (Nodes.empty()) StopSimulation();
	return NodesErased;
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
		Nodes.push_back(new Node(GenerateNewID(200)));
	}
	// Amount of result subscriptions may not match the requirement
	for (size_t i = 0; i < AmountOfSubscriptions; i++)
	{
		Nodes[GetRandom(0, static_cast<int>(Nodes.size()) - 1)]->SubscribeTo((Nodes[GetRandom(0, static_cast<int>(Nodes.size()) - 1)]));
	}
	Update();
	std::cout << "Initial nodes(" << Nodes.size() << "):\n";
	for (auto node : Nodes)
	{
		std::cout << node->GetName() << "\t#Subscribers:" << node->GetSubscribers().size() << "\t#Subscriptions:" << node->GetSubscriptions().size() << std::endl;
	}
}

void Net::PerformIteration()
{
	if (Nodes.empty())
	{
		std::cout << "No nodes remaining\n";
		return;
	}

	std::mutex m;
	m.lock();
	int NewUnsubscriptionAmount = 0;
	int NewSubscriptionAmount = 0;
	int NewNodesAmount = 0;
	int EventsAmount = 0;

	std::cout << "\n-----------------------------------------------\nIteration: " << Iteration << std::endl;

	for (int i = 0; i < Nodes.size(); ++i)
	{
		int Action = GetRandom(0, 100);
		// Make event
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
			int NewID = GenerateNewID(200);
			if (NewID != -1)
			{
				Node* NewNode = Nodes[i]->CreateNewNode(GenerateNewID(200));
				Nodes.push_back(NewNode);
				NewNodesAmount++;
				NewSubscriptionAmount++;
				std::cout << Nodes[i]->GetName() << " Created new node " << NewNode->GetID() << "\t\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
					<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;
			}
			else std::cout << Nodes[i]->GetName() << " Failed to create new node " << "\t\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
				<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;

		}
		// Subscription
		else if (Action <= (Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent))
		{
			auto Neighbours = Nodes[i]->GetNeighbours();
			if (!Neighbours.empty())
			{
				auto Neighbour = Neighbours.begin();
				std::advance(Neighbour, GetRandom(0, static_cast<int>(Neighbours.size()) - 1));
				Neighbours = Neighbour->second->GetNeighbours();
				if (!Neighbours.empty())
				{
					Neighbour = Neighbours.begin();
					std::advance(Neighbour, GetRandom(0, static_cast<int>(Neighbours.size()) - 1));
					if (Nodes[i]->SubscribeTo(Neighbour->second))
					{
						NewSubscriptionAmount++;
						std::cout << Nodes[i]->GetName() << " Subscribed to " << Neighbour->second->GetName() << "\t\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
							<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;
					}
					else std::cout << Nodes[i]->GetName() << " Failed to subscribe\t\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
						<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;;
				}
				else std::cout << Nodes[i]->GetName() << " Failed to subscribe\t\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
					<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;;
			}
			else std::cout << Nodes[i]->GetName() << " Failed to subscribe\t\t#Subscribers:" << Nodes[i]->GetSubscribers().size()
				<< "\t#Subscriptions:" << Nodes[i]->GetSubscriptions().size() << std::endl;
		}
		// Unsubscription
		else if (Action <= (Probabilities.pUnsubscription + Probabilities.pSubscription + Probabilities.pNewNode + Probabilities.pEvent))
		{
			auto Subscriptions = Nodes[i]->GetSubscriptions();
			if (!Subscriptions.empty())
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
	size_t NodesSize = Nodes.size();
	int NodesErased = 0;
	NodesErased = Update();

	std::cout << "______Iteration stats:______\nEvents generated: " << EventsAmount << std::endl;
	std::cout << "New nodes created: " << NewNodesAmount << std::endl;
	std::cout << "New subscriptions amount: " << NewSubscriptionAmount << std::endl;
	std::cout << "Unsubscriptions amount: " << NewUnsubscriptionAmount << std::endl;
	std::cout << "Nodes erased: " << NodesErased << "/" << NodesSize << "\n-----------------------------------------------\n\n";

	m.unlock();
}

void Net::Reset()
{
	for (auto node : Nodes) delete node;
	Nodes.clear();
	Iteration = 1;
	FillRandomNodes(AmountOfNodes, AmountOfSubscriptions);
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
	bool ReturnValue = false;
	for (auto node : Nodes)
	{
		if (node->GetID() == ID)
		{
			ReturnValue = true;
			break;
		}
	}
	return ReturnValue;
}

int Net::GenerateNewID(int Efforts)
{
	int NewID = -1;
	int Counter = 0;
	for (;;)
	{
		NewID = GetRandom(0, 1000);
		Counter++;
		if (Counter > Efforts)
		{
			NewID = -1; 
			break;
		}
		if (!IsIDExists(NewID)) break;
	}
	return NewID;
}
