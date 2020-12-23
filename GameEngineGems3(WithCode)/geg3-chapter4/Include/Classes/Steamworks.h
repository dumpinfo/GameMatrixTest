#include "../Leadwerks.h"

#define USE_IUGC
#ifdef _WIN32
	#undef GetUserName
#endif

namespace Leadwerks
{
	/*
	class WorkshopFile
	{
		std::string name;
		int type;
		uint64_t fileid;
		WorkshopDir* parent;

		WorkshopFile();
	};

	class WorkshopDir : public WorkshopFile
	{
		virtual ~WorkshopDir();

		std::vector<WorkshopFile*> kids;
	};
	*/

	class Texture;
	class Bank;
	
	class Leaderboard : public Object//lua
	{
	public:
#ifdef __STEAM__
		//SteamLeaderboardEntries_t leaderboardentries;
		SteamLeaderboard_t leaderboard;
		int scores[10];
		uint64_t userids[10];
#endif
		int CountEntries();//lua
		std::string GetEntryUserID_(const int index);//lua
		uint64_t GetEntryUserID(const int index);
		int GetEntryScore(const int index);//lua
		bool SetScore(const int score, const bool forceupdate = false);//lua

		static const int Ascending;//lua
		static const int Descending;//lua
		static const int Numeric;//lua
		static const int Time;//lua
	};

	class WorkshopItem
	{
	public:
#ifdef __STEAM__
		uint64_t owner;
		uint64_t fileid;
		uint64_t legacypreviewfilehandle;
		uint64_t appid;
		uint32 timecreated;
		uint32 timeupdated;
		uint64_t legacyfilehandle;
		int filetype;
		std::string filename;
		std::string previewfilename;
		std::string title;
		std::string description;
		std::string url;
		std::vector<std::string> tags;
		std::string localpath;
#ifndef USE_IUGC
		RemoteStorageGetPublishedFileDetailsResult_t remotestoragefiledetails;
#endif

		WorkshopItem();
#endif
		uint32_t GetTimeUpdated();
	};

	class Steamworks;

	class SteamworksCallbackHandler
	{
		STEAM_CALLBACK(SteamworksCallbackHandler, OnCloseBrowser, HTML_CloseBrowser_t, m_CloseBrowser);
		STEAM_CALLBACK(SteamworksCallbackHandler, OnNeedsPaint, HTML_NeedsPaint_t, m_NeedsPaint);
		STEAM_CALLBACK(SteamworksCallbackHandler, OnStartRequest, HTML_StartRequest_t, m_StartRequest);
		STEAM_CALLBACK(SteamworksCallbackHandler, OnFinishedRequest, HTML_FinishedRequest_t, m_FinishedRequest);
		STEAM_CALLBACK(SteamworksCallbackHandler, OnSetCursor, HTML_SetCursor_t, m_SetCursor);
		STEAM_CALLBACK(SteamworksCallbackHandler, OnWorkshopItemDownloaded, DownloadItemResult_t, m_CallbackWorkshopItemDownloaded);
		STEAM_CALLBACK(SteamworksCallbackHandler, OnScreenshotRequested, ScreenshotRequested_t, m_CallbackScreenshotRequested);

	public:
		uint64_t waitfordownloadfileid;
		SteamworksCallbackHandler();
	};

	class HTMLRenderer : public Object//lua
	{
	public:
		std::string url;
		int width, height;
		HHTMLBrowser m_unBrowserHandle;
		Texture* texture;
		Vec3 mousepos;
		bool mousestate[3];
		bool mousestarted;
		bool needsredraw;
		std::string clickedurl;
		bool skipevent;
		int cursor;

		HTMLRenderer();
		virtual ~HTMLRenderer();
		
		//virtual iVec2 GetSize();
		virtual int GetCursor();
		virtual std::string GetClickedURL();//lua
		virtual void SetMousePosition(const float x, const float y, const float z=0.0f);//lua
		virtual void SetMouseButton(const bool state, const int button=1);//lua
		virtual bool NeedsRedraw();//lua
		virtual Texture* GetTexture();//lua
		static std::map<HHTMLBrowser, HTMLRenderer*> map;
		void Navigate(const std::string& url);//lua
		std::string GetURL();//lua
		void OnBrowserReady(HTML_BrowserReady_t *pBrowserReady, bool bIOFailure);
		static HTMLRenderer* Create(const std::string& url, const int width, const int height);//lua
		void SetSize(const int width, const int height);//lua
		CCallResult< HTMLRenderer, HTML_BrowserReady_t > m_SteamCallResultBrowserReady;
		static bool initialized;
	};

	class Steamworks//lua
	{
	public:
#ifdef __STEAM__
		bool hookcomplete;

		static SteamworksCallbackHandler* callbackhandler;
		static bool ConfirmCloudReady();
		static bool initialized;
		static bool initializationfailed;
		static bool controllerinitialized;
		static std::map<uint64_t, std::string> subscribedpackages;
		static ISteamFriends* steamfriends;
		static ISteamClient* steamclient;
		static ISteamUserStats* steamuserstats;
		static ISteamUtils* steamutils;
		static ISteamApps* steamapps;
		static ISteamRemoteStorage* steamremotestorage;
		static ISteamScreenshots* steamscreenshots;
		static ISteamUser* steamuser;
		static ISteamNetworking* steamnetworking;
		static ISteamController* steamcontroller;
		static ISteamUGC* steamugc;
		//static SteamControllerState_t currentControllerState[MAX_STEAM_CONTROLLERS];
		static ControllerHandle_t controllerhandle[STEAM_CONTROLLER_MAX_COUNT];
		static ControllerAnalogActionHandle_t currentControllerAnalogHandle[STEAM_CONTROLLER_MAX_COUNT];
		static ControllerDigitalActionHandle_t currentControllerDigitalHandle[STEAM_CONTROLLER_MAX_COUNT];
		static ControllerAnalogActionData_t currentControllerAnalogState[STEAM_CONTROLLER_MAX_COUNT];
		static ControllerDigitalActionData_t currentControllerDigitalState[STEAM_CONTROLLER_MAX_COUNT];
		static bool controllerPresent[STEAM_CONTROLLER_MAX_COUNT];
		static uint64 buttonHeld[STEAM_CONTROLLER_MAX_COUNT];
		static CSteamID userid;
		static uint64_t GameID;
		static bool GetWorkshopFileInfo(uint64_t fileid, WorkshopItem& workshopfile);
		static std::map<uint64_t, WorkshopItem> subscribedworkshopitems;
		static bool Connect(uint64_t steamid);
		static bool SendPacket(uint64_t steamid, Bank* bank, const bool reliable=false);
		static bool ReceivePacket(Bank* bank);
		static bool Disconnect(uint64_t steamid);
		static void P2PSessionRequest(P2PSessionRequest_t *pCallback);
#endif
		static Leaderboard* GetLeaderboard(std::string name = "Hiscore", const int type = Leaderboard::Numeric, const int sortmode = Leaderboard::Descending);//lua
		static int CountSubscribedWorkshopItems();
		static uint64_t GetSubscribedWorkshopItem(const int index);
		static std::string GetWorkshopItemTitle(const uint64_t fileid);
		static std::string GetWorkshopItemLocalPath(const uint64_t fileid);
		static bool InstallPackage(uint64_t fileid, void installhook(const char* filepath));
		static bool Initialize();//lua
		static bool GetInitialized();//lua
		static void Shutdown();//lua
		static uint64_t GetUserID();
		static std::string GetUserID_();//lua
		static bool SyncSubscribedFiles(int callback(float), const bool downloadgames);
		static std::string GetWorkshopCachePath();
		static bool DLCInstalled(uint64_t appid);
		static bool DLCSubscribed(uint64_t appid);
		static bool UnsubscribeWorkshopFile(uint64_t fileid);
		static std::string GetUserName(uint64_t userid);
		static std::string GetUserName(const std::string& userid);//lua
		static void SetAchievement(const std::string& name);
		static void IncrementAchievementProgress(const std::string& name);
		//static void SetAchievementProgress(const std::string& name, const float progress);
		//static void SetAchievementProgress(const std::string& name, const double progress);
		//static void SetAchievementProgress(const std::string& name, const int progress);
		static bool GetAchievement(const std::string& name);
		static bool ExtractWorkshopFiles(const bool force);
		static bool UninstallPackage(const std::string& path);
		static bool DownloadFile(uint64_t fileid, int callback(uint64_t, float));
		static uint32 GetWorkshopItemTimeUpdated(const uint64_t fileid);
		static Texture* GetUserAvatar(const uint64_t userid);
		static Texture* GetUserAvatar(const std::string& userid);//lua

#ifdef __STEAM__
		static bool InitializeController();
		static bool SaveFile(Stream* stream, std::string filename, int callback(float));
		static std::vector<std::string> GetPublishedFiles();
		static bool ShareFile(std::string filename, std::string title, std::string description);
		static bool DeleteFile(std::string filename);
		static int GetFileType(std::string filename);
		static int GetFileSize(std::string filename);
		static bool SaveFile(std::string filename, void* data, const int size, int callback(float));
		static Stream* ReadFile(std::string filename);
		static uint64 PublishFile(std::string filename, std::string title, std::string description, const std::string& previewfile, std::vector<std::string>& tags, const int price, int callback(float), const int type = 0, const int privacy = 0, uint64_t appid=0);
		//static bool AddScreenshot(void* data, const int width, const int height);
		static bool AddScreenshot(std::string filename, int width, int height);
		static uint64 PublishScreenshot(const std::string filename, const std::string title, const std::string desc, const std::string& tags, int callback(float));
		static void WarningMessageHook(int severity, const char* message);
		static Stream* ReadWorkshopFile(uint64_t fileid, const std::string& path);
		//static void SetLeaderboardScore(const int score, const int type = k_ELeaderboardDisplayTypeNumeric, const int sortmethod = k_ELeaderboardSortMethodDescending);
#else
		//static void SetLeaderboardScore(const int score, const int type = 1, const int sortmethod = 2);
#endif

		//--------------------------------------------------------------------------------------------------------------------------------
		// Controller (Aria)
		//--------------------------------------------------------------------------------------------------------------------------------
#ifdef __STEAM__
		static void UpdateController();
		static uint32 GetPacketNum(const int controllerIndex);
		static uint64 GetButtons(const int controllerIndex);
#endif
		static bool IsControllerEnabled(const int controllerIndex);//lua
		static int GetMaxSteamControllers();//lua
		static void TriggerHapticPulse(const int controllerIndex, const int pad, const int duration);//lua
		static bool ButtonDown(const int controllerIndex, unsigned int button);//lua
		static bool ButtonHit(const int controllerIndex, unsigned int button);//lua
		static Vec2 GetPadPosition(const int controllerindex, const int padindex);//lua
		//static bool UnsubscribeWorkshopFile(const uint64_t fileid, int callback(float));
		static bool SubscribeWorkshopFile(const uint64_t fileid);
		static bool UnsubscribeAllWorkshopItems();

		//--------------------------------------------------------------------------------------------------------------------------------
		//Callback stuff
		//--------------------------------------------------------------------------------------------------------------------------------
#ifdef __STEAM__

		//--------------------------------------------------------------------------------------------------------------------------------
		//Leaderboards
		//--------------------------------------------------------------------------------------------------------------------------------
		
		LeaderboardFindResult_t m_LeaderboardFindResult_t;
		CCallResult < Steamworks, LeaderboardFindResult_t > LeaderboardFindResult_t_CCallResult; 		
		void m_FindOrCreateLeaderboardResult_t_Hook(LeaderboardFindResult_t* result, bool b);
		
		LeaderboardScoresDownloaded_t m_LeaderboardScoresDownloaded_t;
		CCallResult < Steamworks, LeaderboardScoresDownloaded_t > LeaderboardScoresDownloaded_t_CCallResult;
		void m_LeaderboardScoresDownloaded_t_Hook(LeaderboardScoresDownloaded_t* result, bool b);

		LeaderboardScoreUploaded_t m_LeaderboardScoreUploaded_t;
		CCallResult < Steamworks, LeaderboardScoreUploaded_t > LeaderboardScoreUploaded_t_CCallResult;
		void m_LeaderboardScoreUploaded_t_Hook(LeaderboardScoreUploaded_t* result, bool b);
		
		//--------------------------------------------------------------------------------------------------------------------------------
		//
		//--------------------------------------------------------------------------------------------------------------------------------

		RemoteStorageEnumerateUserSubscribedFilesResult_t m_RemoteStorageEnumerateUserSubscribedFilesResult_t;
		RemoteStorageFileShareResult_t m_RemoteStorageFileShareResult_t;
		RemoteStoragePublishFileResult_t m_RemoteStoragePublishFileResult_t;

		CCallResult < Steamworks, RemoteStorageFileShareResult_t > RemoteStorageFileShareResult_t_CCallResult;
		CCallResult < Steamworks, RemoteStoragePublishFileResult_t > RemoteStoragePublishFileResult_t_CCallResult;

		
		void m_RemoteStorageFileShareResult_t_Hook(RemoteStorageFileShareResult_t* pLobbyMatchList, bool b);
		void m_RemoteStoragePublishFileResult_t_Hook(RemoteStoragePublishFileResult_t* pLobbyMatchList, bool b);

		static bool GetSubscribedWorkshopItems(std::vector<WorkshopItem>& results);
		static bool GetSubscribedWorkshopItems(std::vector<WorkshopItem>& results, uint32 startindex);
		static bool GetAllWorkshopItems(std::vector<WorkshopItem>& results, EWorkshopEnumerationType enumarationtype, uint32 startindex, uint32 count, std::vector<const char*> tags, std::vector<const char*>  usertags);

		//UGC
		SteamUGCRequestUGCDetailsResult_t m_SteamUGCRequestUGCDetailsResult_t;
		CCallResult< Steamworks, SteamUGCRequestUGCDetailsResult_t> SteamUGCRequestUGCDetailsResult_t_CCallResult;
		void m_SteamUGCRequestUGCDetailsResult_t_Hook(SteamUGCRequestUGCDetailsResult_t* steamugcrequestugcdetailsresult, bool B);

		CreateItemResult_t m_CreateItemResult_t;
		CCallResult< Steamworks, CreateItemResult_t> CreateItemResult_t_CCallResult;
		void m_CreateItemResult_t_Hook(CreateItemResult_t* createitemresult, bool B);
		
		SubmitItemUpdateResult_t m_SubmitItemUpdateResult_t;
		CCallResult< Steamworks, SubmitItemUpdateResult_t> SubmitItemUpdateResult_t_CCallResult;
		void m_SubmitItemUpdateResult_t_Hook(SubmitItemUpdateResult_t* submititemupdateresult, bool B);

		SteamUGCQueryCompleted_t m_SteamUGCQueryCompleted_t;
		CCallResult< Steamworks, SteamUGCQueryCompleted_t> SteamUGCQueryCompleted_t_CCallResult;
		void m_SteamUGCQueryCompleted_t_Hook(SteamUGCQueryCompleted_t* steamugcquerycompleted, bool B);

		//--------------------------------------------------------------------------------------------------------------------------------
		//klepto2 additions
		//--------------------------------------------------------------------------------------------------------------------------------
		RemoteStorageSubscribePublishedFileResult_t m_RemoteStorageSubscribePublishedFileResult_t;
		RemoteStorageUnsubscribePublishedFileResult_t m_RemoteStorageUnsubscribePublishedFileResult_t;
		CCallResult< Steamworks, RemoteStorageSubscribePublishedFileResult_t> RemoteStorageSubscribePublishedFileResult_t_CCallResult;
		CCallResult< Steamworks, RemoteStorageUnsubscribePublishedFileResult_t> RemoteStorageUnsubscribePublishedFileResult_t_CCallResult;
		void m_RemoteStorageSubscribePublishedFileResult_t_Hook(RemoteStorageSubscribePublishedFileResult_t* remotestoragesubscribepublishedfileresult, bool B);
		void m_RemoteStorageUnsubscribePublishedFileResult_t_Hook(RemoteStorageUnsubscribePublishedFileResult_t* remotestorageunsubscribepublishedfileresult, bool B);

		RemoteStorageEnumerateWorkshopFilesResult_t m_RemoteStorageEnumerateWorkshopFilesResult_t;
		RemoteStorageEnumerateUserSharedWorkshopFilesResult_t m_RemoteStorageEnumerateUserSharedWorkshopFilesResult_t;
		//RemoteStorageEnumerateUserPublishedWorkshopFilesResult_t m_RemoteStorageEnumerateUserSharedWorkshopFilesResult_t;
		RemoteStorageGetPublishedFileDetailsResult_t m_RemoteStorageGetPublishedFileDetailsResult_t;
		RemoteStorageEnumerateUserPublishedFilesResult_t m_RemoteStorageEnumerateUserPublishedFilesResult_t;
		RemoteStorageEnumerateUserSubscribedFilesResult_t m_RemoteStorageEnumerateUserPSubscribedFilesResult_t;
		RemoteStorageDownloadUGCResult_t m_RemoteStorageDownloadUGCResult_t;
		RemoteStorageDeletePublishedFileResult_t m_RemoteStorageDeletePublishedFileResult_t;
		RemoteStorageUpdatePublishedFileResult_t m_RemoteStorageUpdatePublishedFileResult_t;

		void m_RemoteStorageRemoteStorageEnumerateUserSharedWorkshopFiles_t_Hook(RemoteStorageEnumerateUserSharedWorkshopFilesResult_t* remotestoragefileshareresult, bool B);
		void m_RemoteStorageRemoteStorageEnumerateWorkshopFiles_t_Hook(RemoteStorageEnumerateWorkshopFilesResult_t* remotestoragefileshareresult, bool B);
		void m_RemoteStorageGetPublishedFileDetailsResult_t_Hook(RemoteStorageGetPublishedFileDetailsResult_t* remotestoragefileshareresult, bool B);
		void m_RemoteStorageEnumerateUserPublishedFilesResult_t_Hook(RemoteStorageEnumerateUserPublishedFilesResult_t* remotestoragefileshareresult, bool B);
		void m_RemoteStorageEnumerateUserSubscribedFilesResult_t_Hook(RemoteStorageEnumerateUserSubscribedFilesResult_t* remotestoragefileshareresult, bool B);
		void m_RemoteStorageDownloadUGCResult_t_Hook(RemoteStorageDownloadUGCResult_t* remotestoragefileshareresult, bool B);
		void m_RemoteStorageDeletePublishedFileResult_t_Hook(RemoteStorageDeletePublishedFileResult_t* remotestoragedeletepublishedfileresult, bool B);
		void m_RemoteStorageUpdatePublishedFileResult_t_Hook(RemoteStorageUpdatePublishedFileResult_t* remotestorageupdatepublishedfileresult, bool B);

		CCallResult< Steamworks, RemoteStorageEnumerateUserSharedWorkshopFilesResult_t> RemoteStorageEnumerateUserSharedWorkshopFilesResult_t_CCallResult;
		CCallResult< Steamworks, RemoteStorageEnumerateWorkshopFilesResult_t> RemoteStorageEnumerateWorkshopFilesResult_t_CCallResult;
		CCallResult< Steamworks, RemoteStorageGetPublishedFileDetailsResult_t> RemoteStorageGetPublishedFileDetailsResult_t_CCallResult;
		CCallResult< Steamworks, RemoteStorageEnumerateUserPublishedFilesResult_t>RemoteStorageEnumerateUserPublishedFilesResult_t_CCallResult;
		CCallResult< Steamworks, RemoteStorageEnumerateUserSubscribedFilesResult_t>RemoteStorageEnumerateUserSubscribedFilesResult_t_CCallResult;
		CCallResult< Steamworks, RemoteStorageDownloadUGCResult_t>RemoteStorageDownloadUGCResult_t_CCallResult;
		CCallResult< Steamworks, RemoteStorageDeletePublishedFileResult_t>RemoteStorageDeletePublishedFileResult_t_CCallResult;
		CCallResult< Steamworks, RemoteStorageUpdatePublishedFileResult_t>RemoteStorageUpdatePublishedFileResult_t_CCallResult;

		//Common
		static bool GetFileInfo(PublishedFileId_t filehandle, WorkshopItem& workshopfile);// RemoteStorageGetPublishedFileDetailsResult_t& details);
		static bool GetWorkshopItemInfo(uint64_t fileid, WorkshopItem& workshopfile);

		//Enumerations
		//static RemoteStorageEnumerateWorkshopFilesResult_t GetAllWorkshopFiles(EWorkshopEnumerationType enumarationtype, uint32 startindex, uint32 count, std::vector<const char*> tags, std::vector<const char*> usertags);
		static bool GetAllWorkshopPackages(std::vector<WorkshopItem>& results, EWorkshopEnumerationType enumarationtype, uint32 startindex, uint32 count, std::vector<const char*> tags, std::vector<const char*> usertags);
		static RemoteStorageEnumerateUserPublishedFilesResult_t GetUserWorkshopFiles(uint32 startindex);
		//static RemoteStorageEnumerateUserSubscribedFilesResult_t GetSubscribedWorkshopFiles(uint32 startindex);
		static bool GetSubscribedWorkshopPackages(std::vector<WorkshopItem>& results);
		static bool GetSubscribedWorkshopPackages(std::vector<WorkshopItem>& results, uint32 startindex);
		static bool GetUserPublishedWorkshopFiles(std::vector<WorkshopItem>& results, uint64_t consumerappid, EWorkshopEnumerationType enumarationtype, uint32& startindex, uint32 count, std::vector<const char*> tags, std::vector<const char*>  usertags);

		//Upload
		static uint64 PublishWorkshopFile(std::string filename, std::string previewfilename, std::string title, std::string description, std::vector<const char*> tags, const int price, int callback(float) = NULL);
		static uint64 PublishVideo(std::string videoid, std::string title, std::string description, ERemoteStoragePublishedFileVisibility eVisibility, std::vector<const char*> tags, int callback(float)=NULL);

		//Download
		static bool DownloadWorkshopItem(uint64_t handle, uint64_t legacyfilehandle, bool preview, std::string& targetpath, const std::string& extractionpath, int callback(uint64_t, float));

		static const uint64_t MaxDLCAppID;

		//Common
		static bool DeleteWorkshopItem(RemoteStorageGetPublishedFileDetailsResult_t fileinfo);
		static PublishedFileUpdateHandle_t RequestWorkshopItemUpdate(RemoteStorageGetPublishedFileDetailsResult_t fileinfo);
		static bool CommitWorkshopItemUpdate(PublishedFileUpdateHandle_t updateHandle, std::string updatedescription);
		static bool UpdateWorkshopItemTitle(PublishedFileUpdateHandle_t updateHandle, std::string title);
		static bool UpdateWorkshopItemDescription(PublishedFileUpdateHandle_t updateHandle, std::string description);

		static bool UpdateWorkshopItemFile(uint64_t appid, PublishedFileUpdateHandle_t updateHandle, std::string filename, std::string& previewfile, const std::string& comment, int callback(float) = NULL);
		static bool UpdateWorkshopItemPreviewFile(PublishedFileUpdateHandle_t updateHandle, const std::string& previewFilename);
		static bool UpdateWorkshopItemVisibility(PublishedFileUpdateHandle_t updateHandle, ERemoteStoragePublishedFileVisibility eVisibility);
		static bool UpdateWorkshopItemTags(PublishedFileUpdateHandle_t updateHandle, std::vector<const char*> tags);
#endif
	};

	//RemoteStorageGetPublishedFileDetailsResult_t

}
