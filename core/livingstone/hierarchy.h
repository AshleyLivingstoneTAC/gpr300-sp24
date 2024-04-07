#include "node.h"
#include <vector>
namespace livingstone
{
	struct Hierarchy 
	{
		std::vector<Node*> nodeList;
		unsigned int nodeCount = 0;

		void addNode(livingstone::Node* add);
	};
}