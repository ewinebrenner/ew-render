#include <stdio.h>
#include <math.h>
#include <fstream>
#include <sstream>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <assert.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/ew.h>
#include <ew/procGen.h>
#include <ew/ewMath/transformations.h>
#include <ew/ewMath/splines.h>
#include <ew/particles.h>



#include <ew/external/imguizmo/ImGuizmo.h>

struct AppTransformSettings {
	ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::WORLD;
	bool useSnap = false;
	ew::Vec3 translateSnap = 0.5f;
	float rotateSnap = 1.0f;
	float scaleSnap = 0.1f;
}appTransformSettings;

void processInput(GLFWwindow* window);
void onCursorMoved(GLFWwindow* window, double xpos, double ypos);
void onMouseButtonPressed(GLFWwindow* window, int button, int action, int mods);
void onWindowResized(GLFWwindow* window, int width, int height);

void TransformGizmo(const float* view, const float* projection, float* matrix, const AppTransformSettings& settings);
void TransformSettingsPanel(AppTransformSettings& settings);

int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;

struct Camera {
	float fov = 60.0f;
	ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 10.0f);
	ew::Vec3 target = ew::Vec3(0.0f, 0.0f, 1.0f);
	bool orthographic = false;
	float orthographicHeight = 10.0f;
};
struct CameraController {
	float moveSpeed = 5.0f;
	float yaw = -90.0f;
	float pitch = 0.0f;
	double prevMouseX, prevMouseY;
	bool firstMouse = true;
	float mouseSensitivity = 0.1f;
};

ew::Transform splineTransform;

Camera camera;
CameraController cameraController;

struct Light {
	ew::Vec4 color = ew::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct Material {
	ew::Vec4 color = ew::Vec4(0.8f);
	float ambientK = 0.2f;
	float diffuseK = 0.3f;
	float specularK = 0.5f;
	float shininess = 128.0f;
};

float prevTime;
float deltaTime;

Light light;
Material material;

ew::Vec3 controlPoints[4] = {
	{0.0, 0.0, 0.0 },
	{1.0, 1.0, 0.0 },
	{2.0, 0.0, 0.0 },
	{3.0, 1.0, 0.0 }
};
int splineSegments = 32;
float splineWidth = 0.3f;

void drawMesh(const ew::Shader& shader, const ew::Mesh& mesh, const ew::Mat4& modelMatrix) {
	mesh.bind();
	shader.use();
	shader.setMat4("_Model", modelMatrix);
	glDrawElements(GL_TRIANGLES, mesh.getNumIndices(), GL_UNSIGNED_INT, NULL);
}

void drawMesh(const ew::Shader& shader, const ew::Mesh& mesh, const ew::Transform& transform, int drawMode = GL_TRIANGLES) {
	mesh.bind();
	shader.setMat4("_Model", transform.getModelMatrix());
	if (drawMode == GL_TRIANGLES) {
		glDrawElements(drawMode, mesh.getNumIndices(), GL_UNSIGNED_INT, NULL);
	}
	else {
		glDrawArrays(drawMode, 0, mesh.getNumVertices());
	}
}
void createCubicBezierMesh(ew::Vec3 p0, ew::Vec3 p1, ew::Vec3 p2, ew::Vec3 p3, int numSegments, ew::MeshData* mesh) {
	mesh->vertices.clear();
	mesh->indices.clear();
	for (size_t i = 0; i <= numSegments; i++)
	{
		float t = (float)i / numSegments;
		ew::Vertex v;
		v.pos = ew::evaluateCubicBezier(p0, p1, p2, p3, t);
		v.normal = ew::Vec3(0, 0, 1);
		mesh->vertices.push_back(v);
	}
}

void createCubicBezierTrackMesh(ew::Vec3 p0, ew::Vec3 p1, ew::Vec3 p2, ew::Vec3 p3, int numSegments, float splineWidth, ew::MeshData* mesh) {
	mesh->vertices.clear();
	mesh->indices.clear();
	for (size_t i = 0; i <= numSegments; i++)
	{
		float t = (float)i / numSegments;
		ew::Vec3 centerPos = ew::evaluateCubicBezier(p0, p1, p2, p3, t);
		ew::Vec3 tangent = ew::evaluateCubicBezierTangent(p0, p1, p2, p3, t);
		ew::Vec3 right = ew::Normalize(ew::Cross(tangent, ew::Vec3(0, 1, 0)));
		ew::Vec3 normal = ew::Normalize(ew::Cross(right, tangent));

		ew::Vertex v0;
		v0.pos = centerPos + right * splineWidth;
		v0.normal = normal;
		v0.uv = ew::Vec2(0.0f, t);
		ew::Vertex v1;
		v1.pos = centerPos - right * splineWidth;
		v1.normal = normal;
		v1.uv = ew::Vec2(1.0f, t);
		mesh->vertices.push_back(v0);
		mesh->vertices.push_back(v1);
	}
	for (size_t i = 0; i < numSegments; i++)
	{
		int startIndex = i * 2;
		mesh->indices.push_back(startIndex);
		mesh->indices.push_back(startIndex + 2);
		mesh->indices.push_back(startIndex + 1);
		mesh->indices.push_back(startIndex + 1);
		mesh->indices.push_back(startIndex + 2);
		mesh->indices.push_back(startIndex + 3);
	}
}


unsigned int selectionIndex = 0;
ew::Framebuffer* mouseSelectFramebuffer;
ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
int selection_mask = (1 << 2);

struct MeshRenderer {
	ew::Mat4 transform;
	ew::Mesh* mesh;
	ew::Shader* shader;
};

const int NUM_RENDERERS = 8;
MeshRenderer meshRenderers[NUM_RENDERERS];

bool particleBillboard = true;
const int MAX_PARTICLES = 10000;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGLExample", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, onCursorMoved);
	glfwSetMouseButtonCallback(window, onMouseButtonPressed);
	glfwSetFramebufferSizeCallback(window, onWindowResized);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glPointSize(3);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Create meshes
	ew::MeshData splineMeshData, sphereMeshData, cubeMeshData;
	createCubicBezierTrackMesh(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3], 32,splineWidth, &splineMeshData);
	ew::createSphere(0.5f, 32, &sphereMeshData);
	ew::createCube(1.0f, &cubeMeshData);

	ew::Mesh splineMesh, sphereMesh, cubeMesh;
	splineMesh.load(splineMeshData);
	sphereMesh.load(sphereMeshData);
	cubeMesh.load(cubeMeshData);
	
	ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");
	ew::Shader litShader("assets/lit.vert", "assets/lit.frag");
	ew::Shader pickingShader("assets/objectPicking.vert", "assets/objectPicking.frag");
	ew::Shader particleShader("assets/particle.vert", "assets/particle.frag");

	unsigned int texture = ew::loadTexture("assets/bricks_color.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);

	ew::Framebuffer pickingFramebuffer(SCREEN_WIDTH,SCREEN_HEIGHT, ew::TextureInternalFormat::RGB32UI, ew::TextureFormat::RGB_INTEGER, ew::TextureType::UINT);
	mouseSelectFramebuffer = &pickingFramebuffer;

	//Set up scene
	MeshRenderer* splineMeshRenderer = &meshRenderers[0];
	splineMeshRenderer->transform = ew::IdentityMatrix();
	splineMeshRenderer->mesh = &splineMesh;
	splineMeshRenderer->shader = &litShader;

	MeshRenderer* lightMeshRenderer = &meshRenderers[1];
	lightMeshRenderer->transform = ew::TranslationMatrix(3.0f, 3.0f, 0.0f) * ew::ScaleMatrix(0.3f);
	lightMeshRenderer->mesh = &sphereMesh;
	lightMeshRenderer->shader = &unlitShader;

	for (size_t i = 2; i < NUM_RENDERERS; i++)
	{
		meshRenderers[i].transform = ew::IdentityMatrix();
		meshRenderers[i].mesh = &cubeMesh;
		meshRenderers[i].shader = &litShader;
	}

	ew::ParticleSystem particleSystem(MAX_PARTICLES);
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//The current time in seconds this frame
		float time = (float)glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;

		//Update camera forward vector
		float yawRad = ew::Radians(cameraController.yaw);
		float pitchRad = ew::Radians(cameraController.pitch);

		ew::Vec3 forward;
		forward.x = cosf(yawRad) * cosf(pitchRad);
		forward.y = sinf(pitchRad);
		forward.z = sinf(yawRad) * cosf(pitchRad);
		forward = ew::Normalize(forward);

		camera.target = camera.position + forward;

		//Construct View and Projection
		ew::Mat4 view = ew::LookAtMatrix(camera.position, camera.target, ew::Vec3(0, 1, 0));
		ew::Mat4 projection = camera.orthographic ? 
			ew::OrthographicMatrix(camera.orthographicHeight, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.01f,1000.0f) :
			ew::PerspectiveMatrix(ew::Radians(camera.fov), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.01f, 1000.0f);

		ew::Mat4 viewProjection = projection * view;

		//Set uniforms
		litShader.use();
		litShader.setMat4("_ViewProjection", viewProjection);

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, texture);
		litShader.setInt("_Texture", 0);

		litShader.setVec3("_EyePos", camera.position);
		litShader.setVec3("_Light.position", lightMeshRenderer->transform[3].toVec3());
		litShader.setVec4("_Light.color", light.color);

		litShader.setVec4("_Material.color", material.color);
		litShader.setFloat("_Material.ambientK", material.ambientK);
		litShader.setFloat("_Material.diffuseK", material.diffuseK);
		litShader.setFloat("_Material.specularK", material.specularK);
		litShader.setFloat("_Material.shininess", material.shininess);

		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", viewProjection);
		unlitShader.setVec4("_Color", light.color);

		for (size_t i = 0; i < NUM_RENDERERS; i++)
		{
			drawMesh(*meshRenderers[i].shader, *meshRenderers[i].mesh, meshRenderers[i].transform);
		}

		particleSystem.draw(deltaTime, &particleShader, view, projection, camera.position);
		
		//Render pickable meshes to object picking framebuffer
		{
			pickingFramebuffer.bind();
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, pickingFramebuffer.getWidth(), pickingFramebuffer.getHeight());

			pickingShader.use();
			for (size_t i = 0; i < NUM_RENDERERS; i++)
			{
				pickingShader.setInt("_ObjectIndex", i + 1);
				pickingShader.setMat4("_MVP", viewProjection * meshRenderers[i].transform);
				drawMesh(pickingShader, cubeMesh, meshRenderers[i].transform);
			}
			pickingFramebuffer.unbind();
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Light")) {
				ImGui::ColorEdit4("Color", &light.color.x);
			}
			if (ImGui::CollapsingHeader("Material")) {
				ImGui::SliderFloat("Ambient K", &material.ambientK,0.0f,1.0f);
				ImGui::SliderFloat("Diffuse K", &material.diffuseK,0.0f,1.0f);
				ImGui::SliderFloat("Specular K", &material.specularK,0.0f,1.0f);
				ImGui::DragFloat("Shininess", &material.shininess);
			}

			if (ImGui::CollapsingHeader("Camera")) {
				if (camera.orthographic) {
					ImGui::DragFloat("Height", &camera.orthographicHeight, 0.5f);
				}
				else {
					ImGui::DragFloat("FOV", &camera.fov, 1.0f, 0.0f, 180.0f);
				}

				ImGui::Checkbox("Orthographic", &camera.orthographic);
				ImGui::DragFloat("Move speed", &cameraController.moveSpeed, 0.1f);
			}
			if (ImGui::CollapsingHeader("Spline")) {
				bool dirty = false;
				dirty = dirty || (ImGui::DragInt("Segments", &splineSegments, 1, 3, 512));
				for (size_t i = 0; i < 4; i++)
				{
					ImGui::PushID(i);
					dirty = dirty || ImGui::DragFloat3(("P" + std::to_string(i)).c_str(), &controlPoints[i].x, 0.2f);
					ImGui::PopID();
				}
				dirty = dirty || (ImGui::DragFloat("Width", &splineWidth, 0.1f, 0.0f, 5.0f));
				if (dirty) {
					createCubicBezierTrackMesh(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3], splineSegments, splineWidth,&splineMeshData);
					splineMesh.load(splineMeshData);
				}
			}
			if (ImGui::CollapsingHeader("Particles")) {
				ImGui::Text("Num Particles: %d", particleSystem.getNumParticles());
				if (ImGui::Checkbox("Billboard", &particleBillboard)) {
					particleSystem.setBillboarding(particleBillboard);
				}
			}
			
			ImGui::End();

			//IMGUIZMO

			ImGuizmo::SetOrthographic(camera.orthographic);
			ImGuizmo::BeginFrame();
		//	ImGuizmo::DrawGrid(&view[0][0], &projection[0][0], &ew::IdentityMatrix()[0][0], 100.f);


			if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
				if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_W))
					appTransformSettings.mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
				if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_E))
					appTransformSettings.mCurrentGizmoOperation = ImGuizmo::ROTATE;
				if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R)) 
					appTransformSettings.mCurrentGizmoOperation = ImGuizmo::SCALE;
				if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Z)) 
					appTransformSettings.mCurrentGizmoMode = ImGuizmo::WORLD;
				if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_X))
					appTransformSettings.mCurrentGizmoMode = ImGuizmo::LOCAL;
			}

			TransformSettingsPanel(appTransformSettings);

			//Draw hierarchy
			{
				ImGui::Begin("Hierarchy");
				
				int node_clicked = -1;
				for (int i = 0; i < NUM_RENDERERS; i++)
				{
					ImGuiTreeNodeFlags node_flags = base_flags;
					
					const bool is_selected = (selection_mask & (1 << i)) != 0;
					if (is_selected)
						node_flags |= ImGuiTreeNodeFlags_Selected;

					node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Mesh %d", i);
					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()){
						node_clicked = i;
						selectionIndex = i;
					}
				}
				if (node_clicked != -1)
				{
					// Update selection state
					// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
					if (ImGui::GetIO().KeyCtrl)
						selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
					else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
						selection_mask = (1 << node_clicked);           // Click to single-select
				}
				ImGui::End();
			}
			
			//Draw Inspector
			{
				ImGui::Begin("Inspector");
				ImGui::Text("Name");
				ImGui::SameLine();
				ImGui::Text("Cube %d", selectionIndex);
				if (ImGui::CollapsingHeader("Transform")) {
					//Individual transform
					float matrixTranslation[3], matrixRotation[3], matrixScale[3];
					ImGuizmo::DecomposeMatrixToComponents(&meshRenderers[selectionIndex].transform[0][0], matrixTranslation, matrixRotation, matrixScale);
					ImGui::DragFloat3("Translation", matrixTranslation, 0.05f);
					ImGui::DragFloat3("Rotation", matrixRotation, 1.0f);
					ImGui::DragFloat3("Scale", matrixScale, 0.05f);
					ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &meshRenderers[selectionIndex].transform[0][0]);
				}
				
				ImGui::End();
			}
			

			TransformGizmo(&view[0][0], &projection[0][0], &meshRenderers[selectionIndex].transform[0][0], appTransformSettings);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
		return;
	}

	//Only move camera if cursor is locked
	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
		return;

	ew::Vec3 forward = ew::Normalize(camera.target - camera.position);
	ew::Vec3 right = ew::Normalize(ew::Cross(forward, ew::Vec3(0, 1, 0)));
	ew::Vec3 up = ew::Normalize(ew::Cross(forward,right));
	if (glfwGetKey(window,GLFW_KEY_W)) {
		camera.position += forward * cameraController.moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		camera.position -= forward * cameraController.moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		camera.position += right * cameraController.moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		camera.position -= right * cameraController.moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		camera.position += up * cameraController.moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		camera.position -= up * cameraController.moveSpeed * deltaTime;
	}


}

void onCursorMoved(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
		return;

	if (cameraController.firstMouse) {
		cameraController.prevMouseX = xpos;
		cameraController.prevMouseY = ypos;
		cameraController.firstMouse = false;
	}
	float deltaX = xpos - cameraController.prevMouseX;
	float deltaY = ypos - cameraController.prevMouseY;
	cameraController.yaw += deltaX * cameraController.mouseSensitivity;
	cameraController.pitch += -deltaY * cameraController.mouseSensitivity;
	if (cameraController.pitch < -89.9f) {
		cameraController.pitch = -89.9f;
	}
	else if (cameraController.pitch > 89.9f) {
		cameraController.pitch = 89.9f;
	}

	cameraController.prevMouseX = xpos;
	cameraController.prevMouseY = ypos;
}

void onMouseButtonPressed(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cameraController.firstMouse = true;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	bool mouseLocked = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;

	//Mouse is unlocked and left mouse button is pressed
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !mouseLocked) {
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		mouseY = SCREEN_HEIGHT - mouseY;
		ew::PixelInfo pixelInfo = mouseSelectFramebuffer->readPixel((unsigned int)mouseX, (unsigned int)mouseY);
		if (pixelInfo.r > 0) {
			selectionIndex = pixelInfo.r - 1;
			selection_mask = (1 << selectionIndex);
		}
	}
}

void onWindowResized(GLFWwindow* window, int width, int height) {
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	mouseSelectFramebuffer->resize(width, height);
}


void TransformSettingsPanel(AppTransformSettings& settings) {
	ImGui::Begin("Transform Settings");
	//Global settings
	if (ImGui::RadioButton("Translate", settings.mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		settings.mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", settings.mCurrentGizmoOperation == ImGuizmo::ROTATE))
		settings.mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", settings.mCurrentGizmoOperation == ImGuizmo::SCALE))
		settings.mCurrentGizmoOperation = ImGuizmo::SCALE;

	if (settings.mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", settings.mCurrentGizmoMode == ImGuizmo::LOCAL))
			settings.mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", settings.mCurrentGizmoMode == ImGuizmo::WORLD))
			settings.mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	settings.useSnap = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
	//ImGui::Checkbox("Snap", &settings.useSnap);
	//ImGui::SameLine();

	switch (settings.mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::DragFloat3("Snap", &settings.translateSnap.x, 0.1f);
		break;
	case ImGuizmo::ROTATE:
		ImGui::DragFloat("Angle Snap", &settings.rotateSnap, 0.1f);
		break;
	case ImGuizmo::SCALE:
		ImGui::DragFloat("Scale Snap", &settings.scaleSnap, 0.1f);
		break;
	}
	ImGui::End();
}

void TransformGizmo(const float* view, const float* projection, float* matrix, const AppTransformSettings& settings)
{
	
	//Determine current snap amount
	const float* currentSnap = NULL;
	if (settings.useSnap) {
		switch (settings.mCurrentGizmoOperation) 
		{
		case ImGuizmo::TRANSLATE:
			currentSnap = &settings.translateSnap.x;
			break;
		case ImGuizmo::ROTATE:
			currentSnap = &settings.rotateSnap;
			break;
		case ImGuizmo::SCALE:
			currentSnap = &settings.scaleSnap;
			break;
		}
	}
	
	//Draw gizmo in worldspace
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(view, projection, settings.mCurrentGizmoOperation, settings.mCurrentGizmoMode, matrix, NULL, currentSnap);
}