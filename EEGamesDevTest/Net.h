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
	Net(EventProbabilities Probabilities);

	// TickDuration describes amount of time (in ms) between events
	// UpdatePeriod describes amount of ticks before net update starts
	void StartSimulation();

	// Erases all nodes without neighbours
	int Update();

	void StopSimulation();

	// Sets initial amount of nodes with random IDs and random subscriptions between them
	void FillRandomNodes(int AmountOfNodes, int AmountOfSubscriptions);
private:
	void StartSimulationThread();
	bool SimulationRunning;

	std::vector<Node*> Nodes;

	// ExceptionList used to ignore new created nodes
	std::vector<int> ExceptionList;

	// Event probabilities (in %) of nodes making event / subscription / unsubscription / new node respectively
	struct EventProbabilities Probabilities;

	// Self explains
	int GetRandom(int Min, int Max);

	bool IsIDExists(int ID);
};

