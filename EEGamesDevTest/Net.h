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
	void StartSimulation(long long TickDuration, int UpdateRate);
	int Update();
	void StopSimulation();
	void FillRandomNodes(int Amount, int AmountOfSubscriptions);
private:
	void StartSimulationThread(long long TickDuration, int UpdateRate);
	bool SimulationRunning;
	std::vector<Node*> Nodes;
	std::map<int, Node*> ExceptionList;
	struct EventProbabilities Probabilities;
	int GetRandom(int Min, int Max);
};

