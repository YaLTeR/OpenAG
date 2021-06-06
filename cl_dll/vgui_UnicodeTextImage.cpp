#include <cwchar>
#include <vector>
#include <type_traits>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsani_in.h>
#include <Windows.h>
#include <winsani_out.h>

#else
#include <dlfcn.h>
#endif

#include <VGUI_Font.h>
#include "../public/interface.h"
#include "vgui_UnicodeTextImage.h"
#include "hud.h"
#include "cl_util.h"

namespace
{

typedef unsigned int VPANEL;
typedef unsigned long HCursor;
typedef unsigned long HTexture;

class Color_VGUI2;
class IHTMLResponses;
class IHTMLChromeController;
class IHTML;
class IHTMLEvents;
class VGuiVertex;

}

namespace vgui2
{

class ISurface : public IBaseInterface
{
public:
	// call to Shutdown surface; surface can no longer be used after this is called
	virtual void Shutdown() = 0;

	// frame
	virtual void RunFrame() = 0;

	// hierarchy root
	virtual VPANEL GetEmbeddedPanel() = 0;
	virtual void SetEmbeddedPanel(VPANEL pPanel) = 0;

	// drawing context
	virtual void PushMakeCurrent(VPANEL panel, bool useInsets) = 0;
	virtual void PopMakeCurrent(VPANEL panel) = 0;

	// rendering functions
	virtual void DrawSetColor(int r, int g, int b, int a) = 0;
	virtual void DrawSetColor(Color_VGUI2 col) = 0;

	virtual void DrawFilledRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawOutlinedRect(int x0, int y0, int x1, int y1) = 0;

	virtual void DrawLine(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawPolyLine(int *px, int *py, int numPoints) = 0;

	virtual void DrawSetTextFont(UnicodeTextImage::HFont font) = 0;
	virtual void DrawSetTextColor(int r, int g, int b, int a) = 0;
	virtual void DrawSetTextColor(Color_VGUI2 col) = 0;
	virtual void DrawSetTextPos(int x, int y) = 0;
	virtual void DrawGetTextPos(int &x, int &y) = 0;
	virtual void DrawPrintText(const wchar_t *text, int textLen) = 0;
	virtual void DrawUnicodeChar(wchar_t wch) = 0;
	virtual void DrawUnicodeCharAdd(wchar_t wch) = 0;

	virtual void DrawFlushText() = 0;		// flushes any buffered text (for rendering optimizations)
	virtual IHTML *CreateHTMLWindow(IHTMLEvents *events, VPANEL context) = 0;
	virtual void PaintHTMLWindow(IHTML *htmlwin) = 0;
	virtual void DeleteHTMLWindow(IHTML *htmlwin) = 0;

	virtual void DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload) = 0;
	virtual void DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload) = 0;
	virtual void DrawSetTexture(int id) = 0;
	virtual void DrawGetTextureSize(int id, int &wide, int &tall) = 0;
	virtual void DrawTexturedRect(int x0, int y0, int x1, int y1) = 0;
	virtual bool IsTextureIDValid(int id) = 0;

	virtual int CreateNewTextureID(bool procedural = false) = 0;

	virtual void GetScreenSize(int &wide, int &tall) = 0;
	virtual void SetAsTopMost(VPANEL panel, bool state) = 0;
	virtual void BringToFront(VPANEL panel) = 0;
	virtual void SetForegroundWindow(VPANEL panel) = 0;
	virtual void SetPanelVisible(VPANEL panel, bool state) = 0;
	virtual void SetMinimized(VPANEL panel, bool state) = 0;
	virtual bool IsMinimized(VPANEL panel) = 0;
	virtual void FlashWindow(VPANEL panel, bool state) = 0;
	virtual void SetTitle(VPANEL panel, const wchar_t *title) = 0;
	virtual void SetAsToolBar(VPANEL panel, bool state) = 0;		// removes the window's task bar entry (for context menu's, etc.)

	// windows stuff
	virtual void CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true) = 0;
	virtual void SwapBuffers(VPANEL panel) = 0;
	virtual void Invalidate(VPANEL panel) = 0;
	virtual void SetCursor(HCursor cursor) = 0;
	virtual bool IsCursorVisible() = 0;
	virtual void ApplyChanges() = 0;
	virtual bool IsWithin(int x, int y) = 0;
	virtual bool HasFocus() = 0;

	// returns true if the surface supports minimize & maximize capabilities
	enum SurfaceFeature_e
	{
		ANTIALIASED_FONTS = 1,
		DROPSHADOW_FONTS = 2,
		ESCAPE_KEY = 3,
		OPENING_NEW_HTML_WINDOWS = 4,
		FRAME_MINIMIZE_MAXIMIZE = 5,
		OUTLINE_FONTS = 6,
		DIRECT_HWND_RENDER = 7,
	};
	virtual bool SupportsFeature(SurfaceFeature_e feature) = 0;

	// restricts what gets drawn to one panel and it's children
	// currently only works in the game
	virtual void RestrictPaintToSinglePanel(VPANEL panel) = 0;

	// these two functions obselete, use IInput::SetAppModalSurface() instead
	virtual void SetModalPanel(VPANEL) = 0;
	virtual VPANEL GetModalPanel() = 0;

	virtual void UnlockCursor() = 0;
	virtual void LockCursor() = 0;
	virtual void SetTranslateExtendedKeys(bool state) = 0;
	virtual VPANEL GetTopmostPopup() = 0;

	// engine-only focus handling (replacing WM_FOCUS windows handling)
	virtual void SetTopLevelFocus(VPANEL panel) = 0;

	// fonts
	// creates an empty handle to a vgui font.  windows fonts can be add to this via AddGlyphSetToFont().
	virtual UnicodeTextImage::HFont CreateFont() = 0;

	// adds to the font
	enum EFontFlags
	{
		FONTFLAG_NONE,
		FONTFLAG_ITALIC = 0x001,
		FONTFLAG_UNDERLINE = 0x002,
		FONTFLAG_STRIKEOUT = 0x004,
		FONTFLAG_SYMBOL = 0x008,
		FONTFLAG_ANTIALIAS = 0x010,
		FONTFLAG_GAUSSIANBLUR = 0x020,
		FONTFLAG_ROTARY = 0x040,
		FONTFLAG_DROPSHADOW = 0x080,
		FONTFLAG_ADDITIVE = 0x100,
		FONTFLAG_OUTLINE = 0x200,
		FONTFLAG_CUSTOM = 0x400,		// custom generated font - never fall back to asian compatibility mode
		FONTFLAG_BITMAP = 0x800,		// compiled bitmap font - no fallbacks
	};

	virtual bool AddGlyphSetToFont(UnicodeTextImage::HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange) = 0;

	// adds a custom font file (only supports true type font files (.ttf) for now)
	virtual bool AddCustomFontFile(const char *fontFileName) = 0;

	// returns the details about the font
	virtual int GetFontTall(UnicodeTextImage::HFont font) = 0;
	virtual void GetCharABCwide(UnicodeTextImage::HFont font, int ch, int &a, int &b, int &c) = 0;
	virtual int GetCharacterWidth(UnicodeTextImage::HFont font, int ch) = 0;
	virtual void GetTextSize(UnicodeTextImage::HFont font, const wchar_t *text, int &wide, int &tall) = 0;

	// notify icons?!?
	virtual VPANEL GetNotifyPanel() = 0;
	virtual void SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text) = 0;

	// plays a sound
	virtual void PlaySound(const char *fileName) = 0;

	//!! these functions should not be accessed directly, but only through other vgui items
	//!! need to move these to seperate interface
	virtual int GetPopupCount() = 0;
	virtual VPANEL GetPopup(int index) = 0;
	virtual bool ShouldPaintChildPanel(VPANEL childPanel) = 0;
	virtual bool RecreateContext(VPANEL panel) = 0;
	virtual void AddPanel(VPANEL panel) = 0;
	virtual void ReleasePanel(VPANEL panel) = 0;
	virtual void MovePopupToFront(VPANEL panel) = 0;
	virtual void MovePopupToBack(VPANEL panel) = 0;

	virtual void SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false) = 0;
	virtual void PaintTraverse(VPANEL panel) = 0;

	virtual void EnableMouseCapture(VPANEL panel, bool state) = 0;

	// returns the size of the workspace
	virtual void GetWorkspaceBounds(int &x, int &y, int &wide, int &tall) = 0;

	// gets the absolute coordinates of the screen (in windows space)
	virtual void GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall) = 0;

	// gets the base resolution used in proportional mode
	virtual void GetProportionalBase(int &width, int &height) = 0;

	virtual void CalculateMouseVisible() = 0;
	virtual bool NeedKBInput() = 0;

	virtual bool HasCursorPosFunctions() = 0;
	virtual void SurfaceGetCursorPos(int &x, int &y) = 0;
	virtual void SurfaceSetCursorPos(int x, int y) = 0;


	// SRC only functions!!!
	virtual void DrawTexturedPolygon(VGuiVertex *pVertices, int n) = 0;
	virtual int GetFontAscent(UnicodeTextImage::HFont font, wchar_t wch) = 0;

	// web browser
	virtual void SetAllowHTMLJavaScript(bool state) = 0;

	virtual void SetLanguage(const char *pchLang) = 0;

	virtual const char *GetLanguage() = 0;

	virtual bool DeleteTextureByID(int id) = 0;

	virtual void DrawUpdateRegionTextureBGRA(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall) = 0;

	virtual void DrawSetTextureBGRA(int id, const unsigned char *pchData, int wide, int tall) = 0;

	virtual void CreateBrowser(VPANEL panel, IHTMLResponses *pBrowser, bool bPopupWindow, const char *pchUserAgentIdentifier) = 0;

	virtual void RemoveBrowser(VPANEL panel, IHTMLResponses *pBrowser) = 0;

	virtual IHTMLChromeController *AccessChromeHTMLController() = 0;
};

#define VGUI_SURFACE_INTERFACE_VERSION "VGUI_Surface026"

class ILocalize : public IBaseInterface
{
public:
	virtual void Something1() = 0;
	virtual void Something2() = 0;
	virtual void Something3() = 0;

	// Converts UTF-8 to WString
	virtual int ConvertANSIToUnicode(const char *ansi, wchar_t *unicode, int unicodeBufferSizeInBytes) = 0;
};

#define VGUI_LOCALIZE_INTERFACE_VERSION "VGUI_Localize003"

}

namespace
{

bool g_bSurfaceLoaded = false;

// hw.dll or sw.dll on Windows, hw.so on Linux, hw.dylib on macOS.
CSysModule *g_hEngineModule = nullptr;
CSysModule *g_hVGuiModule = nullptr;

vgui2::ISurface *g_pVGuiSurface = nullptr;
vgui2::ILocalize *g_pVGuiLocalize = nullptr;

}

void UnicodeTextImage::initInterfaces()
{
	static bool bTriedToLoad = false;
	if (bTriedToLoad)
		return;

	// Surface needs to be loaded only once, even if it fails.
	bTriedToLoad = true;

#ifdef _WIN32

	HMODULE hw, sw, vgui2;

	// Try hardware engine (hw.dll)
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, "hw.dll", &hw);
	if (hw)
	{
		g_hEngineModule = reinterpret_cast<CSysModule *>(hw);
	}
	else
	{
		// Try software engine (sw.dll)
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, "sw.dll", &sw);
		if (sw)
		{
			g_hEngineModule = reinterpret_cast<CSysModule *>(sw);
		}
	}

	// Load vgui2.dll
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, "vgui2.dll", &vgui2);
	g_hVGuiModule = reinterpret_cast<CSysModule *>(vgui2);

#else
	
#if defined(OSX)
#define DLL_EXT ".dylib"
#elif defined(LINUX)
#define DLL_EXT ".so"
#endif

	// hw.so is guranteed to be loaded as it's the only engine library on Linux/macOS.
	g_hEngineModule = reinterpret_cast<CSysModule *>(dlopen("./hw" DLL_EXT, RTLD_NOW | RTLD_NOLOAD));
	g_hVGuiModule = reinterpret_cast<CSysModule *>(dlopen("./vgui2" DLL_EXT, RTLD_NOW | RTLD_NOLOAD));

#endif

	if (!g_hEngineModule)
	{
		ConsolePrint("UnicodeTextImage: Error: Engine module handle is null.\n");
		return;
	}

	if (!g_hVGuiModule)
	{
		ConsolePrint("UnicodeTextImage: Error: VGUI2 library handle is null.\n");
		return;
	}

	auto fnLoadInterface = [](const char *moduleName, CSysModule *pModule, const char *ifaceName, auto &pIface)
	{
		using IfaceType = typename std::remove_reference<decltype(pIface)>::type;

		CreateInterfaceFn fnFactory = Sys_GetFactory(pModule);
		if (!fnFactory)
		{
			gEngfuncs.Con_Printf("UnicodeTextImage: Error: %s doesn't export factory.\n", moduleName);
			return false;
		}

		pIface = static_cast<IfaceType>(fnFactory(ifaceName, nullptr));
		if (!pIface)
		{
			gEngfuncs.Con_Printf("UnicodeTextImage: Error: %s doesn't export %s.\n", moduleName, ifaceName);
			return false;
		}

		return true;
	};

	if (!fnLoadInterface("Engine", g_hEngineModule, VGUI_SURFACE_INTERFACE_VERSION, g_pVGuiSurface))
		return;

	if (!fnLoadInterface("VGUI2", g_hVGuiModule, VGUI_LOCALIZE_INTERFACE_VERSION, g_pVGuiLocalize))
		return;

	g_bSurfaceLoaded = true;
}

bool UnicodeTextImage::shouldFallback()
{
	return !g_bSurfaceLoaded;
}

UnicodeTextImage::HFont UnicodeTextImage::createFont(const char *fontName, int tall, int weight)
{
	initInterfaces();

	if (!g_bSurfaceLoaded)
		return INVALID_FONT;

#ifdef LINUX
	// On linux fonts are 2px taller than they should be
	tall -= 2;
#endif

	int flags = 0;
	if (tall >= MIN_AA_FONT_SIZE)
		flags |= vgui2::ISurface::FONTFLAG_ANTIALIAS;

	HFont font = g_pVGuiSurface->CreateFont();
	g_pVGuiSurface->AddGlyphSetToFont(font, fontName, tall, weight, 0, 0, flags, 0, 0);
	return font;
}

UnicodeTextImage::UnicodeTextImage() : BaseClass("")
{
	initInterfaces();
}

void UnicodeTextImage::getTextSize(int &wide, int &tall)
{
	if (shouldFallback())
	{
		BaseClass::getTextSize(wide, tall);
		return;
	}

	wide = 0;
	tall = g_pVGuiSurface->GetFontTall(m_Font);

	for (wchar_t ch : m_Text)
	{
		// Ignore linebreaks
		if (ch == L'\r' || ch == L'\n')
			continue;

		wide += g_pVGuiSurface->GetCharacterWidth(m_Font, ch);
	}
}

void UnicodeTextImage::getTextSizeWrapped(int &wide, int &tall)
{
	assert(!("UnicodeTextImage::getTextSizeWrapped is not supported"));
}

UnicodeTextImage::HFont UnicodeTextImage::getUnicodeFont()
{
	return m_Font;
}

void UnicodeTextImage::setText(int textBufferLen, const char *text)
{
	if (shouldFallback())
	{
		BaseClass::setText(textBufferLen, text);
		return;
	}

	if (!text || !text[0])
	{
		m_Text.clear();
	}
	else
	{
		std::vector<wchar_t> wbuf(textBufferLen + 1);
		g_pVGuiLocalize->ConvertANSIToUnicode(text, wbuf.data(), wbuf.size() * sizeof(wchar_t));
		m_Text = wbuf.data();
	}

	// Update image size
	int wide, tall;
	getTextSize(wide, tall);
	setSize(wide, tall);
}

void UnicodeTextImage::setText(const char *text)
{
	if (shouldFallback())
	{
		BaseClass::setText(text);
		return;
	}

	if (!text || !text[0])
	{
		setText(1, "");
		return;
	}

	setText(strlen(text) + 1, text);
}

void UnicodeTextImage::setFont(HFont font, vgui::Font *fallbackFont)
{
	m_Font = font;
	BaseClass::setFont(fallbackFont);
}

void UnicodeTextImage::setFont(vgui::Scheme::SchemeFont schemeFont)
{
	assert(!("UnicodeTextImage::setFont(SchemeFont) is not supported"));
}

void UnicodeTextImage::setFont(vgui::Font *font)
{
	assert(!("UnicodeTextImage::setFont(Font *) is not supported"));
}

void UnicodeTextImage::setPos(int x, int y)
{
	BaseClass::setPos(x, y);
}

void UnicodeTextImage::paint(vgui::Panel *panel)
{
	if (shouldFallback())
	{
		BaseClass::paint(panel);

		// Draw image bounds (good for debugging)
#if 0
		{
			int x0, y0;
			getPos(x0, y0);

			vgui::Color color;
			int r, g, b, a;
			getColor(color);
			color.getColor(r, g, b, a);

			int wide, tall;
			BaseClass::getTextSize(wide, tall);

			g_pVGuiSurface->DrawSetColor(r, g, b, 255);
			g_pVGuiSurface->DrawOutlinedRect(x0, y0, x0 + wide, y0 + tall);
		}
#endif

		return;
	}

	// Can't have no font and a string to draw
	assert(m_Font != INVALID_FONT || m_Text.empty());

	if (m_Font == INVALID_FONT)
	{
		return;
	}

	// HACK: Drawing text after images may use incorrect texture.
	//
	// Explanation (pseudocode):
	// vgui1_drawSetTexture(id) {
	//   updateOpenGLTexture(id);
	// }
	//
	// vgui2_DrawSetTexture(id) {	// This one used by VGUI2 fonts
	//   if (internalTextureId == id) {
	//     return;
	//   }
	//   internalTextureId = id;
	//   vgui1_drawSetTexture(id);
	// }
	//
	// Updating VGUI1 texture causes VGUI2 to think that texture hasn't changed.

	// Call DrawSetTexture twice to make sure that it's updated to 0.
	g_pVGuiSurface->DrawSetTexture(1);
	g_pVGuiSurface->DrawSetTexture(0);

	vgui::Color color;
	int r, g, b, a;
	getColor(color);
	color.getColor(r, g, b, a);
	g_pVGuiSurface->DrawSetTextColor(r, g, b, 255);
	g_pVGuiSurface->DrawSetTextFont(m_Font);

	int x, y;
	getPos(x, y);

	bool bRainbowEnabled = gHUD.m_Rainbow.IsEnabled();

	for (int i = 0; i < m_Text.size(); i++)
	{
		wchar_t ch = m_Text[i];

		// Ignore linebreaks
		if (ch == L'\r' || ch == L'\n')
			continue;

		if (bRainbowEnabled)
		{
			gHUD.m_Rainbow.GetRainbowColor(x, y, r, g, b);
			g_pVGuiSurface->DrawSetTextColor(r, g, b, 255);
		}

		g_pVGuiSurface->DrawSetTextPos(x, y);
		g_pVGuiSurface->DrawUnicodeChar(ch);
		x += g_pVGuiSurface->GetCharacterWidth(m_Font, ch);
	}

	// Draw image bounds (good for debugging)
#if 0
	{
		int x0, y0;
		getPos(x0, y0);
		g_pVGuiSurface->DrawSetColor(r, g, b, 255);
		g_pVGuiSurface->DrawOutlinedRect(x0, y0, x, y0 + g_pVGuiSurface->GetFontTall(m_Font));
	}
#endif
}
