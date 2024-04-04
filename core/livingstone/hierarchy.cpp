#include "hierarchy.h"

void livingstone::Hierarchy::addNode(livingstone::Node add)
{
	nodeList.push_back(add);
	nodeCount++;
}
