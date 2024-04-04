#include "node.h"

livingstone::Node livingstone::createNode(glm::mat4 local, unsigned int index)
{
	livingstone::Node newNode;
	newNode.localTransform = local;
	newNode.parentIndex = index;
	return newNode;
}
