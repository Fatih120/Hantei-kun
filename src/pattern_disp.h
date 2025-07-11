#include <imgui.h>
#include <imgui_input_text.h>
#include "imgui_utils.h"
#include "framedata.h"

//returns has to update
inline bool PatternDisplay(Sequence *seq)
{
	constexpr float spacing = 80;
	bool update = false;
	
	if(ImGui::InputText("Pattern Name", &seq->name))
		update = true;
	if(ImGui::InputText("Code Name", &seq->codeName))
		update = true;
	
/* Probably unused
	ImGui::SetNextItemWidth(40.f);
	ImGui::InputInt("PSTS", &seq->psts, 0, 0);

	ImGui::SetNextItemWidth(40.f);
	ImGui::InputInt("Level", &seq->level, 0, 0);

	ImGui::SetNextItemWidth(80.f);
	//ImGui::InputInt("PFLG", &seq->flag, 0, 0); Unused by the game.
*/

	ImGui::InputInt("Palette ID", &seq->pups, 0, 0);
	ImGui::SameLine(); ImGui::TextDisabled("(?)");
	if(ImGui::IsItemHovered())
		Tooltip("Which palette to use. 0 is the default *.pal,\nwhile additional palettes are labelled *_pX.pal from 1 to 8.");
	
	return update;
}