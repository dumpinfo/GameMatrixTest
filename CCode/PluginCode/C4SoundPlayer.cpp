//=============================================================
//
// C4 Engine version 4.5
// Copyright 1999-2015, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#include "C4SoundPlayer.h"
#include "C4Movies.h"


using namespace C4;


List<SoundWindow> SoundWindow::windowList;


SoundWidget::SoundWidget(const Vector2D& size, int32 count, const ColorRGBA& color) :
		RenderableWidget(kWidgetSound, kRenderQuads, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		diffuseAttribute(color)
{
	quadCount = count;
}

SoundWidget::~SoundWidget()
{
}

void SoundWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	int32 vertexCount = quadCount * 4;
	SetVertexCount(vertexCount);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(Point2D));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	vertexBuffer.Establish(sizeof(Point2D) * vertexCount);

	attributeList.Append(&diffuseAttribute);
	SetMaterialAttributeList(&attributeList);
}

void SoundWidget::BuildSound(const Sound *sound, int32 channel)
{
	volatile Point2D *restrict vertex = vertexBuffer.BeginUpdate<Point2D>();

	float x1 = 0.0F;
	float h = GetWidgetSize().y - 1.0F;
	float y0 = h * 0.5F;
	float scale = h / 65536.0F;

	int32 channelCount = sound->GetChannelCount();

	if (!sound->Streaming())
	{
		const Sample *data = sound->GetSampleData() + channel;
		float interval = (float) sound->GetFrameCount() / (float) quadCount;

		for (machine a = 0; a < quadCount; a++)
		{
			float f = (float) a * interval;
			int32 p1 = (int32) f;
			int32 p2 = (int32) (f + interval);
			int32 value = Abs(ReadLittleEndianS16(&data[p1 * channelCount]));
			for (machine p = p1 + 1; p < p2; p++)
			{
				value = Max(value, Abs(ReadLittleEndianS16(&data[p * channelCount])));
			}

			float y = (float) value * scale;
			float y1 = y0 - y;
			float y2 = y0 + y + 1.0F;
			float x2 = x1 + 1.0F;

			vertex[0].Set(x1, y1);
			vertex[1].Set(x1, y2);
			vertex[2].Set(x2, y2);
			vertex[3].Set(x2, y1);

			x1 = x2;
			vertex += 4;
		}
	}
	else
	{
		int32 count = quadCount / 2;
		float interval = (float) (sound->GetSoundStreamer()->GetStreamBufferSize() / (channelCount * sizeof(Sample))) / (float) count;

		for (machine buffer = 0; buffer < 2; buffer++)
		{
			const Sample *data = sound->GetSoundStreamer()->GetStreamBuffer(buffer)->GetSampleData() + channel;

			for (machine a = 0; a < count; a++)
			{
				float f = (float) a * interval;
				int32 p1 = (int32) f;
				int32 p2 = (int32) (f + interval);
				int32 value = Abs(ReadLittleEndianS16(&data[p1 * channelCount]));
				for (machine p = p1 + 1; p < p2; p++)
				{
					value = Max(value, Abs(ReadLittleEndianS16(&data[p * channelCount])));
				}

				float y = (float) value * scale;
				float y1 = y0 - y;
				float y2 = y0 + y + 1.0F;
				float x2 = x1 + 1.0F; 

				vertex[0].Set(x1, y1);
				vertex[1].Set(x1, y2); 
				vertex[2].Set(x2, y2);
				vertex[3].Set(x2, y1); 

				x1 = x2;
				vertex += 4; 
			}
		} 
	} 

	vertexBuffer.EndUpdate();
}
 

SoundWindow::SoundWindow(const char *name) : Window("SoundTool/Window")
{
	SoundResult		result;

	resourceName = name;
	windowList.Append(this);

	ResourcePath title(name);
	SetWindowTitle(title += SoundResource::GetDescriptor()->GetExtension());
	SetStripTitle(&title[Text::GetDirectoryPathLength(title)]);
	SetStripIcon("SoundTool/window");

	soundObject = new Sound;

	if (SoundResource::DetermineStreaming(name))
	{
		streamingFlag = true;

		WaveStreamer *streamer = new WaveStreamer;
		soundObject->Stream(streamer);

		result = streamer->AddComponent(name);
	}
	else
	{
		streamingFlag = false;
		result = soundObject->Load(name);
	}

	if (result == kSoundOkay)
	{
		soundObject->SetCompletionProc(&SoundComplete, this);
		soundObject->SetSoundFlags(kSoundPersistent);
		soundObject->Play();
	}
	else
	{
		resourceName[0] = 0;
	}
}

SoundWindow::~SoundWindow()
{
	soundObject->Release();
}

SoundWindow *SoundWindow::Open(const char *name)
{
	SoundWindow *window = windowList.First();
	while (window)
	{
		if (window->resourceName == name)
		{
			TheInterfaceMgr->SetActiveWindow(window);
			return (window);
		}

		window = window->ListElement<SoundWindow>::Next();
	}

	window = new SoundWindow(name);
	if (window->GetResourceName()[0] == 0)
	{
		delete window;
		return (nullptr);
	}

	TheInterfaceMgr->AddWidget(window);
	return (window);
}

void SoundWindow::Preprocess(void)
{
	Window::Preprocess();

	soundTime = 0;
	soundDuration = soundObject->GetDuration();

	playButton = static_cast<IconButtonWidget *>(FindWidget("Play"));
	stopButton = static_cast<IconButtonWidget *>(FindWidget("Stop"));
	loopWidget = static_cast<CheckWidget *>(FindWidget("Loop"));
	volumeWidget = static_cast<SliderWidget *>(FindWidget("Volume"));

	progressWidget = static_cast<ProgressWidget *>(FindWidget("Progress"));
	progressWidget->SetMaxValue(soundDuration);

	timeWidget = static_cast<TextWidget *>(FindWidget("Time"));
	timeWidget->SetText(MovieMgr::FormatMovieTime(0));

	TextWidget *durationWidget = static_cast<TextWidget *>(FindWidget("Duration"));
	durationWidget->SetText(MovieMgr::FormatMovieTime(soundDuration * kMovieTicksPerMillisecond));

	TextWidget *frequencyWidget = static_cast<TextWidget *>(FindWidget("Frequency"));
	frequencyWidget->SetText(String<63>(Text::FloatToString((float) soundObject->GetSampleRate() * 0.001F)) += " kHz");

	if (soundObject->GetChannelCount() == 1)
	{
		FindWidget("Stereo")->Hide();

		Widget *widget = FindWidget("Center");
		const Vector2D& size = widget->GetWidgetSize();

		soundLeft = new SoundWidget(size, (int32) size.x, ColorRGBA(0.125F, 0.625F, 1.0F));
		widget->AppendNewSubnode(soundLeft);

		markLeft = new LineWidget(Vector2D(size.y, 1.0F), kLineSolid, ColorRGBA(0.5F, 0.5F, 0.5F));
		markLeft->SetWidgetMatrix3D(K::y_unit, K::minus_x_unit, K::z_unit);
		widget->AppendNewSubnode(markLeft);

		soundLeft->BuildSound(soundObject);

		soundRight = nullptr;
		markRight = nullptr;
	}
	else
	{
		FindWidget("Mono")->Hide();

		Widget *leftWidget = FindWidget("Left");
		Widget *rightWidget = FindWidget("Right");
		const Vector2D& size = leftWidget->GetWidgetSize();

		soundLeft = new SoundWidget(size, (int32) size.x, ColorRGBA(0.125F, 0.625F, 1.0F));
		soundLeft->SetWidgetPosition(leftWidget->GetWidgetPosition());
		leftWidget->AppendNewSubnode(soundLeft);

		soundRight = new SoundWidget(size, (int32) size.x, ColorRGBA(0.125F, 0.625F, 1.0F));
		rightWidget->AppendNewSubnode(soundRight);

		markLeft = new LineWidget(Vector2D(size.y, 1.0F), kLineSolid, ColorRGBA(0.5F, 0.5F, 0.5F));
		markLeft->SetWidgetMatrix3D(K::y_unit, K::minus_x_unit, K::z_unit);
		leftWidget->AppendNewSubnode(markLeft);

		markRight = new LineWidget(Vector2D(size.y, 1.0F), kLineSolid, ColorRGBA(0.5F, 0.5F, 0.5F));
		markRight->SetWidgetMatrix3D(K::y_unit, K::minus_x_unit, K::z_unit);
		rightWidget->AppendNewSubnode(markRight);

		soundLeft->BuildSound(soundObject, 0);
		soundRight->BuildSound(soundObject, 1);
	}
}

void SoundWindow::UpdateSoundTime(int32 time)
{
	soundTime = time;
	progressWidget->SetValue(time);
	timeWidget->SetText(MovieMgr::FormatMovieTime(time * kMovieTicksPerMillisecond));
}

void SoundWindow::Move(void)
{
	Window::Move();

	if (soundObject->GetSoundState() == kSoundPlaying)
	{
		int32 playFrame = soundObject->GetPlayFrame();
		int32 channelCount = soundObject->GetChannelCount();

		if (!streamingFlag)
		{
			int32 length = soundObject->GetFrameCount();
			float x = PositiveFloor((float) playFrame * soundLeft->GetWidgetSize().x / (float) length);

			if (channelCount == 1)
			{
				markLeft->SetWidgetPosition(Point3D(x, 0.0F, 0.0F));
				markLeft->Invalidate();
			}
			else
			{
				markLeft->SetWidgetPosition(Point3D(x, 0.0F, 0.0F));
				markLeft->Invalidate();

				markRight->SetWidgetPosition(Point3D(x, 0.0F, 0.0F));
				markRight->Invalidate();
			}
		}
		else
		{
			int32 length = soundObject->GetSoundStreamer()->GetStreamBufferSize() / sizeof(Sample);
			float w = soundLeft->GetWidgetSize().x * 0.5F;
			float x = PositiveFloor((float) (playFrame * channelCount) * w / (float) length);

			if (soundObject->GetPlayBuffer() != 0)
			{
				x += w;
			}

			if (channelCount == 1)
			{
				markLeft->SetWidgetPosition(Point3D(x, 0.0F, 0.0F));
				markLeft->Invalidate();

				soundLeft->BuildSound(soundObject);
			}
			else
			{
				markLeft->SetWidgetPosition(Point3D(x, 0.0F, 0.0F));
				markLeft->Invalidate();

				markRight->SetWidgetPosition(Point3D(x, 0.0F, 0.0F));
				markRight->Invalidate();

				soundLeft->BuildSound(soundObject, 0);
				soundRight->BuildSound(soundObject, 1);
			}
		}

		int32 time = soundTime + TheTimeMgr->GetSystemDeltaTime();
		if (soundObject->GetLoopCount() == kSoundLoopInfinite)
		{
			time = time % soundDuration;
		}
		else
		{
			time = Min(time, soundDuration);
		}

		UpdateSoundTime(time);
	}
}

void SoundWindow::SoundComplete(Sound *sound, void *cookie)
{
	SoundWindow *window = static_cast<SoundWindow *>(cookie);
	window->UpdateSoundTime(window->soundDuration);

	window->playButton->Enable();
	window->stopButton->Disable();

	window->markLeft->Hide();
	if (window->markRight)
	{
		window->markRight->Hide();
	}
}

bool SoundWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyCommand)
	{
		if (eventData->keyCode == 'W')
		{
			Close();
			return (true);
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void SoundWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == playButton)
		{
			UpdateSoundTime(0);

			soundObject->Play();
			playButton->Disable();
			stopButton->Enable();

			markLeft->Show();
			if (markRight)
			{
				markRight->Show();
			}
		}
		else if (widget == stopButton)
		{
			UpdateSoundTime(0);

			soundObject->Stop();
			playButton->Enable();
			stopButton->Disable();

			markLeft->Hide();
			if (markRight)
			{
				markRight->Hide();
			}
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == loopWidget)
		{
			soundObject->SetLoopCount((loopWidget->GetValue() == 0) ? 0 : kSoundLoopInfinite);
		}
		else if (widget == volumeWidget)
		{
			soundObject->SetSoundProperty(kSoundVolume, (float) volumeWidget->GetValue() * 0.01F);
		}
	}
}

// ZYUQURM
