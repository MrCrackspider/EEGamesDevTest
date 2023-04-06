#include "Node.h"
#include <random>
#include <iostream>

Node::Node(int ID)
{
	this->ID = ID;
	Name = "Node_" + std::to_string(ID);
}

// Meaningless operation
Node::~Node()
{
	for (auto node : Subscriptions) this->UnsubscribeFrom(node.second);
}

bool Node::SubscribeTo(Node* node)
{
	if (node->ID != this->ID)
	{
		Subscriptions.insert({ node->ID, node });
		node->OnSubscribed(this);
		Neighbours.insert({ node->ID, node });
		return true;
	}
	else return false;
}

void Node::UnsubscribeFrom(Node* node)
{
	Subscriptions.erase(node->ID);
	node->OnUnsubscribed(this);
	auto Item = Subscribers.find(node->ID);
	if (Item == Subscribers.end())
	{
		Neighbours.erase(node->ID);
	}
}

void Node::OnSubscribed(Node* node)
{
	Subscribers.insert({ node->ID, node });
	Neighbours.insert({ node->ID, node });
}

void Node::OnUnsubscribed(Node* node)
{
	Subscribers.erase(node->ID);
	auto Item = Subscriptions.find(node->ID);
	if (Item == Subscriptions.end())
	{
		Neighbours.erase(node->ID);
	}
}

int Node::MakeEvent()
{
	srand((unsigned)time(NULL));
	int EventValue = rand() % 100;
	Node* NewNode = new Node(ID);
	for (auto sub : Subscribers) sub.second->OnEventReceived(this, EventValue);
	return EventValue;
}

void Node::OnEventReceived(Node* node, int EventValue)
{
	auto Item = NodesData.find(node->ID);
	if (Item != NodesData.end())
	{
		Item->second.EventsReceived++;
		Item->second.EventSumm += EventValue;
	}
	else
	{
		NodesData.insert({ node->ID, NodeData{ EventValue, 1 } });
	}
	Item = NodesData.find(node->ID);
	std::cout << "\t" << node->GetName() << " -> " << this->GetName() << ": S = " << Item->second.EventSumm << std::endl;
	std::cout << "\t" << node->GetName() << " -> " << this->GetName() << ": N = " << Item->second.EventsReceived << std::endl;
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

std::map<int, Node*> Node::GetSubscribers()
{
	return Subscribers;
}

int Node::GetID()
{
	return ID;
}

std::string Node::GetName()
{
	return Name;
}
