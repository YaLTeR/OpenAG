#include <algorithm>
#include "hud.h"
#include "cl_util.h"
#include "rainbow.h"

#undef min
#undef max

void CRainbow::Init()
{
    // Register cvars
    m_pCvarRainbow = CVAR_CREATE("hud_rainbow", "0", FCVAR_ARCHIVE);
    m_pCvarRainbowS = CVAR_CREATE("hud_rainbow_sat", "100", FCVAR_ARCHIVE);
    m_pCvarRainbowV = CVAR_CREATE("hud_rainbow_val", "100", FCVAR_ARCHIVE);
    m_pCvarRainbowSpeed = CVAR_CREATE("hud_rainbow_speed", "40", FCVAR_ARCHIVE);
    m_pCvarRainbowXPhase = CVAR_CREATE("hud_rainbow_xphase", "0.4", FCVAR_ARCHIVE);
    m_pCvarRainbowYPhase = CVAR_CREATE("hud_rainbow_yphase", "0.7", FCVAR_ARCHIVE);

    // Save engine functions
    m_pfnSPR_Set = gEngfuncs.pfnSPR_Set;
    m_pfnSPR_DrawAdditive = gEngfuncs.pfnSPR_DrawAdditive;
    m_pfnDrawString = gEngfuncs.pfnDrawString;
    m_pfnDrawStringReverse = gEngfuncs.pfnDrawStringReverse;
    m_pfnDrawConsoleString = gEngfuncs.pfnDrawConsoleString;
    m_pfnFillRGBA = gEngfuncs.pfnFillRGBA;
}

void CRainbow::Think()
{
    if (m_pCvarRainbow->value != 0 && !m_bIsEnabled)
        HookFuncs();
    else if (m_pCvarRainbow->value == 0 && m_bIsEnabled)
        UnhookFuncs();

    if (m_bIsEnabled)
    {
        // Update saturation and value and clamp them in [0; 100]
        m_flSat = m_pCvarRainbowS->value;
        m_flSat = std::min(m_flSat, 100.f);
        m_flSat = std::max(m_flSat, 0.f);
        m_flVal = m_pCvarRainbowV->value;
        m_flVal = std::min(m_flVal, 100.f);
        m_flVal = std::max(m_flVal, 0.f);
    }
}

void CRainbow::GetRainbowColor(int x, int y, int &r, int &g, int &b)
{
    float phase = m_pCvarRainbowSpeed->value * gHUD.m_flTime;
    phase += m_pCvarRainbowXPhase->value * x;
    phase += m_pCvarRainbowYPhase->value * y;
    phase = fmod(phase, 360);
    if (phase < 0)
        phase += 360;

    HSVtoRGB(phase, m_flSat, m_flVal, r, g, b);
}

void CRainbow::HookFuncs()
{
    gEngfuncs.pfnSPR_Set = &SPR_SetRainbow;
    gEngfuncs.pfnSPR_DrawAdditive = &SPR_DrawAdditiveRainbow;
    gEngfuncs.pfnDrawString = &DrawString;
    gEngfuncs.pfnDrawStringReverse = &DrawStringReverse;
    gEngfuncs.pfnDrawConsoleString = &DrawConsoleString;
    gEngfuncs.pfnFillRGBA = &FillRGBARainbow;
    m_bIsEnabled = true;
}

void CRainbow::UnhookFuncs()
{
    gEngfuncs.pfnSPR_Set = m_pfnSPR_Set;
    gEngfuncs.pfnSPR_DrawAdditive = m_pfnSPR_DrawAdditive;
    gEngfuncs.pfnDrawString = m_pfnDrawString;
    gEngfuncs.pfnDrawStringReverse = m_pfnDrawStringReverse;
    gEngfuncs.pfnDrawConsoleString = m_pfnDrawConsoleString;
    gEngfuncs.pfnFillRGBA = m_pfnFillRGBA;
    m_bIsEnabled = false;
}

void CRainbow::SPR_SetRainbow(HSPRITE hPic, int r, int g, int b)
{
    // Remember params for the future since coords are not known until rendering
    gHUD.m_Rainbow.m_hSprite = hPic;
    gHUD.m_Rainbow.m_iSpriteColor[0] = r;
    gHUD.m_Rainbow.m_iSpriteColor[1] = g;
    gHUD.m_Rainbow.m_iSpriteColor[2] = b;
}

void CRainbow::SPR_DrawAdditiveRainbow(int frame, int x, int y, const rect_s *prc)
{
    int color[3];
    std::copy(gHUD.m_Rainbow.m_iSpriteColor, gHUD.m_Rainbow.m_iSpriteColor + 3, color);
    gHUD.m_Rainbow.GetRainbowColor(x, y, color[0], color[1], color[2]);
    gHUD.m_Rainbow.m_pfnSPR_Set(gHUD.m_Rainbow.m_hSprite, color[0], color[1], color[2]);
    gHUD.m_Rainbow.m_pfnSPR_DrawAdditive(frame, x, y, prc);
}

int CRainbow::DrawString(int x, int y, const char *str, int r, int g, int b)
{
    if (r == 0 && g == 0 && b == 0)
    {
        // Draw invisible text without rainbow color
        return gHUD.m_Rainbow.m_pfnDrawString(x, y, str, r, g, b);
    }
    else
    {
        return DrawRainbowString(x, y, str, gHUD.m_Rainbow.m_pfnDrawString);
    }
}

int CRainbow::DrawStringReverse(int x, int y, const char *str, int r, int g, int b)
{
    // Get string width
    int width = gHUD.GetHudStringWidth(str);

    // Draw it shifted to the left by width pixels
    return x + DrawString(x - width, y, str, r, g, b);
}

int CRainbow::DrawConsoleString(int x, int y, const char *string)
{
    return x + DrawRainbowString(x, y, string, [](int x, int y, const char *str, int r, int g, int b) {
        gEngfuncs.pfnDrawSetTextColor(r * 255.f, g * 255.f, b * 255.f);
        return gHUD.m_Rainbow.m_pfnDrawConsoleString(x, y, str) - x;
    });
}

void CRainbow::FillRGBARainbow(int x, int y, int width, int height, int r, int g, int b, int a)
{
    gHUD.m_Rainbow.GetRainbowColor(x, y, r, g, b);
    gHUD.m_Rainbow.m_pfnFillRGBA(x, y, width, height, r, g, b, a);
}

int CRainbow::DrawRainbowString(int x, int y, const char *str, const DrawStringFn &func)
{
    int i = 0;
    int r, g, b;
    int width = x;
    char buf[5]; // 4 UTF-8 bytes + null terminator
    while (str[i] != '\0')
    {
        char c = str[i];

        if ((c & 0b1110'0000) == 0b1100'0000)
        {
            // Two bytes
            buf[0] = str[i];
            buf[1] = str[i + 1];
            buf[2] = '\0';
            i += 2;
        }
        else if ((c & 0b1111'0000) == 0b1110'0000)
        {
            // Three bytes
            buf[0] = str[i];
            buf[1] = str[i + 1];
            buf[2] = str[i + 2];
            buf[3] = '\0';
            i += 3;
        }
        else if ((c & 0b1111'1000) == 0b1111'0000)
        {
            // Four bytes
            buf[0] = str[i];
            buf[1] = str[i + 1];
            buf[2] = str[i + 2];
            buf[3] = str[i + 3];
            buf[4] = '\0';
            i += 4;
        }
        else
        {
            // One byte or invalid (assume one byte)
            buf[0] = c;
            buf[1] = '\0';
            i += 1;
        }

        gHUD.m_Rainbow.GetRainbowColor(width, y, r, g, b);
        width += func(width, y, buf, r, g, b);
    }

    return width - x;
}

void CRainbow::HSVtoRGB(float H, float S, float V, int &R, int &G, int &B)
{
    // https://www.codespeedy.com/hsv-to-rgb-in-cpp/
    assert(H >= 0 && H <= 360 && S >= 0 && S <= 100 && V >= 0 && V <= 100);
    float s = S / 100;
    float v = V / 100;
    float C = s * v;
    float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
    float m = v - C;
    float r, g, b;

    if (H >= 0 && H < 60)
    {
        r = C, g = X, b = 0;
    }
    else if (H >= 60 && H < 120)
    {
        r = X, g = C, b = 0;
    }
    else if (H >= 120 && H < 180)
    {
        r = 0, g = C, b = X;
    }
    else if (H >= 180 && H < 240)
    {
        r = 0, g = X, b = C;
    }
    else if (H >= 240 && H < 300)
    {
        r = X, g = 0, b = C;
    }
    else
    {
        r = C, g = 0, b = X;
    }

    R = (r + m) * 255;
    G = (g + m) * 255;
    B = (b + m) * 255;
}
