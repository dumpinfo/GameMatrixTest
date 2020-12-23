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


#include "C4MovieImporter.h"
#include "C4Sequence.h"


using namespace C4;


MovieProgressWindow *C4::TheMovieProgressWindow = nullptr;
ImportMovieWindow *C4::TheImportMovieWindow = nullptr;


MovieImporter::MovieImporter()
{
	outputMovieName[0] = 0;
	inputAudioName[0] = 0;

	videoQuality = 1.0F;
	videoFrameRate = 30.0F;
	videoMaxDeltaCount = 15;
	videoMaxDeltaRatio = 1.0F;
	videoTrackFlags = 0;

	videoFrameIndex = -1;
	importProgress = 0;
	importMagnitude = 1;
	cancelFlag = false;
}

MovieImporter::~MovieImporter()
{
}

int32 MovieImporter::GetSettingCount(void) const
{
	return (10);
}

Setting *MovieImporter::GetSetting(int32 index) const
{
	const StringTable *table = TheMovieTool->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('IMOV', 'MOVI'));
		return (new HeadingSetting('MOVI', title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('IMOV', 'MOVI', 'NAME'));
		return (new TextSetting('MNAM', outputMovieName, title, kMaxResourceNameLength));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('IMOV', 'VDEO'));
		return (new HeadingSetting('VDEO', title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('IMOV', 'VDEO', 'QUAL'));
		return (new IntegerSetting('QUAL', (int32) (Log(videoQuality) * -21.74F + 50.5F), title, 1, 100, 1));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('IMOV', 'VDEO', 'RATE'));
		return (new TextSetting('RATE', videoFrameRate, title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('IMOV', 'VDEO', 'DELT'));
		return (new TextSetting('DELT', Text::IntegerToString(videoMaxDeltaCount), title, 3, &EditTextWidget::NumberFilter));
	}

	if (index == 6)
	{
		const char *title = table->GetString(StringID('IMOV', 'VDEO', 'KEYS'));
		return (new IntegerSetting('KEYS', (int32) (videoMaxDeltaRatio * 100.0F + 0.5F), title, 1, 100, 1));
	}

	if (index == 7)
	{
		const char *title = table->GetString(StringID('IMOV', 'VDEO', 'ALFA'));
		return (new BooleanSetting('ALFA', ((videoTrackFlags & kVideoAlphaChannel) != 0), title));
	}

	if (index == 8)
	{
		const char *title = table->GetString(StringID('IMOV', 'ADIO'));
		return (new HeadingSetting('ADIO', title));
	}

	if (index == 9)
	{
		const char *title = table->GetString(StringID('IMOV', 'ADIO', 'NAME'));
		const char *picker = table->GetString(StringID('IMOV', 'ADIO', 'PICK'));
		return (new ResourceSetting('ANAM', inputAudioName, title, picker, SoundResource::GetDescriptor(), nullptr, kResourceSettingGenericPath | kResourceSettingImportCatalog));
	} 

	return (nullptr);
} 

void MovieImporter::SetSetting(const Setting *setting) 
{
	Type identifier = setting->GetSettingIdentifier();
 
	if (identifier == 'MNAM')
	{ 
		outputMovieName = static_cast<const TextSetting *>(setting)->GetText(); 
	}
	else if (identifier == 'QUAL')
	{
		videoQuality = Exp(((float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() - 50.0F) * -0.046F); 
	}
	else if (identifier == 'RATE')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		videoFrameRate = Clamp(Text::StringToFloat(text), 1.0F, 1000.0F);
	}
	else if (identifier == 'DELT')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		videoMaxDeltaCount = Text::StringToInteger(text);
	}
	else if (identifier == 'KEYS')
	{
		videoMaxDeltaRatio = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
	}
	else if (identifier == 'ALFA')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			videoTrackFlags |= kVideoAlphaChannel;
		}
		else
		{
			videoTrackFlags &= ~kVideoAlphaChannel;
		}
	}
	else if (identifier == 'ANAM')
	{
		inputAudioName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		inputAudioName[Text::GetResourceNameLength(inputAudioName)] = 0;
	}
}

EngineResult MovieImporter::ImportMovie(const List<MovieFileName> *fileList)
{
	File					movieFile;
	ResourcePath			moviePath;
	Integer2D				videoFrameSize;
	MovieResourceHeader		movieResourceHeader;
	MovieTrackHeader		movieTrackHeader[2];

	MovieFileName *fileName = fileList->First();
	if (!fileName)
	{
		return (kEngineOkay);
	}

	EngineResult result = fileName->importPlugin->GetImageFileInfo(*fileName, &videoFrameSize, nullptr, &fileName->imageCount);
	if (result != kEngineOkay)
	{
		return (result);
	}

	if (((videoFrameSize.x | videoFrameSize.y) & 15) != 0)
	{
		return (kMovieBadImportVideoSize);
	}

	int32 videoFrameCount = fileName->imageCount;
	int32 fileCount = fileList->GetElementCount();

	for (machine a = 1; a < fileCount; a++)
	{
		Integer2D	frameSize;

		videoFrameIndex = videoFrameCount;
		fileName = fileName->Next();

		result = fileName->importPlugin->GetImageFileInfo(*fileName, &frameSize, nullptr, &fileName->imageCount);
		if (result != kEngineOkay)
		{
			return (result);
		}

		if (frameSize != videoFrameSize)
		{
			return (kMovieInconsistentImportVideoSize);
		}

		videoFrameCount += fileName->imageCount;
	}

	videoFrameIndex = -1;
	importMagnitude = videoFrameCount;

	SoundResource *soundResource = nullptr;
	if (inputAudioName[0] != 0)
	{
		soundResource = SoundResource::Get(inputAudioName, kResourceNoDefault, ThePluginMgr->GetImportCatalog());
		if (soundResource)
		{
			const WaveHeader *waveHeader = soundResource->GetWaveHeader();
			if ((waveHeader->format != WAVE_FORMAT_PCM) || (waveHeader->bitsPerSample != 16))
			{
				soundResource->Release();
				return (kMovieBadImportAudioFormat);
			}
		}
	}

	if (cancelFlag)
	{
		if (soundResource)
		{
			soundResource->Release();
		}

		return (kMovieImportCancelled);
	}

	TheResourceMgr->GetGenericCatalog()->GetResourcePath(MovieResource::GetDescriptor(), outputMovieName, &moviePath);
	TheResourceMgr->CreateDirectoryPath(moviePath);

	result = movieFile.Open(moviePath, kFileCreate);
	if (result == kFileOkay)
	{
		int32	audioChannelCount;
		int32	audioFrameCount;
		int32	audioBlockCount;

		int32 trackCount = 1;
		unsigned_int32 videoTrackHeaderSize = sizeof(VideoTrackHeader) + sizeof(VideoFrameData) * (videoFrameCount - 1);
		unsigned_int32 audioTrackHeaderSize = 0;

		if (soundResource)
		{
			audioChannelCount = soundResource->GetWaveHeader()->numChannels;
			audioFrameCount = soundResource->GetSampleCount() / audioChannelCount;
			audioBlockCount = (audioFrameCount + (kMovieAudioBlockFrameCount - 1)) / kMovieAudioBlockFrameCount;

			audioTrackHeaderSize = sizeof(AudioTrackHeader) + sizeof(AudioBlockData) * (audioBlockCount - 1);
			trackCount++;
		}

		movieResourceHeader.endian = 1;
		movieResourceHeader.movieTrackCount = trackCount;
		movieResourceHeader.headerDataSize = sizeof(MovieResourceHeader) + sizeof(MovieTrackHeader) * trackCount + videoTrackHeaderSize + audioTrackHeaderSize;

		movieTrackHeader[0].movieTrackType = kMovieTrackVideo;
		movieTrackHeader[0].movieTrackOffset = sizeof(MovieTrackHeader) * trackCount;

		VideoTrackHeader *videoTrackHeader = reinterpret_cast<VideoTrackHeader *>(new char[videoTrackHeaderSize]);

		videoTrackHeader->videoTrackFlags = videoTrackFlags;
		videoTrackHeader->videoFrameCount = videoFrameCount;
		videoTrackHeader->videoFrameSize = videoFrameSize;
		videoTrackHeader->videoFrameTime = (MovieTime) (kMovieFloatTicksPerSecond / videoFrameRate + 0.5F);
		videoTrackHeader->posterFrameIndex = -1;
		videoTrackHeader->maxFrameCodeSize = 0;
		videoTrackHeader->maxFrameDataSize = 0;

		for (machine a = 0; a < videoFrameCount; a++)
		{
			videoTrackHeader->videoFrameData[a].videoFrameDataOffset = 0;
			videoTrackHeader->videoFrameData[a].videoFrameDataSize = 0;
		}

		AudioTrackHeader *audioTrackHeader = nullptr;
		if (soundResource)
		{
			movieTrackHeader[1].movieTrackType = kMovieTrackAudio;
			movieTrackHeader[1].movieTrackOffset = sizeof(MovieTrackHeader) + videoTrackHeaderSize;

			audioTrackHeader = reinterpret_cast<AudioTrackHeader *>(new char[audioTrackHeaderSize]);

			audioTrackHeader->audioTrackFlags = 0;
			audioTrackHeader->audioBlockCount = audioBlockCount;
			audioTrackHeader->blockFrameCount = kMovieAudioBlockFrameCount;
			audioTrackHeader->audioFrameCount = audioFrameCount;
			audioTrackHeader->audioChannelCount = audioChannelCount;
			audioTrackHeader->audioSampleDepth = 16;
			audioTrackHeader->audioSampleFrequency = soundResource->GetWaveHeader()->sampleRate;
			audioTrackHeader->maxBlockCodeSize = 0;
			audioTrackHeader->maxBlockDataSize = 0;

			for (machine a = 0; a < audioBlockCount; a++)
			{
				audioTrackHeader->audioBlockData[a].audioBlockDataOffset = 0;
				audioTrackHeader->audioBlockData[a].audioBlockDataSize = 0;
			}
		}

		movieFile.Write(&movieResourceHeader, sizeof(MovieResourceHeader));
		movieFile.Write(movieTrackHeader, sizeof(MovieTrackHeader) * trackCount);
		movieFile.Write(videoTrackHeader, videoTrackHeaderSize);

		if (soundResource)
		{
			movieFile.Write(audioTrackHeader, audioTrackHeaderSize);
		}

		VideoCompressor *videoCompressor = new VideoCompressor(videoTrackHeader);

		unsigned_int32 maxFrameCodeSize = 0;
		unsigned_int32 maxFrameDataSize = 0;
		unsigned_int32 videoFrameDataOffset = movieResourceHeader.headerDataSize;

		int32 baseFrameIndex = 0;
		int32 deltaFrameCount = videoMaxDeltaCount;

		fileName = fileList->First();
		videoFrameCount = 0;

		for (machine a = 0; a < fileCount; a++)
		{
			ImageImportPlugin *importPlugin = fileName->importPlugin;
			ImageFormat format = importPlugin->GetImageFormat();

			for (machine b = 0; b < fileName->imageCount; b++)
			{
				void						*image;
				Integer2D					frameSize;
				unsigned_int32				compressedSize;
				VideoAlphaFrameHeader		videoFrameHeader[2];

				if (cancelFlag)
				{
					result = kMovieImportCancelled;
					goto end;
				}

				videoFrameIndex = videoFrameCount;

				result = importPlugin->ImportImageFile(*fileName, &image, &frameSize, nullptr, b);
				if (result != kEngineOkay)
				{
					goto end;
				}

				bool delta = (deltaFrameCount < videoMaxDeltaCount);
				if (delta)
				{
					unsigned_int32 baseSize = videoCompressor->CompressFrame(image, format, &videoFrameHeader[0], videoQuality, 0);
					unsigned_int32 deltaSize = videoCompressor->CompressFrame(image, format, &videoFrameHeader[1], videoQuality, kVideoCompressDeltaFrame);

					if ((float) deltaSize <= (float) baseSize * videoMaxDeltaRatio)
					{
						compressedSize = deltaSize;
					}
					else
					{
						videoCompressor->CommitBaseImage();
						compressedSize = baseSize;
						delta = false;
					}
				}
				else
				{
					compressedSize = videoCompressor->CompressFrame(image, format, &videoFrameHeader[0], videoQuality, kVideoCompressBaseFrame);
				}

				importPlugin->ReleaseImageData(image);

				if (delta)
				{
					deltaFrameCount++;
				}
				else
				{
					baseFrameIndex = videoFrameCount;
					deltaFrameCount = 0;
				}

				unsigned_int32 videoFrameDataSize = sizeof(VideoFrameHeader) + compressedSize;
				if (videoTrackFlags & kVideoAlphaChannel)
				{
					videoFrameDataSize += sizeof(VideoChannelData);
				}

				videoTrackHeader->videoFrameData[videoFrameCount].videoFrameDataOffset = videoFrameDataOffset;
				videoTrackHeader->videoFrameData[videoFrameCount].videoFrameDataSize = videoFrameDataSize;
				videoTrackHeader->videoFrameData[videoFrameCount].videoBaseFrameIndex = baseFrameIndex;

				videoFrameDataOffset += videoFrameDataSize;

				maxFrameCodeSize = Max(maxFrameCodeSize, videoFrameDataSize);
				maxFrameDataSize = Max(maxFrameDataSize, videoFrameHeader[delta].luminanceData.flatDataSize + videoFrameHeader[delta].luminanceData.wlenDataSize + videoFrameHeader[delta].luminanceData.wavyDataSize);
				maxFrameDataSize = Max(maxFrameDataSize, videoFrameHeader[delta].chrominanceData.flatDataSize + videoFrameHeader[delta].chrominanceData.wlenDataSize + videoFrameHeader[delta].chrominanceData.wavyDataSize);
				if (videoTrackFlags & kVideoAlphaChannel)
				{
					maxFrameDataSize = Max(maxFrameDataSize, videoFrameHeader[delta].alphaData.flatDataSize + videoFrameHeader[delta].alphaData.wlenDataSize + videoFrameHeader[delta].alphaData.wavyDataSize);
				}

				movieFile.Write(&videoFrameHeader[delta], (videoTrackFlags & kVideoAlphaChannel) ? sizeof(VideoAlphaFrameHeader) : sizeof(VideoFrameHeader));
				result = movieFile.Write(videoCompressor->GetCompressedCode(delta), compressedSize);

				if (result != kEngineOkay)
				{
					goto end;
				}

				importProgress = videoFrameCount;
				videoFrameCount++;
			}

			fileName = fileName->Next();
		}

		end:
		videoTrackHeader->maxFrameCodeSize = (maxFrameCodeSize + 15) & ~15;
		videoTrackHeader->maxFrameDataSize = (maxFrameDataSize + 15) & ~15;

		delete videoCompressor;
		videoFrameIndex = -1;

		if (result == kEngineOkay)
		{
			if (soundResource)
			{
				unsigned_int32 blockHeaderSize = sizeof(AudioBlockHeader) + (audioChannelCount - 1) * sizeof(AudioChannelData);
				AudioBlockHeader *audioBlockHeader = reinterpret_cast<AudioBlockHeader *>(new char[blockHeaderSize]);

				AudioCompressor *audioCompressor = new AudioCompressor(audioTrackHeader);

				unsigned_int32 maxBlockCodeSize = 0;
				unsigned_int32 maxBlockDataSize = 0;
				unsigned_int32 audioBlockDataOffset = videoFrameDataOffset;

				const Sample *audio = soundResource->GetSampleData();
				for (machine a = 0; a < audioBlockCount; a++)
				{
					int32 startFrame = a * kMovieAudioBlockFrameCount;
					int32 frameCount = Min(startFrame + kMovieAudioBlockFrameCount, audioFrameCount) - startFrame;

					unsigned_int32 compressedSize = audioCompressor->CompressBlock(&audio[startFrame * audioChannelCount], frameCount, audioBlockHeader);
					unsigned_int32 audioBlockDataSize = blockHeaderSize + compressedSize;

					audioTrackHeader->audioBlockData[a].audioBlockDataOffset = audioBlockDataOffset;
					audioTrackHeader->audioBlockData[a].audioBlockDataSize = audioBlockDataSize;

					audioBlockDataOffset += audioBlockDataSize;
					maxBlockCodeSize = Max(maxBlockCodeSize, audioBlockDataSize);

					unsigned_int32 dataSize = audioBlockHeader->audioChannelData[0].audioDataSize;
					for (machine b = 1; b < audioChannelCount; b++)
					{
						dataSize += audioBlockHeader->audioChannelData[b].audioDataSize;
					}

					maxBlockDataSize = Max(maxBlockDataSize, dataSize);

					movieFile.Write(audioBlockHeader, blockHeaderSize);
					result = movieFile.Write(audioCompressor->GetCompressedCode(), compressedSize);
					if (result != kEngineOkay)
					{
						break;
					}
				}

				audioTrackHeader->maxBlockCodeSize = (maxBlockCodeSize + 15) & ~15;
				audioTrackHeader->maxBlockDataSize = (maxBlockDataSize + 15) & ~15;

				delete audioCompressor;
				delete[] reinterpret_cast<char *>(audioBlockHeader);
			}

			if (result == kEngineOkay)
			{
				movieFile.SetPosition(sizeof(MovieResourceHeader) + sizeof(MovieTrackHeader) * trackCount);
				movieFile.Write(videoTrackHeader, videoTrackHeaderSize);

				if (soundResource)
				{
					movieFile.Write(audioTrackHeader, audioTrackHeaderSize);
				}
			}
		}

		delete[] reinterpret_cast<char *>(audioTrackHeader);
		delete[] reinterpret_cast<char *>(videoTrackHeader);

		movieFile.Close();

		if (result != kEngineOkay)
		{
			FileMgr::DeleteFile(moviePath);
		}
	}

	if (soundResource)
	{
		soundResource->Release();
	}

	return (result);
}


MovieProgressWindow::MovieProgressWindow(MovieImporter *importer) :
		Window("MovieTool/Progress"),
		Singleton<MovieProgressWindow>(TheMovieProgressWindow)
{
	movieImporter = importer;
	importThread = nullptr;
}

MovieProgressWindow::~MovieProgressWindow()
{
	movieImporter->Cancel();
	delete importThread;
	delete movieImporter;
}

void MovieProgressWindow::Preprocess(void)
{
	Window::Preprocess();

	stopButton = static_cast<PushButtonWidget *>(FindWidget("Stop"));
	progressBar = static_cast<ProgressWidget *>(FindWidget("Progress"));

	importResult = kEngineOkay;
	importThread = new Thread(&ImportThread, this);
}

void MovieProgressWindow::ImportThread(const Thread *thread, void *cookie)
{
	Thread::SetThreadName("C4-MT Import");

	MovieProgressWindow *movieProgressWindow = static_cast<MovieProgressWindow *>(cookie);
	movieProgressWindow->importResult = movieProgressWindow->movieImporter->ImportMovie(&movieProgressWindow->movieFileList);
}

void MovieProgressWindow::Move(void)
{
	progressBar->SetMaxValue(movieImporter->GetImportMagnitude());
	progressBar->SetValue(movieImporter->GetImportProgress());

	if (importResult != kEngineOkay)
	{
		const StringTable *table = TheMovieTool->GetStringTable();

		const char *title = table->GetString(StringID('IMOV', 'ERRR'));
		const char *message = table->GetString(StringID('IMOV', 'EMES'));
		const char *error = Engine::GetExternalResultString(importResult);

		const char *filename = nullptr;
		int32 videoFrameIndex = movieImporter->GetVideoFrameIndex();
		if (videoFrameIndex >= 0)
		{
			filename = *movieFileList[videoFrameIndex];
		}

		ErrorDialog *dialog = new ErrorDialog(title, message, error, filename);
		TheInterfaceMgr->AddWidget(dialog);
		Close();
	}
	else if (importThread->Complete())
	{
		Close();
	}
}

void MovieProgressWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (widget == stopButton))
	{
		Close();
	}
}


MovieImportPicker::MovieImportPicker(const char *title) : FilePicker('IMOV', title, ThePluginMgr->GetImportCatalog(), nullptr, nullptr, kFilePickerMultipleSelection, "MovieTool/ImportMovie")
{
}

MovieImportPicker::~MovieImportPicker()
{
}

ImageImportPlugin *MovieImportPicker::GetImageImportPlugin(void) const
{
	return ((*ThePluginMgr->GetImageImportPluginList())[importPluginMenu->GetSelection()]);
}

void MovieImportPicker::Preprocess(void)
{
	FilePicker::Preprocess();

	importPluginMenu = static_cast<PopupMenuWidget *>(FindWidget("Plugin"));

	const ImageImportPlugin *plugin = ThePluginMgr->GetImageImportPluginList()->First();
	SetResourceDescriptor(plugin->GetImageResourceDescriptor());

	do
	{
		importPluginMenu->AppendMenuItem(new MenuItemWidget(plugin->GetImageTypeName()));
		plugin = plugin->Next();
	} while (plugin);

	importPluginMenu->SetSelection(0);
}

void MovieImportPicker::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetChange) && (widget == importPluginMenu))
	{
		const ImageImportPlugin *plugin = GetImageImportPlugin();
		SetResourceDescriptor(plugin->GetImageResourceDescriptor());
	}
	else
	{
		FilePicker::HandleWidgetEvent(widget, eventData);
	}
}


ImportMovieWindow::ImportMovieWindow() :
		Window("MovieTool/MovieImporter"),
		Singleton<ImportMovieWindow>(TheImportMovieWindow),
		configurationObserver(this, &ImportMovieWindow::HandleConfigurationEvent)
{
	movieImporter = nullptr;

	SetStripIcon("MovieTool/window");
}

ImportMovieWindow::~ImportMovieWindow()
{
	delete movieImporter;
}

void ImportMovieWindow::Open(void)
{
	if (TheMovieProgressWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheMovieProgressWindow);
	}
	else
	{
		if (TheImportMovieWindow)
		{
			TheInterfaceMgr->SetActiveWindow(TheImportMovieWindow);
		}
		else
		{
			TheInterfaceMgr->AddWidget(new ImportMovieWindow);
		}
	}
}

ImportMovieWindow::ImportWidget::ImportWidget(const ListWidget *list, const char *name, ImageImportPlugin *plugin) : TextWidget(list->GetNaturalListItemSize(), name, list->GetFontName())
{
	importPlugin = plugin;
}

ImportMovieWindow::ImportWidget::~ImportWidget()
{
}

void ImportMovieWindow::Preprocess(void)
{
	Window::Preprocess();

	importButton = static_cast<PushButtonWidget *>(FindWidget("Import"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));
	addButton = static_cast<PushButtonWidget *>(FindWidget("Add"));
	removeButton = static_cast<PushButtonWidget *>(FindWidget("Remove"));
	imagesList = static_cast<ListWidget *>(FindWidget("Images"));

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));
	configurationWidget->SetObserver(&configurationObserver);

	movieImporter = new MovieImporter;
	configurationWidget->BuildConfiguration(movieImporter);
}

void ImportMovieWindow::ImagesPicked(FilePicker *picker, void *cookie)
{
	ImportMovieWindow *window = static_cast<ImportMovieWindow *>(cookie);
	ImageImportPlugin *plugin = static_cast<MovieImportPicker *>(picker)->GetImageImportPlugin();

	ListWidget *list = window->imagesList;
	window->SetFocusWidget(list);

	int32 count = picker->GetFileNameCount();
	for (machine a = 0; a < count; a++)
	{
		String<kMaxFileNameLength> name = picker->GetFileName(a);
		if (!list->FindListItem(name))
		{
			list->InsertSortedListItem(new ImportWidget(list, name, plugin), &Text::CompareNumberedTextLessThanCaseless);
		}
	}

	if ((list->GetFirstListItem()) && (window->movieImporter->GetOutputMovieName()[0] != 0))
	{
		window->importButton->Enable();
	}
}

void ImportMovieWindow::HandleConfigurationEvent(SettingInterface *settingInterface)
{
	configurationWidget->CommitConfiguration(movieImporter);
	if (movieImporter->GetOutputMovieName()[0] != 0)
	{
		if (imagesList->GetFirstListItem())
		{
			importButton->Enable();
		}
	}
	else
	{
		importButton->Disable();
	}
}

void ImportMovieWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventWidgetActivate)
	{
		if (widget == importButton)
		{
			configurationWidget->CommitConfiguration(movieImporter);

			// Ownership of the MovieImporter object is transferred to the MovieProgressWindow.

			MovieProgressWindow *progressWindow = new MovieProgressWindow(movieImporter);
			movieImporter = nullptr;

			const ResourceCatalog *importCatalog = ThePluginMgr->GetImportCatalog();
			const Widget *listItem = imagesList->GetFirstListItem();
			while (listItem)
			{
				String<kMaxFileNameLength> name(importCatalog->GetRootPath());
				const ImportWidget *importWidget = static_cast<const ImportWidget *>(listItem);
				progressWindow->AddMovieFile(name += importWidget->GetText(), importWidget->importPlugin);
				listItem = listItem->Next();
			}

			TheInterfaceMgr->AddWidget(progressWindow);
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
		else if (widget == addButton)
		{
			const char *title = TheMovieTool->GetStringTable()->GetString(StringID('IMOV', 'IMAG'));
			MovieImportPicker *picker = new MovieImportPicker(title);
			picker->SetCompletionProc(&ImagesPicked, this);
			AddSubwindow(picker);
		}
		else if (widget == removeButton)
		{
			imagesList->PurgeSelectedListItems();
			if (!imagesList->GetFirstListItem())
			{
				importButton->Disable();
			}

			removeButton->Disable();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == imagesList)
		{
			if (imagesList->GetFirstSelectedListItem())
			{
				removeButton->Enable();
			}
			else
			{
				removeButton->Disable();
			}
		}
	}
}

// ZYUQURM
