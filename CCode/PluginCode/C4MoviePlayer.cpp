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


#include "C4MoviePlayer.h"
#include "C4Interface.h"


using namespace C4;


List<MovieWindow> MovieWindow::windowList;


MovieWindow::MovieWindow(const char *name, const Vector2D& size) : Window("MovieTool/MoviePlayer")
{
	resourceName = name;
	movieSize = size;

	const Vector2D& windowSize = GetWidgetSize();
	SetWidgetSize(Vector2D(Fmax(size.x + 16.0F, windowSize.x), Fmax(size.y + 104.0F, windowSize.y)));
	windowList.Append(this);

	SetWindowTitle(name);
	SetStripTitle(&name[Text::GetDirectoryPathLength(name)]);
	SetStripIcon("MovieTool/window");
}

MovieWindow::~MovieWindow()
{
}

MovieResult MovieWindow::Open(const char *name)
{
	ResourceLoader			movieLoader;
	MovieResourceHeader		movieResourceHeader;
	MovieTrackHeader		*movieTrackHeader;
	Integer2D				movieSize;

	MovieWindow *window = windowList.First();
	while (window)
	{
		if (window->resourceName == name)
		{
			TheInterfaceMgr->SetActiveWindow(window);
			return (kMovieOkay);
		}

		window = window->ListElement<MovieWindow>::Next();
	}

	MovieResource *resource = MovieResource::Get(name, kResourceDeferLoad);
	if (!resource)
	{
		return (kMovieLoadFailed);
	}

	if (resource->OpenLoader(&movieLoader) != kResourceOkay)
	{
		resource->Release();
		return (kMovieLoadFailed);
	}

	if (resource->LoadHeaderData(&movieLoader, &movieResourceHeader, &movieTrackHeader) != kResourceOkay)
	{
		resource->CloseLoader(&movieLoader);
		resource->Release();
		return (kMovieLoadFailed);
	}

	MovieResult result = kMovieVideoTrackMissing;

	int32 trackCount = movieResourceHeader.movieTrackCount;
	const MovieTrackHeader *track = movieTrackHeader;
	for (machine a = 0; a < trackCount; a++)
	{
		MovieTrackType type = track->movieTrackType;
		if (type == kMovieTrackVideo)
		{
			movieSize = track->GetVideoTrackHeader()->videoFrameSize;
			result = kMovieOkay;
			break;
		}

		track++;
	}

	MovieResource::ReleaseHeaderData(movieTrackHeader);
	resource->CloseLoader(&movieLoader);

	if (result == kMovieOkay)
	{
		int32 maxWidth = TheDisplayMgr->GetDisplayWidth() - 24;
		int32 maxHeight = TheDisplayMgr->GetDisplayHeight() - 168;

		if (movieSize.x > maxWidth)
		{
			movieSize.y = movieSize.y * maxWidth / movieSize.x;
			movieSize.x = maxWidth;
		}

		if (movieSize.y > maxHeight)
		{
			movieSize.x = movieSize.x * maxHeight / movieSize.y;
			movieSize.y = maxHeight; 
		}

		window = new MovieWindow(name, Vector2D((float) movieSize.x, (float) movieSize.y)); 
		TheInterfaceMgr->AddWidget(window);
	} 

	resource->Release();
	return (result); 
}
 
void MovieWindow::Preprocess(void) 
{
	Window::Preprocess();

	Widget *movieBorder = FindWidget("MovieBorder"); 
	movieBorder->SetWidgetSize(movieSize);

	MovieWidget *movieWidget = new MovieWidget(movieSize, resourceName);
	movieWidget->SetWidgetPosition(movieBorder->GetWidgetPosition());
	AppendNewSubnode(movieWidget);

	movieObject = movieWidget->GetMovieObject();
	movieObject->SetCompletionProc(&MovieComplete, this);

	const Point3D& moviePosition = movieBorder->GetWidgetPosition();

	playButton = static_cast<IconButtonWidget *>(FindWidget("Play"));
	stopButton = static_cast<IconButtonWidget *>(FindWidget("Stop"));
	loopWidget = static_cast<CheckWidget *>(FindWidget("Loop"));

	sliderWidget = static_cast<SliderWidget *>(FindWidget("Slider"));
	sliderWidget->SetWidgetSize(Vector2D(movieSize.x, 8.0F));

	timeWidget = static_cast<TextWidget *>(FindWidget("Time"));
	timeWidget->SetText(MovieMgr::FormatMovieTime(0));

	TextWidget *durationWidget = static_cast<TextWidget *>(FindWidget("Duration"));
	durationWidget->SetWidgetPosition(Point3D(movieSize.x - durationWidget->GetWidgetSize().x, durationWidget->GetWidgetPosition().y, 0.0F));
	durationWidget->SetText(MovieMgr::FormatMovieTime(movieObject->GetMovieDuration()));

	Widget *group = FindWidget("Group");
	group->SetWidgetPosition(Point3D(group->GetWidgetPosition().x, moviePosition.y + movieSize.y + 8.0F, 0.0F));

	sliderWidget->SetMaxValue((int32) movieObject->GetMovieDuration());
}

void MovieWindow::UpdateMovieTime(MovieTime time)
{
	sliderWidget->SetValue((int32) time);
	timeWidget->SetText(MovieMgr::FormatMovieTime(time));
}

void MovieWindow::Move(void)
{
	Window::Move();

	if (movieObject->GetMovieState() == kMoviePlaying)
	{
		UpdateMovieTime(movieObject->GetMovieTime());
	}
}

void MovieWindow::MovieComplete(Movie *movie, void *cookie)
{
	MovieWindow *window = static_cast<MovieWindow *>(cookie);

	window->playButton->Enable();
	window->stopButton->Disable();

	window->UpdateMovieTime(movie->GetMovieTime());
}

bool MovieWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
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

void MovieWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == playButton)
		{
			if (movieObject->GetMovieTime() >= movieObject->GetMovieDuration())
			{
				movieObject->SetMovieTime(0);
				UpdateMovieTime(0);
			}

			movieObject->Play();

			playButton->Disable();
			stopButton->Enable();
		}
		else if (widget == stopButton)
		{
			movieObject->Stop();

			playButton->Enable();
			stopButton->Disable();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == loopWidget)
		{
			movieObject->SetMovieLoop(loopWidget->GetValue() != 0);
		}
		else if (widget == sliderWidget)
		{
			if (movieObject->GetMovieState() == kMoviePlaying)
			{
				movieObject->Stop();

				playButton->Enable();
				stopButton->Disable();
			}

			MovieTime time = sliderWidget->GetValue();
			movieObject->SetMovieTime(time);
			timeWidget->SetText(MovieMgr::FormatMovieTime(time));
		}
	}
}

// ZYUQURM
