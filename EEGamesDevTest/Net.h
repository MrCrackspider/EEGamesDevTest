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
	void StartSimulation();
	void Update();
	void StopSimulation();
	void FillRandomNodes(int Amount);
private:
	bool SimulationRunning;
	std::vector<Node*> Nodes;
	struct EventProbabilities Probabilities;
};

