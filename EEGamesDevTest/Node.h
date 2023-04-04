#pragma once
#include <map>
#include <string>

class Node
{
	struct NodeData
	{
		float EventSumm;
		int EventsReceived;
	};
public:
	Node(int ID);
	void SubscribeTo(Node* node);
	void UnsubscribeFrom(Node* node);
	int MakeEvent();
	Node* CreateNewNode(int ID);
	std::map<int, Node*> GetNeighbours();
	std::map<int, Node*> GetSubscriptions();
	int GetID();
	std::string GetName();
private:
	void OnSubscribed(Node* node);
	void OnUnsubscribed(Node* node);
	void OnEventReceived(Node* node, float EventValue);
	int ID;
	std::map<int, Node*> Subscribers, Subscriptions, Neighbours;
	std::map<int, NodeData> NodesData;
	std::string Name;
};

