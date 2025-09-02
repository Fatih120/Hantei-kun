#include "main_pane.h"
#include "pattern_disp.h"
#include "frame_disp.h"
#include <imgui.h>


MainPane::MainPane(Render* render, FrameData *framedata, FrameState &fs) : DrawWindow(render, framedata, fs),
decoratedNames(nullptr)
{
	
}

void MainPane::RegenerateNames()
{
	delete[] decoratedNames;
	
	if(frameData && frameData->m_loaded)
	{
		decoratedNames = new std::string[frameData->get_sequence_count()];
		int count = frameData->get_sequence_count();

		for(int i = 0; i < count; i++)
		{
			decoratedNames[i] = frameData->GetDecoratedName(i);
		}
	}
	else
		decoratedNames = nullptr;
}

void MainPane::Draw()
{	
	namespace im = ImGui;
	im::Begin("Left Pane",0);
	if(frameData->m_loaded)
	{
		if (im::BeginCombo("Pattern", decoratedNames[currState.pattern].c_str(), ImGuiComboFlags_HeightLargest))
		{
			auto count = frameData->get_sequence_count();
			for (int n = 0; n < count; n++)
			{
				const bool is_selected = (currState.pattern == n);
				if (im::Selectable(decoratedNames[n].c_str(), is_selected))
				{
					currState.pattern = n;
					currState.frame = 0;
					currState.nextFrame = 0;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					im::SetItemDefaultFocus();
			}
			im::EndCombo();
		}
		auto seq = frameData->get_sequence(currState.pattern);
		if(seq)
		{
			int nframes = seq->frames.size() - 1;
			if(nframes >= 0)
			{			
				float spacing = im::GetStyle().ItemInnerSpacing.x;
				im::SetNextItemWidth(im::GetWindowWidth() - 160.f);
				im::SliderInt("##frameSlider", &currState.frame, 0, nframes);
				im::SameLine();
				im::PushButtonRepeat(true);
				if(im::ArrowButton("##left", ImGuiDir_Left))
					currState.frame--;
				im::SameLine(0.0f, spacing);
				if(im::ArrowButton("##right", ImGuiDir_Right))
					currState.frame++;
				im::PopButtonRepeat();
				im::SameLine();
				im::Text("%d/%d", currState.frame+1, nframes+1);

				if(currState.frame < 0)
					currState.frame = 0;
				else if(currState.frame > nframes)
					currState.frame = nframes;
				
				if(im::Button("Animate"))
				{
					currState.animating = !currState.animating;
					currState.animeSeq = currState.pattern;
				}
			}
			else
			{
				im::Text("This pattern has no frames.");
				if(im::Button("Add Frame"))
				{
					seq->frames.push_back({});
					seq->frames.back().AF.layers.resize(3);
					currState.frame = 0;
				}
			}

			im::BeginChild("FrameInfo", {0, im::GetWindowSize().y-im::GetFrameHeight()*4-8}, false, ImGuiWindowFlags_HorizontalScrollbar);
			if (im::TreeNode("Pattern Data"))
			{
				if(PatternDisplay(seq))
				{
					decoratedNames[currState.pattern] = frameData->GetDecoratedName(currState.pattern);
				}

				if(im::Button("Copy Pattern")){
					currState.copied->pattern = *seq;
				}
				im::SameLine(0,20.f); 
				if(im::Button("Paste Pattern")){
					*seq = currState.copied->pattern;
					decoratedNames[currState.pattern] = frameData->GetDecoratedName(currState.pattern);
					nframes = seq->frames.size() - 1;
				}

				if(im::Button("Push Pattern"))
				{
					patCopyStack.push_back(SequenceWId{currState.pattern, *seq});
				}
				im::SameLine(0,20.f);
				if(im::Button("Pop Patterns"))
				{
					PopCopies();
					RegenerateNames();
					nframes = seq->frames.size() - 1;
				}
				im::SameLine(0,20.f);
				im::Text("%llu copies", patCopyStack.size());
				
				im::SameLine(); im::TextDisabled("(?)");
				if(im::IsItemHovered())
					Tooltip("Copies and pastes patterns to a stack/queue.\nLets you copy patterns by their IDs, then paste multiple patterns back over those copied IDs.\nUsed for copying patterns across several HA6 files into one.");
				
				im::TreePop();
				im::Separator();
			}
			if(nframes >= 0)
			{
				if(currState.frame > nframes)
					currState.frame = nframes;
				Frame &frame = seq->frames[currState.frame];
				if(im::TreeNode("State Data"))
				{
					AsDisplay(&frame.AS);
					if(im::Button("Copy AS")){
						currState.copied->as = frame.AS;
					}

					im::SameLine(0,20.f); 
					if(im::Button("Paste AS")){
						frame.AS = currState.copied->as;
					}
					im::TreePop();
					im::Separator();
				}
				if (im::TreeNode("Animation Data"))
				{
					AfDisplay(&frame.AF, currState.selectedLayer);
					if(im::Button("Copy AF")){
						currState.copied->af = frame.AF;
					}
					im::SameLine(0,20.f); 
					if(im::Button("Paste AF")){
						frame.AF = currState.copied->af;
					}
					
				    if (im::Button("Copy AF Layer")){
						if (frame.AF.layers.size() > currState.selectedLayer)
						{
							currState.af_layer = frame.AF.layers[currState.selectedLayer];
						}
					}
					im::SameLine(0,20.f);
					if (im::Button("Paste AF Layer") && frame.AF.layers.size() > currState.selectedLayer) {
						frame.AF.layers[currState.selectedLayer] = currState.af_layer;
					}
					static int copied_x = 0, copied_y = 0;
					if (im::Button("Copy X")) {
						if (frame.AF.layers.size() > currState.selectedLayer) {
							copied_x = frame.AF.layers[currState.selectedLayer].offset_x;
						}
					}
					im::SameLine(0,20.f);
					if (im::Button("Paste X")) {
						if (frame.AF.layers.size() > currState.selectedLayer) {
							frame.AF.layers[currState.selectedLayer].offset_x = copied_x;
						}
					}
					im::SameLine(0,20.f);
					if (im::Button("Copy Y")) {
						if (frame.AF.layers.size() > currState.selectedLayer) {
							copied_y = frame.AF.layers[currState.selectedLayer].offset_y;
						}
					}
					im::SameLine(0,20.f);
					if (im::Button("Paste Y")) {
						if (frame.AF.layers.size() > currState.selectedLayer) {
							frame.AF.layers[currState.selectedLayer].offset_y = copied_y;
						}
					}
					
					im::TreePop();
					im::Separator();
				}
				

				if (im::TreeNode("Tools"))
				{
					im::Checkbox("Copy Current Frame", &copyThisFrame);
					
					if(im::Button("Append Frame"))
					{
						if(copyThisFrame)
							seq->frames.push_back(frame);
						else
						{
							seq->frames.push_back({});
							seq->frames.back().AF.layers.resize(3);
						}
					}

					im::SameLine(0,20.f); 
					if(im::Button("Insert Frame"))
					{
						if(copyThisFrame)
							seq->frames.insert(seq->frames.begin()+currState.frame, frame);
						else
						{
							seq->frames.insert(seq->frames.begin()+currState.frame, 1, {});
							seq->frames[currState.frame].AF.layers.resize(3);
						}
					}

					im::SameLine(0,20.f);
					if(im::Button("Range tool"))
					{
						afjcRangeVal = 0;
						ranges[0] = 0;
						ranges[1] = 0;
						rangeWindow = !rangeWindow;
					}

					if(im::Button("Copy frame"))
					{
						currState.copied->frame = frame;
					}
					im::SameLine(0,20.f);
					if(im::Button("Paste frame"))
					{
						frame = currState.copied->frame;
					}
					im::SameLine(0,20.f);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1,0,0,0.4));
					if(im::Button("Delete Frame"))
					{
						seq->frames.erase(seq->frames.begin()+currState.frame);
						if(currState.frame >= seq->frames.size())
							currState.frame--;
					}
					
					im::TreePop();
					im::Separator();
				}
			}
			else
				rangeWindow = false;
			im::EndChild();
		}
	}
	else
		im::Text("Load some data first.");

	im::End();

	if(rangeWindow)
	{
		auto seq = frameData->get_sequence(currState.pattern);
		if(ranges[0] < 0)
			ranges[0] = 0;
		if(ranges[1] < 0)
			ranges[1] = 0;
		if(ranges[0] > ranges[1])
			ranges[0] = ranges[1];
		if(ranges[0] + ranges[1] == 0)
			ranges[1] = seq->frames.size()-1;

		im::SetNextWindowSize(ImVec2{400,120}, ImGuiCond_FirstUseEver);
		im::Begin("Range paste", &rangeWindow);
		im::InputInt2("Range of frames", ranges);
		if(im::Button("Paste color"))
		{
			auto &iLayer = seq->frames[currState.frame].AF.layers[currState.selectedLayer];
			for(int i = ranges[0]; i <= ranges[1] && i >= 0 && i < seq->frames.size(); i++)
			{
				for(auto &oLayer : seq->frames[i].AF.layers)
				{
					memcpy(oLayer.rgba, iLayer.rgba, sizeof(float)*4);
					oLayer.blend_mode = iLayer.blend_mode;
				}
			}
		}
		if(im::Button("Set Landing Frame"))
		{
			for(int i = ranges[0]; i <= ranges[1] && i >= 0 && i < seq->frames.size(); i++)
				seq->frames[i].AF.landJump = afjcRangeVal;
		}
		im::SameLine(); im::SetNextItemWidth(100);
		im::InputInt("Value##AFJC", &afjcRangeVal);

		if(im::Button("Copy Frames"))
		{
			currState.copied->frames.clear();
			for(int i = ranges[0]; i <= ranges[1] && i >= 0 && i < seq->frames.size(); i++)
				currState.copied->frames.push_back(seq->frames[i]);
		}
		im::SameLine();
		if(im::Button("Paste Frames Before End Range"))
		{
			if(!currState.copied->frames.empty())
			{
				int pos = ranges[1];
				if(pos > seq->frames.size())
					pos = seq->frames.size();
				seq->frames.insert(seq->frames.begin()+pos, currState.copied->frames.begin(), currState.copied->frames.end());
			}
		}
		if(im::Button("Paste transform (Current layer only)"))
		{
			for(int i = ranges[0]; i <= ranges[1] && i >= 0 && i < seq->frames.size(); i++)
			{
				if(seq->frames[i].AF.layers.size() > currState.selectedLayer)
				{
					seq->frames[i].AF.layers[currState.selectedLayer].offset_x = seq->frames[currState.frame].AF.layers[currState.selectedLayer].offset_x;
					seq->frames[i].AF.layers[currState.selectedLayer].offset_y = seq->frames[currState.frame].AF.layers[currState.selectedLayer].offset_y;

					memcpy(seq->frames[i].AF.layers[currState.selectedLayer].scale, seq->frames[currState.frame].AF.layers[currState.selectedLayer].scale, sizeof(float)*2);
					memcpy(seq->frames[i].AF.layers[currState.selectedLayer].rotation, seq->frames[currState.frame].AF.layers[currState.selectedLayer].rotation, sizeof(float)*2);
				}
			}
		}
		if(im::Button("Set Z-Priority"))
		{
			for (int i = ranges[0]; i <= ranges[1] && i >= 0 && i < seq->frames.size(); i++)
			{
				auto &layers = seq->frames[i].AF.layers;
				for (size_t layerIndex = 0; layerIndex < layers.size(); ++layerIndex) {
					layers[layerIndex].priority = 10;
				}
			}
		}
		if(im::Button("Set X"))
		{
			auto &iLayer = seq->frames[currState.frame].AF.layers[currState.selectedLayer];
			for(int i = ranges[0]; i <= ranges[1] && i >= 0 && i < seq->frames.size(); i++)
			{
				for(auto &oLayer : seq->frames[i].AF.layers)
				{
					oLayer.offset_x = xoffRange;
				}
			}
		}
		im::SameLine();
		im::SetNextItemWidth(100);
		im::InputInt("##X", &xoffRange);
		
		im::SameLine();
		if(im::Button("Set Y"))
		{
			auto &iLayer = seq->frames[currState.frame].AF.layers[currState.selectedLayer];
			for(int i = ranges[0]; i <= ranges[1] && i >= 0 && i < seq->frames.size(); i++)
			{
				for(auto &oLayer : seq->frames[i].AF.layers)
				{
					oLayer.offset_y = yoffRange;
				}
			}
		}
		im::SameLine();
		im::SetNextItemWidth(100);
		im::InputInt("##Y", &yoffRange);
		im::End();
	}
}

void MainPane::PopCopies()
{
	for(auto &pat : patCopyStack)
	{
		*frameData->get_sequence(pat.id) = pat.seq;
	}
	patCopyStack.clear();
}