#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <livingstone/framebuffer.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/transform.h>
#include <ew/cameraController.h>
#include <ew/texture.h>
#include <ew/procGen.h>

#include <livingstone/node.h>
#include <livingstone/hierarchy.h>

struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI(livingstone::Framebuffer shadowMap);
void SolveFK(livingstone::Hierarchy hierarchy);
//Global state
int screenWidth = 1080;
int screenHeight = 720;
int resolution = 2048;
float prevFrameTime;
float deltaTime;
bool bs = false;
ew::Camera camera;
ew::Camera orthoCam;
ew::CameraController cameraController;

int main() {
	GLFWwindow* window = initWindow("Assignment 5", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	
	ew::Model monkeyModel = ew::Model("assets/suzanne.obj");
	ew::Transform monkeyTransform;
	ew::Shader blurShader = ew::Shader("assets/post.vert", "assets/post.frag");
	ew::Shader invertShader = ew::Shader("assets/post.vert", "assets/invert.frag");
	ew::Shader depthShader = ew::Shader("assets/depth.vert", "assets/depth.frag");
	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag"); 
	GLuint brickTexture = ew::loadTexture("assets/RoofingTiles014A_1K-JPG_Color.jpg");
	ew::Mesh planeMesh = ew::Mesh(ew::createPlane(10, 10, 5));
	ew::Transform planeTransform;

	planeTransform.position = glm::vec3(0, -3, 0);

	livingstone::Node root = livingstone::createNode(-1);
	livingstone::Node head = livingstone::createNode(1);
	livingstone::Node torso = livingstone::createNode(0);
	livingstone::Node shoulderL = livingstone::createNode(1);
	livingstone::Node shoulderR = livingstone::createNode(1);
	livingstone::Node elbowL = livingstone::createNode(3);
	livingstone::Node elbowR = livingstone::createNode(4);
	livingstone::Node wristL = livingstone::createNode(5);
	livingstone::Node wristR = livingstone::createNode(6);

	root.transform.position = glm::vec3(0, 0, 0);
	head.transform.position = glm::vec3(1, 2, 0);
	torso.transform.position = glm::vec3(1, 1, 0);
	shoulderL.transform.position = glm::vec3(-2, 0, 0);
	shoulderR.transform.position = glm::vec3(4, 0, 0);
	elbowL.transform.position = glm::vec3(-2, -1, 0);
	elbowR.transform.position = glm::vec3(4, -1, 0);
	wristL.transform.position = glm::vec3(-2, -2, 0);
	wristR.transform.position = glm::vec3(4, -2, 0);

	head.transform.scale = glm::vec3(0.5, 0.5, 0.5);
	torso.transform.scale = glm::vec3(1, 1, 1);
	shoulderL.transform.scale = glm::vec3(0.5, 0.5, 0.5);
	shoulderR.transform.scale = glm::vec3(0.5, 0.5, 0.5);
	elbowL.transform.scale = glm::vec3(0.5, 0.5, 0.5);
	elbowR.transform.scale = glm::vec3(0.5, 0.5, 0.5);
	wristL.transform.scale = glm::vec3(0.5, 0.5, 0.5);
	wristR.transform.scale = glm::vec3(0.5, 0.5, 0.5);
	livingstone::Hierarchy h;

	h.addNode(&root);      //0
	h.addNode(&torso);     //1
	h.addNode(&head);      //2
	h.addNode(&shoulderL); //3
	h.addNode(&shoulderR); //4
	h.addNode(&elbowL);    //5
	h.addNode(&elbowR);    //6
	h.addNode(&wristL);    //7
	h.addNode(&wristR);    //8

	livingstone::Framebuffer framebuffer = livingstone::createFramebuffer(screenWidth, screenHeight, GL_RGB16F);
	livingstone::Framebuffer shadowMap = livingstone::createShadowMap(resolution, resolution, GL_RGB16F);
	camera.position = glm::vec3(0.0f, 0.0f, 2.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical field of view, in degrees

	orthoCam.position = glm::vec3(0.0f, 5.0f, 0.0f);
	orthoCam.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	orthoCam.orthographic = true;
	orthoCam.orthoHeight = 5;
	orthoCam.aspectRatio = 1;
	

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //Back face culling
	glEnable(GL_DEPTH_TEST); //Depth testing
	

	shader.use();
	shader.setInt("_MainTex", 0);
	unsigned int dummyVAO;
	glCreateVertexArrays(1, &dummyVAO);
	while (!glfwWindowShouldClose(window)) {


		glfwPollEvents();
		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		
		root.transform.rotation = glm::rotate(root.transform.rotation, deltaTime, glm::vec3(0, 1, 0));
		torso.transform.rotation = glm::rotate(torso.transform.rotation, deltaTime, glm::vec3(0, 0, 1));
		shoulderL.transform.rotation = glm::rotate(shoulderL.transform.rotation, deltaTime, glm::vec3(0, 0, 1));
		shoulderR.transform.rotation = glm::rotate(shoulderR.transform.rotation, deltaTime, glm::vec3(0, 0, 1));
		elbowL.transform.rotation = glm::rotate(elbowL.transform.rotation, deltaTime, glm::vec3(0, 0, 1));
		elbowR.transform.rotation = glm::rotate(elbowR.transform.rotation, deltaTime, glm::vec3(0, 0, 1));
		wristL.transform.rotation = glm::rotate(wristL.transform.rotation, deltaTime, glm::vec3(1, 0, 0));
		wristR.transform.rotation = glm::rotate(wristR.transform.rotation, deltaTime, glm::vec3(1, 0, 0));
		// placeholeder vec3s
		// (1, 0, 0)
		// (0, 1, 0)
		// (0, 0, 1)
		for (int i = 0; i < h.nodeList.size(); i++)
		{
			h.nodeList[i]->localTransform = h.nodeList[i]->transform.modelMatrix();
		}
		//RENDER
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.fbo); 
		glViewport(0, 0, shadowMap.width, shadowMap.height);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 lightViewProjection = orthoCam.projectionMatrix(); //Based on light type, direction
		depthShader.use();
		depthShader.setMat4("_ViewProjection", orthoCam.projectionMatrix() * orthoCam.viewMatrix());
		depthShader.setMat4("_Model", monkeyTransform.modelMatrix()); 
		monkeyModel.draw();
		
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
		glViewport(0, 0, framebuffer.width, framebuffer.height); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glBindTextureUnit(0, brickTexture); 

		shader.use();
		shader.setInt("_MainTex", 0);
		shader.setInt("_ShadowMap", shadowMap.depthBuffer);
		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix()); 
		shader.setVec3("_EyePos", camera.position); 
		shader.setFloat("_Material.Ka", material.Ka); 
		shader.setFloat("_Material.Kd", material.Kd); 
		shader.setFloat("_Material.Ks", material.Ks); 
		shader.setFloat("_Material.Shininess", material.Shininess); 

		shader.setMat4("_Model", head.globalTransform);
		monkeyModel.draw();
		shader.setMat4("_Model", torso.globalTransform);
		monkeyModel.draw();
		shader.setMat4("_Model", shoulderL.globalTransform);
		monkeyModel.draw();
		shader.setMat4("_Model", shoulderR.globalTransform);
		monkeyModel.draw();
		shader.setMat4("_Model", elbowL.globalTransform);
		monkeyModel.draw();
		shader.setMat4("_Model", elbowR.globalTransform);
		monkeyModel.draw();
		shader.setMat4("_Model", wristL.globalTransform);
		monkeyModel.draw();
		shader.setMat4("_Model", wristR.globalTransform);
		monkeyModel.draw();


		shader.setMat4("_Model", planeTransform.modelMatrix());
		planeMesh.draw();
		SolveFK(h);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo); 
		
		//Rotate model around Y axis
		cameraController.move(window, &camera, deltaTime);
		monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0)); 
		//transform.modelMatrix() combines translation, rotation, and scale into a 4x4 model matrix
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTextureUnit(0, framebuffer.colorBuffer[0]); 
		glBindVertexArray(dummyVAO);   

		blurShader.use();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		depthShader.use();
		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void resetCamera(ew::Camera* camera, ew::CameraController* controller) {
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;
}

void drawUI(livingstone::Framebuffer shadowMap) {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	if (ImGui::Button("Reset Camera")) {
			resetCamera(&camera, &cameraController);
		}
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}
	if (ImGui::CollapsingHeader("Light")) {
		ImGui::SliderFloat("DirectionX", &orthoCam.position.x, -1, 1);
		ImGui::SliderFloat("DirectionY", &orthoCam.position.y, -1, 1);
		ImGui::SliderFloat("DirectionZ", &orthoCam.position.z, -1, 1);
	}
	if (ImGui::CollapsingHeader("Shadows")) {
		
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}


/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

void SolveFK(livingstone::Hierarchy hierarchy)
{
	for (int i = 0; i < hierarchy.nodeCount; i++)
	{
		if (hierarchy.nodeList[i]->parentIndex == -1)
			hierarchy.nodeList[i]->globalTransform = hierarchy.nodeList[i]->localTransform;
		else
			hierarchy.nodeList[i]->globalTransform = hierarchy.nodeList[hierarchy.nodeList[i]->parentIndex]->globalTransform * hierarchy.nodeList[i]->localTransform;
	}
}