#pragma once
#include "Node.h"
#include <vector>

struct EventProbabilities
{
	float pEvent;
	float pSubscription;
	float pUnsubscription;
	float pNewNode;
};

class Net
{
public:
	Net(int AmountOfNodes, int AmountOfSubscriptions, EventProbabilities Probabilities);
	~Net();

	void StartSimulation();

	void PerformIteration();

	void Reset();

	void StopSimulation();
	
private:
	bool SimulationRunning;
	int AmountOfNodes, AmountOfSubscriptions, Iteration;
	std::vector<Node*> Nodes;

	// Event probabilities (in %) of nodes making event / subscription / unsubscription / new node respectively
	struct EventProbabilities Probabilities;

	void StartSimulationThread();

	// Erases all nodes without neighbours
	int Update();

	// Sets initial amount of nodes with random IDs and random subscriptions between them
	void FillRandomNodes(int AmountOfNodes, int AmountOfSubscriptions);

	// Self explains
	int GetRandom(int Min, int Max);

	// Checks each node's ID
	bool IsIDExists(int ID);

	// Returns -1 if generated IDs exist
	int GenerateNewID(int Efforts);
};

