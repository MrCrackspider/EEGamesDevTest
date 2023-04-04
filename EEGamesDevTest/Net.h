#pragma once
#include "Node.h"
#include <vector>
#include <map>

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
	void StartSimulation(long long TickDuration, int UpdatePeriod);

	// Erases all nodes without neighbours
	int Update();
	void StopSimulation();

	// Sets initial amount of nodes with random IDs and random subscriptions between them
	void FillRandomNodes(int Amount, int AmountOfSubscriptions);
private:
	void StartSimulationThread(long long TickDuration, int UpdatePeriod);
	bool SimulationRunning;
	std::vector<Node*> Nodes;

	// ExceptionList used to ignore new created nodes
	std::map<int, Node*> ExceptionList;

	// Event probabilities (in %) of nodes making event / subscription / unsubscription / new node respectively
	struct EventProbabilities Probabilities;

	// Self explains
	int GetRandom(int Min, int Max);
};

