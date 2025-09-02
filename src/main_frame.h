#ifndef MAINFRAME_H_GUARD
#define MAINFRAME_H_GUARD
#include "context_gl.h"
#include "render.h"
#include "main_pane.h"
#include "right_pane.h"
#include "box_pane.h"
#include "about.h"
#include "framedata.h"
#include "cg.h"
#include "parts.h"
#include <glm/mat4x4.hpp>
#include <string>


class MainFrame
{
public:
	MainFrame(ContextGl *context);
	~MainFrame();
	
	void Draw();
	void UpdateBackProj(float x, float y);
	void HandleMouseDrag(int x, int y, bool dragRight, bool dragLeft);
	bool HandleKeys(uint64_t vkey);

	void RightClick(int x, int y);
	void AdjustBoxes(int dx, int dy, int mode);

	void LoadSettings();

	bool drawImgui = true;
	

private:
	ContextGl *context;
	float clearColor[3];
	int style_idx = 0;
	int zoom_idx = 1;
	bool smoothRender = false; 
	

	Render render;
	FrameData framedata;
	FrameState currState{};
	CG cg;
	Parts parts;
	int curPalette;
	float setScaleFactor = 1.0;
	float reScaleFactor = 1.0;

	//Animation
	int duration = 0;
	float interpolationFactor = 0;
	int loopCounter;

	//Option

	std::string currentFilePath;

	void DrawBack();
	void DrawUi();
	void Menu(unsigned int errorId);

	void RenderUpdate();
	void AdvancePattern(int dir);
	void AdvanceFrame(int dir);
	void ChangeOffset(int x, int y);

	void SetZoom(int level);
	void UpdateZoom();
	void LoadTheme(int i );
	void WarmStyle();
	void LeafStyle();
	void ChangeClearColor(float r, float g, float b);

	int mDeltaX = 0, mDeltaY = 0;
	int x=0, y=0;
	

	MainPane mainPane;
	RightPane rightPane;
	BoxPane boxPane;
	AboutWindow aboutWindow;
	HelpWindow helpWindow;
};


#endif /* MAINFRAME_H_GUARD */
