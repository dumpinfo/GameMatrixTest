#include "C4Wintab.h"


using namespace C4;


namespace C4
{
	UINT (WINAPI *WTInfoA)(UINT, UINT, void *) = nullptr;
	HCTX (WINAPI *WTOpenA)(HWND, TabletLogContext *, BOOL) = nullptr;
	BOOL (WINAPI *WTClose)(HCTX) = nullptr;
	int (WINAPI *WTPacketsGet)(HCTX, int, void *) = nullptr;
}

// ZYUQURM
