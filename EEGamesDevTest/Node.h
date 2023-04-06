#pragma once
#include <map>
#include <string>

class Node
{
	struct NodeData
	{
		int EventSumm;
		int EventsReceived;
	};
public:
	Node(int ID);

	// Subscribes to node and notifies it
	bool SubscribeTo(Node* node);

	// Unsubscribes from node and notifies it
	void UnsubscribeFrom(Node* node);

	// Creates event (random value) and notifies all subscribed nodes
	int MakeEvent();

	// Creates new node and subscribes to it
	Node* CreateNewNode(int ID);

	std::map<int, Node*> GetNeighbours();
	std::map<int, Node*> GetSubscriptions();
	std::map<int, Node*> GetSubscribers();
	int GetID();
	std::string GetName();
private:
	// Called when other nodes subscribes on this node
	void OnSubscribed(Node* node);

	// Called when other nodes unsubscribes from this node
	void OnUnsubscribed(Node* node);

	// Called when any other node makes event
	void OnEventReceived(Node* node, int EventValue);
	int ID;

	// Neighbours = Subscribers + Subscriptions (excess field?)
	std::map<int, Node*> Subscribers, Subscriptions, Neighbours;

	// Keeps statistics of events received from subscriptions
	std::map<int, NodeData> NodesData;

	// Keeps Node_ID string
	std::string Name;
};

