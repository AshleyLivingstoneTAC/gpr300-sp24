#include "node.h"

livingstone::Node livingstone::createNode(unsigned int index)
{
	livingstone::Node newNode;
	newNode.parentIndex = index;
	return newNode;
}
