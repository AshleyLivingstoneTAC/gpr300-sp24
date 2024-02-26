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

struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI(livingstone::Framebuffer shadowMap);
//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;
bool bs = false;
ew::Camera camera;
ew::Camera orthoCam;
ew::CameraController cameraController;

int main() {
	GLFWwindow* window = initWindow("Assignment 1", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	
	ew::Model monkeyModel = ew::Model("assets/suzanne.obj");
	ew::Transform monkeyTransform;
	ew::Shader blurShader = ew::Shader("assets/post.vert", "assets/post.frag");
	ew::Shader invertShader = ew::Shader("assets/post.vert", "assets/invert.frag");
	ew::Shader depthShader = ew::Shader("assets/depth.vert", "assets/depth.frag");
	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag"); 
	GLuint brickTexture = ew::loadTexture("assets/RoofingTiles014A_1K-JPG_Color.jpg");
	ew::Mesh planeMesh = ew::Mesh(ew::createPlane(10, 10, 5));

	livingstone::Framebuffer framebuffer = livingstone::createFramebuffer(screenWidth, screenHeight, GL_RGB16F);
	livingstone::Framebuffer shadowMap = livingstone::createShadowMap(2048, 2048, GL_RGB16F);
	camera.position = glm::vec3(0.0f, 2.5f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical field of view, in degrees

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

		//RENDER
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.fbo); 
		glViewport(0, 0, shadowMap.width, shadowMap.height);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 lightViewProjection = orthoCam.projectionMatrix(); //Based on light type, direction
		depthShader.use();
		depthShader.setMat4("_ViewProjection", orthoCam.projectionMatrix() * orthoCam.viewMatrix());
		depthShader.setMat4("_Model", monkeyTransform.modelMatrix()); 
		monkeyModel.draw();
		planeMesh.draw();
		
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
		glViewport(0, 0, framebuffer.width, framebuffer.height); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glBindTextureUnit(0, brickTexture); 

		shader.use();
		shader.setInt("_MainTex", 0);
		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix()); 
		shader.setMat4("_Model", monkeyTransform.modelMatrix()); 
		shader.setVec3("_EyePos", camera.position); 
		shader.setFloat("_Material.Ka", material.Ka); 
		shader.setFloat("_Material.Kd", material.Kd); 
		shader.setFloat("_Material.Ks", material.Ks); 
		shader.setFloat("_Material.Shininess", material.Shininess); 
		monkeyModel.draw(); //Draws monkey model using current shader 
		planeMesh.draw(); 
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
		drawUI(shadowMap); 
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

	ImGui::Begin("Shadow Map");
	//Using a Child allow to fill all the space of the window.
	ImGui::BeginChild("Shadow Map");
	//Stretch image to be window size
	ImVec2 windowSize = ImGui::GetWindowSize();
	//Invert 0-1 V to flip vertically for ImGui display
	//shadowMap is the texture2D handle
	ImGui::Image((ImTextureID)shadowMap.depthBuffer, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
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