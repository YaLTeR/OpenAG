#ifndef RAINBOW_H
#define RAINBOW_H
#include <functional>

class CRainbow
{
public:
	void Init();
	void Think();

	/**
	 * Returns whether Rainbow HUD is enabled.
	 */
	inline bool IsEnabled()
	{
		return m_bIsEnabled;
	}

	/**
	 * Converts input color to rainbow.
	 * RGB input must be valid!
	 */
	void GetRainbowColor(int x, int y, int &r, int &g, int &b);

private:
	/**
	 * Function that draws an input string at input position with input color.
	 * @returns Width the string
	 */
	using DrawStringFn = std::function<int(int x, int y, const char *buf, int r, int g, int b)>;

	bool m_bIsEnabled = false;
	float m_flSat = 100;
	float m_flVal = 100;
	cvar_t *m_pCvarRainbow = nullptr;
	cvar_t *m_pCvarRainbowS = nullptr;
	cvar_t *m_pCvarRainbowV = nullptr;
	cvar_t *m_pCvarRainbowSpeed = nullptr;
	cvar_t *m_pCvarRainbowXPhase = nullptr;
	cvar_t *m_pCvarRainbowYPhase = nullptr;

	HSPRITE m_hSprite = 0;
	int m_iSpriteColor[3] = { 0, 0, 0 };

	pfnEngSrc_pfnSPR_Set_t m_pfnSPR_Set = nullptr;
	pfnEngSrc_pfnSPR_DrawAdditive_t m_pfnSPR_DrawAdditive = nullptr;
	pfnEngSrc_pfnDrawString_t m_pfnDrawString = nullptr;
	pfnEngSrc_pfnDrawStringReverse_t m_pfnDrawStringReverse = nullptr;
	pfnEngSrc_pfnDrawConsoleString_t m_pfnDrawConsoleString = nullptr;
	pfnEngSrc_pfnFillRGBA_t m_pfnFillRGBA = nullptr;

	/**
	 * Overwrites some gEngfuncs members with color-changing wrappers.
	 * Makes the player happier.
	 */
	void HookFuncs();

	/**
	 * Restores original gEngfuncs.
	 */
	void UnhookFuncs();

	static void SPR_SetRainbow(HSPRITE hPic, int r, int g, int b);
	static void SPR_DrawAdditiveRainbow(int frame, int x, int y, const struct rect_s *prc);
	static int DrawString(int x, int y, const char *str, int r, int g, int b);
	static int DrawStringReverse(int x, int y, const char *str, int r, int g, int b);
	static int DrawConsoleString(int x, int y, const char *string);
	static void FillRGBARainbow(int x, int y, int width, int height, int r, int g, int b, int a);

	/**
	 * Draws a string using specified drawing func.
	 * func will be called for every character with a new color.
	 */
	static int DrawRainbowString(int x, int y, const char *str, const DrawStringFn &func);

	/**
	 * Converts color from HSV color space to RGB
	 * @param	H	Hue, [0, 360]
	 * @param	S	Saturation, [0, 100]
	 * @param	V	Value, [0, 100]
	 * @param	R	Red output, [0, 255]
	 * @param	G	Green output, [0, 255]
	 * @param	B	Blue output,  [0, 255]
	 */
	static void HSVtoRGB(float H, float S, float V, int &R, int &G, int &B);

	
};

#endif
