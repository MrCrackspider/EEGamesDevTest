#include "Node.h"
#include <random>



Node::Node(int ID) 
{ 
	this->ID = ID;
	Name = "Node_" + std::to_string(ID); 
}

void Node::SubscribeTo(Node* node)
{
	if (node->ID != this->ID)
	{
		Subscriptions.insert(std::make_pair(node->ID, node));
		Neighbours.insert(std::make_pair(node->ID, node));
		node->OnSubscribed(this);
	}
}

void Node::UnsubscribeFrom(Node* node)
{
	Subscriptions.erase(node->ID);
	Neighbours.erase(node->ID);
	node->OnUnsubscribed(this);
}

void Node::OnSubscribed(Node* node)
{
	Subscribers.insert(std::make_pair(node->ID, node));
	Neighbours.insert(std::make_pair(node->ID, node));
}

void Node::OnUnsubscribed(Node* node)
{
	Subscribers.erase(node->ID);
	Neighbours.erase(node->ID);
}

int Node::MakeEvent()
{
	srand((unsigned)time(NULL));
	float EventValue = rand()%100;

	for (auto it = Subscribers.begin(); it != Subscribers.end(); ++it)
	{
		it->second->OnEventReceived(this, EventValue);
	}
	return EventValue;
}

void Node::OnEventReceived(Node* node, float EventValue)
{
	auto Item = NodesData.find(node->ID);
	if (Item != NodesData.end())
	{
		Item->second.EventAmount++;
		Item->second.EventAmount += EventValue;
	}
	else
	{
		NodesData.insert(std::make_pair(node->ID, NodeData{ EventValue, 1 }));
	}
}

Node* Node::CreateNewNode(int ID)
{
	Node* NewNode = new Node(ID);
	this->SubscribeTo(NewNode);
	return NewNode;
}

std::map<int, Node*> Node::GetNeighbours()
{
	return Neighbours;
}

std::map<int, Node*> Node::GetSubscriptions()
{
	return Subscriptions;
}

int Node::GetID()
{
	return ID;
}

std::string Node::GetName()
{
	return Name;
}
