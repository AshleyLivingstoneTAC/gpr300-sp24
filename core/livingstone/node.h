#ifndef NODE
#define NODE

#include <glm/mat4x4.hpp>
#include "../ew/transform.h"

namespace livingstone
{
	struct Node
	{
		ew::Transform transform;
		glm::mat4 localTransform;
		glm::mat4 globalTransform;
		unsigned int parentIndex; //Index of parent in hierarchy
	};
	 
	livingstone::Node createNode(unsigned int index);
}
#endif