 

#include "C4Dialog.h"


using namespace C4;


Dialog::Dialog(const Vector2D& size, const char *title, const char *okayText, const char *cancelText, const char *ignoreText) :
		Window(size, title, kWindowBackground | kWindowCenter),
		okayButton(Vector2D(72.0F, 24.0F), okayText, "font/Heading"),
		cancelButton(Vector2D(72.0F, 24.0F), cancelText, "font/Heading"),
		ignoreButton(Vector2D(72.0F, 24.0F), ignoreText, "font/Heading")
{
	okayButton.SetPushButtonFlags(kPushButtonPrimary);
	okayButton.SetActivationKey(kActivationKeyEnter);
	okayButton.SetWidgetPosition(Point3D(size.x - 84.0F, size.y - 32.0F, 0.0F));
	AppendSubnode(&okayButton);

	if (cancelText)
	{
		cancelButton.SetActivationKey(kActivationKeyEscape);
		cancelButton.SetWidgetPosition(Point3D(size.x - 172.0F, size.y - 32.0F, 0.0F));
		AppendSubnode(&cancelButton);
	}
	else
	{
		SetActivationKeyWidget(kActivationKeyEscape, &okayButton);
	}

	if (ignoreText)
	{
		ignoreButton.SetWidgetPosition(Point3D(12.0F, size.y - 32.0F, 0.0F));
		AppendSubnode(&ignoreButton);
	}

	dialogStatus = kDialogOkay;
	ignoreKeyCode = 0;
}

Dialog::~Dialog()
{
}

bool Dialog::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;
		if ((keyCode != 0) && (keyCode == ignoreKeyCode))
		{
			ignoreButton.Activate();
			return (true);
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void Dialog::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == &okayButton)
		{
			TheInterfaceMgr->RemoveWidget(this);
			dialogStatus = kDialogOkay;
			CallCompletionProc();
			Close();
		}
		else if (widget == &cancelButton)
		{
			TheInterfaceMgr->RemoveWidget(this);
			dialogStatus = kDialogCancel;
			CallCompletionProc();
			Close();
		}
		else if (widget == &ignoreButton)
		{
			TheInterfaceMgr->RemoveWidget(this);
			dialogStatus = kDialogIgnore;
			CallCompletionProc();
			Close();
		}
	}
}


ErrorDialog::ErrorDialog(const char *title, const char *message, const char *error, const char *filename) : Dialog(Vector2D(400.0F, 176.0F), title, TheInterfaceMgr->GetStringTable()->GetString(StringID('OKAY')))
{
	ImageWidget *image = new ImageWidget(Vector2D(64.0F, 64.0F), "C4/error");
	image->SetWidgetPosition(Point3D(12.0F, 12.0F, 0.0F));
	AppendSubnode(image);

	TextWidget *text = new TextWidget(Vector2D(300.0F, 0.0F), message, "font/Gui");
	text->SetTextFlags(kTextWrapped);
	text->SetWidgetPosition(Point3D(88.0F, 16.0F, 0.0F));
	AppendSubnode(text);

	text = new TextWidget(Vector2D(300.0F, 0.0F), error, "font/Gui");
	text->SetTextFlags(kTextWrapped);
	text->SetWidgetPosition(Point3D(88.0F, 64.0F, 0.0F));
	AppendSubnode(text); 

	if (filename)
	{ 
		text = new TextWidget(Vector2D(300.0F, 0.0F), filename, "font/Gui");
		text->SetTextFlags(kTextWrapped); 
		text->SetWidgetPosition(Point3D(88.0F, 112.0F, 0.0F));
		AppendSubnode(text);
	} 
}
 
ErrorDialog::~ErrorDialog() 
{
}

// ZYUQURM
