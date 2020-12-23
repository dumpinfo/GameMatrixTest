 

#include "C4ToolWindows.h"
#include "C4ConfigData.h"
#include "C4AudioCapture.h"
#include "C4Input.h"
#include "C4World.h"


using namespace C4;


TimeWindow *C4::TheTimeWindow = nullptr;

#if C4STATS

	StatsWindow *C4::TheStatsWindow = nullptr;

#endif

#if C4DIAGS

	FrameBufferWindow *C4::TheFrameBufferWindow = nullptr;
	ShadowMapWindow *C4::TheShadowMapWindow = nullptr;
	NetworkWindow *C4::TheNetworkWindow = nullptr;
	ExtensionsWindow *C4::TheExtensionsWindow = nullptr;

#endif

ConsoleWindow *C4::TheConsoleWindow = nullptr;


unsigned_int32 ConsoleWindow::commandHistoryCount = 0;
unsigned_int32 ConsoleWindow::commandHistoryStart = 0;
unsigned_int32 ConsoleWindow::commandHistoryOffset = 0;
String<kMaxCommandLength> ConsoleWindow::commandHistory[kConsoleCommandHistoryCount];


#if C4DIAGS

	FrameBufferWidget::FrameBufferWidget(unsigned_int32 index, const Vector2D& size) :
			RenderableWidget(kWidgetFrameBuffer, kRenderQuads, size),
			vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
	{
		frameBufferIndex = index;
	}

	FrameBufferWidget::~FrameBufferWidget()
	{
	}

	void FrameBufferWidget::Preprocess(void)
	{
		RenderableWidget::Preprocess();

		SetAmbientBlendState(kBlendReplace);

		int32 vertexCount = (frameBufferIndex == FrameBufferProcess::kFrameBufferInfiniteShadow) ? 16 : ((frameBufferIndex == FrameBufferProcess::kFrameBufferPointShadow) ? 24 : 4);
		SetVertexCount(vertexCount);

		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(BufferVertex));
		SetVertexAttributeArray(kArrayPosition, 0, 2);
		SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 3);
		vertexBuffer.Establish(sizeof(BufferVertex) * vertexCount);

		attributeList.Append(&shaderAttribute);
		SetMaterialAttributeList(&attributeList);

		ShaderGraph *shaderGraph = shaderAttribute.GetShaderGraph();

		Process *texcoordProcess = new RawTexcoordProcess;
		Process *frameBufferProcess = new FrameBufferProcess(frameBufferIndex);
		Process *ambientOutputProcess = new AmbientOutputProcess;

		shaderGraph->AddElement(texcoordProcess);
		shaderGraph->AddElement(frameBufferProcess);
		shaderGraph->AddElement(ambientOutputProcess);

		new Route(texcoordProcess, frameBufferProcess, 0);
		new Route(frameBufferProcess, ambientOutputProcess, 0);
	}

	void FrameBufferWidget::Build(void)
	{
		volatile BufferVertex *restrict vertex = vertexBuffer.BeginUpdate<BufferVertex>();

		float w = GetWidgetSize().x;
		float h = GetWidgetSize().y;

		if (frameBufferIndex == FrameBufferProcess::kFrameBufferInfiniteShadow)
		{
			h *= 0.25F;

			#if C4OPENGL

				vertex[0].position.Set(0.0F, 0.0F);
				vertex[0].texcoord.Set(0.0F, 1.0F, 0.0F);
				vertex[1].position.Set(0.0F, h);
				vertex[1].texcoord.Set(0.0F, 0.0F, 0.0F);
				vertex[2].position.Set(w, h);
				vertex[2].texcoord.Set(1.0F, 0.0F, 0.0F);
				vertex[3].position.Set(w, 0.0F);
				vertex[3].texcoord.Set(1.0F, 1.0F, 0.0F); 

				vertex[4].position.Set(0.0F, h);
				vertex[4].texcoord.Set(0.0F, 1.0F, 1.0F); 
				vertex[5].position.Set(0.0F, h * 2.0F);
				vertex[5].texcoord.Set(0.0F, 0.0F, 1.0F); 
				vertex[6].position.Set(w, h * 2.0F);
				vertex[6].texcoord.Set(1.0F, 0.0F, 1.0F);
				vertex[7].position.Set(w, h); 
				vertex[7].texcoord.Set(1.0F, 1.0F, 1.0F);
 
				vertex[8].position.Set(0.0F, h * 2.0F); 
				vertex[8].texcoord.Set(0.0F, 1.0F, 2.0F);
				vertex[9].position.Set(0.0F, h * 3.0F);
				vertex[9].texcoord.Set(0.0F, 0.0F, 2.0F);
				vertex[10].position.Set(w, h * 3.0F); 
				vertex[10].texcoord.Set(1.0F, 0.0F, 2.0F);
				vertex[11].position.Set(w, h * 2.0F);
				vertex[11].texcoord.Set(1.0F, 1.0F, 2.0F);

				vertex[12].position.Set(0.0F, h * 3.0F);
				vertex[12].texcoord.Set(0.0F, 1.0F, 3.0F);
				vertex[13].position.Set(0.0F, h * 4.0F);
				vertex[13].texcoord.Set(0.0F, 0.0F, 3.0F);
				vertex[14].position.Set(w, h * 4.0F);
				vertex[14].texcoord.Set(1.0F, 0.0F, 3.0F);
				vertex[15].position.Set(w, h * 3.0F);
				vertex[15].texcoord.Set(1.0F, 1.0F, 3.0F);

			#else

				vertex[0].position.Set(0.0F, 0.0F);
				vertex[0].texcoord.Set(0.0F, 0.0F, 0.0F);
				vertex[1].position.Set(0.0F, h);
				vertex[1].texcoord.Set(0.0F, 1.0F, 0.0F);
				vertex[2].position.Set(w, h);
				vertex[2].texcoord.Set(1.0F, 1.0F, 0.0F);
				vertex[3].position.Set(w, 0.0F);
				vertex[3].texcoord.Set(1.0F, 0.0F, 0.0F);

				vertex[4].position.Set(0.0F, h);
				vertex[4].texcoord.Set(0.0F, 0.0F, 1.0F);
				vertex[5].position.Set(0.0F, h * 2.0F);
				vertex[5].texcoord.Set(0.0F, 1.0F, 1.0F);
				vertex[6].position.Set(w, h * 2.0F);
				vertex[6].texcoord.Set(1.0F, 1.0F, 1.0F);
				vertex[7].position.Set(w, h);
				vertex[7].texcoord.Set(1.0F, 0.0F, 1.0F);

				vertex[8].position.Set(0.0F, h * 2.0F);
				vertex[8].texcoord.Set(0.0F, 0.0F, 2.0F);
				vertex[9].position.Set(0.0F, h * 3.0F);
				vertex[9].texcoord.Set(0.0F, 1.0F, 2.0F);
				vertex[10].position.Set(w, h * 3.0F);
				vertex[10].texcoord.Set(1.0F, 1.0F, 2.0F);
				vertex[11].position.Set(w, h * 2.0F);
				vertex[11].texcoord.Set(1.0F, 0.0F, 2.0F);

				vertex[12].position.Set(0.0F, h * 3.0F);
				vertex[12].texcoord.Set(0.0F, 0.0F, 3.0F);
				vertex[13].position.Set(0.0F, h * 4.0F);
				vertex[13].texcoord.Set(0.0F, 1.0F, 3.0F);
				vertex[14].position.Set(w, h * 4.0F);
				vertex[14].texcoord.Set(1.0F, 1.0F, 3.0F);
				vertex[15].position.Set(w, h * 3.0F);
				vertex[15].texcoord.Set(1.0F, 0.0F, 3.0F);

			#endif
		}
		else if (frameBufferIndex == FrameBufferProcess::kFrameBufferPointShadow)
		{
			w *= 0.25F;
			h *= K::one_over_3;

			vertex[0].position.Set(0.0F, h);
			vertex[0].texcoord.Set(-1.0F, 1.0F, 1.0F);
			vertex[1].position.Set(0.0F, h * 2.0F);
			vertex[1].texcoord.Set(-1.0F, 1.0F, -1.0F);
			vertex[2].position.Set(w, h * 2.0F);
			vertex[2].texcoord.Set(1.0F, 1.0F, -1.0F);
			vertex[3].position.Set(w, h);
			vertex[3].texcoord.Set(1.0F, 1.0F, 1.0F);

			vertex[4].position.Set(w, h);
			vertex[4].texcoord.Set(1.0F, 1.0F, 1.0F);
			vertex[5].position.Set(w, h * 2.0F);
			vertex[5].texcoord.Set(1.0F, 1.0F, -1.0F);
			vertex[6].position.Set(w * 2.0F, h * 2.0F);
			vertex[6].texcoord.Set(1.0F, -1.0F, -1.0F);
			vertex[7].position.Set(w * 2.0F, h);
			vertex[7].texcoord.Set(1.0F, -1.0F, 1.0F);

			vertex[8].position.Set(w * 2.0F, h);
			vertex[8].texcoord.Set(1.0F, -1.0F, 1.0F);
			vertex[9].position.Set(w * 2.0F, h * 2.0F);
			vertex[9].texcoord.Set(1.0F, -1.0F, -1.0F);
			vertex[10].position.Set(w * 3.0F, h * 2.0F);
			vertex[10].texcoord.Set(-1.0F, -1.0F, -1.0F);
			vertex[11].position.Set(w * 3.0F, h);
			vertex[11].texcoord.Set(-1.0F, -1.0F, 1.0F);

			vertex[12].position.Set(w * 3.0F, h);
			vertex[12].texcoord.Set(-1.0F, -1.0F, 1.0F);
			vertex[13].position.Set(w * 3.0F, h * 2.0F);
			vertex[13].texcoord.Set(-1.0F, -1.0F, -1.0F);
			vertex[14].position.Set(w * 4.0F, h * 2.0F);
			vertex[14].texcoord.Set(-1.0F, 1.0F, -1.0F);
			vertex[15].position.Set(w * 4.0F, h);
			vertex[15].texcoord.Set(-1.0F, 1.0F, 1.0F);

			vertex[16].position.Set(w, 0.0F);
			vertex[16].texcoord.Set(-1.0F, 1.0F, 1.0F);
			vertex[17].position.Set(w, h);
			vertex[17].texcoord.Set(1.0F, 1.0F, 1.0F);
			vertex[18].position.Set(w * 2.0F, h);
			vertex[18].texcoord.Set(1.0F, -1.0F, 1.0F);
			vertex[19].position.Set(w * 2.0F, 0.0F);
			vertex[19].texcoord.Set(-1.0F, -1.0F, 1.0F);

			vertex[20].position.Set(w, h * 2.0F);
			vertex[20].texcoord.Set(1.0F, 1.0F, -1.0F);
			vertex[21].position.Set(w, h * 3.0F);
			vertex[21].texcoord.Set(-1.0F, 1.0F, -1.0F);
			vertex[22].position.Set(w * 2.0F, h * 3.0F);
			vertex[22].texcoord.Set(-1.0F, -1.0F, -1.0F);
			vertex[23].position.Set(w * 2.0F, h * 2.0F);
			vertex[23].texcoord.Set(1.0F, -1.0F, -1.0F);
		}
		else if (frameBufferIndex == FrameBufferProcess::kFrameBufferSpotShadow)
		{
			#if C4OPENGL

				vertex[0].position.Set(0.0F, 0.0F);
				vertex[0].texcoord.Set(0.0F, 1.0F, 0.0F);
				vertex[1].position.Set(0.0F, h);
				vertex[1].texcoord.Set(0.0F, 0.0F, 0.0F);
				vertex[2].position.Set(w, h);
				vertex[2].texcoord.Set(1.0F, 0.0F, 0.0F);
				vertex[3].position.Set(w, 0.0F);
				vertex[3].texcoord.Set(1.0F, 1.0F, 0.0F);

			#else

				vertex[0].position.Set(0.0F, 0.0F);
				vertex[0].texcoord.Set(0.0F, 0.0F, 0.0F);
				vertex[1].position.Set(0.0F, h);
				vertex[1].texcoord.Set(0.0F, 1.0F, 0.0F);
				vertex[2].position.Set(w, h);
				vertex[2].texcoord.Set(1.0F, 1.0F, 0.0F);
				vertex[3].position.Set(w, 0.0F);
				vertex[3].texcoord.Set(1.0F, 0.0F, 0.0F);

			#endif
		}
		else
		{
			#if C4RENDER_TEXTURE_RECTANGLE

				float tw = (float) TheDisplayMgr->GetDisplayWidth();
				float th = (float) TheDisplayMgr->GetDisplayHeight();

				if (frameBufferIndex == FrameBufferProcess::kFrameBufferGlowBloom)
				{
					tw *= 0.5F;
					th *= 0.5F;
				}

			#else

				float tw = 1.0F;
				float th = 1.0F;

			#endif

			#if C4OPENGL

				vertex[0].position.Set(0.0F, 0.0F);
				vertex[0].texcoord.Set(0.0F, th, 0.0F);
				vertex[1].position.Set(0.0F, h);
				vertex[1].texcoord.Set(0.0F, 0.0F, 0.0F);
				vertex[2].position.Set(w, h);
				vertex[2].texcoord.Set(tw, 0.0F, 0.0F);
				vertex[3].position.Set(w, 0.0F);
				vertex[3].texcoord.Set(tw, th, 0.0F);

			#else

				vertex[0].position.Set(0.0F, 0.0F);
				vertex[0].texcoord.Set(0.0F, 0.0F, 0.0F);
				vertex[1].position.Set(0.0F, h);
				vertex[1].texcoord.Set(0.0F, th, 0.0F);
				vertex[2].position.Set(w, h);
				vertex[2].texcoord.Set(tw, th, 0.0F);
				vertex[3].position.Set(w, 0.0F);
				vertex[3].texcoord.Set(tw, 0.0F, 0.0F);

			#endif
		}

		vertexBuffer.EndUpdate();
	}

#endif


TimeWindow::TimeWindow() :
		Window("C4/Time"),
		Singleton<TimeWindow>(TheTimeWindow),
		rateGraph(ColorRGBA(0.75F, 0.75F, 0.75F, 0.75F))
{
	SetWidgetPosition(Point3D(TheInterfaceMgr->GetDesktopSize().x - GetWidgetSize().x - 10.0F, 26.0F, 0.0F));

	AppendSubnode(&rateGraph);
	AppendSubnode(&renderGraph);

	TheGraphicsMgr->SetDiagnosticFlags(TheGraphicsMgr->GetDiagnosticFlags() | kDiagnosticTimer);
}

TimeWindow::~TimeWindow()
{
	TheGraphicsMgr->SetDiagnosticFlags(TheGraphicsMgr->GetDiagnosticFlags() & ~kDiagnosticTimer);
}

void TimeWindow::Open(void)
{
	if (TheTimeWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheTimeWindow);
	}
	else
	{
		TheInterfaceMgr->AddWidget(new TimeWindow);
	}
}

TimeWindow::GraphWidget::GraphWidget(const ColorRGBA& color) :
		RenderableWidget(kWidgetGraph, kRenderTriangleStrip, Vector2D((float) (kTimeMeasureFrameCount - 1) * 4.0F, 80.0F)),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		diffuseAttribute(color)
{
}

TimeWindow::GraphWidget::~GraphWidget()
{
}

void TimeWindow::GraphWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexCount(kTimeMeasureFrameCount * 2);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(Point2D));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	vertexBuffer.Establish(sizeof(Point2D) * kTimeMeasureFrameCount * 2);

	attributeList.Append(&diffuseAttribute);
	SetMaterialAttributeList(&attributeList);

	float x = 0.0F;
	float h = GetWidgetSize().y;

	Point2D *vertex = vertexArray;
	for (machine a = 0; a < kTimeMeasureFrameCount; a++)
	{
		vertex[0].Set(x, h);
		vertex[1].Set(x, h);
		vertex += 2;
		x += 4.0F;
	}

	vertexBuffer.UpdateBufferSync(0, sizeof(Point2D) * kTimeMeasureFrameCount * 2, vertexArray);
}

void TimeWindow::GraphWidget::AddValue(float value)
{
	Point2D *vertex = vertexArray;
	for (machine a = 0; a < kTimeMeasureFrameCount - 1; a++)
	{
		vertex[0].y = vertex[2].y;
		vertex += 2;
	}

	vertex[0].y = FmaxZero(GetWidgetSize().y - value);

	vertexBuffer.UpdateBuffer(0, sizeof(Point2D) * kTimeMeasureFrameCount * 2, vertexArray);
}

TimeWindow::MultigraphWidget::MultigraphWidget() :
		RenderableWidget(kWidgetMultigraph, kRenderIndexedTriangles, Vector2D((float) (kTimeMeasureFrameCount - 1) * 4.0F, 80.0F)),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		indexBuffer(kVertexBufferIndex | kVertexBufferDynamic)
{
}

TimeWindow::MultigraphWidget::~MultigraphWidget()
{
}

void TimeWindow::MultigraphWidget::Preprocess(void)
{
	static const ConstColor4C graphColor[6] =
	{
		{128, 255, 128, 192}, {255, 255, 128, 192}, {255, 192, 0, 192}, {255, 64, 0, 192}, {232, 0, 155, 192}, {0, 192, 255, 192}
	};

	RenderableWidget::Preprocess();

	SetVertexCount(kTimeMeasureFrameCount * 12);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(GraphVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 1);
	vertexBuffer.Establish(sizeof(GraphVertex) * kTimeMeasureFrameCount * 12);

	SetPrimitiveCount((kTimeMeasureFrameCount - 1) * 12);
	SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);
	indexBuffer.Establish(sizeof(Triangle) * (kTimeMeasureFrameCount - 1) * 12);

	float h = GetWidgetSize().y;
	unsigned_int32 i = 0;

	GraphVertex *vertex = vertexArray;
	for (machine a = 0; a < 6; a++)
	{
		float x = 0.0F;
		for (machine b = 0; b < kTimeMeasureFrameCount; b++)
		{
			vertex[0].position.Set(x, h);
			vertex[0].color = graphColor[a];

			vertex[1].position.Set(x, h);
			vertex[1].color = graphColor[a];

			x += 4.0F;
			vertex += 2;
		}
	}

	vertexBuffer.UpdateBufferSync(0, sizeof(GraphVertex) * kTimeMeasureFrameCount * 12, vertexArray);

	volatile Triangle *restrict triangle = indexBuffer.BeginUpdateSync<Triangle>();

	for (machine a = 0; a < 6; a++)
	{
		for (machine b = 0; b < kTimeMeasureFrameCount - 1; b++)
		{
			triangle[0].Set(i, i + 1, i + 2);
			triangle[1].Set(i + 1, i + 3, i + 2);
			triangle += 2;
			i += 2;
		}

		i += 2;
	}

	indexBuffer.EndUpdateSync();
}

void TimeWindow::MultigraphWidget::AddValues(const float (& value)[6])
{
	float	sum[7];

	float h = GetWidgetSize().y;

	sum[6] = 0.0F;
	sum[5] = Fmin(value[5], h);
	for (machine a = 4; a >= 0; a--)
	{
		sum[a] = Fmin(value[a] + sum[a + 1], h);
	}

	GraphVertex *vertex = vertexArray;
	for (machine a = 0; a < 6; a++)
	{
		for (machine b = 0; b < kTimeMeasureFrameCount - 1; b++)
		{
			vertex[0].position.y = vertex[2].position.y;
			vertex[1].position.y = vertex[3].position.y;
			vertex += 2;
		}

		vertex[0].position.y = h - sum[a];
		vertex[1].position.y = h - sum[a + 1];
		vertex += 2;
	}

	vertexBuffer.UpdateBuffer(0, sizeof(GraphVertex) * kTimeMeasureFrameCount * 12, vertexArray);
}

void TimeWindow::Preprocess(void)
{
	Window::Preprocess();

	rateText = static_cast<TextWidget *>(FindWidget("Rate"));
	timeText = static_cast<TextWidget *>(FindWidget("Time"));

	structureText = static_cast<TextWidget *>(FindWidget("Structure"));
	occlusionText = static_cast<TextWidget *>(FindWidget("Occlusion"));
	renderText = static_cast<TextWidget *>(FindWidget("Render"));
	shadowText = static_cast<TextWidget *>(FindWidget("Shadow"));
	postText = static_cast<TextWidget *>(FindWidget("Post"));
	guiText = static_cast<TextWidget *>(FindWidget("Gui"));

	rateGraph.SetWidgetPosition(FindWidget("CPU")->GetWidgetPosition());
	renderGraph.SetWidgetPosition(FindWidget("GPU")->GetWidgetPosition());

	frameCount = 0;
	int32 time = TheTimeMgr->GetSystemMicrosecondTime();
	for (machine a = 0; a < kTimeMeasureFrameCount; a++)
	{
		timeTable[a] = time;
	}
}

void TimeWindow::SetTimeValue(TextWidget *widget, unsigned_int32 time)
{
	time = Min(time, 9999);
	int32 ms = time / 10;
	String<15> string(ms);
	string += '.';
	string += (char) (time - ms * 10 + 48);
	widget->SetText(string);
}

void TimeWindow::Move(void)
{
	float					graphValue[6];
	TimestampType			timestampType[kMaxTimestampCount];
	unsigned_int64			timestampValue[kMaxTimestampCount];

	unsigned_int32 time = TheTimeMgr->GetSystemMicrosecondTime();

	for (machine a = 0; a < kTimeMeasureFrameCount - 1; a++)
	{
		timeTable[a] = timeTable[a + 1];
	}

	timeTable[kTimeMeasureFrameCount - 1] = time;

	if (frameCount >= kTimeMeasureFrameCount)
	{
		time -= timeTable[0];
		if (time != 0)
		{
			int32 rate = (kTimeMeasureFrameCount - 1) * 10000000 / time;
			if (rate < 1000)
			{
				int32 fps = rate / 10;
				String<15> string(fps);
				string += '.';
				string += (char) (rate - fps * 10 + 48);
				rateText->SetText(string);
			}
			else
			{
				rateText->SetText(String<15>(rate / 10));
			}
		}

		SetTimeValue(timeText, time / ((kTimeMeasureFrameCount - 1) * 100));
	}
	else
	{
		frameCount++;
	}

	int32 dt = timeTable[kTimeMeasureFrameCount - 1] - timeTable[kTimeMeasureFrameCount - 2];
	rateGraph.AddValue((float) (dt / 1000));

	int32 timestampCount = TheGraphicsMgr->GetTimestampData(timestampType, timestampValue);

	int64 renderTime = 0;
	int64 structureTime = 0;
	int64 occlusionTime = 0;
	int64 shadowTime = 0;
	int64 postTime = 0;
	int64 guiTime = 0;

	for (machine a = 0; a < timestampCount; a++)
	{
		unsigned_int64 stamp = timestampValue[a];

		switch (timestampType[a])
		{
			case kTimestampBeginRendering:
				renderTime -= stamp;
				break;
			case kTimestampEndRendering:
				renderTime += stamp;
				break;
			case kTimestampBeginStructure:
				structureTime -= stamp;
				break;
			case kTimestampEndStructure:
				structureTime += stamp;
				break;
			case kTimestampBeginOcclusion:
				occlusionTime -= stamp;
				break;
			case kTimestampEndOcclusion:
				occlusionTime += stamp;
				break;
			case kTimestampBeginShadow:
				shadowTime -= stamp;
				break;
			case kTimestampEndShadow:
				shadowTime += stamp;
				break;
			case kTimestampBeginPost:
				postTime -= stamp;
				break;
			case kTimestampEndPost:
				postTime += stamp;
				break;
			case kTimestampBeginGui:
				guiTime -= stamp;
				break;
			case kTimestampEndGui:
				guiTime += stamp;
				break;
		}
	}

	renderTime -= structureTime + occlusionTime + shadowTime + postTime + guiTime;

	if (structureTime > 0)
	{
		time = unsigned_int32(structureTime);
		graphValue[0] = (float) (time / 1000000);
		SetTimeValue(structureText, time / 100000);
	}
	else
	{
		graphValue[0] = 0;
		structureText->SetText("\xE2\x80\x92");	// U+2012
	}

	if (occlusionTime > 0)
	{
		time = unsigned_int32(occlusionTime);
		graphValue[1] = (float) (time / 1000000);
		SetTimeValue(occlusionText, time / 100000);
	}
	else
	{
		graphValue[1] = 0;
		occlusionText->SetText("\xE2\x80\x92");
	}

	if (renderTime > 0)
	{
		time = unsigned_int32(renderTime);
		graphValue[2] = (float) (time / 1000000);
		SetTimeValue(renderText, time / 100000);
	}
	else
	{
		graphValue[2] = 0;
		renderText->SetText("\xE2\x80\x92");
	}

	if (shadowTime > 0)
	{
		time = unsigned_int32(shadowTime);
		graphValue[3] = (float) (time / 1000000);
		SetTimeValue(shadowText, time / 100000);
	}
	else
	{
		graphValue[3] = 0;
		shadowText->SetText("\xE2\x80\x92");
	}

	if (postTime > 0)
	{
		time = unsigned_int32(postTime);
		graphValue[4] = (float) (time / 1000000);
		SetTimeValue(postText, time / 100000);
	}
	else
	{
		graphValue[4] = 0;
		postText->SetText("\xE2\x80\x92");
	}

	if (guiTime > 0)
	{
		time = unsigned_int32(guiTime);
		graphValue[5] = (float) (time / 1000000);
		SetTimeValue(guiText, time / 100000);
	}
	else
	{
		graphValue[5] = 0;
		guiText->SetText("\xE2\x80\x92");
	}

	renderGraph.AddValues(graphValue);

	TheGraphicsMgr->SetDiagnosticFlags(TheGraphicsMgr->GetDiagnosticFlags() | kDiagnosticTimer);
}

bool TimeWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
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


#if C4STATS

	StatsWindow::StatsWindow() :
			Window("C4/Stats"),
			Singleton<StatsWindow>(TheStatsWindow)
	{
		const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();
		SetWidgetPosition(Point3D(desktopSize.x - GetWidgetSize().x - 10.0F, desktopSize.y - GetWidgetSize().y - 10.0F, 0.0F));
	}

	StatsWindow::~StatsWindow()
	{
	}

	void StatsWindow::Open(void)
	{
		if (TheStatsWindow)
		{
			TheInterfaceMgr->SetActiveWindow(TheStatsWindow);
		}
		else
		{
			TheInterfaceMgr->AddWidget(new StatsWindow);
		}
	}

	void StatsWindow::Preprocess(void)
	{
		static const char *const renderKey[kRenderStatCount] =
		{
			"RenderVerts", "RenderPrims", "RenderCmds", "ShadowVerts", "ShadowPrims", "ShadowCmds", "StructVerts", "StructPrims", "StructCmds",
			"DistortVerts", "DistortPrims", "DistortCmds", "TotalVerts", "TotalPrims", "TotalCmds", "Texture", "TextureMem", "VBO", "VertexMem"
		};

		static const char *const worldKey[kWorldStatCount] =
		{
			"Geometry", "Terrain", "Water", "Impostor", "InfiniteLight", "InfiniteShadow", "PointLight", "PointShadow", "SpotLight", "SpotShadow",
			"DirectPortal", "RemotePortal", "Occlusion"
		};

		static const char *const physicsKey[kPhysicsStatCount] =
		{
			"RigidBody", "DeformMove", "DeformUpdate", "WaterMove", "WaterUpdate", "Buoyancy", "GeomIntersect", "BodyIntersect", "Island"
		};

		static const char *const miscKey[kMiscStatCount] =
		{
			"PlayingSource", "EngagedSource", "Script"
		};

		Window::Preprocess();

		paneWidget = static_cast<MultipaneWidget *>(FindWidget("Pane"));
		renderGroup = FindWidget("Render");
		worldGroup = FindWidget("World");
		physicsGroup = FindWidget("Physics");
		miscGroup = FindWidget("Misc");

		for (machine a = 0; a < kRenderStatCount; a++)
		{
			renderStatText[a] = static_cast<TextWidget *>(FindWidget(renderKey[a]));
		}

		for (machine a = 0; a < kWorldStatCount; a++)
		{
			worldStatText[a] = static_cast<TextWidget *>(FindWidget(worldKey[a]));
		}

		for (machine a = 0; a < kPhysicsStatCount; a++)
		{
			physicsStatText[a] = static_cast<TextWidget *>(FindWidget(physicsKey[a]));
		}

		for (machine a = 0; a < kMiscStatCount; a++)
		{
			miscStatText[a] = static_cast<TextWidget *>(FindWidget(miscKey[a]));
		}
	}

	void StatsWindow::Move(void)
	{
		int32 pane = paneWidget->GetSelection();
		if (pane == 0)
		{
			int32 vertexTotal = 0;
			int32 primitiveTotal = 0;
			int32 commandTotal = 0;

			int32 vertexCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterDirectVertices);
			int32 primitiveCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterDirectPrimitives);
			int32 commandCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterDirectCommands);

			vertexTotal += vertexCount;
			primitiveTotal += primitiveCount;
			commandTotal += commandCount;

			renderStatText[kRenderStatDirectVerts]->SetText(String<7>(vertexCount));
			renderStatText[kRenderStatDirectPrims]->SetText(String<7>(primitiveCount));
			renderStatText[kRenderStatDirectCmds]->SetText(String<7>(commandCount));

			vertexCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterShadowVertices);
			primitiveCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterShadowPrimitives);
			commandCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterShadowCommands);

			vertexTotal += vertexCount;
			primitiveTotal += primitiveCount;
			commandTotal += commandCount;

			renderStatText[kRenderStatShadowVerts]->SetText(String<7>(vertexCount));
			renderStatText[kRenderStatShadowPrims]->SetText(String<7>(primitiveCount));
			renderStatText[kRenderStatShadowCmds]->SetText(String<7>(commandCount));

			vertexCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterStructureVertices);
			primitiveCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterStructurePrimitives);
			commandCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterStructureCommands);

			vertexTotal += vertexCount;
			primitiveTotal += primitiveCount;
			commandTotal += commandCount;

			renderStatText[kRenderStatVelocityVerts]->SetText(String<7>(vertexCount));
			renderStatText[kRenderStatVelocityPrims]->SetText(String<7>(primitiveCount));
			renderStatText[kRenderStatVelocityCmds]->SetText(String<7>(commandCount));

			vertexCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterDistortionVertices);
			primitiveCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterDistortionPrimitives);
			commandCount = TheGraphicsMgr->GetGraphicsCounter(kGraphicsCounterDistortionCommands);

			vertexTotal += vertexCount;
			primitiveTotal += primitiveCount;
			commandTotal += commandCount;

			renderStatText[kRenderStatDistortionVerts]->SetText(String<7>(vertexCount));
			renderStatText[kRenderStatDistortionPrims]->SetText(String<7>(primitiveCount));
			renderStatText[kRenderStatDistortionCmds]->SetText(String<7>(commandCount));

			renderStatText[kRenderStatTotalVerts]->SetText(String<7>(vertexTotal));
			renderStatText[kRenderStatTotalPrims]->SetText(String<7>(primitiveTotal));
			renderStatText[kRenderStatTotalCmds]->SetText(String<7>(commandTotal));

			renderStatText[kRenderStatTextureCount]->SetText(String<7>(Texture::GetTotalTextureCount()));
			renderStatText[kRenderStatTextureMemory]->SetText(String<7>((Texture::GetTotalTextureMemory() + 0x03FF) >> 10));
			renderStatText[kRenderStatVertexBufferCount]->SetText(String<7>(VertexBuffer::GetTotalVertexBufferCount()));
			renderStatText[kRenderStatVertexBufferMemory]->SetText(String<7>((VertexBuffer::GetTotalVertexBufferMemory() + 0x03FF) >> 10));
		}
		else if (pane == 1)
		{
			const World *world = TheWorldMgr->GetWorld();
			if (world)
			{
				worldStatText[kWorldStatGeometryCount]->SetText(String<7>(world->GetWorldCounter(kWorldCounterGeometry)));
				worldStatText[kWorldStatTerrainCount]->SetText(String<7>(world->GetWorldCounter(kWorldCounterTerrain)));
				worldStatText[kWorldStatWaterCount]->SetText(String<7>(world->GetWorldCounter(kWorldCounterWater)));
				worldStatText[kWorldStatImpostorCount]->SetText(String<7>(world->GetWorldCounter(kWorldCounterImpostor)));
				worldStatText[kWorldStatInfiniteLightCount]->SetText(((String<15>(world->GetWorldCounter(kWorldCounterInfiniteLight)) += " / ") += world->GetWorldCounter(kWorldCounterInfiniteShadowCascade)));
				worldStatText[kWorldStatInfiniteShadowCount]->SetText(String<7>(world->GetWorldCounter(kWorldCounterInfiniteShadowGeometry)));
				worldStatText[kWorldStatPointLightCount]->SetText(((String<15>(world->GetWorldCounter(kWorldCounterPointLight)) += " / ") += world->GetWorldCounter(kWorldCounterPointShadowFace)));
				worldStatText[kWorldStatPointShadowCount]->SetText(String<7>(world->GetWorldCounter(kWorldCounterPointShadowGeometry)));
				worldStatText[kWorldStatSpotLightCount]->SetText(String<7>(world->GetWorldCounter(kWorldCounterSpotLight)));
				worldStatText[kWorldStatSpotShadowCount]->SetText(String<7>(world->GetWorldCounter(kWorldCounterSpotShadowGeometry)));
				worldStatText[kWorldStatDirectPortals]->SetText(String<7>(world->GetWorldCounter(kWorldCounterDirectPortal)));
				worldStatText[kWorldStatRemotePortals]->SetText(String<7>(world->GetWorldCounter(kWorldCounterRemotePortal)));
				worldStatText[kWorldStatOcclusionRegions]->SetText(String<7>(world->GetWorldCounter(kWorldCounterOcclusionRegion)));
			}
			else
			{
				worldStatText[kWorldStatGeometryCount]->SetText("0");
				worldStatText[kWorldStatTerrainCount]->SetText("0");
				worldStatText[kWorldStatWaterCount]->SetText("0");
				worldStatText[kWorldStatImpostorCount]->SetText("0");
				worldStatText[kWorldStatInfiniteLightCount]->SetText("0");
				worldStatText[kWorldStatInfiniteShadowCount]->SetText("0");
				worldStatText[kWorldStatPointLightCount]->SetText("0");
				worldStatText[kWorldStatPointShadowCount]->SetText("0");
				worldStatText[kWorldStatSpotLightCount]->SetText("0");
				worldStatText[kWorldStatSpotShadowCount]->SetText("0");
				worldStatText[kWorldStatDirectPortals]->SetText("0");
				worldStatText[kWorldStatRemotePortals]->SetText("0");
				worldStatText[kWorldStatOcclusionRegions]->SetText("0");
			}
		}
		else if (pane == 2)
		{
			const World *world = TheWorldMgr->GetWorld();
			if (world)
			{
				int32 rigidBodyTotal = 0;
				int32 buoyancyTotal = 0;
				int32 geometryIntersectionTotal = 0;
				int32 rigidBodyIntersectionTotal = 0;
				int32 constraintSolverIslandTotal = 0;
				int32 deformableBodyMoveTotal = 0;
				int32 deformableBodyUpdateTotal = 0;

				const PhysicsController *physicsController = world->FindPhysicsController();
				if (physicsController)
				{
					rigidBodyTotal = physicsController->GetPhysicsCounter(kPhysicsCounterRigidBody);
					buoyancyTotal = physicsController->GetPhysicsCounter(kPhysicsCounterBuoyancy);
					geometryIntersectionTotal = physicsController->GetPhysicsCounter(kPhysicsCounterGeometryIntersection);
					rigidBodyIntersectionTotal = physicsController->GetPhysicsCounter(kPhysicsCounterShapeIntersection);
					constraintSolverIslandTotal = physicsController->GetPhysicsCounter(kPhysicsCounterConstraintSolverIsland);
					deformableBodyMoveTotal = physicsController->GetPhysicsCounter(kPhysicsCounterDeformableBodyMove);
					deformableBodyUpdateTotal = physicsController->GetPhysicsCounter(kPhysicsCounterDeformableBodyUpdate);
				}

				physicsStatText[kPhysicsStatRigidBodyCount]->SetText(String<7>(rigidBodyTotal));
				physicsStatText[kPhysicsStatBuoyancyCount]->SetText(String<7>(buoyancyTotal));
				physicsStatText[kPhysicsStatGeometryIntersections]->SetText(String<7>(geometryIntersectionTotal));
				physicsStatText[kPhysicsStatRigidBodyIntersections]->SetText(String<7>(rigidBodyIntersectionTotal));
				physicsStatText[kPhysicsStatConstraintSolverIslands]->SetText(String<7>(constraintSolverIslandTotal));
				physicsStatText[kPhysicsStatDeformableBodyMoveCount]->SetText(String<7>(deformableBodyMoveTotal));
				physicsStatText[kPhysicsStatDeformableBodyUpdateCount]->SetText(String<7>(deformableBodyUpdateTotal));
				physicsStatText[kPhysicsStatWaterMoveCount]->SetText(String<7>(world->GetWorldCounter(kWorldCounterWaterMove)));
				physicsStatText[kPhysicsStatWaterUpdateCount]->SetText(String<7>(world->GetWorldCounter(kWorldCounterWaterUpdate)));
			}
			else
			{
				physicsStatText[kPhysicsStatRigidBodyCount]->SetText("0");
				physicsStatText[kPhysicsStatBuoyancyCount]->SetText("0");
				physicsStatText[kPhysicsStatGeometryIntersections]->SetText("0");
				physicsStatText[kPhysicsStatRigidBodyIntersections]->SetText("0");
				physicsStatText[kPhysicsStatDeformableBodyMoveCount]->SetText("0");
				physicsStatText[kPhysicsStatDeformableBodyUpdateCount]->SetText("0");
				physicsStatText[kPhysicsStatWaterMoveCount]->SetText("0");
				physicsStatText[kPhysicsStatWaterUpdateCount]->SetText("0");
			}
		}
		else if (pane == 3)
		{
			const World *world = TheWorldMgr->GetWorld();
			if (world)
			{
				miscStatText[kMiscStatPlayingSources]->SetText(String<7>(world->GetWorldCounter(kWorldCounterPlayingSource)));
				miscStatText[kMiscStatEngagedSources]->SetText(String<7>(world->GetWorldCounter(kWorldCounterEngagedSource)));
				miscStatText[kMiscStatRunningScripts]->SetText(String<7>(world->GetWorldCounter(kWorldCounterRunningScript)));
			}
			else
			{
				miscStatText[kMiscStatPlayingSources]->SetText("0");
				miscStatText[kMiscStatEngagedSources]->SetText("0");
				miscStatText[kMiscStatRunningScripts]->SetText("0");
			}
		}
	}

	bool StatsWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
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

	void StatsWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
	{
		if (eventData->eventType == kEventWidgetChange)
		{
			if (widget == paneWidget)
			{
				int32 pane = paneWidget->GetSelection();
				if (pane == 0)
				{
					renderGroup->Show();
					worldGroup->Hide();
					physicsGroup->Hide();
					miscGroup->Hide();
				}
				else if (pane == 1)
				{
					renderGroup->Hide();
					worldGroup->Show();
					physicsGroup->Hide();
					miscGroup->Hide();
				}
				else if (pane == 2)
				{
					renderGroup->Hide();
					worldGroup->Hide();
					physicsGroup->Show();
					miscGroup->Hide();
				}
				else if (pane == 3)
				{
					renderGroup->Hide();
					worldGroup->Hide();
					physicsGroup->Hide();
					miscGroup->Show();
				}
			}
		}
	}

#endif


#if C4DIAGS

	FrameBufferWindow::FrameBufferWindow(int32 bufferIndex) :
			Window(Vector2D((float) TheDisplayMgr->GetDisplayWidth() / (float) TheDisplayMgr->GetDisplayHeight() * 200.0F, 200.0F), "Frame Buffer", kWindowCloseBox | kWindowPassive),
			Singleton<FrameBufferWindow>(TheFrameBufferWindow),
			frameBufferWidget(bufferIndex, GetWidgetSize())
	{
		SetWidgetPosition(Point3D(8.0F, 26.0F, 0.0F));
		AppendSubnode(&frameBufferWidget);
	}

	FrameBufferWindow::~FrameBufferWindow()
	{
	}

	void FrameBufferWindow::Open(int32 bufferIndex)
	{
		if (TheFrameBufferWindow)
		{
			TheInterfaceMgr->SetActiveWindow(TheFrameBufferWindow);
		}
		else
		{
			TheInterfaceMgr->AddWidget(new FrameBufferWindow(bufferIndex));
		}
	}

	bool FrameBufferWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
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


	ShadowMapWindow::ShadowMapWindow(int32 bufferIndex) :
			Window(GetWindowSize(bufferIndex), "Shadow Map", kWindowCloseBox | kWindowPassive),
			Singleton<ShadowMapWindow>(TheShadowMapWindow),
			frameBufferWidget(bufferIndex, GetWidgetSize())
	{
		if (bufferIndex == FrameBufferProcess::kFrameBufferPointShadow)
		{
			SetWindowFlags(GetWindowFlags() | kWindowBackground);
		}

		SetWidgetPosition(Point3D(8.0F, 26.0F, 0.0F));
		AppendSubnode(&frameBufferWidget);
	}

	ShadowMapWindow::~ShadowMapWindow()
	{
	}

	Vector2D ShadowMapWindow::GetWindowSize(int32 bufferIndex) const
	{
		if (bufferIndex == FrameBufferProcess::kFrameBufferInfiniteShadow)
		{
			return (Vector2D(200.0F, 800.0F));
		}

		if (bufferIndex == FrameBufferProcess::kFrameBufferPointShadow)
		{
			return (Vector2D(400.0F, 300.0F));
		}

		return (Vector2D(200.0F, 200.0F));
	}

	void ShadowMapWindow::Open(int32 bufferIndex)
	{
		if (TheShadowMapWindow)
		{
			TheInterfaceMgr->SetActiveWindow(TheShadowMapWindow);
		}
		else
		{
			TheInterfaceMgr->AddWidget(new ShadowMapWindow(bufferIndex));
		}
	}

	bool ShadowMapWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
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


	NetworkWindow::NetworkWindow() :
			Window("C4/Network"),
			Singleton<NetworkWindow>(TheNetworkWindow)
	{
		SetWidgetPosition(Point3D(8.0F, TheInterfaceMgr->GetDesktopSize().y - GetWidgetSize().y - 10.0F, 0.0F));
	}

	NetworkWindow::~NetworkWindow()
	{
		for (machine a = kPacketTypeCount - 1; a >= 0; a--)
		{
			outgoingGraph[a]->~NetworkWidget();
			incomingGraph[a]->~NetworkWidget();
		}
	}

	void NetworkWindow::Open(void)
	{
		if (TheNetworkWindow)
		{
			TheInterfaceMgr->SetActiveWindow(TheNetworkWindow);
		}
		else
		{
			TheInterfaceMgr->AddWidget(new NetworkWindow);
		}
	}

	NetworkWindow::NetworkWidget::NetworkWidget(const Vector2D& size, const ColorRGBA& color) :
			RenderableWidget(kWidgetNetwork, kRenderQuads, size),
			vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
			diffuseAttribute(color)
	{
	}

	NetworkWindow::NetworkWidget::~NetworkWidget()
	{
	}

	void NetworkWindow::NetworkWidget::Preprocess(void)
	{
		RenderableWidget::Preprocess();

		SetVertexCount(128);
		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(Point2D));
		SetVertexAttributeArray(kArrayPosition, 0, 2);
		vertexBuffer.Establish(sizeof(Point2D) * 128);

		attributeList.Append(&diffuseAttribute);
		SetMaterialAttributeList(&attributeList);

		float x1 = 0.0F;
		float x2 = 1.0F;
		float h = GetWidgetSize().y;

		Point2D *vertex = vertexArray;
		for (machine a = 0; a < 32; a++)
		{
			vertex[0].Set(x1, h);
			vertex[1].Set(x1, h);
			vertex[2].Set(x2, h);
			vertex[3].Set(x2, h);
			vertex += 4;

			x1 = x2;
			x2 = x2 + 1.0F;
		}

		vertexBuffer.UpdateBufferSync(0, sizeof(Point2D) * 128, vertexArray);
	}

	void NetworkWindow::NetworkWidget::AddValue(float value)
	{
		for (machine a = 1; a < 32; a++)
		{
			int32 index = a * 4;
			float y = vertexArray[index].y;
			vertexArray[index - 4].y = y;
			vertexArray[index - 1].y = y;
		}

		value = GetWidgetSize().y - value;
		vertexArray[124].y = value;
		vertexArray[127].y = value;

		vertexBuffer.UpdateBuffer(0, sizeof(Point2D) * 128, vertexArray);
	}

	void NetworkWindow::Preprocess(void)
	{
		Window::Preprocess();

		connectionText = static_cast<TextWidget *>(FindWidget("Connect"));
		chatRateText = static_cast<TextWidget *>(FindWidget("Chat"));

		char inKey[4] = "IN0";
		char outKey[5] = "OUT0";

		for (machine a = 0; a < kPacketTypeCount; a++)
		{
			inKey[2] = outKey[3] = (char) (a + 48);

			const Widget *inWidget = FindWidget(inKey);
			const Widget *outWidget = FindWidget(outKey);

			NetworkWidget *graph = new(incomingGraph[a]) NetworkWidget(inWidget->GetWidgetSize(), ColorRGBA(0.5F, 1.0F, 0.0F, 1.0F));
			graph->SetWidgetPosition(inWidget->GetWidgetPosition());
			AppendNewSubnode(graph);

			graph = new(outgoingGraph[a]) NetworkWidget(outWidget->GetWidgetSize(), ColorRGBA(1.0F, 0.5F, 0.0F, 1.0F));
			graph->SetWidgetPosition(outWidget->GetWidgetPosition());
			AppendNewSubnode(graph);
		}
	}

	void NetworkWindow::Move(void)
	{
		connectionText->SetText(String<7>(TheNetworkMgr->GetConnectionCount()));
		chatRateText->SetText(String<7>(TheAudioCaptureMgr->GetChatReceiveRate()));

		for (machine a = 0; a < kPacketTypeCount; a++)
		{
			incomingGraph[a]->AddValue((float) Min(TheNetworkMgr->GetIncomingPacketCounter(a), 32));
			outgoingGraph[a]->AddValue((float) Min(TheNetworkMgr->GetOutgoingPacketCounter(a), 32));
		}
	}

	bool NetworkWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
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


	ExtensionsWindow::ExtensionsWindow() :
			Window("C4/Extensions"),
			Singleton<ExtensionsWindow>(TheExtensionsWindow)
	{
	}

	ExtensionsWindow::~ExtensionsWindow()
	{
	}

	void ExtensionsWindow::Open(void)
	{
		if (TheExtensionsWindow)
		{
			TheInterfaceMgr->SetActiveWindow(TheExtensionsWindow);
		}
		else
		{
			TheInterfaceMgr->AddWidget(new ExtensionsWindow);
		}
	}

	void ExtensionsWindow::Preprocess(void)
	{
		Window::Preprocess();

		okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
		cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

		extensionsList = static_cast<ListWidget *>(FindWidget("List"));
		Vector2D size = extensionsList->GetNaturalListItemSize();

		const GraphicsExtensionData *extensionData = GraphicsMgr::GetExtensionData();
		for (machine a = 0; a < kGraphicsExtensionCount; a++)
		{
			enableBox[a] = new CheckWidget(size, extensionData->name1, "font/Gui");
			extensionsList->AppendListItem(enableBox[a]);

			if (extensionData->enabled)
			{
				enableBox[a]->SetValue(1);
			}

			if (extensionData->required)
			{
				enableBox[a]->Disable();
			}

			extensionData++;
		}

		#if C4WINDOWS || C4LINUX

			const WindowSystemExtensionData *windowSystemExtensionData = GraphicsMgr::GetWindowSystemExtensionData();
			for (machine a = 0; a < kWindowSystemExtensionCount; a++)
			{
				windowSystemEnableBox[a] = new CheckWidget(size, windowSystemExtensionData->name, "font/Gui");
				extensionsList->AppendListItem(windowSystemEnableBox[a]);

				if (windowSystemExtensionData->enabled)
				{
					windowSystemEnableBox[a]->SetValue(1);
				}

				windowSystemEnableBox[a]->Enable();
				windowSystemExtensionData++;
			}

		#endif

		SetFocusWidget(extensionsList);
	}

	void ExtensionsWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
	{
		if (eventData->eventType == kEventWidgetActivate)
		{
			if (widget == okayButton)
			{
				const GraphicsExtensionData *data = GraphicsMgr::GetExtensionData();
				for (machine a = 0; a < kGraphicsExtensionCount; a++)
				{
					data->enabled = (enableBox[a]->GetValue() != 0);
					data++;
				}

				#if C4WINDOWS || C4LINUX

					const WindowSystemExtensionData *windowSystemExtensionData = GraphicsMgr::GetWindowSystemExtensionData();
					for (machine a = 0; a < kWindowSystemExtensionCount; a++)
					{
						windowSystemExtensionData->enabled = (windowSystemEnableBox[a]->GetValue() != 0);
						windowSystemExtensionData++;
					}

				#endif

				GraphicsMgr::Delete();
				GraphicsMgr::New();

				Close();
			}
			else if (widget == cancelButton)
			{
				Close();
			}
		}
	}

#endif


ConsoleWindow::ConsoleWindow() :
		Window("C4/Console"),
		Singleton<ConsoleWindow>(TheConsoleWindow),
		reporter(&Report, this)
{
	SetWidgetPosition(Point3D(8.0F, 26.0F, 0.0F));
	Hide();

	dirtyFlag = true;
	historyFlag = false;
}

ConsoleWindow::~ConsoleWindow()
{
	if (historyFlag)
	{
		ConfigDataDescription::WriteEngineConfig();
	}

	TheInputMgr->SetConsoleProc(nullptr);
}

void ConsoleWindow::New(void)
{
	if (!TheConsoleWindow)
	{
		TheInterfaceMgr->AddWidget(new ConsoleWindow);
	}
}

void ConsoleWindow::ConsoleProc(void *cookie)
{
	TheInterfaceMgr->SetActiveWindow(static_cast<ConsoleWindow *>(cookie));
}

void ConsoleWindow::Report(const char *text, unsigned_int32 flags, void *cookie)
{
	if (!(flags & kReportLog))
	{
		static_cast<ConsoleWindow *>(cookie)->AddText(text);
	}
}

void ConsoleWindow::Preprocess(void)
{
	Window::Preprocess();

	textWidget = static_cast<TextWidget *>(FindWidget("Text"));
	commandLine = static_cast<EditTextWidget *>(FindWidget("Edit"));
	scrollWidget = static_cast<ScrollWidget *>(FindWidget("Scroll"));

	textWidget->SetTextFormatExclusionMask(0);
	textWidget->SetRenderLineCount(kConsoleLineCount);

	scrollWidget->SetPageDistance(kConsoleLineCount - 1);
	scrollWidget->SetValue(kConsoleHistoryCount - kConsoleLineCount);

	#if C4DEBUG

		textWidget->SetText((String<95>("[#FFF]C4 Engine\n[#FF8]Version ") += C4VERSION) += " [#AA5](Debug)\n");

	#else

		textWidget->SetText((String<95>("[#FFF]C4 Engine\n[#FF8]Version ") += C4VERSION) += '\n');

	#endif

	TheInputMgr->SetConsoleProc(&ConsoleProc, this);
	Engine::InstallReporter(&reporter);
}

void ConsoleWindow::EnterForeground(void)
{
	Window::EnterForeground();

	if (!Visible())
	{
		Show();
		SetFocusWidget(commandLine);
		commandLine->SelectAll();
	}
}

void ConsoleWindow::Close(void)
{
	Hide();
	TheInterfaceMgr->GetStrip()->HideEmpty();
}

bool ConsoleWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;

		if (keyCode == kKeyCodeEnter)
		{
			const char *text = commandLine->GetText();
			if (text[0] != 0)
			{
				AddCommandHistory(text);
				TheEngine->ExecuteText(text);

				commandLine->SetText(nullptr);
				historyFlag = true;
			}

			return (true);
		}
		else if (keyCode == kKeyCodeEscape)
		{
			Close();
			return (true);
		}
		else if (keyCode == kKeyCodeUpArrow)
		{
			unsigned_int32 offset = commandHistoryOffset;
			if (offset < commandHistoryCount)
			{
				unsigned_int32 start = commandHistoryStart;
				if (++offset == 1)
				{
					commandHistory[start] = commandLine->GetText();
				}

				commandHistoryOffset = offset;

				unsigned_int32 position = (start - offset) & (kConsoleCommandHistoryCount - 1);
				commandLine->SetText(commandHistory[position]);
				commandLine->SetSelection(kMaxCommandLength, kMaxCommandLength);
			}
		}
		else if (keyCode == kKeyCodeDownArrow)
		{
			unsigned_int32 offset = commandHistoryOffset;
			if (offset > 0)
			{
				commandHistoryOffset = --offset;

				unsigned_int32 position = (commandHistoryStart - offset) & (kConsoleCommandHistoryCount - 1);
				commandLine->SetText(commandHistory[position]);
				commandLine->SetSelection(kMaxCommandLength, kMaxCommandLength);
			}
		}
		else if (keyCode == kKeyCodePageUp)
		{
			int32 value = scrollWidget->GetValue();
			if (value > 0)
			{
				scrollWidget->SetValue(MaxZero(value - scrollWidget->GetPageDistance()));
				UpdateDisplayLine();
			}
		}
		else if (keyCode == kKeyCodePageDown)
		{
			int32 value = scrollWidget->GetValue();
			int32 maxValue = scrollWidget->GetMaxValue();
			if (value < maxValue)
			{
				scrollWidget->SetValue(Min(value + scrollWidget->GetPageDistance(), maxValue));
				UpdateDisplayLine();
			}
		}
	}
	else if (eventType == kEventKeyCommand)
	{
		if (eventData->keyCode == 'W')
		{
			Close();
			return (true);
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void ConsoleWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		if (widget == scrollWidget)
		{
			UpdateDisplayLine();
		}
	}
}

void ConsoleWindow::Render(List<Renderable> *renderList)
{
	if (dirtyFlag)
	{
		dirtyFlag = false;

		textWidget->SplitLines();
		int32 count = textWidget->GetLineCount();
		if (count > kConsoleHistoryCount)
		{
			String<> string(textWidget->GetText() + textWidget->GetLineEnd(count - kConsoleHistoryCount - 1));
			textWidget->SetText(string);

			count = kConsoleHistoryCount;
		}

		count = MaxZero(count - kConsoleLineCount);
		scrollWidget->SetMaxValue(count);
		scrollWidget->SetValue(count);
		UpdateDisplayLine();
	}

	Window::Render(renderList);
}

void ConsoleWindow::AddCommandHistory(const char *command)
{
	unsigned_int32 start = commandHistoryStart;

	if ((commandHistoryCount == 0) || (!Text::CompareText(commandHistory[(start - 1) & (kConsoleCommandHistoryCount - 1)], command)))
	{
		commandHistory[start] = command;
		commandHistoryCount = Min(commandHistoryCount + 1, kConsoleCommandHistoryCount - 1);
		commandHistoryStart = (start + 1) & (kConsoleCommandHistoryCount - 1);
	}

	commandHistoryOffset = 0;
}

void ConsoleWindow::AddText(const char *text)
{
	const char *history = textWidget->GetText();
	if (text)
	{
		textWidget->SetText((String<>(history) += "\n[INIT][LEFT]") += text);
	}
	else
	{
		textWidget->SetText(String<>(history) += '\n');
	}

	dirtyFlag = true;
}

// ZYUQURM
