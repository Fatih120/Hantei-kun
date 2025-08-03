#include <imgui.h>
#include "imgui_utils.h"
#include "framedata.h"

namespace im = ImGui;

// Conditions
inline void IfDisplay(std::vector<Frame_IF> *ifList_, Frame_IF* copied)
{
	std::vector<Frame_IF> & ifList = *ifList_;
	constexpr float width = 75.f;
	int deleteI = -1;
	for( int i = 0; i < ifList.size(); i++)
	{
		if(i>0)
			im::Separator();
		im::PushID(i); 

		im::SetNextItemWidth(width); 
		im::InputInt("Type", &ifList[i].type, 0, 0); im::SameLine(0.f, 40);
		im::SetNextItemWidth(width);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1,0,0,0.4));
		if(im::Button("Delete"))
		{
			deleteI = i;
		}
		ImGui::PopStyleColor();

		im::InputScalarN("##params", ImGuiDataType_S32, ifList[i].parameters, 6, NULL, NULL, "%d", 0); im::SameLine();
		if(im::Button("Copy"))
		{
			*copied = ifList[i];
		}
		im::InputScalarN("##params2", ImGuiDataType_S32, ifList[i].parameters+6, 3, NULL, NULL, "%d", 0);
		im::PopID();
	};

	if(deleteI >= 0)
		ifList.erase(ifList.begin() + deleteI);

	if(im::Button("Add"))
		ifList.push_back({});
}

// Effects
struct EfTypeList {
    int value;
    const char* label;
};

const EfTypeList EfTypeCases[] = {
    { 1,   "Spawn Pattern" },
    { 4,   "Set Enemy State" },
    { 6,   "Modify Self" },
    { 8,   "Spawn System Pattern" },
    { 9,   "Play SFX" },
    { 14,  "Set Enemy Throw State" },
    { 101, "Spawn Pattern (Relative)" }
};
constexpr int EfTypeCasesCount = sizeof(EfTypeCases)/sizeof(EfTypeCases[0]);

const char* const Eff6NoList[] = {
    "Screen Shake",
    "Translation",
    "Announce End"
};
const int Eff6NoArr[] = { 1, 12, 255 };
constexpr int Eff6NoSize = sizeof(Eff6NoArr)/sizeof(Eff6NoArr[0]);

inline void EfDisplay(std::vector<Frame_EF> *efList_, Frame_EF* copied)
{
    std::vector<Frame_EF> & efList = *efList_;
    constexpr float width = 50.f;
    int deleteI = -1;
    for( int i = 0; i < efList.size(); i++)
    {
        if(i>0)
            im::Separator();
        im::PushID(i);

        // type
        int typeComboIndex = -1;
        for(int j=0; j<EfTypeCasesCount; ++j)
            if(efList[i].type == EfTypeCases[j].value) typeComboIndex = j;

        im::SetNextItemWidth(width*2);
        if(im::Combo("Type", &typeComboIndex, 
                     [](void*, int idx, const char** out_text) {
                         *out_text = EfTypeCases[idx].label;
                         return true;
                     }, nullptr, EfTypeCasesCount))
        {
            efList[i].type = EfTypeCases[typeComboIndex].value;
        }
        im::SameLine();
        im::SetNextItemWidth(width);
        im::InputInt(("ID##eftype" + std::to_string(i)).c_str(), &efList[i].type, 0, 0);
        im::SameLine();
        im::TextDisabled("(?)");
        if(im::IsItemHovered())
            Tooltip("Advanced effects that happen on the start of this frame.\nType is the possible effect, while Number is something like an argument.\nEach effect has different additional values and arguments. Hover over them for hints.");

        // number
        switch(efList[i].type)
        {
            case 4: // Set Enemy State
            case 6: // Modify Self
            {
                int idx = 0;
                for(int j=0; j<Eff6NoSize; ++j)
                    if(efList[i].number == Eff6NoArr[j]) idx = j;

                im::SetNextItemWidth(width*2.5);
                if(im::Combo("Number", &idx, Eff6NoList, Eff6NoSize))
                    efList[i].number = Eff6NoArr[idx];

                im::SameLine(); im::SetNextItemWidth(width);
                im::InputInt(("Pattern ID##efno" + std::to_string(i)).c_str(), &efList[i].number, 0, 0);
                break;
            }
            case 8: // Spawn System Pattern
            {
                im::SetNextItemWidth(width*2);
                im::InputInt(("Pattern ID##efno" + std::to_string(i)).c_str(), &efList[i].number, 0, 0);
                break;
            }
            case 9: // Play SFX
            default:
            {
                im::SetNextItemWidth(width*2);
                im::Combo("Number", &efList[i].number, "", 0); im::SameLine(); im::SetNextItemWidth(width);
                im::InputInt(("No.##efno" + std::to_string(i)).c_str(), &efList[i].number, 0, 0);
                break;
            }
        }

		bool tooltipped =
			efList[i].type == 1 ||
			efList[i].type == 4 ||
			efList[i].type == 8 ||
			efList[i].type == 14 ||
			efList[i].type == 101 ||
			(efList[i].type == 6 && efList[i].number == 12);
			
		
		if (tooltipped)
		{
			im::SetNextItemWidth(width);
			im::InputScalar(("##0" + std::to_string(i)).c_str(), ImGuiDataType_S32, &efList[i].parameters[0], NULL, NULL, "%d", 0);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("X Offset");
			
			im::SameLine(); im::SetNextItemWidth(width);
			im::InputScalar(("##1" + std::to_string(i)).c_str(), ImGuiDataType_S32, &efList[i].parameters[1], NULL, NULL, "%d", 0);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Y Offset");
		}

		// PARAM 2 3
		if (efList[i].type == 1 || efList[i].type == 8 || efList[i].type == 101)
		{
			// A BIT
			int flagIndexA = -1;
			BitField("##2", reinterpret_cast<unsigned*>(&efList[i].parameters[2]), reinterpret_cast<unsigned*>(&flagIndexA), 8);
			switch(flagIndexA)
			{
				case 0: Tooltip("Clear when hit / _ObjFlags_EraseParentDamage"); break;
				case 2: Tooltip("Track parent position / _ObjFlags_ParentMove"); break;
				case 3: Tooltip("Has hitstop"); break;
				case 4: Tooltip("X/Y Anchors to middle of screen"); break;
				case 5: Tooltip("Clear when parent pattern changes / _ObjFlags_EraseParentPatChange"); break;
				case 6: Tooltip("Doesn't clip to floor / _ObjFlags_NoGround"); break;
				case 7: Tooltip("X/Y Anchors to floor"); break;
			}
		
			// B BIT
			int flagIndexB = -1;
			BitField("##3", reinterpret_cast<unsigned*>(&efList[i].parameters[3]), reinterpret_cast<unsigned*>(&flagIndexB), 10);	
			switch(flagIndexB)
			{
				case 1: Tooltip("Track parent position"); break;
				case 2: Tooltip("Clear when Thrown"); break;
				case 6: Tooltip("Does not freeze on screen freezes / _ObjFlags_MoveTimeStop"); break;
				case 7: Tooltip("Fixed position on screen / _ObjFlags_NoCamera"); break;
				case 8: Tooltip("X/Y Anchors to enemy"); break;
				case 9: Tooltip("X/Y middle of stage"); break;
				case 10: Tooltip("X/Y middle of stage with P1 Muki"); break;
			}
		}
		else { im::SetNextItemWidth(width*2); im::InputScalarN("##params23", ImGuiDataType_S32, efList[i].parameters+2, 2, NULL, NULL, "%d", 0); }

		// PARAM 4 5
		im::InputScalarN("##params45", ImGuiDataType_S32, efList[i].parameters+4, 2, NULL, NULL, "%d", 0);
		
		im::SameLine(); im::SetNextItemWidth(width);
		if(im::Button("Copy"))
		{
			*copied = efList[i];
		}
		
		// PARAM 6+
		im::InputScalarN("##params6p", ImGuiDataType_S32, efList[i].parameters+6, 6, NULL, NULL, "%d", 0);
		
        im::SameLine(); im::SetNextItemWidth(width);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1,0,0,0.4));
        if(im::Button("Delete"))
        {
            deleteI = i;
        }
        ImGui::PopStyleColor();

        im::PopID();
    };

    if(deleteI >= 0)
        efList.erase(efList.begin() + deleteI);

    im::Separator();
    if(im::Button("Add"))
        efList.push_back({});
}

inline void AsDisplay(Frame_AS *as)
{
	const char* const stateList[] = {
		"Standing",
		"Airborne",
		"Crouching"
	};

	const char* const cancelList[] = {
		"Never",
		"On Hit",
		"Always",
		"On Successful Hit"
	};

	const char* const counterList[] = {
		"No Change",
		"High Counter",
		"Low Counter",
		"Clear"
	};

	const char* const invulList[] = {
		"None",
		"High and Mid",
		"Low and Mid",
		"All but Throw",
		"Throw Only",
		"Unknown"
	};

	constexpr float width = 75.f;

	unsigned int flagIndex = -1;
	BitField("Movement Flags", &as->movementFlags, &flagIndex, 8);
	switch (flagIndex)
	{
		case 0: Tooltip("Set Y"); break;
		case 1: Tooltip("Add Y"); break;
		case 4: Tooltip("Set X"); break;
		case 5: Tooltip("Add X"); break;
	}

	im::SetNextItemWidth(width*2);
	im::InputInt2("Speed", as->speed); im::SameLine(0.f, 20); im::SetNextItemWidth(width); 
	im::InputInt("Max X speed", &as->maxSpeedX, 0, 0);
	im::SetNextItemWidth(width*2);
	im::InputInt2("Accel", as->accel);
	
	im::Separator();
	flagIndex = -1;
	BitField("Flagset 1", &as->statusFlags[0], &flagIndex);
	switch (flagIndex)
	{
		case 0: Tooltip("Vector influences other animations (dash momentum)"); break;
		case 1: Tooltip("Force clean vector (kill dash momentum)"); break;
		case 2: Tooltip("Don't transition to walking"); break;
		case 4: Tooltip("Can Ground Tech"); break;
		case 31: Tooltip("Vector initialization only at the beginning (?)"); break;
	}
	
	flagIndex = -1;
	BitField("Flagset 2", &as->statusFlags[1], &flagIndex);
	switch (flagIndex)
	{
		case 0: Tooltip("Can Always EX Cancel"); break;
		case 31: Tooltip("Can't Block"); break;
	}

	im::SetNextItemWidth(width);
	im::InputInt("Hits # Times", &as->hitsNumber, 0, 0); im::SameLine(0,20.f);
	im::SetNextItemWidth(width);
	im::InputInt("ASCF", &as->ascf, 0, 0);
	im::Checkbox("Able to Move", &as->canMove); //
	im::Combo("State", &as->stanceState, stateList, IM_ARRAYSIZE(stateList));
	im::Combo("Invincibility", &as->invincibility, invulList, IM_ARRAYSIZE(invulList));
	im::Combo("Counterhit", &as->counterType, counterList, IM_ARRAYSIZE(counterList)); 
	im::Combo("Normal Cancel", &as->cancelNormal, cancelList, IM_ARRAYSIZE(cancelList));
	im::Combo("Special Cancel", &as->cancelSpecial, cancelList, IM_ARRAYSIZE(cancelList));
	

	im::Separator();
	flagIndex = -1;
	BitField("Sine Flags", &as->sineFlags, &flagIndex, 8);
	switch (flagIndex)
	{
		case 0: Tooltip("Use Y"); break;
		case 4: Tooltip("Use X"); break;
	}
	im::InputInt4("Sinewave", as->sineParameters); im::SameLine();
	im::TextDisabled("(?)");
	if(im::IsItemHovered())
		Tooltip("Sine parameters:\nX Distance, Y Amplitude\nX Frequency, Y Frequency");
	im::InputFloat2("Phases", as->sinePhases);
}

inline void AtDisplay(Frame_AT *at)
{
	const char* const hitEffectList[] = {
		"Weak Punch",                    // 0
		"Medium Punch",                  // 1
		"Strong Punch",                  // 2
		"Weak Kick",                     // 3
		"Medium Kick",                   // 4
		"Strong Kick",                   // 5
		"Super Punch",                   // 6
		"Super Kick",                    // 7
		"Old Cut",                       // 8
		"Burn",                          // 9
		"Freeze",                        // 10
		"Shock",                         // 11
		"",                         	 // 12
		"",                         	 // 13
		"",                         	 // 14
		"Medium Kick",                   // 15
		"Double Cut",                    // 16
		"Super Cut (Full Screen)",       // 17
		"Weak Cut",                      // 18
		"Medium Cut",                    // 19
		"Strong Cut",                    // 20
		"Feint Cut (Different SE)",      // 21
		"Strong Cut?",                   // 22
		"Weak Cut?",                     // 23
		"Medium Cut?",                   // 24
		"Byakuya Cut",                   // 25
		"Medium Punch?",                 // 26
		"Medium Punch?",                 // 27
		"EX Punch Finisher",             // 28
		"EX Kick Finisher",              // 29
		"EX Cut Finisher",               // 30
		"EX F.Cut Finisher",             // 31
		"",                              // 32
		"",                              // 33
		"",                              // 34
		"",                              // 35
		"",                              // 36
		"F.Cut Medium",                  // 37
		"F.Cut Medium",                  // 38
		"Heavy Punch",                   // 39
		"Heavy Kick",                    // 40
		"",                              // 41
		"",                              // 42
		"",                              // 43
		"",                              // 44
		"Small Punch",                   // 45
		"Beeg Punch",                    // 46
		"Strong Punch Center",           // 47
		"Strong Punch Center",           // 48
		"Strong Punch Center",           // 49
		"Finisher Big Punch Radial/Center Hit", // 50
		""                              // 51
		// 52+ unlike other NULL values these don't even have sparks
	};


	const char* const addedEffectList[] = {
		"None",
		"Burn",
		"Freeze",
		"Shock",
		"Confusion"
	};

	const char* const vectorFlags[] = {
		"Normal",
		"No X Movement",
		"Reverse X Movement",
		"Both(?)"
	};

	const char* const hitStopList[] = {
		"Weakest",
		"Weak",
		"Medium (5F)",
		"None",
		"Strong (10F)",
		"Very Strong (20F)",
		"",
		"",
		"Medium (Ogre?)"
	};
	
	constexpr float width = 75.f;
	unsigned int flagIndex = -1;

	BitField("Guard Flags", &at->guard_flags, &flagIndex);
	switch (flagIndex)
	{
		case 0: Tooltip("Stand Blockable"); break;
		case 1: Tooltip("Air Blockable"); break;
		case 2: Tooltip("Crouch Blockable"); break;
		
		case 4: Tooltip("Always Stand-Shieldable"); break;
		case 5: Tooltip("Always Air-Shieldable"); break;
		case 6: Tooltip("Always Crouch-Shieldable"); break;
		
		case 8: Tooltip("Miss if enemy is Standing"); break;
		case 9: Tooltip("Miss if enemy is Airborne"); break;
		case 10: Tooltip("Miss if enemy is Crouching"); break;
		case 11: Tooltip("Miss if enemy is in Hitstun"); break;
		case 12: Tooltip("Miss if enemy is in Blockstun"); break;
		case 13: Tooltip("Miss if OTG"); break;
		case 14: Tooltip("Hit only in hitstun?"); break;
		case 15: Tooltip("Can't hit playable character?"); break;
	}

	flagIndex = -1;
	BitField("Hit Flags", &at->otherFlags, &flagIndex);
	switch (flagIndex)
	{
		case 0: Tooltip("Deals Chip Damage"); break;
		case 1: Tooltip("Can't KO"); break;
		case 2: Tooltip("Make enemy Unhittable"); break;
		case 3: Tooltip("Can't be clashed with"); break;
		case 4: Tooltip("Auto super jump cancel"); break;
		case 5: Tooltip("Don't Increase Combo Hit Counter"); break;
		case 6: Tooltip("Shake the Screen on Hit"); break;
		case 7: Tooltip("Not Air Techable"); break;
		case 8: Tooltip("Not Ground Techable (HKD)"); break;

		case 10: Tooltip("No Self Hitstop"); break;
		
		case 16: Tooltip("Use custom blockstop"); break;
		case 17: Tooltip("OTG Relaunches"); break;
		case 18: Tooltip("Can't Counter-Hit"); break;

		//Not in AACC.
		case 25: Tooltip("No Hitstop on Multihit?"); break; 
		case 29: Tooltip("Block enemy blast during Stun?"); break;
	}

	flagIndex = -1;
	BitField("Hitgrab Flags", &at->hitgrab, &flagIndex, 8);
	switch (flagIndex)
	{
		case 0: Tooltip("Enabled"); break;
		case 1: Tooltip("Target collision"); break;
		case 2: Tooltip("Target origin"); break;
		case 3: Tooltip("Ignore player invulnerability timer"); break;
	}

	im::SetNextItemWidth(width);
	im::InputInt("Blockstop", &at->blockStopTime, 0,0); im::SameLine(0.f, 20); im::SetNextItemWidth(width);
	im::InputInt("Custom##Hitstop", &at->hitStopTime, 0,0);

	im::SetNextItemWidth(width*1.2);
	im::Combo("Preset Hitstop", &at->hitStop, hitStopList, IM_ARRAYSIZE(hitStopList)); im::SameLine();	im::SetNextItemWidth(width);
	im::InputInt("ID", &at->hitStop, 0,0);

	im::SetNextItemWidth(width);
	im::InputInt("Set Hitstun", &at->hitStun, 0,0);  im::SameLine(0.f, 20); im::SetNextItemWidth(width);
	im::InputInt("Self Hitstop", &at->addHitStun, 0,0);

	im::SetNextItemWidth(width*2);
	im::InputInt2("Hitstun Decay", at->hitStunDecay); // Was set to 3 before but it's unused in UNI2 so let's see what happens
	im::SameLine(); im::TextDisabled("(?)");
	if(im::IsItemHovered())
		Tooltip("1. Set % of current hitstun scaling out of 100. The lower, the shorter scaling remaining.\n2. Combopoint set if combo starter. The higher, the stronger hitstun.");
	im::SameLine();
	im::SetNextItemWidth(width/2);
	im::InputInt("Untech Time", &at->untechTime, 0,0);
	// im::InputInt("Circuit break time", &at->breakTime, 0,0);


	im::SetNextItemWidth(width);
	// im::InputFloat("Extra gravity", &at->extraGravity, 0,0); im::SameLine(0.f, 20); im::SetNextItemWidth(width);
	im::InputInt("Correction %", &at->correction2, 0, 0); im::SameLine();
	im::SetNextItemWidth(width);
	im::InputInt("Starter Correction", &at->correction, 0, 0);
	// im::Combo("Type##Correction", &at->correction_type, "Normal\0Multiplicative\0Substractive\0");

	im::SetNextItemWidth(width);
//	im::InputInt("VS damage", &at->red_damage, 0, 0); im::SameLine(0.f, 20); im::SetNextItemWidth(width);
	im::InputInt("Damage", &at->damage, 0, 0); im::SameLine(0.f, 20); im::SetNextItemWidth(width);
/* 
	im::SetNextItemWidth(width);
	im::InputInt("Guard damage", &at->guard_damage, 0, 0); im::SameLine(0.f, 20); im::SetNextItemWidth(width);
	im::InputInt("Meter gain", &at->meter_gain, 0, 0); im::SetNextItemWidth(width*2); */
	im::InputInt("Minimum Damage %", &at->minDamage, 0, 0);

	im::Separator();
	auto comboWidth = (im::GetWindowWidth())/4.f;
	im::InputInt3("Guard Vector", at->guardVector);
	im::SameLine(); im::TextDisabled("(?)");
	if(im::IsItemHovered())
		Tooltip("What happens to the enemy when they block the attack.\nFor ground, air, and crouching.\nThese will typically be 15X, 17X, 16X.\nSee vectors documentation file.");
	//im::InputInt3("GV flags", at->gVFlags);
	for(int i = 0; i < 3; i++)
	{	
		im::SetNextItemWidth(comboWidth);
		if(i > 0)
			im::SameLine();
		im::PushID(i); 
		im::Combo("##GFLAG", &at->gVFlags[i], vectorFlags, IM_ARRAYSIZE(vectorFlags));
		im::PopID();
	}

	im::Separator();
	im::InputInt3("Hit Vector", at->hitVector);
	//im::InputInt3("HV flags", at->hVFlags);
	im::SameLine(); im::TextDisabled("(?)");
	if(im::IsItemHovered())
		Tooltip("What happens to the enemy when they're hit.\nFor ground, air, and crouching.\nSee vectors documentation file.");
	
	for(int i = 0; i < 3; i++)
	{	
		im::SetNextItemWidth(comboWidth);
		if(i > 0)
			im::SameLine();
		im::PushID(i); 
		im::Combo("##HFLAG", &at->hVFlags[i], vectorFlags, IM_ARRAYSIZE(vectorFlags));
		im::PopID();
	}
	im::Separator();
	
	im::SetNextItemWidth(150);
	im::Combo("Hit Effect", &at->hitEffect, hitEffectList, IM_ARRAYSIZE(hitEffectList)); im::SameLine(0, 20.f);
	im::SetNextItemWidth(70);
	im::InputInt("ID##Hit Effect", &at->hitEffect, 0, 0); 
	
	im::SetNextItemWidth(70);
	im::InputInt("Sound Effect", &at->soundEffect, 0, 0); im::SetNextItemWidth(120);

	im::Combo("Added Effect", &at->addedEffect, addedEffectList, IM_ARRAYSIZE(addedEffectList)); im::SameLine(0, 20.f);
	im::SetNextItemWidth(70);
	im::InputInt("ID##Added Effect", &at->addedEffect, 0, 0);



}

inline void AfDisplay(Frame_AF *af, int &selectedLayer)
{
	const char* const interpolationList[] = {
		"None",
		"Linear",
		"Slow->Fast",
		"Fast->Slow",
		"Fast Middle",
		"Slow Middle", //Never used, but it works.
	};

	const char* const animationList[] = {
		"Go to Pattern",
		"Next Frame",
		"Go to Frame",
		"End"
	};

	constexpr float width = 50.f;

	im::SetNextItemWidth(width*3);
	const int maxLayers = af->layers.size();
	im::SliderInt("Layer", &selectedLayer, 0, maxLayers-1);
	if(selectedLayer >= maxLayers)
		selectedLayer = maxLayers-1;
	if(selectedLayer < 0)
		selectedLayer = 0;
	
	im::SameLine(0,20.f); im::SetNextItemWidth(width/2);
	if(im::Button("Add"))
	{
		af->layers.push_back({});
			selectedLayer++;
	}
	im::SameLine(0,0.f); im::SetNextItemWidth(width/2);
	if(im::Button("Del") && maxLayers > 1)
	{
		af->layers.erase(af->layers.begin()+selectedLayer);
		if(selectedLayer >= af->layers.size())
			selectedLayer--;
	}

	auto *layer = &af->layers[selectedLayer];

	im::SetNextItemWidth(width*3);
	im::InputInt("Sprite", &layer->spriteId);
	im::SameLine(0, 20.f);
	im::Checkbox("Use .pat", &layer->usePat);

	im::Separator();

	im::SetNextItemWidth(width);
	im::DragInt("X", &layer->offset_x);
	im::SameLine();
	im::SetNextItemWidth(width);
	im::DragInt("Y", &layer->offset_y);

	int mode = layer->blend_mode-1;
	if(mode < 1)
		mode = 0;
	if (im::Combo("Blend Mode", &mode, "Normal\0Additive\0Substractive\0"))
	{
		layer->blend_mode=mode+1;
	}
	im::ColorEdit4("Color", layer->rgba);

	im::DragFloat3("Rot XYZ", layer->rotation, 0.005); 
	im::DragFloat2("Scale", layer->scale, 0.1);
	im::InputInt("Layer priority", &layer->priority);
	im::Separator();

	im::Checkbox("Rotation keeps scale set by EF", &af->AFRT);
	im::Checkbox("AFJH", &af->afjh);
	im::InputScalarN("AF Params", ImGuiDataType_U8, af->param, 4, NULL, NULL, "%hhu", 0);
	im::SetNextItemWidth(width);
	im::InputInt("Frame id", &af->frameId, 0, 0);

	unsigned int flagIndex = -1;
	BitField("Animation flags", &af->aniFlag, &flagIndex, 8);
	switch (flagIndex)
	{
		case 0: Tooltip("Land to pattern?"); break;
		case 1: Tooltip("Check loop counter"); break;
		case 2: Tooltip("Go to relative offset"); break;
		case 3: Tooltip("Relative end of loop"); break;
		case 7: Tooltip("Unknown"); break;
	}

	im::Combo("Animation", &af->aniType, animationList, IM_ARRAYSIZE(animationList));

	im::SetNextItemWidth(width);
	im::InputInt("Go to", &af->jump, 0, 0); im::SameLine(0.f, 20); im::SetNextItemWidth(width);
	im::InputInt("Landing frame", &af->landJump, 0, 0);

	im::SetNextItemWidth(width);
	im::InputInt("Z-Priority", &af->priority, 0, 0); im::SetNextItemWidth(width);
	im::InputInt("Loop N times", &af->loopCount, 0, 0); im::SameLine(0,20); im::SetNextItemWidth(width);
	im::InputInt("End of loop", &af->loopEnd, 0, 0);
	im::InputInt("Duration", &af->duration, 1, 0);

	
	im::Combo("Interpolation", &af->interpolationType, interpolationList, IM_ARRAYSIZE(interpolationList));
	im::SameLine(); im::TextDisabled("(?)");
	if(im::IsItemHovered())
		Tooltip("How the current animation parameters transition to the next frame during this frame's duration.\nTry to look up interpolation examples online.");
}