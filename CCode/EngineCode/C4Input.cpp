 

#include "C4Input.h"
#include "C4Engine.h"

#if C4XINPUT && C4FASTBUILD

	#include "C4XInput.h"

#endif

#if C4CONSOLE //[ CONSOLE

	// -- Console code hidden --

#endif //]


using namespace C4;


namespace
{
	enum
	{
		kInputQueueSize		= 1024
	};


	#if C4MACOS || C4CONSOLE

		// Key assignments from HID usage page 0x07.

		const char *const keyButtonName[kKeyboardUsageCount] =
		{
			/* 0x00 */	"0x00",
			/* 0x01 */	"0x01",
			/* 0x02 */	"0x02",
			/* 0x03 */	"0x03",
			/* 0x04 */	"A",
			/* 0x05 */	"B",
			/* 0x06 */	"C",
			/* 0x07 */	"D",
			/* 0x08 */	"E",
			/* 0x09 */	"F",
			/* 0x0A */	"G",
			/* 0x0B */	"H",
			/* 0x0C */	"I",
			/* 0x0D */	"J",
			/* 0x0E */	"K",
			/* 0x0F */	"L",
			/* 0x10 */	"M",
			/* 0x11 */	"N",
			/* 0x12 */	"O",
			/* 0x13 */	"P",
			/* 0x14 */	"Q",
			/* 0x15 */	"R",
			/* 0x16 */	"S",
			/* 0x17 */	"T",
			/* 0x18 */	"U",
			/* 0x19 */	"V",
			/* 0x1A */	"W",
			/* 0x1B */	"X",
			/* 0x1C */	"Y",
			/* 0x1D */	"Z",
			/* 0x1E */	"1",
			/* 0x1F */	"2",
			/* 0x20 */	"3",
			/* 0x21 */	"4",
			/* 0x22 */	"5",
			/* 0x23 */	"6",
			/* 0x24 */	"7",
			/* 0x25 */	"8",
			/* 0x26 */	"9",
			/* 0x27 */	"0",
		
			#if C4MACOS

				/* 0x28 */	"Return",

			#else

				/* 0x28 */	"Enter",
		
			#endif

			/* 0x29 */	"Escape",
			/* 0x2A */	"Backspace",
			/* 0x2B */	"Tab",
			/* 0x2C */	"Space",
			/* 0x2D */	"-",
			/* 0x2E */	"=",
			/* 0x2F */	"[",
			/* 0x30 */	"]",
			/* 0x31 */	"\\",
			/* 0x32 */	"Pound",
			/* 0x33 */	";",
			/* 0x34 */	"'",
			/* 0x35 */	"`",
			/* 0x36 */	",",
			/* 0x37 */	".",
			/* 0x38 */	"/",
			/* 0x39 */	"Caps Lock", 
			/* 0x3A */	"F1",
			/* 0x3B */	"F2",
			/* 0x3C */	"F3", 
			/* 0x3D */	"F4",
			/* 0x3E */	"F5", 
			/* 0x3F */	"F6",
			/* 0x40 */	"F7",
			/* 0x41 */	"F8", 
			/* 0x42 */	"F9",
			/* 0x43 */	"F10", 
			/* 0x44 */	"F11", 
			/* 0x45 */	"F12",
			/* 0x46 */	"Print",
			/* 0x47 */	"Scroll Lock",
			/* 0x48 */	"Pause", 
			/* 0x49 */	"Insert",
			/* 0x4A */	"Home",
			/* 0x4B */	"Page Up",
			/* 0x4C */	"Delete",
			/* 0x4D */	"End",
			/* 0x4E */	"Page Down",
			/* 0x4F */	"Right Arrow",
			/* 0x50 */	"Left Arrow",
			/* 0x51 */	"Down Arrow",
			/* 0x52 */	"Up Arrow",
			/* 0x53 */	"Num Lock",
			/* 0x54 */	"Pad /",
			/* 0x55 */	"Pad *",
			/* 0x56 */	"Pad -",
			/* 0x57 */	"Pad +",
			/* 0x58 */	"Pad Enter",
			/* 0x59 */	"Pad 1",
			/* 0x5A */	"Pad 2",
			/* 0x5B */	"Pad 3",
			/* 0x5C */	"Pad 4",
			/* 0x5D */	"Pad 5",
			/* 0x5E */	"Pad 6",
			/* 0x5F */	"Pad 7",
			/* 0x60 */	"Pad 8",
			/* 0x61 */	"Pad 9",
			/* 0x62 */	"Pad 0",
			/* 0x63 */	"Pad .",
			/* 0x64 */	"Backslash",
			/* 0x65 */	"Appl",
			/* 0x66 */	"Power",
			/* 0x67 */	"Pad =",
			/* 0x68 */	"F13",
			/* 0x69 */	"F14",
			/* 0x6A */	"F15",
			/* 0x6B */	"F16",
			/* 0x6C */	"F17",
			/* 0x6D */	"F18",
			/* 0x6E */	"F19",
			/* 0x6F */	"F20",
			/* 0x70 */	"F21",
			/* 0x71 */	"F22",
			/* 0x72 */	"F23",
			/* 0x73 */	"F24",
			/* 0x74 */	"Execute",
			/* 0x75 */	"Help",
			/* 0x76 */	"Menu",
			/* 0x77 */	"Select",
			/* 0x78 */	"Stop",
			/* 0x79 */	"Again",
			/* 0x7A */	"Undo",
			/* 0x7B */	"Cut",
			/* 0x7C */	"Copy",
			/* 0x7D */	"Paste",
			/* 0x7E */	"Find",
			/* 0x7F */	"Mute",
			/* 0x80 */	"Vol Up",
			/* 0x81 */	"Vol Down",
			/* 0x82 */	"Lock Caps",
			/* 0x83 */	"Lock Num",
			/* 0x84 */	"Lock Scroll",
			/* 0x85 */	"Pad ,",
			/* 0x86 */	"Pad Equals",
			/* 0x87 */	"Inter1",
			/* 0x88 */	"Inter2",
			/* 0x89 */	"Inter3",
			/* 0x8A */	"Inter4",
			/* 0x8B */	"Inter5",
			/* 0x8C */	"Inter6",
			/* 0x8D */	"Inter7",
			/* 0x8E */	"Inter8",
			/* 0x8F */	"Inter9",
			/* 0x90 */	"Lang1",
			/* 0x91 */	"Lang2",
			/* 0x92 */	"Lang3",
			/* 0x93 */	"Lang4",
			/* 0x94 */	"Lang5",
			/* 0x95 */	"Lang6",
			/* 0x96 */	"Lang7",
			/* 0x97 */	"Lang8",
			/* 0x98 */	"Lang9",
			/* 0x99 */	"Erase",
			/* 0x9A */	"SysReq",
			/* 0x9B */	"Cancel",
			/* 0x9C */	"Clear",
			/* 0x9D */	"Prior",
			/* 0x9E */	"Return",
			/* 0x9F */	"Separator",
			/* 0xA0 */	"Out",
			/* 0xA1 */	"Oper",
			/* 0xA2 */	"Clear/Again",
			/* 0xA3 */	"CrSel/Props",
			/* 0xA4 */	"ExSel",
			/* 0xA5 */	"0xA5",
			/* 0xA6 */	"0xA6",
			/* 0xA7 */	"0xA7",
			/* 0xA8 */	"0xA8",
			/* 0xA9 */	"0xA9",
			/* 0xAA */	"0xAA",
			/* 0xAB */	"0xAB",
			/* 0xAC */	"0xAC",
			/* 0xAD */	"0xAD",
			/* 0xAE */	"0xAE",
			/* 0xAF */	"0xAF",
			/* 0xB0 */	"Pad 00",
			/* 0xB1 */	"Pad 000",
			/* 0xB2 */	"Thousands Sep",
			/* 0xB3 */	"Decimal Sep",
			/* 0xB4 */	"Cur Unit",
			/* 0xB5 */	"Cur Sub-unit",
			/* 0xB6 */	"Pad (",
			/* 0xB7 */	"Pad )",
			/* 0xB8 */	"Pad {",
			/* 0xB9 */	"Pad }",
			/* 0xBA */	"Pad Tab",
			/* 0xBB */	"Pad Backspace",
			/* 0xBC */	"Pad A",
			/* 0xBD */	"Pad B",
			/* 0xBE */	"Pad C",
			/* 0xBF */	"Pad D",
			/* 0xC0 */	"Pad E",
			/* 0xC1 */	"Pad F",
			/* 0xC2 */	"Pad Xor",
			/* 0xC3 */	"Pad ^",
			/* 0xC4 */	"Pad %",
			/* 0xC5 */	"Pad <",
			/* 0xC6 */	"Pad >",
			/* 0xC7 */	"Pad &",
			/* 0xC8 */	"Pad And",
			/* 0xC9 */	"Pad |",
			/* 0xCA */	"Pad Or",
			/* 0xCB */	"Pad :",
			/* 0xCC */	"Pad Num",
			/* 0xCD */	"Pad Space",
			/* 0xCE */	"Pad @",
			/* 0xCF */	"Pad !",
			/* 0xD0 */	"Pad MS",
			/* 0xD1 */	"Pad MR",
			/* 0xD2 */	"Pad MC",
			/* 0xD3 */	"Pad M+",
			/* 0xD4 */	"Pad M-",
			/* 0xD5 */	"Pad M*",
			/* 0xD6 */	"Pad M/",
			/* 0xD7 */	"Pad +/-",
			/* 0xD8 */	"Pad Clear",
			/* 0xD9 */	"Pad CE",
			/* 0xDA */	"Pad Bin",
			/* 0xDB */	"Pad Oct",
			/* 0xDC */	"Pad Dec",
			/* 0xDD */	"Pad Hex",
			/* 0xDE */	"0xDE",
			/* 0xDF */	"0xDF",
			/* 0xE0 */	"Left Control",
			/* 0xE1 */	"Left Shift",

			#if C4MACOS

				/* 0xE2 */	"Left Option",
				/* 0xE3 */	"Left Command",

			#else

				/* 0xE2 */	"Left Alt",
				/* 0xE3 */	"Left Menu",

			#endif

			/* 0xE4 */	"Right Control",
			/* 0xE5 */	"Right Shift",

			#if C4MACOS

				/* 0xE6 */	"Right Option",
				/* 0xE7 */	"Right Command"

			#else

				/* 0xE6 */	"Right Alt",
				/* 0xE7 */	"Right Menu"

			#endif
		};

	#endif


	#if C4WINDOWS

		const unsigned_int8 keyButtonUsage[0xE0] =
		{
			0x00, 0x29, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2D, 0x2E, 0x2A, 0x2B,
			0x14, 0x1A, 0x08, 0x15, 0x17, 0x1C, 0x18, 0x0C, 0x12, 0x13, 0x2F, 0x30, 0x28, 0xE0, 0x04, 0x16,
			0x07, 0x09, 0x0A, 0x0B, 0x0D, 0x0E, 0x0F, 0x33, 0x34, 0x35, 0xE1, 0x31, 0x1D, 0x1B, 0x06, 0x19,
			0x05, 0x11, 0x10, 0x36, 0x37, 0x38, 0xE5, 0x55, 0xE2, 0x2C, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E,
			0x3F, 0x40, 0x41, 0x42, 0x43, 0x53, 0x47, 0x5F, 0x60, 0x61, 0x56, 0x5C, 0x5D, 0x5E, 0x57, 0x59,
			0x5A, 0x5B, 0x62, 0x63, 0x00, 0x00, 0x64, 0x44, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x68, 0x69, 0x6A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0xE4, 0x00, 0x00,
			0x7F, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00,
			0x80, 0x00, 0x00, 0x85, 0x00, 0x54, 0x00, 0x9A, 0xE6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x00, 0x4A, 0x52, 0x4B, 0x00, 0x50, 0x00, 0x4F, 0x00, 0x4D,
			0x51, 0x4E, 0x49, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE3, 0xE7, 0x65, 0x00, 0x00
		};

	#elif C4MACOS

		const char *const linearAxisName[3] =
		{
			"X Axis", "Y Axis", "Z Axis"
		};

		const char *const rotationAxisName[3] =
		{
			"X Rotation", "Y Rotation", "Z Rotation"
		};

		const char *const deltaAxisName[3] =
		{
			"X Delta", "Y Delta", "Z Delta"
		};

		const char *const directionalName[4] =
		{
			"Up", "Down", "Right", "Left"
		};

	#elif C4LINUX

		const char *const keySymbolName[256] =
		{
			"", "", "", "", "", "", "", "", "Backspace", "Tab", "", "Clear", "", "Enter", "", "",
			"", "", "", "Pause", "Scroll Lock", "Sys Req", "", "", "", "", "", "Escape", "", "", "", "",
			"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
			"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
			"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
			"Home", "Left Arrow", "Up Arrow", "Right Arrow", "Down Arrow", "Page Up", "Page Down", "End", "Begin", "", "", "", "", "", "", "",
			"Select", "Print", "Exec", "Insert", "", "Undo", "Redo", "Menu", "Find", "Cancel", "Help", "Break", "", "", "", "",
			"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
			"Pad Space", "", "", "", "", "", "", "", "", "Pad Tab", "", "", "", "Pad Enter", "", "",
			"", "Pad F1", "Pad F2", "Pad F3", "Pad F4", "Pad 7", "Pad 4", "Pad 8", "Pad 6", "Pad 2", "Pad 9", "Pad 3", "Pad 1", "Pad 5", "Pad 0", "Pad .",
			"", "", "", "", "", "", "", "", "", "", "Pad *", "Pad +", "Thousands Sep", "Pad -", "Decimal Sep", "Pad /",
			"Pad 0", "Pad 1", "Pad 2", "Pad 3", "Pad 4", "Pad 5", "Pad 6", "Pad 7", "Pad 8", "Pad 9", "", "", "", "Pad =", "F1", "F2",
			"F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16", "F17", "F18",
			"F19", "F20", "F21", "F22", "F23", "F24", "F25", "F26", "F27", "F28", "F29", "F30", "F31", "F32", "F33", "F34",
			"F35", "Left Shift", "Right Shift", "Left Control", "Right Control", "Caps Lock", "Shift Lock", "Left Meta", "Right Meta", "Left Alt", "Right Alt", "Left Super", "Right Super", "Left Hyper", "Right Hyper", "",
			"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "Delete"
		};

		const unsigned_int32 keySymbolUsage[256] =
		{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x2B, 0x00, 0x9C, 0x00, 0x28, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x48, 0x47, 0x9A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x4A, 0x50, 0x52, 0x4F, 0x51, 0x4B, 0x4E, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x77, 0x46, 0x74, 0x49, 0x00, 0x7A, 0x00, 0x76, 0x7E, 0x9B, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0xCD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBA, 0x00, 0x00, 0x00, 0x58, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x5C, 0x60, 0x5E, 0x5A, 0x61, 0x5B, 0x59, 0x5D, 0x62, 0x63,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x57, 0xB2, 0x56, 0xB3, 0x54,
			0x62, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x00, 0x00, 0x00, 0x67, 0x3A, 0x3B,
			0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
			0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0xE1, 0xE5, 0xE0, 0xE4, 0x39, 0x00, 0xE3, 0xE7, 0xE2, 0xE6, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C
		};

		const unsigned_int32 keyAsciiUsage[95] =
		{
			0x2C, 0x1E, 0x34, 0x20, 0x21, 0x22, 0x24, 0x1E, 0x26, 0x27, 0x25, 0x2E, 0x36, 0x2D, 0x37, 0x38,
			0x27, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x33, 0x33, 0x36, 0x2E, 0x37, 0x38,
			0x1F, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
			0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x2F, 0x31, 0x30, 0x23, 0x2D,
			0x35, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
			0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x2F, 0x31, 0x30, 0x35
		};

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]


	#if C4XINPUT

		const char *const xboxButtonName[kXboxButtonCount] =
		{
			"Up", "Down", "Left", "Right", "Start", "Back", "Left Stick", "Right Stick", "LB", "RB", "A", "B", "X", "Y"
		};

		const unsigned_int32 xboxButtonUsage[kXboxButtonCount] =
		{
			0x00010090, 0x00010091, 0x00010093, 0x00010092, 0x00090008, 0x00090007, 0x00090009, 0x0009000A, 0x00090005, 0x00090006, 0x00090001, 0x00090002, 0x00090003, 0x00090004
		};

		const char *const xboxTriggerName[kXboxTriggerCount] =
		{
			"LT", "RT"
		};

		const unsigned_int32 xboxTriggerUsage[kXboxTriggerCount] =
		{
			0x00010033, 0x00010034
		};

		const char *const xboxAxisName[kXboxAxisCount] =
		{
			"Left Stick X", "Left Stick Y", "Right Stick X", "Right Stick Y"
		};

		const unsigned_int32 xboxAxisUsage[kXboxAxisCount] =
		{
			0x00010030, 0x00010031, 0x00010032, 0x00010035
		};

	#endif
}


InputMgr *C4::TheInputMgr = nullptr;


namespace C4
{
	template <> InputMgr Manager<InputMgr>::managerObject(0);
	template <> InputMgr **Manager<InputMgr>::managerPointer = &TheInputMgr;

	template <> const char *const Manager<InputMgr>::resultString[] =
	{
		nullptr,
		"InputMgr initialization failed"
	};

	template <> const unsigned_int32 Manager<InputMgr>::resultIdentifier[] =
	{
		0, 'INIT'
	};

	template class Manager<InputMgr>;

	template <> Heap EngineMemory<InputDevice>::heap("InputDevice", 8192, kHeapMutexless);
	template class EngineMemory<InputDevice>;
}


InputMode InputMgr::inputMode = kInputInactive;
InputMode InputMgr::internalInputMode = kInputInactive;


#if C4LINUX

	js_event JoystickDevice::eventData[kInputQueueSize];

#endif


Action::Action(ActionType type)
{
	actionType = type;
	actionFlags = 0;
	activeCount = 0;
}

Action::~Action()
{
}

void Action::Begin(void)
{
}

void Action::End(void)
{
}

void Action::Move(int32 value)
{
}

void Action::Update(float value)
{
}


ConsoleAction::ConsoleAction() : Action(kActionConsole)
{
	SetActionFlags(kActionImmutable | kActionPersistent);
}

ConsoleAction::~ConsoleAction()
{
}

void ConsoleAction::Begin(void)
{
	InputMgr::KeyProc *proc = TheInputMgr->GetConsoleProc();
	if (proc)
	{
		(*proc)(TheInputMgr->GetConsoleCookie());
	}
}


EscapeAction::EscapeAction() : Action(kActionEscape)
{
	SetActionFlags(kActionImmutable | kActionPersistent);
}

EscapeAction::~EscapeAction()
{
}

void EscapeAction::Begin(void)
{
	InputMgr::KeyProc *proc = TheInputMgr->GetEscapeProc();
	if (proc)
	{
		(*proc)(TheInputMgr->GetEscapeCookie());
	}
}


MouseAction::MouseAction(ActionType type) : Action(type)
{
	SetActionFlags(kActionImmutable | kActionPersistent);
}

MouseAction::~MouseAction()
{
}

void MouseAction::Update(float value)
{
	if (GetActionType() == kActionMouseX)
	{
		TheInputMgr->mouseDeltaX -= value;
	}
	else
	{
		TheInputMgr->mouseDeltaY += value;
	}
}


CommandAction::CommandAction(const char *cmd) : Action(0)
{
	command = cmd;
	SetActionFlags(kActionPersistent);
}

CommandAction::~CommandAction()
{
}

void CommandAction::Begin(void)
{
	TheEngine->ExecuteText(command);
}


#if C4WINDOWS

	InputControl::InputControl(InputDevice *device, InputControlType type, unsigned_int32 usage, const DIDEVICEOBJECTINSTANCEW *instance)
	{
		owningDevice = device;
		controlType = type;
		controlHidUsage = (usage != 0) ? usage : (instance->wUsagePage << 16) | instance->wUsage;
		Text::CopyText(instance->tszName, controlName, kMaxInputControlNameLength);

		dataGuid = instance->guidType;
		dataType = instance->dwType;
		dataFlags = instance->dwFlags;
	}

#endif

InputControl::InputControl(InputDevice *device, InputControlType type, unsigned_int32 usage, const char *name)
{
	owningDevice = device;
	controlType = type;
	controlHidUsage = usage;
	controlName = name;

	#if C4MACOS

		controlCookie = 0;
		controlActive = false;

	#endif
}

InputControl::InputControl(InputDevice *device)
{
	owningDevice = device;
	controlType = kInputControlGroup;
	controlHidUsage = 0;
	controlName[0] = 0;

	#if C4MACOS

		controlCookie = 0;
		controlActive = false;

	#endif
}

InputControl::~InputControl()
{
}

void InputControl::SetControlAction(Action *action)
{
	Action *prevAction = controlAction;
	if ((prevAction) && (prevAction->GetActionType() == 0))
	{
		delete prevAction;
	}

	controlAction = action;
}

#if C4MACOS

	void InputControl::Activate(IOHIDQueueInterface **deviceQueue)
	{
		if ((!controlActive) && (controlType != kInputControlGroup))
		{
			controlActive = true;

			(**deviceQueue).addElement(deviceQueue, controlCookie, 0);
		}
	}

	void InputControl::Deactivate(IOHIDQueueInterface **deviceQueue)
	{
		if (controlActive)
		{
			controlActive = false;

			(**deviceQueue).removeElement(deviceQueue, controlCookie);
		}
	}

#endif

void InputControl::HandleNormalEvent(int32 value)
{
}

bool InputControl::HandleConfigEvent(int32 value)
{
	return (false);
}


#if C4WINDOWS

	ButtonControl::ButtonControl(InputDevice *device, InputControlType type, unsigned_int32 usage, const DIDEVICEOBJECTINSTANCEW *instance) : InputControl(device, type, usage, instance)
	{
		dataMask = 0x80;
	}

#endif

ButtonControl::ButtonControl(InputDevice *device, InputControlType type, unsigned_int32 usage, const char *name) : InputControl(device, type, usage, name)
{
	dataMask = 0xFFFFFFFF;
}

ButtonControl::~ButtonControl()
{
}

void ButtonControl::HandleNormalEvent(int32 value)
{
	Action *action = GetControlAction();
	if (action)
	{
		if ((value & dataMask) != 0)
		{
			SetActiveAction(action);
			int32 count = action->GetActiveCount();
			action->SetActiveCount(count + 1);

			if (count == 0)
			{
				action->Begin();
			}
		}
		else
		{
			SetActiveAction(nullptr);
			int32 count = action->GetActiveCount();
			action->SetActiveCount(MaxZero(count - 1));

			if (count == 1)
			{
				action->End();
			}
		}
	}
}

bool ButtonControl::HandleConfigEvent(int32 value)
{
	if ((value & dataMask) != 0)
	{
		TheInputMgr->CallConfigProc(this, 1.0F);
		return (true);
	}

	return (false);
}


#if C4WINDOWS

	KeyButtonControl::KeyButtonControl(InputDevice *device, unsigned_int32 usage, const DIDEVICEOBJECTINSTANCEW *instance) : ButtonControl(device, kInputControlKeyButton, usage, instance)
	{
	}

#endif

KeyButtonControl::KeyButtonControl(InputDevice *device, unsigned_int32 usage, const char *name) : ButtonControl(device, kInputControlKeyButton, usage, name)
{
}

KeyButtonControl::~KeyButtonControl()
{
}


#if C4WINDOWS

	GenericButtonControl::GenericButtonControl(InputDevice *device, const DIDEVICEOBJECTINSTANCEW *instance) : ButtonControl(device, kInputControlGenericButton, 0, instance)
	{
	}

#endif

GenericButtonControl::GenericButtonControl(InputDevice *device, unsigned_int32 usage, const char *name) : ButtonControl(device, kInputControlGenericButton, usage, name)
{
}

GenericButtonControl::~GenericButtonControl()
{
}


#if C4IOS //[ MOBILE

	// -- Mobile code hidden --

#endif //]


#if C4WINDOWS

	AxisControl::AxisControl(InputDevice *device, InputControlType type, InputAxis axis, const DIDEVICEOBJECTINSTANCEW *instance) : InputControl(device, type, 0, instance)
	{
		controlAxis = axis;
	}

#endif

AxisControl::AxisControl(InputDevice *device, InputControlType type, InputAxis axis, unsigned_int32 usage, const char *name) : InputControl(device, type, usage, name)
{
	controlAxis = axis;
}

AxisControl::~AxisControl()
{
}

void AxisControl::SetRange(float vmin, float vmax, float dead)
{
	float center = (vmin + vmax) * 0.5F;
	centerValue = center;

	minValue = vmin - center;
	maxValue = vmax - center;
	deadZone = dead;
	normalizer = 1.0F / (maxValue - dead);
}

void AxisControl::HandleNormalEvent(int32 value)
{
	Action *action = GetControlAction();
	if (action)
	{
		float v = float(value) - centerValue;
		if (Fabs(v) <= deadZone)
		{
			action->Update(0.0F);
		}
		else if (v > 0.0F)
		{
			action->Update((Fmin(v, maxValue) - deadZone) * normalizer);
		}
		else
		{
			action->Update((Fmax(v, minValue) + deadZone) * normalizer);
		}
	}
}

bool AxisControl::HandleConfigEvent(int32 value)
{
	float v = float(value) - centerValue;
	if (Fabs(v) > deadZone)
	{
		if (v > 0.0F)
		{
			v = (Fmin(v, maxValue) - deadZone) * normalizer;
		}
		else
		{
			v = (Fmax(v, minValue) + deadZone) * normalizer;
		}

		TheInputMgr->CallConfigProc(this, v);
		return (true);
	}

	return (false);
}


#if C4WINDOWS

	LinearAxisControl::LinearAxisControl(InputDevice *device, InputAxis axis, const DIDEVICEOBJECTINSTANCEW *instance) : AxisControl(device, kInputControlLinearAxis, axis, instance)
	{
	}

#endif

LinearAxisControl::LinearAxisControl(InputDevice *device, InputAxis axis, unsigned_int32 usage, const char *name) : AxisControl(device, kInputControlLinearAxis, axis, usage, name)
{
}

LinearAxisControl::~LinearAxisControl()
{
}


#if C4WINDOWS

	RotationAxisControl::RotationAxisControl(InputDevice *device, InputAxis axis, const DIDEVICEOBJECTINSTANCEW *instance) : AxisControl(device, kInputControlRotationAxis, axis, instance)
	{
	}

#endif

RotationAxisControl::RotationAxisControl(InputDevice *device, InputAxis axis, unsigned_int32 usage, const char *name) : AxisControl(device, kInputControlRotationAxis, axis, usage, name)
{
}

RotationAxisControl::~RotationAxisControl()
{
}


#if C4WINDOWS

	DeltaAxisControl::DeltaAxisControl(InputDevice *device, InputAxis axis, const DIDEVICEOBJECTINSTANCEW *instance) : AxisControl(device, kInputControlDeltaAxis, axis, instance)
	{
	}

#endif

DeltaAxisControl::DeltaAxisControl(InputDevice *device, InputAxis axis, unsigned_int32 usage, const char *name) : AxisControl(device, kInputControlDeltaAxis, axis, usage, name)
{
}

DeltaAxisControl::~DeltaAxisControl()
{
}

void DeltaAxisControl::HandleNormalEvent(int32 value)
{
	Action *action = GetControlAction();
	if (action)
	{
		if (value != 0)
		{
			action->Update(float(value));
		}
	}
}

bool DeltaAxisControl::HandleConfigEvent(int32 value)
{
	if ((GetOwningDevice()->GetDeviceType() != kInputDeviceMouse) || (GetControlAxis() == kInputAxisZ))
	{
		if (value != 0)
		{
			TheInputMgr->CallConfigProc(this, float(value));
			return (true);
		}
	}

	return (false);
}


#if C4WINDOWS

	SliderControl::SliderControl(InputDevice *device, const DIDEVICEOBJECTINSTANCEW *instance) : InputControl(device, kInputControlSlider, 0, instance)
	{
	}

#endif

SliderControl::SliderControl(InputDevice *device, unsigned_int32 usage, const char *name) : InputControl(device, kInputControlSlider, usage, name)
{
}

SliderControl::~SliderControl()
{
}

void SliderControl::SetRange(float vmin, float vmax, float thresh)
{
	maxValue = vmax;
	threshold = vmin + thresh;
	normalizer = 1.0F / (vmax - threshold);
}

void SliderControl::HandleNormalEvent(int32 value)
{
	Action *action = GetControlAction();
	if (action)
	{
		float v = float(value);
		if (v <= threshold)
		{
			action->Update(0.0F);
		}
		else
		{
			action->Update((Fmin(v, maxValue) - threshold) * normalizer);
		}
	}
}

bool SliderControl::HandleConfigEvent(int32 value)
{
	float v = float(value);
	if (v > threshold)
	{
		TheInputMgr->CallConfigProc(this, (Fmin(v, maxValue) - threshold) * normalizer);
		return (true);
	}

	return (false);
}


#if C4WINDOWS

	DirectionalControl::DirectionalControl(InputDevice *device, const DIDEVICEOBJECTINSTANCEW *instance) : InputControl(device, kInputControlDirectional, 0, instance)
	{
		divider = 4500;
	}

#endif

DirectionalControl::DirectionalControl(InputDevice *device, unsigned_int32 usage, const char *name) : InputControl(device, kInputControlDirectional, usage, name)
{
	divider = 1;
}

DirectionalControl::~DirectionalControl()
{
}

void DirectionalControl::HandleNormalEvent(int32 value)
{
	Action *action = GetControlAction();
	if (action)
	{
		value /= divider;
		if (unsigned_int32(value) < 8U)
		{
			action->Move(value);
		}
		else
		{
			action->Move(-1);
		}
	}
}

bool DirectionalControl::HandleConfigEvent(int32 value)
{
	if ((value & 0xFFFF) != 0xFFFF)
	{
		TheInputMgr->CallConfigProc(this, float(value / divider));
		return (true);
	}

	return (false);
}


#if C4WINDOWS

	InputFeedback::InputFeedback(const DIEFFECTINFOW *effect)
	{
		Text::CopyText(effect->tszName, feedbackName, kMaxInputFeedbackNameLength);
	}

#else

	InputFeedback::InputFeedback(const char *name)
	{
		feedbackName = name;
	}

#endif

InputFeedback::~InputFeedback()
{
}


#if C4WINDOWS

	InputDevice::InputDevice(InputDeviceType type, unsigned_int32 usage, const DIDEVICEINSTANCEW *instance) : controlTree(this)
	{
		deviceType = type;
		deviceHidUsage = (usage != 0) ? usage : (instance->wUsagePage << 16) | instance->wUsage;
		deviceActive = false;

		Text::CopyText(instance->tszProductName, deviceName, kMaxInputDeviceNameLength);
	}

	InputDevice::InputDevice(InputDeviceType type, unsigned_int32 usage, const char *name) : controlTree(this)
	{
		deviceType = type;
		deviceHidUsage = usage;
		deviceActive = false;
		deviceName = name;
	}

	InputDevice::~InputDevice()
	{
	}

#elif C4MACOS

	InputDevice::InputDevice(InputDeviceType type, unsigned_int32 usage, io_object_t object, CFMutableDictionaryRef properties) : controlTree(this)
	{
		deviceType = type;
		deviceHidUsage = usage;
		deviceActive = false;

		pluginInterface = nullptr;
		deviceInterface = nullptr;
		deviceQueue = nullptr;

		const void *productValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDProductKey));
		if (productValue)
		{
			SInt32		score;

			CFStringGetCString((CFStringRef) productValue, deviceName, kMaxInputDeviceNameLength + 1, kCFStringEncodingUTF8);

			if (IOCreatePlugInInterfaceForService(object, kIOHIDDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &pluginInterface, &score) == kIOReturnSuccess)
			{
				if ((**pluginInterface).QueryInterface(pluginInterface, CFUUIDGetUUIDBytes(kIOHIDDeviceInterfaceID), (void **) &deviceInterface) == S_OK)
				{
					(**deviceInterface).open(deviceInterface, 0);
					deviceQueue = (**deviceInterface).allocQueue(deviceInterface);
					if ((deviceQueue) && ((**deviceQueue).create(deviceQueue, 0, kInputQueueSize) == kIOReturnSuccess))
					{
						BuildControlTree(&controlTree, properties);
					}
				}
			}
		}
	}

	InputDevice::~InputDevice()
	{
		if (deviceQueue)
		{
			(**deviceQueue).dispose(deviceQueue);
			(**deviceQueue).Release(deviceQueue);
		}

		if (deviceInterface)
		{
			(**deviceInterface).close(deviceInterface);
			(**deviceInterface).Release(deviceInterface);
		}

		if (pluginInterface)
		{
			IODestroyPlugInInterface(pluginInterface);
		}
	}

	void InputDevice::BuildControlTree(InputControl *root, CFMutableDictionaryRef dictionary)
	{
		const void *elementValue = CFDictionaryGetValue(dictionary, CFSTR(kIOHIDElementKey));
		if ((elementValue) && (CFGetTypeID(elementValue) == CFArrayGetTypeID()))
		{
			CFArrayRef elementArray = (CFArrayRef) elementValue;

			CFIndex count = CFArrayGetCount(elementArray);
			for (CFIndex index = 0; index < count; index++)
			{
				const void *value = CFArrayGetValueAtIndex(elementArray, index);
				if ((value) && (CFGetTypeID(value) == CFDictionaryGetTypeID()))
				{
					CFMutableDictionaryRef properties = (CFMutableDictionaryRef) value;

					const void *elementTypeValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDElementTypeKey));
					if (elementTypeValue)
					{
						int32	elementType;

						CFNumberGetValue((CFNumberRef) elementTypeValue, kCFNumberSInt32Type, &elementType);
						switch (elementType)
						{
							case kIOHIDElementTypeCollection:
							{
								InputControl *group = new InputControl(this);
								root->AppendSubnode(group);

								BuildControlTree(group, properties);
								break;
							}

							case kIOHIDElementTypeInput_Misc:
							case kIOHIDElementTypeInput_Button:
							case kIOHIDElementTypeInput_Axis:
							case kIOHIDElementTypeInput_ScanCodes:
							{
								const void *cookieValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDElementCookieKey));
								if (cookieValue)
								{
									int32	cookie;

									CFNumberGetValue((CFNumberRef) cookieValue, kCFNumberSInt32Type, &cookie);

									const void *usagePageValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDElementUsagePageKey));
									const void *usageValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDElementUsageKey));
									if ((usagePageValue) && (usageValue))
									{
										int32	usagePage;
										int32	usage;

										CFNumberGetValue((CFNumberRef) usagePageValue, kCFNumberSInt32Type, &usagePage);
										CFNumberGetValue((CFNumberRef) usageValue, kCFNumberSInt32Type, &usage);

										InputControl *control = nullptr;
										if (usagePage == kHIDPage_GenericDesktop)
										{
											switch (usage)
											{
												case kHIDUsage_GD_X:
												case kHIDUsage_GD_Y:
												case kHIDUsage_GD_Z:
												{
													int32 relative = false;
													const void *relativeValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDElementIsRelativeKey));
													if (relativeValue)
													{
														relative = CFBooleanGetValue((CFBooleanRef) relativeValue);
													}

													InputAxis axis = static_cast<InputAxis>(usage - kHIDUsage_GD_X);

													if ((deviceType == kInputDeviceMouse) || (relative))
													{
														control = new DeltaAxisControl(this, axis, 0x00010000 | usage, deltaAxisName[axis]);

														if (usage == kHIDUsage_GD_X)
														{
															control->SetControlAction(TheInputMgr->FindAction(kActionMouseX));
														}
														else if (usage == kHIDUsage_GD_Y)
														{
															control->SetControlAction(TheInputMgr->FindAction(kActionMouseY));
														}
													}
													else
													{
														control = new LinearAxisControl(this, axis, 0x00010000 | usage, linearAxisName[axis]);
													}

													break;
												}

												case kHIDUsage_GD_Wheel:

													control = new DeltaAxisControl(this, kInputAxisZ, 0x00010038, deltaAxisName[kInputAxisZ]);
													break;

												case kHIDUsage_GD_Rx:
												case kHIDUsage_GD_Ry:
												case kHIDUsage_GD_Rz:
												{
													InputAxis axis = static_cast<InputAxis>(usage - kHIDUsage_GD_Rx);
													control = new RotationAxisControl(this, axis, 0x00010000 | usage, rotationAxisName[axis]);
													break;
												}

												case kHIDUsage_GD_Slider:

													control = new SliderControl(this, 0x010036, "Slider");
													break;

												case kHIDUsage_GD_Hatswitch:

													control = new DirectionalControl(this, 0x00010039, "Hat Switch");
													break;

												case kHIDUsage_GD_DPadUp:
												case kHIDUsage_GD_DPadDown:
												case kHIDUsage_GD_DPadRight:
												case kHIDUsage_GD_DPadLeft:

													control = new GenericButtonControl(this, 0x00010000 | usage, directionalName[usage - kHIDUsage_GD_DPadUp]);
													break;
											}
										}
										else if (usagePage == kHIDPage_KeyboardOrKeypad)
										{
											if (unsigned_int32(usage - 4) < unsigned_int32(kKeyboardUsageCount - 4))
											{
												if (unsigned_int32(usage - 0xE0) < 8U)
												{
													const void *arrayValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDElementIsArrayKey));
													if ((arrayValue) && (CFBooleanGetValue((CFBooleanRef) arrayValue)))
													{
														break;
													}
												}

												control = new KeyButtonControl(this, 0x00070000 | usage, keyButtonName[usage]);

												if (usage == kHIDUsage_KeyboardGraveAccentAndTilde)
												{
													control->SetControlAction(TheInputMgr->FindAction(kActionConsole));
												}
												else if (usage == kHIDUsage_KeyboardEscape)
												{
													control->SetControlAction(TheInputMgr->FindAction(kActionEscape));
												}
											}
										}
										else if (usagePage == kHIDPage_Button)
										{
											control = new GenericButtonControl(this, 0x00090000 | usage, InputControlName("Button ") += usage);
										}

										if (control)
										{
											control->controlHidUsage = (usagePage << 16) | usage;
											control->controlCookie = (IOHIDElementCookie) cookie;

											InputControlType type = control->GetControlType();
											if ((type == kInputControlLinearAxis) || (type == kInputControlRotationAxis))
											{
												int32 imin = 0x80000000;
												int32 imax = 0x7FFFFFFF;

												const void *minValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDElementMinKey));
												if (minValue)
												{
													CFNumberGetValue((CFNumberRef) minValue, kCFNumberSInt32Type, &imin);
												}

												const void *maxValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDElementMaxKey));
												if (maxValue)
												{
													CFNumberGetValue((CFNumberRef) maxValue, kCFNumberSInt32Type, &imax);
												}

												float vmin = float(imin);
												float vmax = float(imax);
												static_cast<AxisControl *>(control)->SetRange(vmin, vmax, (vmax - vmin) * 0.25F);
											}
											else if (type == kInputControlSlider)
											{
												int32 imin = 0;
												int32 imax = 0x7FFFFFFF;

												const void *minValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDElementMinKey));
												if (minValue)
												{
													CFNumberGetValue((CFNumberRef) minValue, kCFNumberSInt32Type, &imin);
												}

												const void *maxValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDElementMaxKey));
												if (maxValue)
												{
													CFNumberGetValue((CFNumberRef) maxValue, kCFNumberSInt32Type, &imax);
												}

												float vmin = float(imin);
												float vmax = float(imax);
												static_cast<SliderControl *>(control)->SetRange(vmin, vmax, (vmax - vmin) * 0.0625F);
											}

											root->AppendSubnode(control);
										}
									}
								}

								break;
							}
						}
					}
				}
			}
		}
	}

#elif C4LINUX

	InputDevice::InputDevice(InputDeviceType type, unsigned_int32 usage, const char *name) : controlTree(this)
	{
		deviceType = type;
		deviceHidUsage = usage;
		deviceName = name;
		deviceActive = false;
	}

	InputDevice::~InputDevice()
	{
	}

#elif C4IOS //[ MOBILE

	// -- Mobile code hidden --

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

InputControl *InputDevice::FindControl(unsigned_int32 usage) const
{
	InputControl *control = GetFirstControl();
	while (control)
	{
		if (control->GetControlHidUsage() == usage)
		{
			break;
		}

		control = GetNextControl(control);
	}

	return (control);
}

InputControl *InputDevice::FindControl(const char *name) const
{
	InputControl *control = GetFirstControl();
	while (control)
	{
		if (Text::CompareTextCaseless(control->GetControlName(), name))
		{
			break;
		}

		control = GetNextControl(control);
	}

	return (control);
}

void InputDevice::ResetActions(void) const
{
	InputControl *control = GetFirstControl();
	while (control)
	{
		Action *action = control->GetControlAction();
		if ((action) && (action->GetActiveCount() > 0))
		{
			action->SetActiveCount(0);
			action->End();
		}

		control = GetNextControl(control);
	}
}

void InputDevice::Activate(void)
{
	deviceActive = true;

	#if C4MACOS

		InputControl *control = GetFirstControl();
		while (control)
		{
			control->Activate(deviceQueue);
			control = GetNextControl(control);
		}

		(**deviceQueue).start(deviceQueue);

	#endif
}

void InputDevice::Deactivate(void)
{
	deviceActive = false;

	#if C4MACOS

		(**deviceQueue).stop(deviceQueue);

	#endif

	InputControl *control = GetFirstControl();
	while (control)
	{
		Action *action = control->activeAction;
		if (action)
		{
			control->activeAction = nullptr;
			int32 count = action->GetActiveCount();
			action->SetActiveCount(MaxZero(count - 1));

			if (count == 1)
			{
				action->End();
			}
		}

		#if C4MACOS

			control->Deactivate(deviceQueue);

		#endif

		control = GetNextControl(control);
	}
}

void InputDevice::SetInputMode(InputMode mode)
{
	if (mode & kInputGameActive)
	{
		if (!DeviceActive())
		{
			Activate();
		}
	}
	else
	{
		if (DeviceActive())
		{
			Deactivate();
		}
	}
}

bool InputDevice::ProcessEvents(InputMode mode)
{
	#if C4MACOS

		static const AbsoluteTime zero = {0, 0};

		if (mode != kInputConfiguration)
		{
			for (;;)
			{
				IOHIDEventStruct	event;

				if ((**deviceQueue).getNextEvent(deviceQueue, &event, zero, 0) != kIOReturnSuccess)
				{
					break;
				}

				IOHIDElementCookie cookie = event.elementCookie;

				InputControl *control = GetFirstControl();
				while (control)
				{
					if (control->controlCookie == cookie)
					{
						control->HandleNormalEvent(event.value);
						break;
					}

					control = GetNextControl(control);
				}
			}
		}
		else
		{
			IOHIDEventStruct	event;

			if ((**deviceQueue).getNextEvent(deviceQueue, &event, zero, 0) == kIOReturnSuccess)
			{
				IOHIDElementCookie cookie = event.elementCookie;

				InputControl *control = GetFirstControl();
				while (control)
				{
					if (control->controlCookie == cookie)
					{
						if (control->HandleConfigEvent(event.value))
						{
							return (true);
						}

						break;
					}

					control = GetNextControl(control);
				}
			}
		}

	#endif

	return (false);
}


#if C4WINDOWS

	DirectInputDevice::DirectInputDevice(InputDeviceType type, unsigned_int32 usage, IDirectInput8W *directInput, const DIDEVICEINSTANCEW *instance) : InputDevice(type, usage, instance)
	{
		directInput->CreateDevice(instance->guidInstance, &deviceInstance, nullptr);

		controlCount = 0;
		buttonCount = 0;

		controlTableSize = 0;
		controlTable = nullptr;
	}

	DirectInputDevice::~DirectInputDevice()
	{
		delete[] controlTable;

		deviceInstance->Release();
	}

	BOOL CALLBACK DirectInputDevice::EnumObjectsCallback(const DIDEVICEOBJECTINSTANCEW *instance, void *cookie)
	{
		DirectInputDevice *inputDevice = static_cast<DirectInputDevice *>(cookie);
		InputControl *controlTree = &inputDevice->controlTree;

		InputControl *control = nullptr;
		DWORD objectType = DIDFT_GETTYPE(instance->dwType);

		bool buttonFlag = false;

		if (IsEqualGUID(instance->guidType, GUID_XAxis))
		{
			if ((objectType & DIDFT_RELAXIS) != 0)
			{
				control = new DeltaAxisControl(inputDevice, kInputAxisX, instance);
				if (inputDevice->GetDeviceType() == kInputDeviceMouse)
				{
					control->controlHidUsage = 0x00010030;
					control->SetControlAction(TheInputMgr->FindAction(kActionMouseX));
				}
			}
			else
			{
				control = new LinearAxisControl(inputDevice, kInputAxisX, instance);
			}
		}
		else if (IsEqualGUID(instance->guidType, GUID_YAxis))
		{
			if ((objectType & DIDFT_RELAXIS) != 0)
			{
				control = new DeltaAxisControl(inputDevice, kInputAxisY, instance);
				if (inputDevice->GetDeviceType() == kInputDeviceMouse)
				{
					control->controlHidUsage = 0x00010031;
					control->SetControlAction(TheInputMgr->FindAction(kActionMouseY));
				}
			}
			else
			{
				control = new LinearAxisControl(inputDevice, kInputAxisY, instance);
			}
		}
		else if (IsEqualGUID(instance->guidType, GUID_ZAxis))
		{
			if ((objectType & DIDFT_RELAXIS) != 0)
			{
				control = new DeltaAxisControl(inputDevice, kInputAxisZ, instance);
				if (inputDevice->GetDeviceType() == kInputDeviceMouse)
				{
					control->controlHidUsage = 0x00010032;
				}
			}
			else
			{
				control = new LinearAxisControl(inputDevice, kInputAxisZ, instance);
			}
		}
		else if (IsEqualGUID(instance->guidType, GUID_RxAxis))
		{
			control = new RotationAxisControl(inputDevice, kInputAxisX, instance);
		}
		else if (IsEqualGUID(instance->guidType, GUID_RyAxis))
		{
			control = new RotationAxisControl(inputDevice, kInputAxisY, instance);
		}
		else if (IsEqualGUID(instance->guidType, GUID_RzAxis))
		{
			control = new RotationAxisControl(inputDevice, kInputAxisZ, instance);
		}
		else if (IsEqualGUID(instance->guidType, GUID_Slider))
		{
			control = new SliderControl(inputDevice, instance);
		}
		else if (IsEqualGUID(instance->guidType, GUID_Button))
		{
			control = new GenericButtonControl(inputDevice, instance);
			if (inputDevice->GetDeviceType() == kInputDeviceMouse)
			{
				control->controlHidUsage = 0x00090001 + inputDevice->buttonCount;
			}

			buttonFlag = true;
		}
		else if (IsEqualGUID(instance->guidType, GUID_Key))
		{
			unsigned_int32 offset = instance->dwOfs;
			control = new KeyButtonControl(inputDevice, 0x00070000 | keyButtonUsage[offset], instance);

			if ((offset == DIK_GRAVE) || (control->GetControlName()[0] == '`'))
			{
				control->SetControlAction(TheInputMgr->FindAction(kActionConsole));
			}
			else if (offset == DIK_ESCAPE)
			{
				control->SetControlAction(TheInputMgr->FindAction(kActionEscape));
			}

			buttonFlag = true;
		}
		else if (IsEqualGUID(instance->guidType, GUID_POV))
		{
			control = new DirectionalControl(inputDevice, instance);
		}

		if (control)
		{
			InputControlType type = control->GetControlType();
			if ((type == kInputControlLinearAxis) || (type == kInputControlRotationAxis))
			{
				DIPROPRANGE		range;
				DIPROPDWORD		dead;

				range.diph.dwSize = sizeof(DIPROPRANGE);
				range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
				range.diph.dwObj = instance->dwType;
				range.diph.dwHow = DIPH_BYID;
				inputDevice->deviceInstance->GetProperty(DIPROP_RANGE, &range.diph);

				dead.diph.dwSize = sizeof(DIPROPDWORD);
				dead.diph.dwHeaderSize = sizeof(DIPROPHEADER);
				dead.diph.dwObj = instance->dwType;
				dead.diph.dwHow = DIPH_BYID;
				HRESULT hr = inputDevice->deviceInstance->GetProperty(DIPROP_DEADZONE, &dead.diph);

				float d = (hr == DI_OK) ? Fmax(float(dead.dwData) * 0.00005F, 0.25F) : 0.25F;

				float vmin = float(range.lMin);
				float vmax = float(range.lMax);
				static_cast<AxisControl *>(control)->SetRange(vmin, vmax, (vmax - vmin) * d);
			}
			else if (type == kInputControlSlider)
			{
				DIPROPRANGE		range;

				range.diph.dwSize = sizeof(DIPROPRANGE);
				range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
				range.diph.dwObj = instance->dwType;
				range.diph.dwHow = DIPH_BYID;
				inputDevice->deviceInstance->GetProperty(DIPROP_RANGE, &range.diph);

				float vmin = float(range.lMin);
				float vmax = float(range.lMax);
				static_cast<SliderControl *>(control)->SetRange(vmin, vmax, (vmax - vmin) * 0.0625F);
			}

			unsigned_int32 size = inputDevice->controlTableSize;
			if (buttonFlag)
			{
				inputDevice->controlTableSize = size + 1;
				inputDevice->buttonCount++;
			}
			else
			{
				inputDevice->controlTableSize = ((size + 3) & ~3) + 4;
			}

			inputDevice->controlCount++;
			controlTree->AppendSubnode(control);
		}

		return (DIENUM_CONTINUE);
	}

	void DirectInputDevice::BuildDataFormat(void)
	{
		DIDATAFORMAT	dataFormat;

		unsigned_int32 tableSize = (controlTableSize + 3) & ~3;

		dataFormat.dwSize = sizeof(DIDATAFORMAT);
		dataFormat.dwObjSize = sizeof(DIOBJECTDATAFORMAT);

		InputDeviceType type = GetDeviceType();
		if (type == kInputDeviceMouse)
		{
			dataFormat.dwFlags = DIDF_RELAXIS;
		}
		else if (type == kInputDeviceJoystick)
		{
			dataFormat.dwFlags = DIDF_ABSAXIS;
		}
		else
		{
			dataFormat.dwFlags = 0;
		}

		dataFormat.dwDataSize = tableSize;
		dataFormat.dwNumObjs = controlCount;

		DIOBJECTDATAFORMAT *objectDataFormat = new DIOBJECTDATAFORMAT[controlCount];
		dataFormat.rgodf = objectDataFormat;

		controlTable = new InputControl *[tableSize];
		MemoryMgr::ClearMemory(controlTable, sizeof(InputControl *) * tableSize);

		int32 count = 0;
		DWORD offset = 0;

		InputControl *control = controlTree.GetFirstSubnode();
		while (control)
		{
			InputControlType type = control->GetControlType();
			bool button = ((type == kInputControlKeyButton) || (type == kInputControlGenericButton));
			if (!button)
			{
				offset = (offset + 3) & ~3;
			}

			controlTable[offset] = control;
			control->dataOffset = offset;

			DIOBJECTDATAFORMAT *format = &objectDataFormat[count];
			format->pguid = &control->dataGuid;
			format->dwOfs = offset;
			format->dwType = control->dataType;
			format->dwFlags = control->dataFlags;

			offset += (button) ? 1 : 4;
			count++;

			control = control->Next();
		}

		deviceInstance->SetDataFormat(&dataFormat);

		delete[] objectDataFormat;
	}

	void DirectInputDevice::Activate(void)
	{
		InputDevice::Activate();

		deviceInstance->Acquire();

		DWORD count = INFINITE;
		deviceInstance->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), nullptr, &count, 0);
	}

	void DirectInputDevice::Deactivate(void)
	{
		deviceInstance->Unacquire();

		InputDevice::Deactivate();
	}

	bool DirectInputDevice::ProcessEvents(InputMode mode)
	{
		static DIDEVICEOBJECTDATA	data[kInputQueueSize];

		if (mode != kInputConfiguration)
		{
			DWORD count = kInputQueueSize;
			HRESULT hr = deviceInstance->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), data, &count, 0);
			if (hr == DI_OK)
			{
				for (unsigned_machine a = 0; a < count; a++)
				{
					InputControl *control = controlTable[data[a].dwOfs];
					if (control)
					{
						control->HandleNormalEvent(data[a].dwData);
					}
				}
			}
			else if ((hr == DIERR_NOTACQUIRED) || (hr == DIERR_INPUTLOST))
			{
				deviceInstance->Acquire();
				ResetActions();
			}
		}
		else
		{
			DWORD count = 1;
			HRESULT hr = deviceInstance->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), data, &count, 0);
			if (((hr == DI_OK) || (hr == DI_BUFFEROVERFLOW)) && (count != 0))
			{
				InputControl *control = controlTable[data[0].dwOfs];
				if (control)
				{
					if (control->HandleConfigEvent(data[0].dwData))
					{
						return (true);
					}
				}
			}
			else if ((hr == DIERR_NOTACQUIRED) || (hr == DIERR_INPUTLOST))
			{
				deviceInstance->Acquire();
			}
		}

		return (false);
	}

#endif


#if C4WINDOWS

	MouseDevice::MouseDevice(IDirectInput8W *directInput, const DIDEVICEINSTANCEW *instance) : StandardInputDevice(kInputDeviceMouse, 0x00010002, directInput, instance)
	{
		DIPROPDWORD		prop;

		deviceInstance->SetCooperativeLevel(TheEngine->GetEngineWindow(), DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		deviceInstance->EnumObjects(&EnumObjectsCallback, static_cast<DirectInputDevice *>(this), DIDFT_BUTTON | DIDFT_RELAXIS);
		BuildDataFormat();

		prop.diph.dwSize = sizeof(DIPROPDWORD);
		prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		prop.diph.dwObj = 0;
		prop.diph.dwHow = DIPH_DEVICE;
		prop.dwData = kInputQueueSize;
		deviceInstance->SetProperty(DIPROP_BUFFERSIZE, &prop.diph);
	}

	MouseDevice::~MouseDevice()
	{
		if (DeviceActive())
		{
			Deactivate();
		}
	}

	void MouseDevice::Activate(void)
	{
		StandardInputDevice::Activate();

		if (GetCapture())
		{
			ReleaseCapture();
		}
	}

#elif C4MACOS

	MouseDevice::MouseDevice(unsigned_int32 usage, io_object_t object, CFMutableDictionaryRef properties) : StandardInputDevice(kInputDeviceMouse, usage, object, properties)
	{
	}

	MouseDevice::~MouseDevice()
	{
		if (DeviceActive())
		{
			Deactivate();
		}
	}

#elif C4LINUX

	MouseDevice::MouseDevice() :
			StandardInputDevice(kInputDeviceMouse, 0x00010002, "Mouse"),
			leftButton(this, 0x00090001, "Left Mouse"),
			middleButton(this, 0x00090003, "Middle Mouse"),
			rightButton(this, 0x00090002, "Right Mouse"),
			horizontalAxis(this, kInputAxisX, 0x00010030, "X Axis"),
			verticalAxis(this, kInputAxisY, 0x00010031, "Y Axis"),
			wheelAxis(this, kInputAxisZ, 0x00010032, "Wheel")
	{
		motionEventFlag = false;

		controlTree.AppendSubnode(&leftButton);
		controlTree.AppendSubnode(&middleButton);
		controlTree.AppendSubnode(&rightButton);
		controlTree.AppendSubnode(&horizontalAxis);
		controlTree.AppendSubnode(&verticalAxis);
		controlTree.AppendSubnode(&wheelAxis);

		horizontalAxis.SetControlAction(TheInputMgr->FindAction(kActionMouseX));
		verticalAxis.SetControlAction(TheInputMgr->FindAction(kActionMouseY));
	}

	MouseDevice::~MouseDevice()
	{
		if (DeviceActive())
		{
			Deactivate();
		}
	}

	void MouseDevice::Activate(void)
	{
		::Window		rootWindow;
		::Window		childWindow;
		int				rootX, rootY;
		int				windowX, windowY;
		unsigned int	mask;

		StandardInputDevice::Activate();

		::Display *display = TheEngine->GetEngineDisplay();
		XChangePointerControl(display, true, true, 1, 1, 0);

		XQueryPointer(display, TheEngine->GetEngineWindow(), &rootWindow, &childWindow, &rootX, &rootY, &windowX, &windowY, &mask);
		originalPosition.Set(windowX, windowY);
		currentPosition.Set(windowX, windowY);
	}

	void MouseDevice::Deactivate(void)
	{
		XChangePointerControl(TheEngine->GetEngineDisplay(), true, true, -1, -1, -1);

		StandardInputDevice::Deactivate();
	}

	bool MouseDevice::ProcessEvents(InputMode mode)
	{
		if (mode != kInputConfiguration)
		{
			int32 x = currentPosition.x;
			int32 y = currentPosition.y;
			int32 dx = x - originalPosition.x;
			int32 dy = y - originalPosition.y;

			if ((dx | dy) != 0)
			{
				if (dx != 0)
				{
					horizontalAxis.HandleNormalEvent(dx);
				}

				if (dy != 0)
				{
					verticalAxis.HandleNormalEvent(dy);
				}

				currentPosition = originalPosition;
				XWarpPointer(TheEngine->GetEngineDisplay(), None, TheEngine->GetEngineWindow(), 0, 0, 0, 0, originalPosition.x, originalPosition.y);
			}
		}

		return (false);
	}

	void MouseDevice::HandleMouseButtonEvent(InputMode mode, const XButtonEvent *event)
	{
		if (mode != kInputConfiguration)
		{
			if (mode & kInputMouseActive)
			{
				if (event->type == ButtonPress)
				{
					switch (event->button)
					{
						case Button1:

							leftButton.HandleNormalEvent(1);
							break;

						case Button2:

							middleButton.HandleNormalEvent(1);
							break;

						case Button3:

							rightButton.HandleNormalEvent(1);
							break;

						case Button4:

							wheelAxis.HandleNormalEvent(1);
							break;

						case Button5:

							wheelAxis.HandleNormalEvent(-1);
							break;
					}
				}
				else
				{
					switch (event->button)
					{
						case Button1:

							leftButton.HandleNormalEvent(0);
							break;

						case Button2:

							middleButton.HandleNormalEvent(0);
							break;

						case Button3:

							rightButton.HandleNormalEvent(0);
							break;
					}
				}
			}
			else
			{
				motionEventFlag = false;
				Integer2D point(event->x, event->y);

				switch (event->button)
				{
					case Button1:

						if (event->type == ButtonPress)
						{
							TheEngine->HandleMouseEvent(kEventMouseDown, point);
						}
						else
						{
							TheEngine->HandleMouseEvent(kEventMouseUp, point);
						}

						break;

					case Button2:

						if (event->type == ButtonPress)
						{
							TheEngine->HandleMouseEvent(kEventMiddleMouseDown, point);
						}
						else
						{
							TheEngine->HandleMouseEvent(kEventMiddleMouseUp, point);
						}

						break;

					case Button3:

						if (event->type == ButtonPress)
						{
							TheEngine->HandleMouseEvent(kEventRightMouseDown, point);
						}
						else
						{
							TheEngine->HandleMouseEvent(kEventRightMouseUp, point);
						}

						break;

					case Button4:

						if (event->type == ButtonPress)
						{
							TheEngine->HandleMouseEvent(kEventMouseWheel, point, 1);
						}

						break;

					case Button5:

						if (event->type == ButtonPress)
						{
							TheEngine->HandleMouseEvent(kEventMouseWheel, point, -1);
						}

						break;
				}
			}
		}
		else
		{
			if (event->type == ButtonPress)
			{
				switch (event->button)
				{
					case Button1:

						leftButton.HandleConfigEvent(1);
						break;

					case Button2:

						middleButton.HandleConfigEvent(1);
						break;

					case Button3:

						rightButton.HandleConfigEvent(1);
						break;

					case Button4:

						wheelAxis.HandleConfigEvent(1);
						break;

					case Button5:

						wheelAxis.HandleConfigEvent(-1);
						break;
				}
			}
		}
	}

	void MouseDevice::HandleMouseMotionEvent(InputMode mode, const XMotionEvent *event)
	{
		if (mode != kInputConfiguration)
		{
			int32 x = event->x;
			int32 y = event->y;

			if (mode & kInputMouseActive)
			{
				if (((x - originalPosition.x) | (y - originalPosition.y)) != 0)
				{
					currentPosition.Set(x, y);
				}
			}
			else
			{
				currentPosition.Set(x, y);
				motionEventFlag = true;
			}
		}
	}

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

void MouseDevice::SetInputMode(InputMode mode)
{
	if (mode & kInputMouseActive)
	{
		if (!DeviceActive())
		{
			Activate();
		}
	}
	else
	{
		if (DeviceActive())
		{
			Deactivate();
			Engine::ResetMouseButtonMask();
		}
	}

	#if C4MACOS

		if ((mode & kInputMouseActive) && (mode != kInputConfiguration))
		{
			CGAssociateMouseAndMouseCursorPosition(false);
		}
		else
		{
			CGAssociateMouseAndMouseCursorPosition(true);
		}

	#endif
}


#if C4WINDOWS

	KeyboardDevice::KeyboardDevice(IDirectInput8W *directInput, const DIDEVICEINSTANCEW *instance) : StandardInputDevice(kInputDeviceKeyboard, 0x00010006, directInput, instance)
	{
		DIPROPDWORD		prop;

		deviceInstance->SetCooperativeLevel(TheEngine->GetEngineWindow(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		deviceInstance->EnumObjects(&EnumObjectsCallback, static_cast<DirectInputDevice *>(this), DIDFT_BUTTON);
		BuildDataFormat();

		prop.diph.dwSize = sizeof(DIPROPDWORD);
		prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		prop.diph.dwObj = 0;
		prop.diph.dwHow = DIPH_DEVICE;
		prop.dwData = kInputQueueSize;
		deviceInstance->SetProperty(DIPROP_BUFFERSIZE, &prop.diph);
	}

	KeyboardDevice::~KeyboardDevice()
	{
		if (DeviceActive())
		{
			Deactivate();
		}
	}

#elif C4MACOS

	KeyboardDevice::KeyboardDevice(unsigned_int32 usage, io_object_t object, CFMutableDictionaryRef properties) : StandardInputDevice(kInputDeviceKeyboard, usage, object, properties)
	{
	}

	KeyboardDevice::~KeyboardDevice()
	{
		if (DeviceActive())
		{
			Deactivate();
		}
	}

#elif C4LINUX

	KeyboardDevice::KeyboardDevice() : StandardInputDevice(kInputDeviceKeyboard, 0x00010006, "Keyboard")
	{
		char ascii[2] = {0, 0};

		::Display *display = TheEngine->GetEngineDisplay();
		for (machine a = 0; a < kKeyboardRawCodeCount; a++)
		{
			keyButton[a] = nullptr;

			KeySym symbol = XkbKeycodeToKeysym(display, a + kKeyboardRawCodeBase, 0, 0);
			if (symbol != NoSymbol)
			{
				const char *name = "";
				unsigned_int32 usage = 0;

				if (unsigned_int32(symbol - 0x20) < 95U)
				{
					KeySym s = symbol;
					if (s != 0x20)
					{
						name = ascii;
						if (unsigned_int32(s - 'a') < 26U)
						{
							s -= 32;
						}

						ascii[0] = char(s);
					}
					else
					{
						name = "Space";
					}

					usage = keyAsciiUsage[s - 0x20];
				}
				else if (unsigned_int32(symbol - 0xFF00) < 0x0100U)
				{
					KeySym s = symbol & 0x00FF;
					name = keySymbolName[s];
					usage = keySymbolUsage[s];
				}

				if (name[0] == 0)
				{
					name = XKeysymToString(symbol);
					if (!name)
					{
						continue;
					}
				}

				KeyButtonControl *button = new KeyButtonControl(this, 0x00070000 | usage, name);
				controlTree.AppendSubnode(button);
				keyButton[a] = button;

				if (symbol == XK_grave)
				{
					button->SetControlAction(TheInputMgr->FindAction(kActionConsole));
				}
				else if (symbol == XK_Escape)
				{
					button->SetControlAction(TheInputMgr->FindAction(kActionEscape));
				}
			}
		}
	}

	KeyboardDevice::~KeyboardDevice()
	{
		if (DeviceActive())
		{
			Deactivate();
		}
	}

	void KeyboardDevice::HandleKeyboardEvent(InputMode mode, const XKeyEvent *event)
	{
		if (mode != kInputConfiguration)
		{
			if (mode & kInputKeyboardActive)
			{
				unsigned_int32 code = event->keycode - kKeyboardRawCodeBase;
				if (code < kKeyboardRawCodeCount)
				{
					KeyButtonControl *control = keyButton[code];
					if (control)
					{
						control->HandleNormalEvent(event->type == KeyPress);
					}
				}
			}
			else
			{
				EventType	eventType;
				char		string[4];
				KeySym		symbol;

				if (event->type == KeyPress)
				{
					eventType = ((event->state & ControlMask) == 0) ? kEventKeyDown : kEventKeyCommand;
				}
				else
				{
					if ((event->state & ControlMask) != 0)
					{
						return;
					}

					eventType = kEventKeyUp;
				}

				unsigned_int32 modifierKeys = ((event->state & ShiftMask) != 0) ? kModifierKeyShift : 0;

				XLookupString(const_cast<XKeyEvent *>(event), string, 4, &symbol, nullptr);
				unsigned_int32 code = 0;

				if (unsigned_int32(symbol - 0x0020) < 0x005FU)
				{
					if (eventType != kEventKeyCommand)
					{
						code = string[0];
					}
					else
					{
						code = symbol;
						if (code - 'a' < 26U)
						{
							code -= 32;
						}
					}
				}
				else if (unsigned_int32(symbol - 0xFF08) < 0x00F8U)
				{
					if (unsigned_int32(symbol - XK_F1) >= 24U)
					{
						switch (symbol)
						{
							case XK_BackSpace:
								code = kKeyCodeBackspace;
								break;
							case XK_Tab:
								code = kKeyCodeTab;
								break;
							case XK_Return:
								code = kKeyCodeEnter;
								break;
							case XK_Escape:
								code = kKeyCodeEscape;
								break;
							case XK_Home:
								code = kKeyCodeHome;
								break;
							case XK_Left:
								code = kKeyCodeLeftArrow;
								break;
							case XK_Up:
								code = kKeyCodeUpArrow;
								break;
							case XK_Right:
								code = kKeyCodeRightArrow;
								break;
							case XK_Down:
								code = kKeyCodeDownArrow;
								break;
							case XK_Page_Up:
								code = kKeyCodePageUp;
								break;
							case XK_Page_Down:
								code = kKeyCodePageDown;
								break;
							case XK_End:
								code = kKeyCodeEnd;
								break;
							case XK_Delete:
								code = kKeyCodeDelete;
								break;
						}
					}
					else
					{
						code = symbol + (kKeyCodeF1 - XK_F1);
					}
				}

				if (code != 0)
				{
					TheEngine->HandleKeyboardEvent(eventType, code, modifierKeys);
				}
			}
		}
		else
		{
			unsigned_int32 code = event->keycode - kKeyboardRawCodeBase;
			if (code < kKeyboardRawCodeCount)
			{
				KeyButtonControl *control = keyButton[code];
				if (control)
				{
					control->HandleConfigEvent(1);
				}
			}
		}
	}

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

void KeyboardDevice::SetInputMode(InputMode mode)
{
	if (mode & kInputKeyboardActive)
	{
		if (!DeviceActive())
		{
			Activate();

			#if C4LINUX

				XAutoRepeatOff(TheEngine->GetEngineDisplay());

			#endif
		}
	}
	else
	{
		if (DeviceActive())
		{
			Deactivate();

			#if C4LINUX

				XAutoRepeatOn(TheEngine->GetEngineDisplay());

			#endif
		}
	}
}


#if C4WINDOWS

	JoystickDevice::JoystickDevice(IDirectInput8W *directInput, const DIDEVICEINSTANCEW *instance) : StandardInputDevice(kInputDeviceJoystick, 0, directInput, instance)
	{
		DIPROPDWORD		prop;

		deviceInstance->SetCooperativeLevel(TheEngine->GetEngineWindow(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);

		prop.diph.dwSize = sizeof(DIPROPDWORD);
		prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		prop.diph.dwObj = 0;
		prop.diph.dwHow = DIPH_DEVICE;
		prop.dwData = kInputQueueSize;
		deviceInstance->SetProperty(DIPROP_BUFFERSIZE, &prop.diph);

		prop.diph.dwSize = sizeof(DIPROPDWORD);
		prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		prop.diph.dwObj = 0;
		prop.diph.dwHow = DIPH_DEVICE;
		prop.dwData = DIPROPAXISMODE_ABS;
		deviceInstance->SetProperty(DIPROP_AXISMODE, &prop.diph);

		deviceInstance->EnumObjects(&EnumObjectsCallback, static_cast<DirectInputDevice *>(this), DIDFT_ALL);
		BuildDataFormat();

		deviceInstance->EnumEffects(&EnumEffectsCallback, this, DIEFT_CONSTANTFORCE | DIEFT_CUSTOMFORCE);
	}

	JoystickDevice::~JoystickDevice()
	{
		if (DeviceActive())
		{
			Deactivate();
		}
	}

	BOOL CALLBACK JoystickDevice::EnumEffectsCallback(const DIEFFECTINFOW *effect, void *cookie)
	{
		JoystickDevice *joystickDevice = static_cast<JoystickDevice *>(cookie);
		joystickDevice->feedbackList.Append(new InputFeedback(effect));

		return (DIENUM_CONTINUE);
	}

	bool JoystickDevice::ProcessEvents(InputMode mode)
	{
		deviceInstance->Poll();

		return (StandardInputDevice::ProcessEvents(mode));
	}

#elif C4MACOS

	JoystickDevice::JoystickDevice(unsigned_int32 usage, io_object_t object, CFMutableDictionaryRef properties) : StandardInputDevice(kInputDeviceJoystick, usage, object, properties)
	{
	}

	JoystickDevice::~JoystickDevice()
	{
		if (DeviceActive())
		{
			Deactivate();
		}
	}

#elif C4LINUX

	JoystickDevice::JoystickDevice(int fileDesc, const char *name) : StandardInputDevice(kInputDeviceJoystick, 0x00010004, name)
	{
		char	count;

		joystickFileDesc = fileDesc;

		if (ioctl(fileDesc, JSIOCGBUTTONS, &count) != -1)
		{
			count = MaxZero(count);
			for (machine a = 0; a < count; a++)
			{
				GenericButtonControl *control = new GenericButtonControl(this, unsigned_int32(0x00090001 + a), InputControlName("Button ") += int32(a));
				controlTree.AppendSubnode(control);
				buttonTable.AddElement(control);
			}
		}

		if (ioctl(fileDesc, JSIOCGAXES, &count) != -1)
		{
			static const unsigned_int32 usage[4] = {0x00010030, 0x00010031, 0x00010032, 0x00010035};

			count = Min(MaxZero(count), 4);
			for (machine a = 0; a < count; a++)
			{
				LinearAxisControl *control = new LinearAxisControl(this, ((a & 1) == 0) ? kInputAxisX : kInputAxisY, usage[a], InputControlName("Axis ") += int32(a));
				control->SetRange(-32767.0F, 32767.0F, 4096.0F);
				controlTree.AppendSubnode(control);
				axisTable.AddElement(control);
			}
		}
	}

	JoystickDevice::~JoystickDevice()
	{
		if (DeviceActive())
		{
			Deactivate();
		}

		close(joystickFileDesc);
	}

	void JoystickDevice::Activate(void)
	{
		StandardInputDevice::Activate();

		read(joystickFileDesc, eventData, kInputQueueSize * sizeof(js_event));
	}

	bool JoystickDevice::ProcessEvents(InputMode mode)
	{
		if (mode != kInputConfiguration)
		{
			int count = read(joystickFileDesc, eventData, kInputQueueSize * sizeof(js_event));
			if (count > 0)
			{
				count /= sizeof(js_event);
				for (machine a = 0; a < count; a++)
				{
					const js_event *data = &eventData[a];

					unsigned_int32 type = data->type & (JS_EVENT_BUTTON | JS_EVENT_AXIS);
					unsigned_int32 index = data->number;

					if (type == JS_EVENT_BUTTON)
					{
						if (index < buttonTable.GetElementCount())
						{
							buttonTable[index]->HandleNormalEvent(data->value);
						}
					}
					else if (type == JS_EVENT_AXIS)
					{
						if (index < axisTable.GetElementCount())
						{
							axisTable[index]->HandleNormalEvent(data->value);
						}
					}
				}
			}
		}
		else
		{
			if (read(joystickFileDesc, eventData, sizeof(js_event)) > 0)
			{
				unsigned_int32 type = eventData->type;
				unsigned_int32 index = eventData->number;

				if (type == JS_EVENT_BUTTON)
				{
					if (index < buttonTable.GetElementCount())
					{
						if (buttonTable[index]->HandleConfigEvent(eventData->value))
						{
							return (true);
						}
					}
				}
				else if (type == JS_EVENT_AXIS)
				{
					if (index < axisTable.GetElementCount())
					{
						if (axisTable[index]->HandleConfigEvent(eventData->value))
						{
							return (true);
						}
					}
				}
			}
		}

		return (false);
	}

#elif C4IOS //[ MOBILE

	// -- Mobile code hidden --

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]


#if C4XINPUT

	XboxDevice::XboxDevice(int32 index) : InputDevice(kInputDeviceXbox, 0x00010005, String<31>("Xbox 360 Controller ") += index)
	{
		deviceIndex = index;
		packetNumber = 0xFFFFFFFF;

		char *storage = controlStorage;
		for (machine a = 0; a < kXboxButtonCount; a++)
		{
			GenericButtonControl *control = new(storage) GenericButtonControl(this, xboxButtonUsage[a], xboxButtonName[a]);
			buttonControl[a] = control;

			controlTree.AppendSubnode(control);
			buttonState[a] = false;

			storage += sizeof(GenericButtonControl);
		}

		for (machine a = 0; a < kXboxTriggerCount; a++)
		{
			SliderControl *control = new(storage) SliderControl(this, xboxTriggerUsage[a], xboxTriggerName[a]);
			triggerControl[a] = control;

			control->SetRange(0.0F, 255.0F, float(XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
			controlTree.AppendSubnode(control);
			triggerState[a] = 0;

			storage += sizeof(SliderControl);
		}

		for (machine a = 0; a < kXboxAxisCount; a++)
		{
			LinearAxisControl *control = new(storage) LinearAxisControl(this, static_cast<InputAxis>(a & 1), xboxAxisUsage[a], xboxAxisName[a]);
			axisControl[a] = control;

			control->SetRange(-32768.0F, 32767.0F, (a < 2) ? float(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) : float(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
			controlTree.AppendSubnode(control);
			axisState[a] = 0;

			storage += sizeof(LinearAxisControl);
		}
	}

	XboxDevice::~XboxDevice()
	{
		if (DeviceActive())
		{
			Deactivate();
		}

		for (machine a = kXboxAxisCount - 1; a >= 0; a--)
		{
			axisControl[a]->~LinearAxisControl();
		}

		for (machine a = kXboxTriggerCount - 1; a >= 0; a--)
		{
			triggerControl[a]->~SliderControl();
		}

		for (machine a = kXboxButtonCount - 1; a >= 0; a--)
		{
			buttonControl[a]->~GenericButtonControl();
		}
	}

	bool XboxDevice::ProcessEvents(InputMode mode)
	{
		XINPUT_STATE	inputState;

		if (XInputGetState(deviceIndex, &inputState) == ERROR_SUCCESS)
		{
			unsigned_int32 num = inputState.dwPacketNumber;
			if (packetNumber != num)
			{
				packetNumber = num;

				if (mode != kInputConfiguration)
				{
					unsigned_int32 state = inputState.Gamepad.wButtons;
					for (machine a = 0; a < 10; a++)
					{
						unsigned_int32 v = state & 1;
						if (v ^ buttonState[a])
						{
							buttonState[a] = (v != 0);
							buttonControl[a]->HandleNormalEvent(v);
						}

						state >>= 1;
					}

					state >>= 2;

					for (machine a = 10; a < kXboxButtonCount; a++)
					{
						unsigned_int32 v = state & 1;
						if (v ^ buttonState[a])
						{
							buttonState[a] = (v != 0);
							buttonControl[a]->HandleNormalEvent(v);
						}

						state >>= 1;
					}

					for (machine a = 0; a < kXboxTriggerCount; a++)
					{
						unsigned_int8 v = (&inputState.Gamepad.bLeftTrigger)[a];
						if (v != triggerState[a])
						{
							triggerState[a] = v;
							triggerControl[a]->HandleNormalEvent(v);
						}
					}

					for (machine a = 0; a < kXboxAxisCount; a++)
					{
						int16 v = (&inputState.Gamepad.sThumbLX)[a];
						if (v != axisState[a])
						{
							axisState[a] = v;
							int32 flip = int32(-(a & 1));
							axisControl[a]->HandleNormalEvent((v ^ flip) - flip);
						}
					}
				}
				else
				{
					unsigned_int32 state = inputState.Gamepad.wButtons;
					for (machine a = 0; a < 10; a++)
					{
						unsigned_int32 v = state & 1;
						if ((v != 0) && (buttonControl[a]->HandleConfigEvent(1)))
						{
							return (true);
						}

						state >>= 1;
					}

					state >>= 2;

					for (machine a = 10; a < kXboxButtonCount; a++)
					{
						unsigned_int32 v = state & 1;
						if ((v != 0) && (buttonControl[a]->HandleConfigEvent(1)))
						{
							return (true);
						}

						state >>= 1;
					}

					for (machine a = 0; a < kXboxTriggerCount; a++)
					{
						unsigned_int8 v = (&inputState.Gamepad.bLeftTrigger)[a];
						if (v != triggerState[a])
						{
							triggerState[a] = v;
							if (triggerControl[a]->HandleConfigEvent(v))
							{
								return (true);
							}
						}
					}

					for (machine a = 0; a < kXboxAxisCount; a++)
					{
						int16 v = (&inputState.Gamepad.sThumbLX)[a];
						if (v != axisState[a])
						{
							axisState[a] = v;
							if (axisControl[a]->HandleConfigEvent(v))
							{
								return (true);
							}
						}
					}
				}
			}
		}

		return (false);
	}

#endif


InputMgr::InputMgr(int) :

		#if C4IOS //[ MOBILE

			// -- Mobile code hidden --

		#endif //]

		mouseXAction(kActionMouseX),
		mouseYAction(kActionMouseY),
		sensitivityObserver(this, &InputMgr::HandleSensitivityEvent),
		invertMouseObserver(this, &InputMgr::HandleInvertMouseEvent),
		smoothMouseObserver(this, &InputMgr::HandleSmoothMouseEvent)
{
	#if C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

InputMgr::~InputMgr()
{
}

EngineResult InputMgr::Construct(void)
{
	AddAction(&consoleAction);
	AddAction(&escapeAction);
	AddAction(&mouseXAction);
	AddAction(&mouseYAction);

	#if C4WINDOWS

		xinputDeviceCount = 0;

		IWbemLocator *wbemLocator = nullptr;
		IWbemServices *wbemServices = nullptr;
		IEnumWbemClassObject *wbemEnumerator = nullptr;

		if (SUCCEEDED(CoCreateInstance(__uuidof(WbemLocator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWbemLocator), (void **) &wbemLocator)))
		{
			BSTR resourceString = SysAllocString(L"root\\cimv2");
			BSTR classString = SysAllocString(L"Win32_PNPEntity");

			if (SUCCEEDED(wbemLocator->ConnectServer(resourceString, nullptr, nullptr, nullptr, 0, nullptr, nullptr, &wbemServices)))
			{
				CoSetProxyBlanket(wbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

				if (SUCCEEDED(wbemServices->CreateInstanceEnum(classString, WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY, nullptr, &wbemEnumerator)))
				{
					for (;;)
					{
						IWbemClassObject	*wbemObject[16];
						ULONG				count;

						if ((FAILED(wbemEnumerator->Next(1000, 16, wbemObject, &count))) || (count == 0))
						{
							break;
						}

						for (unsigned_machine a = 0; a < count; a++)
						{
							VARIANT		variant;

							if (SUCCEEDED(wbemObject[a]->Get(L"DeviceID", 0, &variant, nullptr, nullptr)))
							{
								if (variant.vt == VT_BSTR)
								{
									const wchar_t *id = variant.bstrVal;
									if ((id) && (FindDeviceSubstring(id, L"IG_")))
									{
										unsigned_int32 guid = 0;

										const wchar_t *pid = FindDeviceSubstring(id, L"PID_");
										if (pid)
										{
											guid = ReadDeviceHexString(pid);
										}

										const wchar_t *vid = FindDeviceSubstring(id, L"VID_");
										if (vid)
										{
											guid = (guid << 16) | ReadDeviceHexString(vid);
										}

										if (guid != 0)
										{
											int32 deviceCount = xinputDeviceCount;
											if (deviceCount < kMaxXinputDeviceCount)
											{
												xinputDeviceGuid[deviceCount] = guid;
												xinputDeviceCount = deviceCount + 1;
											}
										}
									}
								}

								VariantClear(&variant);
							}

							wbemObject[a]->Release();
						}
					}
				}
			}

			SysFreeString(classString);
			SysFreeString(resourceString);
		}

		InputResult result = kInputOkay;
		if (FAILED(DirectInput8Create(TheEngine->GetEngineInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8W, (void **) &directInput, nullptr)))
		{
			result = kInputInitFailed;
		}
		else
		{
			directInput->EnumDevices(DI8DEVCLASS_ALL, &EnumDevicesCallback, this, DIEDFL_ATTACHEDONLY);
		}

		if (wbemLocator)
		{
			if (wbemServices)
			{
				if (wbemEnumerator)
				{
					wbemEnumerator->Release();
				}

				wbemServices->Release();
			}

			wbemLocator->Release();
		}

		if (result != kInputOkay)
		{
			return (result);
		}

		for (machine a = 0; a < 4; a++)
		{
			XINPUT_CAPABILITIES		capabilities;

			if (XInputGetCapabilities(a, XINPUT_FLAG_GAMEPAD, &capabilities) == ERROR_SUCCESS)
			{
				XboxDevice *device = new XboxDevice(a);
				deviceList.Append(device);
			}
		}

	#elif C4MACOS

		io_iterator_t	iterator;

		CFMutableDictionaryRef dictionary = IOServiceMatching(kIOHIDDeviceKey);
		if ((!dictionary) || (IOServiceGetMatchingServices(kIOMasterPortDefault, dictionary, &iterator) != kIOReturnSuccess))
		{
			return (kInputInitFailed);
		}

		for (;;)
		{
			CFMutableDictionaryRef	properties;

			io_object_t object = IOIteratorNext(iterator);
			if (!object)
			{
				break;
			}

			if (IORegistryEntryCreateCFProperties(object, &properties, kCFAllocatorDefault, kNilOptions) == KERN_SUCCESS)
			{
				const void *usagePageValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDPrimaryUsagePageKey));
				const void *usageValue = CFDictionaryGetValue(properties, CFSTR(kIOHIDPrimaryUsageKey));
				if ((usagePageValue) && (usageValue))
				{
					int32	usagePage;
					int32	usage;

					CFNumberGetValue((CFNumberRef) usagePageValue, kCFNumberSInt32Type, &usagePage);
					CFNumberGetValue((CFNumberRef) usageValue, kCFNumberSInt32Type, &usage);

					if (usagePage == kHIDPage_GenericDesktop)
					{
						if ((usage == kHIDUsage_GD_Pointer) || (usage == kHIDUsage_GD_Mouse))
						{
							deviceList.Append(new MouseDevice((usagePage << 16) | usage, object, properties));
						}
						else if ((usage == kHIDUsage_GD_Keyboard) || (usage == kHIDUsage_GD_Keypad))
						{
							deviceList.Append(new KeyboardDevice((usagePage << 16) | usage, object, properties));
						}
						else if ((usage == kHIDUsage_GD_Joystick) || (usage == kHIDUsage_GD_GamePad))
						{
							deviceList.Append(new JoystickDevice((usagePage << 16) | usage, object, properties));
						}
					}
					else if ((usagePage == kHIDPage_Simulation) || (usagePage == kHIDPage_Sport) || (usagePage == kHIDPage_Game))
					{
						deviceList.Append(new JoystickDevice((usagePage << 16) | usage, object, properties));
					}
				}

				CFRelease(properties);
			}

			IOObjectRelease(object);
		}

		IOObjectRelease(iterator);

	#elif C4LINUX

		mouseDevice = new MouseDevice;
		deviceList.Append(mouseDevice);

		keyboardDevice = new KeyboardDevice;
		deviceList.Append(keyboardDevice);

		for (machine a = 0; a < 10; a++)
		{
			int fileDesc = open(String<31>("/dev/input/js") += int32(a), O_RDONLY | O_NONBLOCK);
			if (fileDesc >= 0)
			{
				InputDeviceName		name;

				if (ioctl(fileDesc, JSIOCGNAME(kMaxInputDeviceNameLength), (char *) name) != -1)
				{
					// Ownership of the file is transferred to the JoystickDevice object.

					deviceList.Append(new JoystickDevice(fileDesc, name));
				}
				else
				{
					close(fileDesc);
				}
			}
		}

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	InputDevice *device = deviceList.First();
	while (device)
	{
		InputDevice *next = device->Next();

		if (!device->GetFirstControl())
		{
			delete device;
		}

		device = next;
	}

	consoleProc = nullptr;
	escapeProc = nullptr;
	configProc = nullptr;

	mouseFlags = 0;

	TheEngine->InitVariable("sensitivity", "15", kVariablePermanent, &sensitivityObserver);
	TheEngine->InitVariable("invertMouse", "0", kVariablePermanent, &invertMouseObserver);
	TheEngine->InitVariable("smoothMouse", "1", kVariablePermanent, &smoothMouseObserver);

	#if C4LOG_FILE

		UpdateLog();

	#endif

	return (kInputOkay);
}

void InputMgr::Destruct(void)
{
	SetInputMode(kInputInactive);

	RemoveAction(&mouseYAction);
	RemoveAction(&mouseXAction);
	RemoveAction(&escapeAction);
	RemoveAction(&consoleAction);

	actionList.Purge();
	deviceList.Purge();

	#if C4WINDOWS

		directInput->Release();

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

#if C4WINDOWS

	const wchar_t *InputMgr::FindDeviceSubstring(const wchar_t *string, const wchar_t *substring)
	{
		int32 subposition = 0;
		int32 subchar = substring[0];

		for (;;)
		{
			int32 c = *string++;
			if (c == 0)
			{
				break;
			}

			if (c == subchar)
			{
				subchar = substring[++subposition];
				if (subchar == 0)
				{
					return (string);
				}
			}
			else if (subposition != 0)
			{
				subposition = 0;
				subchar = substring[0];
				continue;
			}
		}

		return (nullptr);
	}

	unsigned_int32 InputMgr::ReadDeviceHexString(const wchar_t *string)
	{
		unsigned_int32 id = 0;

		for (machine a = 0; a < 4; a++)
		{
			unsigned_int32 c = string[a];
			if (c == 0)
			{
				break;
			}

			id <<= 4;
			if ((c -= 48) < 10U)
			{
				id |= c;
			}
			else if (((c -= 17) < 6U) || ((c -= 32) < 6U))
			{
				id |= c + 10;
			}
		}

		return (id);
	}

	BOOL CALLBACK InputMgr::EnumDevicesCallback(const DIDEVICEINSTANCEW *instance, void *cookie)
	{
		InputMgr *inputMgr = static_cast<InputMgr *>(cookie);

		DWORD type = GET_DIDEVICE_TYPE(instance->dwDevType);
		if (type == DI8DEVTYPE_MOUSE)
		{
			inputMgr->deviceList.Append(new MouseDevice(inputMgr->directInput, instance));
		}
		else if (type == DI8DEVTYPE_KEYBOARD)
		{
			inputMgr->deviceList.Append(new KeyboardDevice(inputMgr->directInput, instance));
		}
		else
		{
			unsigned_int32 usagePage = unsigned_int32(instance->wUsagePage);
			if ((usagePage == 0x0001) || (usagePage == 0x0002) || (usagePage == 0x0004) || (usagePage == 0x0005))
			{
				if ((usagePage != 0x0001) || (unsigned_int32(instance->wUsage) < 0x0009U))
				{
					unsigned_int32 guid = instance->guidProduct.Data1;

					int32 count = inputMgr->xinputDeviceCount;
					for (machine a = 0; a < count; a++)
					{
						if (inputMgr->xinputDeviceGuid[a] == guid)
						{
							return (DIENUM_CONTINUE);
						}
					}

					inputMgr->deviceList.Append(new JoystickDevice(inputMgr->directInput, instance));
				}
			}
		}

		return (DIENUM_CONTINUE);
	}

#elif C4IOS //[ MOBILE

	// -- Mobile code hidden --

#endif //]

void InputMgr::HandleSensitivityEvent(Variable *variable)
{
	SetMouseSensitivity(Min(Max(variable->GetIntegerValue(), 1), 100));
}

void InputMgr::HandleInvertMouseEvent(Variable *variable)
{
	unsigned_int32 flags = mouseFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kMouseInverted;
	}
	else
	{
		flags &= ~kMouseInverted;
	}

	SetMouseFlags(flags);
}

void InputMgr::HandleSmoothMouseEvent(Variable *variable)
{
	unsigned_int32 flags = mouseFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kMouseSmooth;
	}
	else
	{
		flags &= ~kMouseSmooth;
	}

	SetMouseFlags(flags);
}

#if C4LOG_FILE

	void InputMgr::UpdateLog(void) const
	{
		Engine::Report("Input Manager", kReportLog | kReportHeading);
		Engine::Report("<table class=\"data\" cellspacing=\"0\" cellpadding=\"0\">\r\n", kReportLog);

		const InputDevice *device = GetFirstDevice();
		while (device)
		{
			Engine::Report("<tr><th>", kReportLog);
			Engine::Report(device->GetDeviceName(), kReportLog);

			unsigned_int32 hidUsage = device->GetDeviceHidUsage();
			if (hidUsage != 0)
			{
				Engine::Report("<br/><span style=\"font-weight: normal;\">HID usage: ", kReportLog);
				Engine::Report(Text::IntegerToHexString8(hidUsage), kReportLog);
				Engine::Report("</span>", kReportLog);
			}

			switch (device->GetDeviceType())
			{
				case kInputDeviceMouse:

					Engine::Report("<br/><span style=\"font-weight: normal;\">(Mouse)</span>", kReportLog);
					break;

				case kInputDeviceKeyboard:

					Engine::Report("<br/><span style=\"font-weight: normal;\">(Keyboard)</span>", kReportLog);
					break;
			}

			Engine::Report("</th><td><div style=\"height: 128px; overflow: auto;\">\r\n", kReportLog);

			const InputControl *control = device->GetFirstControl();
			while (control)
			{
				if (control->GetControlType() != kInputControlGroup)
				{
					Engine::Report(control->GetControlName(), kReportLog);
					Engine::Report("<br/>\r\n", kReportLog);
				}

				control = device->GetNextControl(control);
			}

			Engine::Report("</div></td></tr>\r\n", kReportLog);

			device = device->Next();
		}

		Engine::Report("</table>\r\n", kReportLog);
	}

#endif

InputDevice *InputMgr::FindDevice(const char *name) const
{
	InputDevice *device = deviceList.First();
	while (device)
	{
		if (Text::CompareTextCaseless(device->GetDeviceName(), name))
		{
			break;
		}

		device = device->Next();
	}

	return (device);
}

InputDevice *InputMgr::FindDevice(InputDeviceType type) const
{
	InputDevice *device = deviceList.First();
	while (device)
	{
		if (device->GetDeviceType() == type)
		{
			break;
		}

		device = device->Next();
	}

	return (device);
}

InputDevice *InputMgr::FindDevice(InputDeviceType type, const char *name) const
{
	InputDevice *device = deviceList.First();
	while (device)
	{
		if ((device->GetDeviceType() == type) && (Text::CompareTextCaseless(device->GetDeviceName(), name)))
		{
			break;
		}

		device = device->Next();
	}

	return (device);
}

Action *InputMgr::FindAction(ActionType type) const
{
	Action *action = actionList.First();
	while (action)
	{
		if (action->GetActionType() == type)
		{
			break;
		}

		action = action->Next();
	}

	return (action);
}

void InputMgr::SetInputMode(InputMode mode)
{
	if (inputMode != mode)
	{
		internalInputMode = inputMode;
		inputMode = mode;

		InputDevice *device = deviceList.First();
		while (device)
		{
			device->SetInputMode(mode);
			device = device->Next();
		}

		prevMouseDeltaX = 0.0F;
		prevMouseDeltaY = 0.0F;
	}
}

void InputMgr::SetMouseSensitivity(int32 sensitivity)
{
	mouseSensitivity = sensitivity;

	deltaXMultiplier = float(mouseSensitivity) * 2.0e-4F;
	deltaYMultiplier = (mouseFlags & kMouseInverted) ? deltaXMultiplier : -deltaXMultiplier;
}

void InputMgr::SetMouseFlags(unsigned_int32 flags)
{
	mouseFlags = flags;

	float m = deltaXMultiplier;
	deltaYMultiplier = (flags & kMouseInverted) ? m : -m;
}

InputControl *InputMgr::GetActionControl(const Action *action, int32 index)
{
	InputDevice *device = deviceList.First();
	while (device)
	{
		InputControl *control = device->GetFirstControl();
		while (control)
		{
			if (control->GetControlAction() == action)
			{
				if (index == 0)
				{
					return (control);
				}

				index--;
			}

			control = device->GetNextControl(control);
		}

		device = device->Next();
	}

	return (nullptr);
}

void InputMgr::ClearAllControlActions(void)
{
	InputDevice *device = deviceList.First();
	while (device)
	{
		InputControl *control = device->GetFirstControl();
		while (control)
		{
			Action *action = control->GetControlAction();
			if ((action) && (!(action->GetActionFlags() & kActionPersistent)))
			{
				control->SetControlAction(nullptr);
			}

			control = device->GetNextControl(control);
		}

		device = device->Next();
	}
}

void InputMgr::ResetAllActions(void)
{
	Action *action = actionList.First();
	while (action)
	{
		if (action->GetActiveCount() > 0)
		{
			action->SetActiveCount(0);
			action->End();
		}

		action = action->Next();
	}
}

void InputMgr::InputTask(void)
{
	mouseDeltaX = 0.0F;
	mouseDeltaY = 0.0F;

	InputMode mode = inputMode;
	internalInputMode = mode;

	if (mode != kInputInactive)
	{
		InputDevice *device = deviceList.First();
		while (device)
		{
			if ((device->DeviceActive()) && (device->ProcessEvents(mode)))
			{
				break;
			}

			device = device->Next();
		}

		if (mouseFlags & kMouseSmooth)
		{
			float x = mouseDeltaX * 0.5F;
			float y = mouseDeltaY * 0.5F;
			mouseDeltaX = prevMouseDeltaX + x;
			mouseDeltaY = prevMouseDeltaY + y;
			prevMouseDeltaX = x;
			prevMouseDeltaY = y;
		}
	}

	#if C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]
}

// ZYUQURM
