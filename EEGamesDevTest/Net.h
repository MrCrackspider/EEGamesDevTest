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
	void StartSimulation(long long TickDuration);
	void Update();
	void StopSimulation();
	void FillRandomNodes(int Amount);
private:
	void StartSimulationThread(long long TickDuration);
	bool SimulationRunning;
	std::vector<Node*> Nodes;
	struct EventProbabilities Probabilities;
	int GetRandom(int Min, int Max);
};

