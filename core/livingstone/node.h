#include <glm/mat4x4.hpp>
namespace livingstone
{
	struct Node 
	{
		glm::mat4 localTransform;
		glm::mat4 globalTransform;
		unsigned int parentIndex; //Index of parent in hierarchy
	};
}
