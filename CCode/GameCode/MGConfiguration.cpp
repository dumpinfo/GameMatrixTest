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


#include "C4ConfigData.h"
#include "MGConfiguration.h"
#include "MGSoldier.h"
#include "MGGame.h"


using namespace C4;


SharedVertexBuffer InputWidget::indexBuffer(kVertexBufferIndex | kVertexBufferStatic);


PlayerSettingsWindow *C4::ThePlayerSettingsWindow = nullptr;
GraphicsSettingsWindow *C4::TheGraphicsSettingsWindow = nullptr;
AudioSettingsWindow *C4::TheAudioSettingsWindow = nullptr;
ControlSettingsWindow *C4::TheControlSettingsWindow = nullptr;


CrosshairWidget::CrosshairWidget(int32 value) :
		ImageWidget(Vector2D(128.0F, 256.0F), "game/Crosshairs"),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		diffuseAttribute(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite)),
		hiliteRenderable(kRenderTriangleStrip)
{
	crosshairValue = MaxZero(Min(value, 31));
}

CrosshairWidget::~CrosshairWidget()
{
}

void CrosshairWidget::SetValue(int32 value)
{
	crosshairValue = value;
	SetBuildFlag();
}

void CrosshairWidget::Preprocess(void)
{
	ImageWidget::Preprocess();

	InitRenderable(&hiliteRenderable);
	hiliteRenderable.SetVertexCount(4);
	hiliteRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(Point2D));
	hiliteRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	vertexBuffer.Establish(sizeof(Point2D) * 4);

	attributeList.Append(&diffuseAttribute);
	hiliteRenderable.SetMaterialAttributeList(&attributeList);
}

void CrosshairWidget::Build(void)
{
	volatile Point2D *restrict vertex = vertexBuffer.BeginUpdate<Point2D>();

	float x = (float) ((crosshairValue & 3) * 32);
	float y = (float) ((crosshairValue / 4) * 32);

	vertex[0].Set(x, y);
	vertex[1].Set(x, y + 32.0F);
	vertex[2].Set(x + 32.0F, y);
	vertex[3].Set(x + 32.0F, y + 32.0F);

	vertexBuffer.EndUpdate();
	ImageWidget::Build();
}

void CrosshairWidget::Render(List<Renderable> *renderList)
{
	renderList->Append(&hiliteRenderable);
	ImageWidget::Render(renderList);
}

void CrosshairWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	if (eventData->eventType == kEventMouseDown)
	{
		int32 x = MaxZero(Min((int32) (eventData->mousePosition.x * 0.03125), 3));
		int32 y = MaxZero(Min((int32) (eventData->mousePosition.y * 0.03125), 7));
		SetValue(y * 4 + x);
	}
}


PlayerSettingsWindow::PlayerSettingsWindow() :
		GameWindow("game/Player"),
		Singleton<PlayerSettingsWindow>(ThePlayerSettingsWindow)
{
}

PlayerSettingsWindow::~PlayerSettingsWindow()
{
}

void PlayerSettingsWindow::Open(void)
{
	if (ThePlayerSettingsWindow)
	{
		TheInterfaceMgr->SetActiveWindow(ThePlayerSettingsWindow);
	}
	else 
	{
		TheGame->AddWindow(new PlayerSettingsWindow);
	} 
}
 
void PlayerSettingsWindow::Preprocess(void)
{
	GameWindow::Preprocess(); 

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK")); 
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel")); 
	//customizeButton = static_cast<PushButtonWidget *>(FindWidget("Custom"));

	playerNameBox = static_cast<EditTextWidget *>(FindWidget("Name"));
 
	weaponSwitchBox = static_cast<CheckWidget *>(FindWidget("Switch"));
	betterWeaponBox = static_cast<CheckWidget *>(FindWidget("Better"));
	ammoSwitchBox = static_cast<CheckWidget *>(FindWidget("Ammo"));
	chatOpenBox = static_cast<CheckWidget *>(FindWidget("Chat"));

	crossColorBox = static_cast<ColorWidget *>(FindWidget("Color"));
	crossSizeSlider = static_cast<SliderWidget *>(FindWidget("Size"));
	crossSizeText = static_cast<TextWidget *>(FindWidget("SizeText"));

	playerNameBox->SetText(TheEngine->GetVariable("playerName")->GetValue());
	SetFocusWidget(playerNameBox);

	weaponSwitchBox->SetValue(TheEngine->GetVariable("weaponSwitch")->GetIntegerValue() != 0);
	betterWeaponBox->SetValue(TheEngine->GetVariable("weaponBetterSwitch")->GetIntegerValue() != 0);
	ammoSwitchBox->SetValue(TheEngine->GetVariable("weaponAmmoSwitch")->GetIntegerValue() != 0);

	crossColorBox->SetValue(ColorRGBA().SetHexString(TheEngine->GetVariable("crossColor")->GetValue()));

	int32 size = TheEngine->GetVariable("crossSize")->GetIntegerValue();
	crossSizeSlider->SetValue(size);
	crossSizeText->SetText(String<7>(size + 1));

	chatOpenBox->SetValue(TheEngine->GetVariable("chatOpen")->GetIntegerValue() != 0);

	crosshairBox = new CrosshairWidget(TheEngine->GetVariable("crossType")->GetIntegerValue());
	crosshairBox->SetWidgetPosition(FindWidget("Crosshair")->GetWidgetPosition());
	AppendNewSubnode(crosshairBox);
}

void PlayerSettingsWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			char	string[9];

			const char *name = playerNameBox->GetText();
			TheEngine->GetVariable("playerName")->SetValue(name);

			crossColorBox->GetValue().GetHexString(string);
			TheEngine->GetVariable("crossColor")->SetValue(string);

			TheEngine->GetVariable("crossType")->SetIntegerValue(crosshairBox->GetValue());
			TheEngine->GetVariable("crossSize")->SetIntegerValue(crossSizeSlider->GetValue());
			TheEngine->GetVariable("chatOpen")->SetIntegerValue(chatOpenBox->GetValue());

			TheEngine->GetVariable("weaponSwitch")->SetIntegerValue(weaponSwitchBox->GetValue());
			TheEngine->GetVariable("weaponBetterSwitch")->SetIntegerValue(betterWeaponBox->GetValue());
			TheEngine->GetVariable("weaponAmmoSwitch")->SetIntegerValue(ammoSwitchBox->GetValue());

			ConfigDataDescription::WriteEngineConfig();

			CallCompletionProc();
			Close();
		}
		else if (widget == cancelButton)
		{
			CallCompletionProc();
			Close();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == playerNameBox)
		{
			const char *name = playerNameBox->GetText();
			if (name[0] != 0)
			{
				okayButton->Enable();
			}
			else
			{
				okayButton->Disable();
			}
		}
		else if (widget == crossSizeSlider)
		{
			crossSizeText->SetText(String<7>(crossSizeSlider->GetValue() + 1));
		}
	}
}


CustomizeCharacterWindow::CustomizeCharacterWindow() : GameWindow("game/Character")
{
	modelWorld = nullptr;
}

CustomizeCharacterWindow::~CustomizeCharacterWindow()
{
	delete modelWorld;
}

void CustomizeCharacterWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	Game::InitPlayerStyle(playerStyle);

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	lightSlider = static_cast<SliderWidget *>(FindWidget("Light"));
	colorSlider = static_cast<SliderWidget *>(FindWidget("Color"));
	textureSlider = static_cast<SliderWidget *>(FindWidget("Texture"));
	helmetSlider = static_cast<SliderWidget *>(FindWidget("Helmet"));

	lightText = static_cast<TextWidget *>(FindWidget("LightText"));
	colorText = static_cast<TextWidget *>(FindWidget("ColorText"));
	textureText = static_cast<TextWidget *>(FindWidget("TextureText"));
	helmetText = static_cast<TextWidget *>(FindWidget("HelmetText"));

	lightSlider->SetValue(playerStyle[kPlayerStyleEmissionColor]);
	colorSlider->SetValue(playerStyle[kPlayerStyleArmorColor]);
	textureSlider->SetValue(playerStyle[kPlayerStyleArmorTexture]);
	helmetSlider->SetValue(playerStyle[kPlayerStyleHelmetType]);

	lightText->SetText(String<7>(playerStyle[kPlayerStyleEmissionColor]));
	colorText->SetText(String<7>(playerStyle[kPlayerStyleArmorColor]));
	textureText->SetText(String<7>(playerStyle[kPlayerStyleArmorTexture]));
	helmetText->SetText(String<7>(playerStyle[kPlayerStyleHelmetType]));

	modelViewport = static_cast<FrustumViewportWidget *>(FindWidget("Viewport"));
	modelViewport->SetMouseEventProc(&ViewportHandleMouseEvent, this);
	modelViewport->SetRenderProc(&ViewportRender, this);

	cameraAzimuth = K::tau_over_2;
	modelViewport->SetCameraTransform(cameraAzimuth, -0.3F, Point3D(1.5F, 0.0F, 1.4F));

	zoneNode = new InfiniteZone;

	ZoneObject *zoneObject = new InfiniteZoneObject;
	zoneNode->SetObject(zoneObject);
	zoneObject->Release();

	zoneObject->SetAmbientLight(ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F));
	zoneObject->SetEnvironmentMap("C4/environment");

	modelWorld = new World(zoneNode, kWorldViewport | kWorldClearColor | kWorldZeroBackgroundVelocity | kWorldListenerInhibit);
	modelWorld->SetRenderSize((int32) modelViewport->GetWidgetSize().x, (int32) modelViewport->GetWidgetSize().y);
	modelWorld->Preprocess();

	FrustumCamera *camera = modelViewport->GetViewportCamera();
	camera->GetObject()->SetClearColor(K::transparent);
	modelWorld->SetCamera(camera);

	modelNode = Model::Get(kModelSoldier);
	zoneNode->AppendSubnode(modelNode);

	modelController = new SoldierController;
	modelNode->SetController(modelController);

	modelNode->Update();
	modelNode->Preprocess();
	modelNode->StopMotion();

	modelController->SetFighterStyle(playerStyle);
	modelController->SetFighterMotion(kFighterMotionStand);

	lightNode = new PointLight(ColorRGB(1.0F, 1.0F, 1.0F), 100.0F);
	lightNode->SetNodePosition(Point3D(5.0F, 5.0F, 5.0F));
	zoneNode->AppendNewSubnode(lightNode);
}

void CustomizeCharacterWindow::ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie)
{
	CustomizeCharacterWindow *window = static_cast<CustomizeCharacterWindow *>(cookie);

	EventType eventType = eventData->eventType;
	if (eventType == kEventMouseDown)
	{
		window->trackFlag = true;
		window->previousPosition = eventData->mousePosition;
	}
	else if (eventType == kEventMouseMoved)
	{
		if (window->trackFlag)
		{
			float azm = window->cameraAzimuth + (window->previousPosition.x - eventData->mousePosition.x) * 0.03125F;
			if (azm < -K::tau_over_2)
			{
				azm += K::tau;
			}
			else if (azm > K::tau_over_2)
			{
				azm -= K::tau;
			}

			window->cameraAzimuth = azm;
			window->previousPosition = eventData->mousePosition;

			Vector2D v = CosSin(azm);
			window->modelViewport->SetCameraTransform(azm, -0.3F, Point3D(v.x * -1.5F, v.y * -1.5F, 1.4F));
		}
	}
	else
	{
		window->trackFlag = false;
	}
}

void CustomizeCharacterWindow::ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie)
{
	CustomizeCharacterWindow *window = static_cast<CustomizeCharacterWindow *>(cookie);

	window->modelNode->Animate();

	window->modelWorld->Update();
	window->modelWorld->BeginRendering();
	window->modelWorld->Render();
	window->modelWorld->EndRendering();
}

void CustomizeCharacterWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			TheEngine->GetVariable("playerEmissionColor")->SetIntegerValue(playerStyle[kPlayerStyleEmissionColor]);
			TheEngine->GetVariable("playerArmorColor")->SetIntegerValue(playerStyle[kPlayerStyleArmorColor]);
			TheEngine->GetVariable("playerArmorTexture")->SetIntegerValue(playerStyle[kPlayerStyleArmorTexture]);
			TheEngine->GetVariable("playerHelmetType")->SetIntegerValue(playerStyle[kPlayerStyleHelmetType]);

			ConfigDataDescription::WriteEngineConfig();

			Player *player = TheMessageMgr->GetLocalPlayer();
			if (player)
			{
				TheMessageMgr->SendMessage(kPlayerServer, ClientStyleMessage(playerStyle));
				if (!TheMessageMgr->Server())
				{
					static_cast<GamePlayer *>(player)->SetPlayerStyle(playerStyle);
				}
			}

			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == lightSlider)
		{
			int32 value = lightSlider->GetValue();
			playerStyle[kPlayerStyleEmissionColor] = value;
			lightText->SetText(String<7>(value));
			modelController->SetFighterStyle(playerStyle);
		}
		else if (widget == colorSlider)
		{
			int32 value = colorSlider->GetValue();
			playerStyle[kPlayerStyleArmorColor] = value;
			colorText->SetText(String<7>(value));
			modelController->SetFighterStyle(playerStyle);
		}
		else if (widget == textureSlider)
		{
			int32 value = textureSlider->GetValue();
			playerStyle[kPlayerStyleArmorTexture] = value;
			textureText->SetText(String<7>(value));
			modelController->SetFighterStyle(playerStyle);
		}
		else if (widget == helmetSlider)
		{
			int32 value = helmetSlider->GetValue();
			playerStyle[kPlayerStyleHelmetType] = value;
			helmetText->SetText(String<7>(value));
			modelController->SetFighterStyle(playerStyle);
		}
	}
}


GraphicsSettingsWindow::GraphicsSettingsWindow() :
		GameWindow("game/Graphics"),
		Singleton<GraphicsSettingsWindow>(TheGraphicsSettingsWindow)
{
}

GraphicsSettingsWindow::~GraphicsSettingsWindow()
{
}

void GraphicsSettingsWindow::Open(void)
{
	if (TheGraphicsSettingsWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheGraphicsSettingsWindow);
	}
	else
	{
		TheGame->AddWindow(new GraphicsSettingsWindow);
	}
}

void GraphicsSettingsWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));
	applyButton = static_cast<PushButtonWidget *>(FindWidget("Apply"));

	resolutionList = static_cast<ListWidget *>(FindWidget("Resolution"));
	fullscreenBox = static_cast<CheckWidget *>(FindWidget("Full"));
	refreshSyncMenu = static_cast<PopupMenuWidget *>(FindWidget("Sync"));
	multisamplingMenu = static_cast<PopupMenuWidget *>(FindWidget("AA"));
	brightnessSlider = static_cast<SliderWidget *>(FindWidget("Bright"));
	brightnessText = static_cast<TextWidget *>(FindWidget("BrightText"));

	lightDetailMenu = static_cast<PopupMenuWidget *>(FindWidget("LightLOD"));
	textureDetailMenu = static_cast<PopupMenuWidget *>(FindWidget("TextureLOD"));
	anisotropySlider = static_cast<SliderWidget *>(FindWidget("Aniso"));
	anisotropyText = static_cast<TextWidget *>(FindWidget("AnisoText"));

	parallaxBox = static_cast<CheckWidget *>(FindWidget("Parallax"));
	horizonBox = static_cast<CheckWidget *>(FindWidget("Horizon"));
	ambientBumpBox = static_cast<CheckWidget *>(FindWidget("AmbientBump"));
	terrainBumpBox = static_cast<CheckWidget *>(FindWidget("TerrainBump"));
	terrainDetailBox = static_cast<CheckWidget *>(FindWidget("TerrainLOD"));

	structureBox = static_cast<CheckWidget *>(FindWidget("Structure"));
	motionBlurBox = static_cast<CheckWidget *>(FindWidget("Motion"));
	distortionBox = static_cast<CheckWidget *>(FindWidget("Distortion"));
	glowBloomBox = static_cast<CheckWidget *>(FindWidget("Glow"));
	occlusionBox = static_cast<CheckWidget *>(FindWidget("SSAO"));

	int32 index = 0;
	int32 displayWidth = TheDisplayMgr->GetDisplayWidth();
	int32 displayHeight = TheDisplayMgr->GetDisplayHeight();

	const DisplayMode *mode = TheDisplayMgr->GetCurrentDisplay()->GetFirstDisplayMode();
	while (mode)
	{
		int32 w = mode->GetDisplayWidth();
		int32 h = mode->GetDisplayHeight();

		String<31> string(w);
		string += " \xC3\x97 ";		// U+00D7
		string += h;
		resolutionList->AppendListItem(string);

		if ((w == displayWidth) && (h == displayHeight))
		{
			resolutionList->SelectListItem(index);
		}

		index++;
		mode = mode->Next();
	}

	unsigned_int32 flags = TheDisplayMgr->GetDisplayFlags();
	if (flags & kDisplayFullscreen)
	{
		fullscreenBox->SetValue(1);
	}

	if (flags & kDisplayRefreshSync)
	{
		if (flags & kDisplaySyncTear)
		{
			refreshSyncMenu->SetSelection(2);
		}
		else
		{
			refreshSyncMenu->SetSelection(1);
		}
	}
	else
	{
		refreshSyncMenu->SetSelection(0);
	}

	int32 displaySamples = TheDisplayMgr->GetDisplaySamples();
	if (displaySamples == 2)
	{
		multisamplingMenu->SetSelection(1);
	}
	else if (displaySamples == 4)
	{
		multisamplingMenu->SetSelection(2);
	}
	else if (displaySamples == 8)
	{
		multisamplingMenu->SetSelection(3);
	}
	else
	{
		multisamplingMenu->SetSelection(0);
	}

	float brightness = TheGraphicsMgr->GetBrightnessMultiplier();
	originalBrightness = brightness;

	int32 value = (int32) ((brightness - 1.0F) * 50.0F);
	brightnessSlider->SetValue(value);
	brightnessText->SetText(String<7>(value));

	lightDetailMenu->SetSelection(TheWorldMgr->GetLightDetailLevel());
	textureDetailMenu->SetSelection(2 - TheGraphicsMgr->GetTextureDetailLevel());

	int32 anisotropy = TheGraphicsMgr->GetTextureFilterAnisotropy();
	anisotropySlider->SetValue(anisotropy - 1);
	anisotropyText->SetText(String<7>(anisotropy));

	flags = TheGraphicsMgr->GetRenderOptionFlags();

	if (flags & kRenderOptionParallaxMapping)
	{
		parallaxBox->SetValue(1);
	}

	if (flags & kRenderOptionHorizonMapping)
	{
		horizonBox->SetValue(1);
	}

	if (flags & kRenderOptionAmbientBumps)
	{
		ambientBumpBox->SetValue(1);
	}

	if (flags & kRenderOptionTerrainBumps)
	{
		terrainBumpBox->SetValue(1);
	}

	if (flags & kRenderOptionStructureEffects)
	{
		structureBox->SetValue(1);
	}

	if (flags & kRenderOptionMotionBlur)
	{
		motionBlurBox->SetValue(1);
	}

	if (flags & kRenderOptionDistortion)
	{
		distortionBox->SetValue(1);
	}

	if (flags & kRenderOptionGlowBloom)
	{
		glowBloomBox->SetValue(1);
	}

	if (flags & kRenderOptionAmbientOcclusion)
	{
		occlusionBox->SetValue(1);
	}

	if (TheGraphicsMgr->GetPaletteDetailLevel() == 0)
	{
		terrainDetailBox->SetValue(1);
	}

	SetFocusWidget(resolutionList);
}

inline float GraphicsSettingsWindow::GetBrightnessMultiplier(void) const
{
	return ((float) brightnessSlider->GetValue() * 0.02F + 1.0F);
}

bool GraphicsSettingsWindow::ChangeDisplayMode(void)
{
	unsigned_int32 renderOptionFlags = TheGraphicsMgr->GetRenderOptionFlags();

	TheEngine->GetVariable("lightDetailLevel")->SetIntegerValue(lightDetailMenu->GetSelection());
	TheEngine->GetVariable("textureDetailLevel")->SetIntegerValue(2 - textureDetailMenu->GetSelection());
	TheEngine->GetVariable("paletteDetailLevel")->SetIntegerValue(1 - terrainDetailBox->GetValue());
	TheEngine->GetVariable("textureAnisotropy")->SetIntegerValue(anisotropySlider->GetValue() + 1);
	TheEngine->GetVariable("renderParallaxMapping")->SetIntegerValue(parallaxBox->GetValue());
	TheEngine->GetVariable("renderHorizonMapping")->SetIntegerValue(horizonBox->GetValue());
	TheEngine->GetVariable("renderAmbientBumps")->SetIntegerValue(ambientBumpBox->GetValue());
	TheEngine->GetVariable("renderTerrainBumps")->SetIntegerValue(terrainBumpBox->GetValue());
	TheEngine->GetVariable("renderStructureEffects")->SetIntegerValue(structureBox->GetValue());
	TheEngine->GetVariable("renderAmbientOcclusion")->SetIntegerValue(occlusionBox->GetValue());
	TheEngine->GetVariable("postBrightness")->SetFloatValue(GetBrightnessMultiplier());
	TheEngine->GetVariable("postMotionBlur")->SetIntegerValue(motionBlurBox->GetValue());
	TheEngine->GetVariable("postDistortion")->SetIntegerValue(distortionBox->GetValue());
	TheEngine->GetVariable("postGlowBloom")->SetIntegerValue(glowBloomBox->GetValue());

	int32 width = TheDisplayMgr->GetDisplayWidth();
	int32 height = TheDisplayMgr->GetDisplayHeight();

	const Widget *listItem = resolutionList->GetFirstSelectedListItem();
	if (listItem)
	{
		const DisplayMode *mode = TheDisplayMgr->GetCurrentDisplay()->GetDisplayMode(listItem->GetNodeIndex());
		width = mode->GetDisplayWidth();
		height = mode->GetDisplayHeight();
	}

	unsigned_int32 flags = (fullscreenBox->GetValue() != 0) ? kDisplayFullscreen : 0;

	int32 selection = refreshSyncMenu->GetSelection();
	if (selection >= 1)
	{
		flags |= kDisplayRefreshSync;
		if (selection == 2)
		{
			flags |= kDisplaySyncTear;
		}
	}

	int32 samples = 1;
	selection = multisamplingMenu->GetSelection();
	if (selection == 1)
	{
		samples = 2;
	}
	else if (selection == 2)
	{
		samples = 4;
	}
	else if (selection == 3)
	{
		samples = 8;
	}

	if ((width != TheDisplayMgr->GetDisplayWidth()) || (height != TheDisplayMgr->GetDisplayHeight()) || (samples != TheDisplayMgr->GetDisplaySamples()) || (flags != TheDisplayMgr->GetDisplayFlags()))
	{
		TheDisplayMgr->SetDisplayMode(width, height, samples, flags);

		int32 displayWidth = TheDisplayMgr->GetDisplayWidth();
		int32 displayHeight = TheDisplayMgr->GetDisplayHeight();
		if ((displayWidth != width) || (displayHeight != height))
		{
			int32 index = 0;
			const Display *display = TheDisplayMgr->GetCurrentDisplay();
			const DisplayMode *mode = display->GetFirstDisplayMode();
			while (mode)
			{
				int32 w = mode->GetDisplayWidth();
				int32 h = mode->GetDisplayHeight();
				if ((w == displayWidth) && (h == displayHeight))
				{
					resolutionList->SelectListItem(index);
					break;
				}

				index++;
				mode = mode->Next();
			}
		}

		TheEngine->GetVariable("displayWidth")->SetIntegerValue(width);
		TheEngine->GetVariable("displayHeight")->SetIntegerValue(height);
		TheEngine->GetVariable("displaySamples")->SetIntegerValue(samples);
		TheEngine->GetVariable("displayFull")->SetIntegerValue((flags & kDisplayFullscreen) != 0);
		TheEngine->GetVariable("displaySync")->SetIntegerValue((flags & kDisplayRefreshSync) != 0);
		TheEngine->GetVariable("displayTear")->SetIntegerValue((flags & kDisplaySyncTear) != 0);

		return (true);
	}
	else if (((TheGraphicsMgr->GetRenderOptionFlags() ^ renderOptionFlags) & (kRenderOptionStructureEffects | kRenderOptionMotionBlur | kRenderOptionDistortion | kRenderOptionGlowBloom | kRenderOptionAmbientOcclusion)) != 0)
	{
		GraphicsMgr::Delete();
		GraphicsMgr::New();
	}

	return (false);
}

void GraphicsSettingsWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			ChangeDisplayMode();
			ConfigDataDescription::WriteEngineConfig();

			CallCompletionProc();
			Close();
		}
		else if (widget == applyButton)
		{
			ChangeDisplayMode();
			ConfigDataDescription::WriteEngineConfig();
		}
		else if (widget == cancelButton)
		{
			TheGraphicsMgr->SetBrightnessMultiplier(originalBrightness);
			CallCompletionProc();
			Close();
		}
		else if (widget == resolutionList)
		{
			ChangeDisplayMode();
			ConfigDataDescription::WriteEngineConfig();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == brightnessSlider)
		{
			brightnessText->SetText(String<7>(brightnessSlider->GetValue()));
			TheGraphicsMgr->SetBrightnessMultiplier(GetBrightnessMultiplier());
		}
		else if (widget == anisotropySlider)
		{
			anisotropyText->SetText(String<7>(anisotropySlider->GetValue() + 1));
		}
	}
}


AudioSettingsWindow::AudioSettingsWindow() :
		GameWindow("game/Audio"),
		Singleton<AudioSettingsWindow>(TheAudioSettingsWindow)
{
	originalEffectsVolume = TheEngine->GetVariable("volumeEffects")->GetIntegerValue();
	originalMusicVolume = TheEngine->GetVariable("volumeMusic")->GetIntegerValue();
	originalVoiceVolume = TheEngine->GetVariable("volumeVoice")->GetIntegerValue();
}

AudioSettingsWindow::~AudioSettingsWindow()
{
}

void AudioSettingsWindow::Open(void)
{
	if (TheAudioSettingsWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheAudioSettingsWindow);
	}
	else
	{
		TheGame->AddWindow(new AudioSettingsWindow);
	}
}

void AudioSettingsWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	effectsSlider = static_cast<SliderWidget *>(FindWidget("Effects"));
	effectsText = static_cast<TextWidget *>(FindWidget("EffectsText"));
	musicSlider = static_cast<SliderWidget *>(FindWidget("Music"));
	musicText = static_cast<TextWidget *>(FindWidget("MusicText"));
	reverbBox = static_cast<CheckWidget *>(FindWidget("Reverb"));

	voiceReceiveBox = static_cast<CheckWidget *>(FindWidget("ReceiveChat"));
	voiceSendBox = static_cast<CheckWidget *>(FindWidget("SendChat"));
	voiceSlider = static_cast<SliderWidget *>(FindWidget("Voice"));
	voiceText = static_cast<TextWidget *>(FindWidget("VoiceText"));

	effectsSlider->SetValue(originalEffectsVolume);
	effectsText->SetText(String<7>(originalEffectsVolume));

	musicSlider->SetValue(originalMusicVolume);
	musicText->SetText(String<7>(originalMusicVolume));

	if (TheSoundMgr->GetSoundOptionFlags() & kSoundOptionReverb)
	{
		reverbBox->SetValue(1);
	}

	if (TheEngine->GetVariable("voiceReceive")->GetIntegerValue() != 0)
	{
		voiceReceiveBox->SetValue(1);
	}

	if (TheEngine->GetVariable("voiceSend")->GetIntegerValue() != 0)
	{
		voiceSendBox->SetValue(1);
	}

	voiceSlider->SetValue(originalVoiceVolume);
	voiceText->SetText(String<7>(originalVoiceVolume));
}

void AudioSettingsWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			TheEngine->GetVariable("soundReverb")->SetIntegerValue(reverbBox->GetValue());
			TheEngine->GetVariable("voiceReceive")->SetIntegerValue(voiceReceiveBox->GetValue());
			TheEngine->GetVariable("voiceSend")->SetIntegerValue(voiceSendBox->GetValue());

			ConfigDataDescription::WriteEngineConfig();
			CallCompletionProc();
			Close();
		}
		else if (widget == cancelButton)
		{
			TheEngine->GetVariable("volumeEffects")->SetIntegerValue(originalEffectsVolume);
			TheEngine->GetVariable("volumeMusic")->SetIntegerValue(originalMusicVolume);
			TheEngine->GetVariable("volumeVoice")->SetIntegerValue(originalVoiceVolume);

			CallCompletionProc();
			Close();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == effectsSlider)
		{
			int32 volume = effectsSlider->GetValue();
			effectsText->SetText(Text::IntegerToString(volume));
			TheEngine->GetVariable("volumeEffects")->SetIntegerValue(volume);
		}
		else if (widget == musicSlider)
		{
			int32 volume = musicSlider->GetValue();
			musicText->SetText(Text::IntegerToString(volume));
			TheEngine->GetVariable("volumeMusic")->SetIntegerValue(volume);
		}
		else if (widget == voiceSlider)
		{
			int32 volume = voiceSlider->GetValue();
			voiceText->SetText(Text::IntegerToString(volume));
			TheEngine->GetVariable("volumeVoice")->SetIntegerValue(volume);
		}
	}
}


InputWidget::InputWidget(const ListWidget *list, Action *action) :
		RenderableWidget(kWidgetInput, kRenderIndexedTriangles, Vector2D(136.0F, 16.0F)),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		pulsateMutator(ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F), kPulsateWaveSine, 0.001F),
		textWidget(Vector2D(136.0F, 14.0F), nullptr, "font/Gui")
{
	controlList = list;

	inputAction = action;
	inputControl = TheInputMgr->GetActionControl(action);
	if (inputControl)
	{
		textWidget.SetText(inputControl->GetControlName());
	}

	textWidget.SetTextAlignment(kTextAlignCenter);
	textWidget.SetWidgetPosition(Point3D(0.0F, 2.0F, 0.0F));
	textWidget.Disable();
	AppendSubnode(&textWidget);

	SetWidgetUsage(kWidgetTrackInhibit);
	SetDefaultColorType(kWidgetColorBackground);
	RenderableWidget::SetWidgetColor(ColorRGBA(0.875F, 0.875F, 0.875F, 1.0F));

	pulsateMutator.SetMutatorState(kMutatorDisabled);
	AddMutator(&pulsateMutator);

	if (indexBuffer.Retain() == 1)
	{
		static const Triangle inputTriangle[10] =
		{
			{{ 0,  1,  2}}, {{ 0,  2,  3}}, {{ 8,  9,  4}}, {{ 4,  9,  5}},
			{{ 9, 10,  5}}, {{ 5, 10,  6}}, {{10, 11,  6}}, {{ 6, 11,  7}},
			{{11,  8,  7}}, {{ 7,  8,  4}}
		};

		indexBuffer.Establish(sizeof(Triangle) * 10, inputTriangle);
	}
}

InputWidget::~InputWidget()
{
	indexBuffer.Release();
}

void InputWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBackground))
	{
		dynamicBackgroundColor = color;
		SetBuildFlag();
	}
}

void InputWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetWidgetState(GetWidgetState() & ~kWidgetDisabled);

	dynamicBackgroundColor = GetWidgetColor();

	SetVertexCount(12);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(InputVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
	vertexBuffer.Establish(sizeof(InputVertex) * 12);

	SetPrimitiveCount(10);
	SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);
}

void InputWidget::Build(void)
{
	volatile InputVertex *restrict vertex = vertexBuffer.BeginUpdate<InputVertex>();

	float w = GetWidgetSize().x;
	float h = GetWidgetSize().y;

	vertex[0].position.Set(0.0F, 0.0F);
	vertex[0].color = dynamicBackgroundColor;
	vertex[1].position.Set(0.0F, h);
	vertex[1].color = dynamicBackgroundColor;
	vertex[2].position.Set(w, h);
	vertex[2].color = dynamicBackgroundColor;
	vertex[3].position.Set(w, 0.0F);
	vertex[3].color = dynamicBackgroundColor;

	vertex[4].position.Set(0.0F, 0.0F);
	vertex[4].color.Set(0.25F, 0.25F, 0.25F, 1.0F);
	vertex[5].position.Set(0.0F, h);
	vertex[5].color.Set(0.25F, 0.25F, 0.25F, 1.0F);
	vertex[6].position.Set(w, h);
	vertex[6].color.Set(0.25F, 0.25F, 0.25F, 1.0F);
	vertex[7].position.Set(w, 0.0F);
	vertex[7].color.Set(0.25F, 0.25F, 0.25F, 1.0F);

	vertex[8].position.Set(-1.0F, -1.0F);
	vertex[8].color.Set(0.25F, 0.25F, 0.25F, 1.0F);
	vertex[9].position.Set(-1.0F, h + 1.0F);
	vertex[9].color.Set(0.25F, 0.25F, 0.25F, 1.0F);
	vertex[10].position.Set(w + 1.0F, h + 1.0F);
	vertex[10].color.Set(0.25F, 0.25F, 0.25F, 1.0F);
	vertex[11].position.Set(w + 1.0F, -1.0F);
	vertex[11].color.Set(0.25F, 0.25F, 0.25F, 1.0F);

	vertexBuffer.EndUpdate();
}

void InputWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	if (eventData->eventType == kEventMouseDown)
	{
		TheControlSettingsWindow->SelectControl(this);
		BeginConfig();
	}
}

void InputWidget::SetInputControl(InputControl *control, float value)
{
	if (control)
	{
		EndConfig();

		Action *action = control->GetControlAction();
		if ((!action) || (!(action->GetActionFlags() & kActionImmutable)))
		{
			inputControl = control;
			textWidget.SetText(control->GetControlName());

			TheControlSettingsWindow->ClearDuplicateControls(control, this);
		}
	}
	else
	{
		inputControl = nullptr;
		textWidget.SetText(nullptr);
	}
}

void InputWidget::BeginConfig(void)
{
	TheControlSettingsWindow->SetConfigWidget(this);
	TheInputMgr->SetInputMode(kInputConfiguration);
	TheInterfaceMgr->HideCursor();

	pulsateMutator.SetMutatorState(0);
}

void InputWidget::EndConfig(void)
{
	pulsateMutator.Reset();
	pulsateMutator.SetMutatorState(kMutatorDisabled);
	SetDynamicWidgetColor(GetWidgetColor());

	TheInputMgr->SetInputMode(kInputInactive);
	TheInterfaceMgr->ShowCursor();
}


ControlSettingsWindow::ControlSettingsWindow() :
		GameWindow("game/Controls"),
		Singleton<ControlSettingsWindow>(TheControlSettingsWindow)
{
	configWidget = nullptr;
}

ControlSettingsWindow::~ControlSettingsWindow()
{
	TheInputMgr->SetConfigProc(nullptr);
	TheInputMgr->SetInputMode(kInputInactive);

	TheGame->UpdateLookSpeedX(0.0F);
	TheGame->UpdateLookSpeedY(0.0F);
}

void ControlSettingsWindow::Open(void)
{
	if (TheControlSettingsWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheControlSettingsWindow);
	}
	else
	{
		TheGame->AddWindow(new ControlSettingsWindow);
	}
}

void ControlSettingsWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	controlList = static_cast<ListWidget *>(FindWidget("List"));

	mouseSlider = static_cast<SliderWidget *>(FindWidget("Mouse"));
	mouseText = static_cast<TextWidget *>(FindWidget("MouseText"));
	invertMouseBox = static_cast<CheckWidget *>(FindWidget("InvertMouse"));
	smoothMouseBox = static_cast<CheckWidget *>(FindWidget("SmoothMouse"));

	controllerSlider = static_cast<SliderWidget *>(FindWidget("Controller"));
	controllerText = static_cast<TextWidget *>(FindWidget("ControllerText"));
	invertXAxisBox = static_cast<CheckWidget *>(FindWidget("InvertX"));
	invertYAxisBox = static_cast<CheckWidget *>(FindWidget("InvertY"));

	const StringTable *actionTable = new StringTable("game/Input");

	Action *action = TheInputMgr->GetFirstAction();
	while (action)
	{
		if (!(action->GetActionFlags() & kActionPersistent))
		{
			Widget *group = new Widget;

			TextWidget *textWidget = new TextWidget(Vector2D(100.0F, 16.0F), actionTable->GetString(StringID(action->GetActionType())), "font/Normal");
			textWidget->SetWidgetPosition(Point3D(0.0F, 5.0F, 0.0F));
			textWidget->SetTextAlignment(kTextAlignRight);
			textWidget->Disable();
			group->AppendSubnode(textWidget);

			InputWidget *inputWidget = new InputWidget(controlList, action);
			inputWidget->SetWidgetPosition(Point3D(112.0F, 3.0F, 0.0F));
			group->AppendSubnode(inputWidget);

			controlList->AppendListItem(group);
			group->Enable();
		}

		action = action->Next();
	}

	delete actionTable;

	int32 sensitivity = TheInputMgr->GetMouseSensitivity();
	mouseSlider->SetValue(sensitivity - 1);
	mouseText->SetText(String<7>(sensitivity));

	unsigned_int32 flags = TheInputMgr->GetMouseFlags();

	if (flags & kMouseInverted)
	{
		invertMouseBox->SetValue(1);
	}

	if (flags & kMouseSmooth)
	{
		smoothMouseBox->SetValue(1);
	}

	sensitivity = TheGame->GetLookSensitivity();
	controllerSlider->SetValue(sensitivity - 1);
	controllerText->SetText(String<7>(sensitivity));

	flags = TheGame->GetInputFlags();

	if (flags & kInputXInverted)
	{
		invertXAxisBox->SetValue(1);
	}

	if (flags & kInputYInverted)
	{
		invertYAxisBox->SetValue(1);
	}

	TheInputMgr->SetConfigProc(&ConfigProc, this);
	SetFocusWidget(controlList);
}

void ControlSettingsWindow::ConfigProc(InputControl *control, float value, void *cookie)
{
	ControlSettingsWindow *window = static_cast<ControlSettingsWindow *>(cookie);

	InputWidget *widget = window->configWidget;
	if (widget)
	{
		window->configWidget = nullptr;
		widget->SetInputControl(control, value);
	}
}

bool ControlSettingsWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;

		if (keyCode == kKeyCodeEnter)
		{
			if (GetFocusWidget() == controlList)
			{
				controlList->Activate();
				return (true);
			}
		}
		else if ((keyCode == kKeyCodeBackspace) || (keyCode == kKeyCodeDelete))
		{
			if (GetFocusWidget() == controlList)
			{
				const Widget *group = controlList->GetFirstSelectedListItem();
				if (group)
				{
					static_cast<InputWidget *>(group->GetLastSubnode())->SetInputControl(nullptr);
				}

				return (true);
			}
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void ControlSettingsWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			TheInputMgr->ClearAllControlActions();

			const Widget *group = controlList->GetFirstListItem();
			while (group)
			{
				const InputWidget *inputWidget = static_cast<InputWidget *>(group->GetLastSubnode());

				InputControl *control = inputWidget->GetInputControl();
				if (control)
				{
					control->SetControlAction(inputWidget->GetInputAction());
				}

				group = group->Next();
			}

			TheEngine->GetVariable("sensitivity")->SetIntegerValue(mouseSlider->GetValue() + 1);
			TheEngine->GetVariable("invertMouse")->SetIntegerValue(invertMouseBox->GetValue());
			TheEngine->GetVariable("smoothMouse")->SetIntegerValue(smoothMouseBox->GetValue());

			TheEngine->GetVariable("looksens")->SetIntegerValue(controllerSlider->GetValue() + 1);
			TheEngine->GetVariable("invertX")->SetIntegerValue(invertXAxisBox->GetValue());
			TheEngine->GetVariable("invertY")->SetIntegerValue(invertYAxisBox->GetValue());

			ConfigDataDescription::WriteEngineConfig();
			ConfigDataDescription::WriteInputConfig();

			if (TheDisplayBoard)
			{
				TheDisplayBoard->UpdateWeaponControls();
			}

			CallCompletionProc();
			Close();
		}
		else if (widget == cancelButton)
		{
			CallCompletionProc();
			Close();
		}
		else if (widget == controlList)
		{
			const Widget *group = controlList->GetFirstSelectedListItem();
			if (group)
			{
				static_cast<InputWidget *>(group->GetLastSubnode())->BeginConfig();
			}
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == mouseSlider)
		{
			mouseText->SetText(String<7>(mouseSlider->GetValue() + 1));
		}
		else if (widget == controllerSlider)
		{
			controllerText->SetText(String<7>(controllerSlider->GetValue() + 1));
		}
	}
}

void ControlSettingsWindow::SelectControl(const InputWidget *inputWidget)
{
	int32 index = 0;
	const Widget *group = controlList->GetFirstListItem();
	while (group)
	{
		InputWidget *widget = static_cast<InputWidget *>(group->GetLastSubnode());
		if (widget == inputWidget)
		{
			controlList->SelectListItem(index);
		}

		index++;
		group = group->Next();
	}
}

void ControlSettingsWindow::ClearDuplicateControls(const InputControl *control, const InputWidget *inputWidget)
{
	const Widget *group = controlList->GetFirstListItem();
	while (group)
	{
		InputWidget *widget = static_cast<InputWidget *>(group->GetLastSubnode());
		if ((widget != inputWidget) && (widget->GetInputControl() == control))
		{
			widget->SetInputControl(nullptr);
		}

		group = group->Next();
	}
}

// ZYUQURM
