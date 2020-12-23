 

#ifndef C4ToolWindows_h
#define C4ToolWindows_h


#include "C4Interface.h"
#include "C4Graphics.h"
#include "C4Shaders.h"
#include "C4Network.h"


namespace C4
{
	#if C4DIAGS

		enum : WidgetType
		{
			kWidgetFrameBuffer = 'FBUF'
		};


		class FrameBufferWidget final : public RenderableWidget
		{
			private:

				struct BufferVertex
				{
					Point2D		position;
					Point3D		texcoord;
				};

				unsigned_int32		frameBufferIndex;

				VertexBuffer		vertexBuffer;
				List<Attribute>		attributeList;
				ShaderAttribute		shaderAttribute;

			public:

				FrameBufferWidget(unsigned_int32 index, const Vector2D& size);
				~FrameBufferWidget();

				void Preprocess(void) override;
				void Build(void) override;
		};

	#endif


	class TimeWindow : public Window, public Singleton<TimeWindow>
	{
		private:

			enum
			{
				kWidgetGraph		= 'GRPH',
				kWidgetMultigraph	= 'MGPH'
			};

			enum
			{
				kTimeMeasureFrameCount = 48
			};

			class GraphWidget final : public RenderableWidget
			{
				private:

					VertexBuffer		vertexBuffer;
					List<Attribute>		attributeList;
					DiffuseAttribute	diffuseAttribute;

					Point2D				vertexArray[kTimeMeasureFrameCount * 2];

				public:

					GraphWidget(const ColorRGBA& color);
					~GraphWidget();

					void Preprocess(void) override;

					void AddValue(float value);
			};

			class MultigraphWidget final : public RenderableWidget
			{
				private:

					struct GraphVertex
					{
						Point2D		position;
						Color4C		color;
					};

					VertexBuffer		vertexBuffer;
					VertexBuffer		indexBuffer;

					GraphVertex			vertexArray[kTimeMeasureFrameCount * 12];

				public:

					MultigraphWidget(); 
					~MultigraphWidget();

					void Preprocess(void) override; 

					void AddValues(const float (& value)[6]); 
			};

			TextWidget			*rateText; 
			TextWidget			*timeText;
 
			TextWidget			*structureText; 
			TextWidget			*occlusionText;
			TextWidget			*renderText;
			TextWidget			*shadowText;
			TextWidget			*postText; 
			TextWidget			*guiText;

			int32				frameCount;
			int32				timeTable[kTimeMeasureFrameCount];

			GraphWidget			rateGraph;
			MultigraphWidget	renderGraph;

			TimeWindow();

			static void SetTimeValue(TextWidget *widget, unsigned_int32 time);

		public:

			~TimeWindow();

			static void Open(void);

			void Preprocess(void) override;
			void Move(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
	};


	#if C4STATS

		class StatsWindow : public Window, public Singleton<StatsWindow>
		{
			private:

				enum
				{
					kRenderStatDirectVerts,
					kRenderStatDirectPrims,
					kRenderStatDirectCmds,
					kRenderStatShadowVerts,
					kRenderStatShadowPrims,
					kRenderStatShadowCmds,
					kRenderStatVelocityVerts,
					kRenderStatVelocityPrims,
					kRenderStatVelocityCmds,
					kRenderStatDistortionVerts,
					kRenderStatDistortionPrims,
					kRenderStatDistortionCmds,
					kRenderStatTotalVerts,
					kRenderStatTotalPrims,
					kRenderStatTotalCmds,
					kRenderStatTextureCount,
					kRenderStatTextureMemory,
					kRenderStatVertexBufferCount,
					kRenderStatVertexBufferMemory,
					kRenderStatCount
				};

				enum
				{
					kWorldStatGeometryCount,
					kWorldStatTerrainCount,
					kWorldStatWaterCount,
					kWorldStatImpostorCount,
					kWorldStatInfiniteLightCount,
					kWorldStatInfiniteShadowCount,
					kWorldStatPointLightCount,
					kWorldStatPointShadowCount,
					kWorldStatSpotLightCount,
					kWorldStatSpotShadowCount,
					kWorldStatDirectPortals,
					kWorldStatRemotePortals,
					kWorldStatOcclusionRegions,
					kWorldStatCount
				};

				enum
				{
					kPhysicsStatRigidBodyCount,
					kPhysicsStatDeformableBodyMoveCount,
					kPhysicsStatDeformableBodyUpdateCount,
					kPhysicsStatWaterMoveCount,
					kPhysicsStatWaterUpdateCount,
					kPhysicsStatBuoyancyCount,
					kPhysicsStatGeometryIntersections,
					kPhysicsStatRigidBodyIntersections,
					kPhysicsStatConstraintSolverIslands,
					kPhysicsStatCount
				};

				enum
				{
					kMiscStatPlayingSources,
					kMiscStatEngagedSources,
					kMiscStatRunningScripts,
					kMiscStatCount
				};

				MultipaneWidget		*paneWidget;
				Widget				*renderGroup;
				Widget				*worldGroup;
				Widget				*physicsGroup;
				Widget				*miscGroup;

				TextWidget			*renderStatText[kRenderStatCount];
				TextWidget			*worldStatText[kWorldStatCount];
				TextWidget			*physicsStatText[kPhysicsStatCount];
				TextWidget			*miscStatText[kMiscStatCount];

				StatsWindow();

			public:

				~StatsWindow();

				static void Open(void);

				void Preprocess(void) override;
				void Move(void) override;

				bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
				void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
		};

	#endif


	#if C4DIAGS

		class FrameBufferWindow : public Window, public Singleton<FrameBufferWindow>
		{
			private:

				FrameBufferWidget		frameBufferWidget;

				FrameBufferWindow(int32 bufferIndex);

			public:

				~FrameBufferWindow();

				static void Open(int32 bufferIndex);

				bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
		};


		class ShadowMapWindow : public Window, public Singleton<ShadowMapWindow>
		{
			private:

				FrameBufferWidget		frameBufferWidget;

				ShadowMapWindow(int32 bufferIndex);

				Vector2D GetWindowSize(int32 bufferIndex) const;

			public:

				~ShadowMapWindow();

				static void Open(int32 bufferIndex);

				bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
		};


		class NetworkWindow : public Window, public Singleton<NetworkWindow>
		{
			private:

				enum
				{
					kWidgetNetwork = 'NTWK'
				};

				class NetworkWidget final : public RenderableWidget
				{
					private:

						VertexBuffer		vertexBuffer;
						List<Attribute>		attributeList;
						DiffuseAttribute	diffuseAttribute;

						Point2D				vertexArray[128];

					public:

						NetworkWidget(const Vector2D& size, const ColorRGBA& color);
						~NetworkWidget();

						void Preprocess(void) override;

						void AddValue(float value);
				};

				TextWidget					*connectionText;
				TextWidget					*chatRateText;

				Storage<NetworkWidget>		incomingGraph[kPacketTypeCount];
				Storage<NetworkWidget>		outgoingGraph[kPacketTypeCount];

				NetworkWindow();

			public:

				~NetworkWindow();

				static void Open(void);

				void Preprocess(void) override;
				void Move(void) override;

				bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
		};


		class ExtensionsWindow : public Window, public Singleton<ExtensionsWindow>
		{
			private:

				PushButtonWidget		*okayButton;
				PushButtonWidget		*cancelButton;
				ListWidget				*extensionsList;

				CheckWidget				*enableBox[kGraphicsExtensionCount];

				#if C4WINDOWS || C4LINUX

					CheckWidget			*windowSystemEnableBox[kWindowSystemExtensionCount];

				#endif

				ExtensionsWindow();

			public:

				~ExtensionsWindow();

				static void Open(void);

				void Preprocess(void) override;
				void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
		};

	#endif


	class ConsoleWindow : public Window, public Singleton<ConsoleWindow>
	{
		private:

			enum
			{
				kConsoleLineCount				= 17,
				kConsoleHistoryCount			= 256,
				kConsoleCommandHistoryCount		= 16
			};

			static_assert((kConsoleCommandHistoryCount & (kConsoleCommandHistoryCount - 1)) == 0, "kConsoleCommandHistoryCount must be a power of two");

			bool								dirtyFlag;
			bool								historyFlag;

			TextWidget							*textWidget;
			EditTextWidget						*commandLine;
			ScrollWidget						*scrollWidget;

			Reporter							reporter;

			static unsigned_int32				commandHistoryCount;
			static unsigned_int32				commandHistoryStart;
			static unsigned_int32				commandHistoryOffset;
			static String<kMaxCommandLength>	commandHistory[kConsoleCommandHistoryCount];

			ConsoleWindow();

			static void ConsoleProc(void *cookie);
			static void Report(const char *text, unsigned_int32 flags, void *cookie);

			void UpdateDisplayLine(void)
			{
				textWidget->SetFirstRenderLine(scrollWidget->GetValue());
			}

		public:

			~ConsoleWindow();

			static void New(void);

			static unsigned_int32 GetCommandHistoryCount(void)
			{
				return (commandHistoryCount);
			}

			static const char *GetCommandHistory(int32 index)
			{
				return (commandHistory[(commandHistoryStart - commandHistoryCount + index) & (kConsoleCommandHistoryCount - 1)]);
			}

			void Preprocess(void) override;
			void EnterForeground(void) override;
			void Close(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;

			void Render(List<Renderable> *renderList) override;

			static void AddCommandHistory(const char *command);

			C4API void AddText(const char *text = nullptr);
	};


	extern TimeWindow *TheTimeWindow;

	#if C4STATS

		extern StatsWindow *TheStatsWindow;

	#endif

	#if C4DIAGS

		extern NetworkWindow *TheNetworkWindow;
		extern FrameBufferWindow *TheFrameBufferWindow;
		extern ShadowMapWindow *TheShadowMapWindow;
		extern ExtensionsWindow *TheExtensionsWindow;

	#endif

	C4API extern ConsoleWindow *TheConsoleWindow;
}


#endif

// ZYUQURM
