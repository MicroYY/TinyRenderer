// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ImGuiFileDialog.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <string>
#include <iostream>

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

import shader;
import io;
import renderer;
import scene;

///
///      y
///      ^
///      |
///      |
///      |
///      ----------------> x
///             2D
/// 
///           y
///           ^
///           |
///           |
///           |
///           ------------> x
///          /
///         / 
///        /
///       v
///       z
///              3D

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int              windowWidth;
int              windowHeight;
std::string      filePathName;
std::string      filePath;
bool             pictureLoaded = false;
bool             modelLoaded = false;
ImGuiFileDialog  pictureFD;
ImGuiFileDialog  modelFD;
ImGuiFileDialog  textureFD;
std::chrono::duration<double> renderTiming;

tr::Config       config;
tr::Camera       camera;
tr::Scene        scene;
tr::TinyRender   renderer;
math::Vec3f      lightDir(0, 0, -1);

static void ShowMenuFile()
{
	//ImGui::MenuItem("(demo menu)", NULL, false, false);
	if (ImGui::MenuItem("Open image", "Ctrl+I"))
	{
		pictureFD.OpenDialog("ChooseFileDlgKey", "Choose File", ".png,.jpg", ".");
	}
	if (ImGui::MenuItem("Open model", "Ctrl+M"))
	{
		modelFD.OpenDialog("ChooseFileDlgKey", "Choose File", ".*obj", ".");
	}
}

static void LoadTexture()
{
	textureFD.OpenDialog("ChooseFileDlgKey", "Choose File", ".tga", ".");
}


// Main code
int main(int, char**)
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Tiny renderer", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync


	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		//std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	// - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	Shader shaderTexture("../../src/graphicsAPIdemo/shaders/texShader.vs", "../../src/graphicsAPIdemo/shaders/texShader.fs");

	unsigned int textureHandle;

	float rectangle[] = {
		1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f,  -1.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle), rectangle, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	config.proj = tr::Config::perspective;
	camera.dir = math::Vec3f(0, 0, -1);
	camera.pos = math::Vec3f(0, 0, 10);

	// Main loop
#ifdef __EMSCRIPTEN__
	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = NULL;
	EMSCRIPTEN_MAINLOOP_BEGIN
#else
	while (!glfwWindowShouldClose(window))
#endif
	{
		glfwGetWindowSize(window, &windowWidth, &windowHeight);

		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;

			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_MenuBar;

			ImGui::Begin("Control panel", 0, window_flags);                          // Create a window called "Hello, world!" and append into it.

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					ShowMenuFile();
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			if (pictureFD.Display("ChooseFileDlgKey"))
			{
				if (pictureFD.IsOk())
				{
					filePathName = pictureFD.GetFilePathName();
					filePath = pictureFD.GetCurrentPath();

					if (pictureLoaded || modelLoaded)
					{
						glDeleteTextures(1, &textureHandle);
					}

					int width, height, nChannels;
					if (io::LoadTextureFromFile(filePathName, width, height, nChannels, textureHandle))
					{
						pictureLoaded = true;
						modelLoaded = false;
					}
				}
				pictureFD.Close();
			}

			if (modelFD.Display("ChooseFileDlgKey"))
			{
				if (modelFD.IsOk())
				{
					filePathName = modelFD.GetFilePathName();
					filePath = modelFD.GetCurrentPath();

					if (pictureLoaded || modelLoaded)
					{
						glDeleteTextures(1, &textureHandle);
					}

					scene.Init(windowWidth, windowHeight, lightDir);

					renderer.Init(filePathName, textureHandle);

					modelLoaded = true;
					pictureLoaded = false;
				}
				modelFD.Close();
			}

			//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

			static int projection = 1;
			ImGui::RadioButton("perspective projection", &projection, tr::Config::perspective); ImGui::SameLine();
			ImGui::RadioButton("orthogonal projection", &projection, tr::Config::orthogonal);
			config.proj = (tr::Config::Projection)projection;

			// camera
			static float camPos[3] = { 0.0f, 0.0f, 10.0f };
			static float camRot[3] = { 0.0f, 0.0f, 0.0f };
			ImGui::DragFloat3("camera pos", camPos, 0.1f, -100.0f, 100.0f, "%.1f");
			ImGui::DragFloat3("camera rot", camRot, 0.1f, 0.0f,    360.0f, "%.1f deg");
			camera.pos = { camPos[0], camPos[1], camPos[2] };
			camera.rot = { camRot[0], camRot[1], camRot[2] };

			// 

			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Load texture"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				LoadTexture();
			}

			if (modelLoaded)
			{
				if (textureFD.Display("ChooseFileDlgKey"))
				{
					if (textureFD.IsOk())
					{
						filePathName = textureFD.GetFilePathName();
						filePath = textureFD.GetCurrentPath();

						renderer.LoadTexture(filePathName);

					}
					textureFD.Close();
				}
			}
			ImGui::SameLine();

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Current frame rendering %.3f s/frame (%.1f FPS)", renderTiming.count(), 1.0f / renderTiming.count());
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		if (modelLoaded)
		{
			scene.Update(windowWidth, windowHeight, camera);
			renderer.UpdateConfig(config);
			auto start = std::chrono::system_clock::now();
			renderer.Render(scene);
			auto end = std::chrono::system_clock::now();
			renderTiming = end - start;
		}

		if (pictureLoaded || modelLoaded)
		{
			shaderTexture.use();
			glBindTexture(GL_TEXTURE_2D, textureHandle);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
#ifdef __EMSCRIPTEN__
	EMSCRIPTEN_MAINLOOP_END;
#endif

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	if (pictureLoaded || modelLoaded)
	{
		glDeleteTextures(1, &textureHandle);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
	}
