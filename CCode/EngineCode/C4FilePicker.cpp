 

#include "C4FilePicker.h"
#include "C4ConfigData.h"
#include "C4Viewports.h"
#include "C4World.h"


using namespace C4;


Map<FilePickerVisit> FilePicker::visitMap;


FilePickerVisit::FilePickerVisit(Type type, const char *path)
{
	pickerType = type;
	visitPath = path;
}

FilePickerVisit::~FilePickerVisit()
{
}


FilePicker::FilePicker(const char *title, const char *directory, unsigned_int32 flags, const char *panel) :
		Window((panel) ? panel : ((flags & kFilePickerSave) ? "C4/FileSave" : "C4/FileOpen")),
		currentDirectory(directory)
{
	if (title)
	{
		SetWindowTitle(title);
	}

	filePickerFlags = flags;
	filePickerType = 0;

	resourceCatalog = nullptr;
	resourceDescriptor = nullptr;
	rootPathLength = 0;
	directoryDepth = 0;
}

FilePicker::FilePicker(Type pickerType, const char *title, const ResourceCatalog *catalog, const ResourceDescriptor *descriptor, const char *directory, unsigned_int32 flags, const char *panel) :
		Window((panel) ? panel : ((flags & kFilePickerSave) ? "C4/FileSave" : "C4/FileOpen")),
		currentDirectory(catalog->GetRootPath())
{
	if (title)
	{
		SetWindowTitle(title);
	}

	filePickerFlags = flags;
	filePickerType = pickerType;

	resourceCatalog = catalog;
	resourceDescriptor = descriptor;

	rootPathLength = resourceCatalog->GetRootPathLength();
	directoryDepth = 0;

	FilePickerVisit *visit = visitMap.Find(pickerType);
	if (visit)
	{
		directory = visit->GetVisitPath();
	}
	else if ((directory) && (directory[0] == 0))
	{
		directory = nullptr;
	}

	if (directory)
	{
		currentDirectory += directory;
	}
	else
	{
		currentDirectory[rootPathLength - 1] = 0;
	}
}

FilePicker::~FilePicker()
{
}

void FilePicker::Preprocess(void)
{
	Window::Preprocess();

	openButton = static_cast<PushButtonWidget *>(FindWidget("Open"));
	saveButton = static_cast<PushButtonWidget *>(FindWidget("Save"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));
	upButton = static_cast<IconButtonWidget *>(FindWidget("Up"));

	resourceList = static_cast<ListWidget *>(FindWidget("List"));
	directoryMenu = static_cast<PopupMenuWidget *>(FindWidget("Path"));
	nameBox = static_cast<EditTextWidget *>(FindWidget("Edit"));

	if (nameBox)
	{
		nameBox->SetFilterProc(&FileNameFilter);
		SetFocusWidget(nameBox);
	} 
	else
	{
		if (filePickerFlags & kFilePickerMultipleSelection) 
		{
			resourceList->SetListFlags(resourceList->GetListFlags() | kListMultipleSelection); 
		}

		SetFocusWidget(resourceList); 
	}
 
	updateFileListFlag = true; 
	updateButtonStateFlag = true;
}

void FilePicker::UpdateButtonState(void) 
{
	if (!saveButton)
	{
		const Widget *listItem = resourceList->GetFirstSelectedListItem();
		if (listItem)
		{
			openButton->Enable();
		}
		else
		{
			openButton->Disable();
		}
	}
	else
	{
		unsigned_int32 openState = openButton->GetWidgetState() & ~(kWidgetDisabled | kWidgetHidden);
		unsigned_int32 saveState = saveButton->GetWidgetState() & ~(kWidgetDisabled | kWidgetHidden);

		if ((nameBox->GetText()[0] == 0) || ((!(filePickerFlags & kFilePickerEnableRootDirectory)) && (directoryDepth == 1)))
		{
			saveState |= kWidgetDisabled;
		}

		if (GetFocusWidget() == nameBox)
		{
			openState |= kWidgetHidden;
		}
		else
		{
			const Widget *listItem = resourceList->GetFirstSelectedListItem();
			if ((!listItem) || (listItem->GetNodeIndex() >= directoryCount))
			{
				openState |= kWidgetHidden;
			}
			else
			{
				saveState |= kWidgetHidden;
			}
		}

		openButton->SetWidgetState(openState);
		saveButton->SetWidgetState(saveState);
	}
}

void FilePicker::SetResourceDescriptor(const ResourceDescriptor *descriptor)
{
	resourceDescriptor = descriptor;
	updateFileListFlag = true;
}

void FilePicker::SetCurrentDirectory(const char *directory)
{
	currentDirectory = directory;
	BuildFileList();
}

void FilePicker::BuildFileList(void)
{
	Map<FileReference>		fileMap;

	updateFileListFlag = false;

	resourceList->PurgeListItems();
	resourceList->SetDisplayIndex(0);

	if (resourceCatalog)
	{
		int32 start = Min(Text::GetTextLength(currentDirectory), rootPathLength);
		resourceCatalog->BuildResourceMap(resourceDescriptor, &currentDirectory[start], &fileMap);
	}
	else
	{
		FileMgr::BuildFileMap(currentDirectory, &fileMap);
	}

	int32 count = 0;
	FileReference *reference = fileMap.First();
	while (reference)
	{
		FileReference *next = reference->Next();

		if (reference->GetFlags() & kFileDirectory)
		{
			Widget *widget = new Widget;

			ImageWidget *image = new ImageWidget(Vector2D(16.0F, 16.0F), "C4/folder");
			widget->AppendSubnode(image);

			TextWidget *text = new TextWidget(Vector2D(resourceList->GetWidgetSize().x - 32.0F, 16.0F), reference->GetName(), "font/Gui");
			text->SetWidgetPosition(Point3D(16.0F, 2.0F, 0.0F));
			widget->AppendSubnode(text);

			resourceList->AppendListItem(widget);

			delete reference;
			count++;
		}

		reference = next;
	}

	directoryCount = count;

	reference = fileMap.First();
	while (reference)
	{
		Widget *widget = new Widget;

		ImageWidget *image = new ImageWidget(Vector2D(16.0F, 16.0F), "C4/file");
		widget->AppendSubnode(image);

		TextWidget *text = new TextWidget(Vector2D(resourceList->GetWidgetSize().x - 32.0F, 16.0F), reference->GetName(), "font/Gui");
		text->SetWidgetPosition(Point3D(16.0F, 2.0F, 0.0F));
		widget->AppendSubnode(text);

		resourceList->AppendListItem(widget);

		reference = reference->Next();
	}

	if (!nameBox)
	{
		resourceList->SelectListItem(0);
	}

	BuildDirectoryMenu();

	matchTime = TheTimeMgr->GetSystemAbsoluteTime() - 1000;
	matchLength = 0;

	updateButtonStateFlag = true;
}

void FilePicker::BuildDirectoryMenu(void)
{
	ResourcePath path(currentDirectory);

	directoryMenu->PurgeMenuItems();
	int32 count = 0;

	machine start = (filePickerFlags & kFilePickerStayInsideRootDirectory) ? rootPathLength : 0;
	for (machine len = 0;; len++)
	{
		char c = path[len];
		if ((c == '/') || (c == 0))
		{
			if (len - start > 0)
			{
				directoryMenu->AppendMenuItem(new MenuItemWidget(ResourcePath(path, len)));
				count++;
			}

			if (c == 0)
			{
				break;
			}

			start = len + 1;
		}
	}

	directoryMenu->SetSelection(count - 1);
	directoryDepth = count;

	if (count > 1)
	{
		upButton->Enable();
	}
	else
	{
		upButton->Disable();
	}
}

void FilePicker::SetVisit(Type type, const char *path)
{
	FilePickerVisit *visit = visitMap.Find(type);
	if (!visit)
	{
		visit = new FilePickerVisit(type, path);
		delete visitMap.InsertReplace(visit);
	}
	else
	{
		visit->SetVisitPath(path);
	}
}

void FilePicker::RememberVisit(void)
{
	Type type = filePickerType;
	if (type != 0)
	{
		SetVisit(type, &currentDirectory[Min(Text::GetTextLength(currentDirectory), rootPathLength)]);
		ConfigDataDescription::WriteEngineConfig();
	}
}

bool FilePicker::OpenFolder(void)
{
	const Widget *listItem = resourceList->GetFirstSelectedListItem();
	if ((listItem) && (!resourceList->GetNextSelectedListItem(listItem)) && (listItem->GetNodeIndex() < directoryCount))
	{
		String<kMaxFileNameLength> directory(currentDirectory);
		if (directory[directory.Length() - 1] != '/')
		{
			directory += '/';
		}

		currentDirectory = directory + GetListItemText(listItem);
		updateFileListFlag = true;
		return (true);
	}

	return (false);
}

void FilePicker::MoveUpward(void)
{
	const MenuItemWidget *widget = directoryMenu->GetLastMenuItem()->Previous();
	if (widget)
	{
		currentDirectory = widget->GetTextWidget()->GetText();
		updateFileListFlag = true;
	}
}

const char *FilePicker::GetListItemText(const Widget *widget)
{
	widget = widget->GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetType() == kWidgetText)
		{
			return (static_cast<const TextWidget *>(widget)->GetText());
		}

		widget = widget->Next();
	}

	return ("");
}

String<kMaxFileNameLength> FilePicker::GetFileName(int32 index) const
{
	String<kMaxFileNameLength> name(&currentDirectory[Min(rootPathLength, Text::GetTextLength(currentDirectory))]);
	if ((name[0] != 0) && (name[name.Length() - 1] != '/'))
	{
		name += '/';
	}

	if (!nameBox)
	{
		const Widget *listItem = resourceList->GetSelectedListItem(index);
		if ((listItem) && (listItem->GetNodeIndex() >= directoryCount))
		{
			name += GetListItemText(listItem);
			return (name);
		}
	}
	else
	{
		name += nameBox->GetText();
		if (resourceDescriptor)
		{
			name += resourceDescriptor->GetExtension();
		}

		return (name);
	}

	return ("");
}

ResourceName FilePicker::GetResourceName(int32 index) const
{
	String<kMaxFileNameLength> fileName(GetFileName(index));
	ResourceName resourceName(&fileName[(filePickerFlags & kFilePickerEnableRootDirectory) ? 0 : Text::GetPrefixDirectoryLength(fileName)]);
	if ((!resourceDescriptor) || (!(resourceDescriptor->GetFlags() & kResourceDontAppendType)))
	{
		resourceName[Text::GetResourceNameLength(resourceName)] = 0;
	}

	return (resourceName);
}

void FilePicker::SetFileName(const char *name)
{
	if (nameBox)
	{
		nameBox->SetText(name);
	}
}

bool FilePicker::FileNameFilter(unsigned_int32 code)
{
	return ((code - 0x0030 < 10U) || (code - 0x0041 < 26U) || (code - 0x0061 < 26U) || (code == '-') || (code == '_') || (code == '/'));
}

void FilePicker::Move(void)
{
	if (updateFileListFlag)
	{
		BuildFileList();
	}

	if (updateButtonStateFlag)
	{
		updateButtonStateFlag = false;
		UpdateButtonState();
	}

	Window::Move();
}

void FilePicker::SetFocusWidget(Widget *widget)
{
	if (GetFocusWidget() != widget)
	{
		updateButtonStateFlag = true;
		Window::SetFocusWidget(widget);
	}
}

bool FilePicker::ConfirmReplace(void)
{
	if (nameBox)
	{
		String<kMaxFileNameLength> name(nameBox->GetText());
		if (resourceDescriptor)
		{
			name += resourceDescriptor->GetExtension();
		}

		const Widget *listItem = resourceList->GetFirstListItem();
		while (listItem)
		{
			if (name == GetListItemText(listItem))
			{
				const StringTable *table = TheInterfaceMgr->GetStringTable();

				Dialog *dialog = new Dialog(Vector2D(320.0F, 128.0F), table->GetString(StringID('FPCK', 'CFRM')), table->GetString(StringID('FPCK', 'YES ')), table->GetString(StringID('FPCK', 'NO  ')));
				dialog->SetCompletionProc(&ConfirmComplete, this);

				TextWidget *text = new TextWidget(Vector2D(296.0F, 12.0F), table->GetString(StringID('FPCK', 'REPL')), "font/Gui");
				text->SetWidgetPosition(Point3D(88.0F, 16.0F, 0.0F));
				dialog->AppendSubnode(text);

				ImageWidget *image = new ImageWidget(Vector2D(64.0F, 64.0F), "C4/warning");
				image->SetWidgetPosition(Point3D(12.0F, 12.0F, 0.0F));
				dialog->AppendSubnode(image);

				AddSubwindow(dialog);
				return (false);
			}

			listItem = listItem->Next();
		}
	}

	return (true);
}

void FilePicker::ConfirmComplete(Dialog *dialog, void *cookie)
{
	if (dialog->GetDialogStatus() == kDialogOkay)
	{
		FilePicker *filePicker = static_cast<FilePicker *>(cookie);
		TheInterfaceMgr->RemoveWidget(filePicker);
		filePicker->RememberVisit();
		filePicker->CallCompletionProc();
		filePicker->Close();
	}
}

bool FilePicker::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;

		if (keyCode == kKeyCodeEnter)
		{
			if (updateButtonStateFlag)
			{
				updateButtonStateFlag = false;
				UpdateButtonState();
			}

			if (openButton->Visible())
			{
				openButton->Activate();
			}
			else
			{
				saveButton->Activate();
			}

			return (true);
		}
		else if (keyCode == kKeyCodeBackspace)
		{
			if (GetFocusWidget() == resourceList)
			{
				MoveUpward();
				return (true);
			}
		}
		else if (FileNameFilter(keyCode))
		{
			if (GetFocusWidget() == resourceList)
			{
				unsigned_int32 time = TheTimeMgr->GetSystemAbsoluteTime();
				if (time - matchTime < 1000)
				{
					int32 length = matchLength;
					if (length < kMaxFileNameLength)
					{
						matchText[length] = (char) keyCode;
					}

					matchLength = length + 1;
				}
				else
				{
					matchLength = 1;
					matchText[0] = (char) keyCode;
				}

				matchTime = time;

				int32 count = 0;
				int32 length = matchLength;
				const Widget *listItem = resourceList->GetFirstListItem();
				while (listItem)
				{
					bool match = true;
					const char *text = GetListItemText(listItem);
					for (machine a = 0; a < length; a++)
					{
						unsigned_int32 c1 = text[a];
						unsigned_int32 c2 = matchText[a];

						if (c1 - 'A' < 26)
						{
							c1 += 32;
						}

						if (c2 - 'A' < 26)
						{
							c2 += 32;
						}

						if (c1 != c2)
						{
							match = false;
							break;
						}
					}

					if (match)
					{
						resourceList->UnselectAllListItems();
						resourceList->SelectListItem(count);
						resourceList->RevealListItem(count);
						resourceList->PostWidgetEvent(WidgetEventData(kEventWidgetChange));
						break;
					}

					listItem = listItem->Next();
					count++;
				}

				return (true);
			}
		}
		else
		{
			matchTime = TheTimeMgr->GetSystemAbsoluteTime() - 1000;
			matchLength = 0;
		}
	}
	else if (eventType == kEventKeyCommand)
	{
		if (eventData->keyCode == kKeyCodeUpArrow)
		{
			MoveUpward();
			SetFocusWidget(resourceList);
			return (true);
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void FilePicker::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == openButton)
		{
			if (!OpenFolder())
			{
				TheInterfaceMgr->RemoveWidget(this);
				RememberVisit();
				CallCompletionProc();
				Close();
			}
		}
		else if (widget == saveButton)
		{
			if (ConfirmReplace())
			{
				TheInterfaceMgr->RemoveWidget(this);
				RememberVisit();
				CallCompletionProc();
				Close();
			}
		}
		else if (widget == cancelButton)
		{
			Close();
		}
		else if (widget == upButton)
		{
			MoveUpward();
		}
		else if (widget == resourceList)
		{
			if ((resourceList->GetFirstSelectedListItem()) && (!OpenFolder()) && (ConfirmReplace()))
			{
				TheInterfaceMgr->RemoveWidget(this);
				RememberVisit();
				CallCompletionProc();
				Close();
			}
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == directoryMenu)
		{
			currentDirectory = directoryMenu->GetSelectedMenuItem()->GetTextWidget()->GetText();
			updateFileListFlag = true;
		}
		else if (widget == resourceList)
		{
			updateButtonStateFlag = true;

			const Widget *listItem = resourceList->GetFirstSelectedListItem();
			if ((listItem) && (nameBox) && (listItem->GetNodeIndex() >= directoryCount))
			{
				ResourceName name(GetListItemText(listItem));
				name[Text::GetResourceNameLength(name)] = 0;
				nameBox->SetText(name);
			}
		}
		else if ((nameBox) && (widget == nameBox))
		{
			updateButtonStateFlag = true;
		}
	}
}


PreviewPicker::PreviewPicker(const char *title, const char *directory, unsigned_int32 flags, const char *panel) : FilePicker(title, directory, flags, panel)
{
}

PreviewPicker::PreviewPicker(Type pickerType, const char *title, const ResourceCatalog *catalog, const ResourceDescriptor *descriptor, const char *directory, unsigned_int32 flags, const char *panel) : FilePicker(pickerType, title, catalog, descriptor, directory, flags, panel)
{
}

PreviewPicker::~PreviewPicker()
{
}

void PreviewPicker::BuildFileList(void)
{
	FilePicker::BuildFileList();
	UpdatePreview();
}

void PreviewPicker::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetChange) && (widget == GetResourceListWidget()))
	{
		UpdatePreview();
	}

	FilePicker::HandleWidgetEvent(widget, eventData);
}


TexturePicker::TexturePicker(const char *title, const char *directory, unsigned_int32 flags) : PreviewPicker(title, directory, flags, "C4/TextureOpen")
{
}

TexturePicker::TexturePicker(Type pickerType, const char *title, const ResourceCatalog *catalog, const ResourceDescriptor *descriptor, const char *directory, unsigned_int32 flags) : PreviewPicker(pickerType, title, catalog, descriptor, directory, flags, "C4/TextureOpen")
{
}

TexturePicker::~TexturePicker()
{
}

void TexturePicker::Preprocess(void)
{
	FilePicker::Preprocess();

	previewBorder = static_cast<BorderWidget *>(FindWidget("Border"));
	previewImage = static_cast<ImageWidget *>(FindWidget("Preview"));
	previewText = static_cast<TextWidget *>(FindWidget("Message"));

	UpdatePreview();
}

void TexturePicker::UpdatePreview(void)
{
	if (GetFileNameCount() == 1)
	{
		const char *name = GetFileName();
		if (name[0] != 0)
		{
			ResourcePath path(name);
			path[Text::GetResourceNameLength(path)] = 0;
			previewImage->SetTexture(0, &path[Text::GetPrefixDirectoryLength(path)]);

			previewImage->Show();
			previewText->Hide();

			const Texture *texture = previewImage->GetTexture();
			float width = (float) texture->GetTextureWidth();
			float height = (float) texture->GetTextureHeight();

			const Vector2D& size = previewBorder->GetWidgetSize();

			if (width > height)
			{
				float r = height / width;
				width = Fmin(width, size.x);
				height = width * r;
			}
			else
			{
				float r = width / height;
				height = Fmin(height, size.y);
				width = height * r;
			}

			Vector3D position(Floor((size.x - width) * 0.5F), Floor((size.y - height) * 0.5F), 0.0F);
			previewImage->SetWidgetPosition(previewBorder->GetWidgetPosition() + position);
			previewImage->SetWidgetSize(Vector2D(width, height));
			previewImage->Invalidate();
			return;
		}
	}

	previewImage->SetTexture(0, "C4/missing");

	previewImage->Hide();
	previewText->Show();
}


SoundPicker::SoundPicker(const char *title, const char *directory, unsigned_int32 flags) : PreviewPicker(title, directory, flags, "C4/SoundOpen")
{
	previewSound = nullptr;
}

SoundPicker::SoundPicker(Type pickerType, const char *title, const ResourceCatalog *catalog, const ResourceDescriptor *descriptor, const char *directory, unsigned_int32 flags) : PreviewPicker(pickerType, title, catalog, descriptor, directory, flags, "C4/SoundOpen")
{
	previewSound = nullptr;
}

SoundPicker::~SoundPicker()
{
	if (previewSound)
	{
		previewSound->Release();
	}
}

void SoundPicker::Preprocess(void)
{
	FilePicker::Preprocess();

	playButton = static_cast<IconButtonWidget *>(FindWidget("Play"));
	stopButton = static_cast<IconButtonWidget *>(FindWidget("Stop"));
	previewText = static_cast<TextWidget *>(FindWidget("Message"));

	UpdatePreview();
}

void SoundPicker::UpdatePreview(void)
{
	if (previewSound)
	{
		previewSound->Release();
		previewSound = nullptr;
	}

	if (GetFileNameCount() == 1)
	{
		const char *name = GetFileName();
		if (name[0] != 0)
		{
			SoundResult		result;

			ResourcePath path(name);
			path[Text::GetResourceNameLength(path)] = 0;
			name = &path[Text::GetPrefixDirectoryLength(path)];

			Sound *sound = new Sound;

			if (SoundResource::DetermineStreaming(name))
			{
				WaveStreamer *streamer = new WaveStreamer;
				sound->Stream(streamer);
				result = streamer->AddComponent(name);
			}
			else
			{
				result = sound->Load(name);
			}

			if (result == kSoundOkay)
			{
				previewSound = sound;
				sound->SetCompletionProc(&SoundComplete, this);
				sound->SetSoundFlags(kSoundPersistent);

				playButton->Show();
				playButton->Enable();
				stopButton->Show();
				stopButton->Disable();
				previewText->Hide();
				return;
			}

			sound->Release();
		}
	}

	playButton->Hide();
	stopButton->Hide();
	previewText->Show();
}

void SoundPicker::SoundComplete(Sound *sound, void *cookie)
{
	SoundPicker *soundPicker = static_cast<SoundPicker *>(cookie);

	soundPicker->playButton->Enable();
	soundPicker->stopButton->Disable();
}

void SoundPicker::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == playButton)
		{
			if ((previewSound) && (previewSound->Play() == kSoundOkay))
			{
				playButton->Disable();
				stopButton->Enable();
			}

			return;
		}
		else if (widget == stopButton)
		{
			if (previewSound)
			{
				previewSound->Stop();
			}

			playButton->Enable();
			stopButton->Disable();
			return;
		}
	}

	PreviewPicker::HandleWidgetEvent(widget, eventData);
}


WorldPicker::WorldPicker(const char *title, const char *directory, unsigned_int32 flags) : PreviewPicker(title, directory, flags, "C4/WorldOpen")
{
}

WorldPicker::WorldPicker(Type pickerType, const char *title, const ResourceCatalog *catalog, const ResourceDescriptor *descriptor, const char *directory, unsigned_int32 flags) : PreviewPicker(pickerType, title, catalog, descriptor, directory, flags, "C4/WorldOpen")
{
}

WorldPicker::~WorldPicker()
{
}

void WorldPicker::Preprocess(void)
{
	FilePicker::Preprocess();

	previewViewport = static_cast<WorldViewportWidget *>(FindWidget("Preview"));
	previewText = static_cast<TextWidget *>(FindWidget("Message"));

	UpdatePreview();
}

void WorldPicker::UpdatePreview(void)
{
	if (GetFileNameCount() == 1)
	{
		const char *name = GetFileName();
		if (name[0] != 0)
		{
			File	file;

			const char *root = TheResourceMgr->GetGenericCatalog()->GetRootPath();
			if (file.Open(ResourcePath(root) += name) == kFileOkay)
			{
				unsigned_int64 size = file.GetSize();
				file.Close();

				if (size < 1048576U)
				{
					ResourcePath path(name);
					path[Text::GetResourceNameLength(path)] = 0;
					previewViewport->LoadWorld(&path[Text::GetPrefixDirectoryLength(path)]);

					World *world = previewViewport->GetViewportWorld();
					if (world)
					{
						Box3D	worldBox;

						bool boxFlag = false;
						Zone *zone = world->GetRootNode();
						const Node *node = zone->GetFirstSubnode();
						while (node)
						{
							if (node->GetNodeType() == kNodeGeometry)
							{
								const Box3D& box = node->GetWorldBoundingBox();
								if (boxFlag)
								{
									worldBox.Union(box);
								}
								else
								{
									boxFlag = true;
									worldBox = box;
								}
							}

							node = zone->GetNextNode(node);
						}

						if (boxFlag)
						{
							previewViewport->EnableCameraOrbit((worldBox.max + worldBox.min) * 0.5F, Magnitude(worldBox.max - worldBox.min) * 1.25F);
							previewViewport->SetCameraAngles(K::tau_over_12 * -5.0F, K::tau_over_12 * -0.75F);
						}
						else
						{
							previewViewport->DisableCameraOrbit();
							previewViewport->SetCameraTransform(0.0F, 0.0F, Zero3D);
						}

						ZoneObject *zoneObject = zone->GetObject();
						zoneObject->SetAmbientLight(ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F));
						zoneObject->SetEnvironmentMap("C4/environment");

						Light *light = new InfiniteLight(ColorRGB(1.0F, 1.0F, 1.0F));
						light->SetNodeMatrix3D(Matrix3D().SetRotationAboutZ(-K::tau_over_8) * Matrix3D().SetRotationAboutY(K::tau_over_8));
						zone->AppendNewSubnode(light);
						light->Update();

						previewViewport->Show();
						previewText->Hide();
						return;
					}
				}
			}
		}
	}

	previewViewport->UnloadWorld();

	previewViewport->Hide();
	previewText->Show();
}

// ZYUQURM
