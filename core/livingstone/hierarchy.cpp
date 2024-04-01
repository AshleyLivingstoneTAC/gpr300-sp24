#include "hierarchy.h"

void livingstone::SolveFK(Hierarchy hierarchy)
{
	for each (livingstone::Node node in hierarchy)
		if (node.parentIndex == -1)
			node.globalTransform = node.localTransform;
		else
			node.globalTransform = hierarchy[node.parentIndex].globalTransform * node.localTransform;
}
