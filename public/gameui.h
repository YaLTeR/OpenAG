#pragma once

#include "interface.h"

#define GAMEUI_INTERFACE "GameUI007"

#ifdef _WIN32
#define GAMEUI_DLLNAME "cl_dlls/gameui.dll"
#elif defined(OSX)
#define GAMEUI_DLLNAME "cl_dlls/gameui.dylib"
#elif defined(LINUX)
#define GAMEUI_DLLNAME "cl_dlls/gameui.so"
#else
#error
#endif

class IGameUI : public IBaseInterface
{
public:
	virtual void Unk1() = 0;
	virtual void Unk2() = 0;
	virtual void Unk3() = 0;
	virtual void Unk4() = 0;
	virtual void Unk5() = 0;
	virtual int IsGameUIVisible() = 0;
};

extern IGameUI *g_pGameUI;
