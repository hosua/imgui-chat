// Dear ImGui: standalone example application for SDL2 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Important to understand: SDL_Renderer is an _optional_ component of SDL2.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL2/SDL.h>

#include "client.hh"
#include "server.hh"

#include <memory>
#include <iostream>
#include <string>

std::unique_ptr<Server> s_server = nullptr;
std::unique_ptr<Client> s_client = nullptr;

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

// Main code
int main(int, char**)
{
	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	// Create window with SDL_Renderer graphics context
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	if (window == nullptr)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		SDL_Log("Error creating SDL_Renderer!");
		return 0;
	}
	//SDL_RendererInfo info;
	//SDL_GetRendererInfo(renderer, &info);
	//SDL_Log("Current SDL_Renderer: %s", info.name);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);

	// Our state
	bool show_demo_window = false;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	static char client_ip_buffer[25] = { '1', '2', '7', '.', '0', '.', '0', '.', '1' };
	static char client_port_buffer[6] = { '8', '9', '6', '9' };
	static char server_ip_buffer[25] = { '1', '2', '7', '.', '0', '.', '0', '.', '1' };
	static char server_port_buffer[6] = { '8', '9', '6', '9' };
	static char client_buffer[2048] = { 0 };
	static char server_buffer[4096] = { 0 };

	// Main loop
	bool done = false;

	while (!done)
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		{
			ImGui::Begin("Demo Window");                         
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::End();
		}

		// Client GUI
		{
			static ImGuiInputTextFlags chat_flags = ImGuiInputTextFlags_AllowTabInput 
				| ImGuiInputTextFlags_CtrlEnterForNewLine
				| ImGuiInputTextFlags_EscapeClearsAll
				| ImGuiInputTextFlags_EnterReturnsTrue;

			ImGui::Begin("Chat Application");                         
			ImGui::Text("Welcome, user!");               // Display some text (you can use a format strings too)
														 //
			if (ImGui::InputTextMultiline("##source", client_buffer, IM_ARRAYSIZE(client_buffer), 
					ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), chat_flags) ||
				ImGui::Button("Send") ){
				std::cout << "Sending message\n";
				if (!s_client){
					strcat(client_buffer, "You need to connect to a server first!\n");
				} else if (strnlen(client_buffer, 2048) > 0){
					s_client->sendMessage();
					memset(client_buffer, 0, 2048);
					s_client = std::make_unique<Client>(client_ip_buffer, client_port_buffer, client_buffer);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Connect")){
				std::cout << "Connecting to server...\n";
				if (!s_client)
					s_client = std::make_unique<Client>(client_ip_buffer, client_port_buffer, client_buffer);
			}

			ImGui::InputText("IP", client_ip_buffer, IM_ARRAYSIZE(client_ip_buffer), ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CharsDecimal);
			ImGui::InputText("Port", client_port_buffer, IM_ARRAYSIZE(client_port_buffer), ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CharsDecimal);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// Server GUI 
		{
			static ImGuiInputTextFlags server_flags = ImGuiInputTextFlags_ReadOnly;
			static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
			// | ImGuiWindowFlags_NoResize;
			ImGui::Begin("Server", NULL, window_flags);
			ImGui::InputTextMultiline("##source", server_buffer, IM_ARRAYSIZE(server_buffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 32), server_flags);
			ImGui::InputText("IP", server_ip_buffer, IM_ARRAYSIZE(server_ip_buffer), ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CharsDecimal);
			ImGui::SameLine();
			if (ImGui::Button("Host", ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 1.5))){
				s_server = std::make_unique<Server>(server_ip_buffer, server_port_buffer, server_buffer);
			}
			ImGui::InputText("Port", server_port_buffer, IM_ARRAYSIZE(server_port_buffer), ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CharsDecimal);
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
	}

	// Cleanup
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
