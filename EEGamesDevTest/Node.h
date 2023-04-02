#pragma once
#include <map>
#include <string>



class Node
{
	struct NodeData
	{
		float EventSumm;
		int EventAmount;
	};
public:
	Node(int ID);
	void SubscribeTo(Node* node);
	void UnsubscribeFrom(Node* node);
	void OnSubscribed(Node* node);
	void OnUnsubscribed(Node* node);
	int MakeEvent();
	void OnEventReceived(Node* node, float EventValue);
	Node* CreateNewNode(int ID);
	std::map<int, Node*> GetNeighbours();
	std::map<int, Node*> GetSubscriptions();
	int GetID();
	std::string GetName();
private:
	int ID;
	std::map<int, Node*> Subscribers, Subscriptions, Neighbours;
	std::map<int, NodeData> NodesData;
	std::string Name;
};

