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


#ifndef C4MovieImporter_h
#define C4MovieImporter_h


#include "C4Configuration.h"
#include "C4MovieTool.h"


namespace C4
{
	enum
	{
		kMovieAudioBlockFrameCount	= 131072
	};


	class MovieFileName : public ListElement<MovieFileName>, public String<kMaxFileNameLength>
	{
		public:

			ImageImportPlugin		*importPlugin;
			int32					imageCount;

			MovieFileName(const char *name, ImageImportPlugin *plugin) : String<kMaxFileNameLength>(name)
			{
				importPlugin = plugin;
			}

			~MovieFileName()
			{
			}
	};


	class MovieImporter : public Configurable
	{
		private:

			ResourceName		outputMovieName;
			ResourceName		inputAudioName;

			float				videoQuality;
			float				videoFrameRate;
			int32				videoMaxDeltaCount;
			float				videoMaxDeltaRatio;
			unsigned_int32		videoTrackFlags;

			volatile int32		videoFrameIndex;
			volatile int32		importProgress;
			volatile int32		importMagnitude;
			volatile bool		cancelFlag;

		public:

			MovieImporter();
			~MovieImporter();

			const ResourceName& GetOutputMovieName(void) const
			{
				return (outputMovieName);
			}

			int32 GetVideoFrameIndex(void) const
			{
				return (videoFrameIndex);
			}

			int32 GetImportProgress(void) const
			{
				return (importProgress);
			}

			int32 GetImportMagnitude(void) const
			{
				return (importMagnitude);
			}

			void Cancel(void)
			{
				cancelFlag = true;
			}

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			EngineResult ImportMovie(const List<MovieFileName> *fileList);
	};


	class MovieProgressWindow : public Window, public Singleton<MovieProgressWindow>
	{
		private:

			MovieImporter			*movieImporter;
			List<MovieFileName>		movieFileList;

			PushButtonWidget		*stopButton;
			ProgressWidget			*progressBar;

			Thread					*importThread; 
			volatile EngineResult	importResult;

			static void ImportThread(const Thread *thread, void *cookie); 

		public: 

			MovieProgressWindow(MovieImporter *importer);
			~MovieProgressWindow(); 

			void AddMovieFile(const char *name, ImageImportPlugin *plugin) 
			{ 
				movieFileList.Append(new MovieFileName(name, plugin));
			}

			void Preprocess(void) override; 
			void Move(void) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class MovieImportPicker : public FilePicker
	{
		private:

			PopupMenuWidget		*importPluginMenu;

		public:

			MovieImportPicker(const char *title);
			~MovieImportPicker();

			ImageImportPlugin *GetImageImportPlugin(void) const;

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class ImportMovieWindow : public Window, public Singleton<ImportMovieWindow>
	{
		private:

			class ImportWidget final : public TextWidget
			{
				public:

					ImageImportPlugin		*importPlugin;

					ImportWidget(const ListWidget *list, const char *name, ImageImportPlugin *plugin);
					~ImportWidget();
			};

			MovieImporter			*movieImporter;

			PushButtonWidget		*importButton;
			PushButtonWidget		*cancelButton;
			PushButtonWidget		*addButton;
			PushButtonWidget		*removeButton;

			ListWidget				*imagesList;
			ConfigurationWidget		*configurationWidget;

			ConfigurationObserver<ImportMovieWindow>	configurationObserver;

			static void ImagesPicked(FilePicker *picker, void *cookie);

			void HandleConfigurationEvent(SettingInterface *settingInterface);

		public:

			ImportMovieWindow();
			~ImportMovieWindow();

			static void Open(void);

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	extern MovieProgressWindow *TheMovieProgressWindow;
	extern ImportMovieWindow *TheImportMovieWindow;
}


#endif

// ZYUQURM
