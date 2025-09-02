#ifndef ABOUT_H_GUARD
#define ABOUT_H_GUARD

#include "draw_window.h"
#include <imgui.h>

#ifndef HA6GUIVERSION
#define HA6GUIVERSION " custom"
#endif

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "?"
#endif

//This is the main pane on the left
class AboutWindow 
{
public:
	bool isVisible = false;

	void Draw()
	{
		if(isVisible)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1,1,1,0.3));
			ImGui::SetNextWindowSize(ImVec2{400,200});
			ImGui::Begin("About", &isVisible, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0,0,0,1));
			ImGui::Text("Hantei-kun v" HA6GUIVERSION);
			ImGui::Separator();
			ImGui::Text("Made by omanko.\nForked and updated by several contributors.\nThis fork can be found at github.com/Fatih120/Hantei-kun");
			ImGui::Spacing();
			ImGui::Text("Special thanks to:\nEiton for the effect rendering code\nu4ick and Rhekar for advice and reporting bugs.");
			ImGui::Separator();
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Separator();
			ImGui::Text("Built: %s %s (%s)", __DATE__, __TIME__, GIT_COMMIT_HASH);
			ImGui::End();
			ImGui::PopStyleColor(2);
		}
	}
};


class HelpWindow 
{
public:
	bool isVisible = false;

	void Draw()
	{
		if(isVisible)
		{
			ImGui::SetNextWindowSize(ImVec2{400,400});
			ImGui::Begin("Shortcuts", &isVisible, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

			constexpr const char *shortcutText[] = {
				"Up", "Next pattern",
				"Down", "Previous pattern",
				"Right", "Next frame",
				"Left", "Previous frame",
				"Ctrl + Arrow Keys", "Move sprite 1px",
				"P", "Output current view as image",
				"Ctrl + P", "Output current animated element image",
				"L", "Toggle axis",
				"H", "Toggle hitboxes",
				"F", "Hide user interface",
				"Mouse Wheel", "Adjust Zoom",
				"-","-",
				"Right click & drag", "Draw box",
				"Left click & drag", "Pan view",
				"X", "Next box",
				"Z", "Previous box",
				"Shift + Arrow Keys", "Move Box",
				"Shift + A + Arrow Keys", "Move All Boxes",
				"Shift + Ctrl + Arrow Keys", "Expand Box",
				"Shift + Ctrl + Alt + Arrow Keys", "Shrink Box",
				"Hold Ctrl on +/-", "Resize Box by 20px",
				"Hold Alt on +/-", "Resize Box by 2px",
			};

			if (ImGui::BeginTable("table1", 2))
			{
				for (int row = 0; row < sizeof(shortcutText)/sizeof(char*)/2; row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 2; column++)
					{
						ImGui::TableSetColumnIndex(column);
						ImGui::Text("%s", shortcutText[row*2 + column]);
					}
				}
				ImGui::EndTable();
			}

			ImGui::End();
		}
	}

private:
};


#endif /* ABOUT_H_GUARD */
