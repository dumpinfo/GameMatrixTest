#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Widget;
	//class GUIEvent;

	class GUI : public Object//lua
	{
	public:
		std::list<GUI*>::iterator link;
		bool mousepositioninvalidated;
		iVec2 mouseposition;
		int mousestate[3];
		Widget* root;
		Widget* hoverwidget;
		Widget* activewidget;
		bool hidden;
		Sound* sound_click;
		Sound* sound_rollover;
		unsigned int updatecount;
		unsigned int hoversoundupdatecount;

		GUI();
		virtual ~GUI();
		
		virtual void Show();//lua
		virtual void Hide();//lua
		virtual std::string GetClassName();
		virtual void Update();//lua
		//virtual bool PeekEvent();
		//virtual GUIEvent GetEvent();
		//virtual void EmitEvent(GUIEvent guievent);
		virtual void Update(const int mousex, const int mousey, const int mousestate0, const int mousestate2, const int mousestate3);//lua
		virtual void Draw(Context* context);//lua
		Widget* GetRoot();//lua
		
		static GUI* Create();//lua

		static GUI* Master;
		static std::list<GUI*> list;
	};
}