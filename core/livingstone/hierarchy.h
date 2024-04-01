#include "node.h"
namespace livingstone
{
	struct Hierarchy 
	{
		Node* nodes;
		unsigned int nodeCount;
	};
	void SolveFK(Hierarchy hierarchy);
}