/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// hud_redraw.cpp
//
#include <math.h>
#include "hud.h"
#include "cl_util.h"
#include "bench.h"

#include "vgui_TeamFortressViewport.h"

#define MAX_LOGO_FRAMES 56

int grgLogoFrame[MAX_LOGO_FRAMES] = 
{
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13, 13, 13, 13, 13, 12, 11, 10, 9, 8, 14, 15,
	16, 17, 18, 19, 20, 20, 20, 20, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
	29, 29, 29, 29, 29, 28, 27, 26, 25, 24, 30, 31 
};



float HUD_GetFOV( void );

extern cvar_t *sensitivity;

// Think
void CHud::Think(void)
{
	m_scrinfo.iSize = sizeof(m_scrinfo);
	GetScreenInfo(&m_scrinfo);

	m_Rainbow.Think();

	int newfov;
	HUDLIST *pList = m_pHudList;

	while (pList)
	{
		if (pList->p->m_iFlags & HUD_ACTIVE)
			pList->p->Think();
		pList = pList->pNext;
	}

	newfov = HUD_GetFOV();
	if ( newfov == 0 )
	{
		m_iFOV = default_fov->value;
	}
	else
	{
		m_iFOV = newfov;
	}

	// the clients fov is actually set in the client data update section of the hud

	// Set a new sensitivity
	if ( m_iFOV == default_fov->value )
	{  
		// reset to saved sensitivity
		m_flMouseSensitivity = 0;
	}
	else
	{  
		// set a new sensitivity that is proportional to the change from the FOV default
		m_flMouseSensitivity = sensitivity->value * ((float)newfov / (float)default_fov->value) * CVAR_GET_FLOAT("zoom_sensitivity_ratio");
	}

	// think about default fov
	if ( m_iFOV == 0 )
	{  // only let players adjust up in fov,  and only if they are not overriden by something else
		m_iFOV = max( default_fov->value, 90 );  
	}

	// Don't change the FOV for HLTV to a constant, use the updated value like when in a normal game
	/*if ( gEngfuncs.IsSpectateOnly() )
	{
		m_iFOV = gHUD.m_Spectator.GetFOV();
	}*/

	Bench_CheckStart();
}

// Redraw
// step through the local data,  placing the appropriate graphics & text as appropriate
// returns 1 if they've changed, 0 otherwise
int CHud :: Redraw( float flTime, int intermission )
{
	m_fOldTime = m_flTime;	// save time of previous redraw
	m_flTime = flTime;
	m_flTimeDelta = (double)m_flTime - m_fOldTime;
	static float m_flShotTime = 0;
	static float m_flStopTime = 0;

	// Clock was reset, reset delta
	if ( m_flTimeDelta < 0 )
		m_flTimeDelta = 0;

	// Bring up the scoreboard during intermission
	if (gViewPort)
	{
		if ( m_iIntermission && !intermission )
		{
			// Have to do this here so the scoreboard goes away
			m_iIntermission = intermission;
			gViewPort->HideCommandMenu();
			gViewPort->HideScoreBoard();
			gViewPort->UpdateSpectatorPanel();
		}
		else if ( !m_iIntermission && intermission )
		{
			m_iIntermission = intermission;
			gViewPort->HideCommandMenu();
			gViewPort->HideVGUIMenu();
			gViewPort->ShowScoreBoard();
			gViewPort->UpdateSpectatorPanel();

			// Take a screenshot if the client's got the cvar set
			if ( CVAR_GET_FLOAT( "hud_takesshots" ) != 0 )
				m_flShotTime = flTime + 1.0;	// Take a screenshot in a second

			if ( m_pCvarAutostop->value > 0.0f )
				m_flStopTime = flTime + 3.0; // Stop demo recording in three seconds
		}
	}

	if (m_flShotTime && m_flShotTime < flTime)
	{
		gEngfuncs.pfnClientCmd("snapshot\n");
		m_flShotTime = 0;
	}

	if (m_flStopTime && m_flStopTime < flTime)
	{
		gEngfuncs.pfnClientCmd("stop");
		m_flStopTime = 0;
	}

	m_iIntermission = intermission;

	// if no redrawing is necessary
	// return 0;

	UpdateDefaultHUDColor();
	
	// draw all registered HUD elements
	if ( m_pCvarDraw->value )
	{
		HUDLIST *pList = m_pHudList;

		while (pList)
		{
			if ( !Bench_Active() )
			{
				if ( !intermission )
				{
					if ( (pList->p->m_iFlags & HUD_ACTIVE) && !(m_iHideHUDDisplay & HIDEHUD_ALL) )
						pList->p->Draw(flTime);
				}
				else
				{  // it's an intermission,  so only draw hud elements that are set to draw during intermissions
					if ( pList->p->m_iFlags & HUD_INTERMISSION )
						pList->p->Draw( flTime );
				}
			}
			else
			{
				if ( ( pList->p == &m_Benchmark ) &&
					 ( pList->p->m_iFlags & HUD_ACTIVE ) &&
					 !( m_iHideHUDDisplay & HIDEHUD_ALL ) )
				{
					pList->p->Draw(flTime);
				}
			}

			pList = pList->pNext;
		}
	}
	else
	{
		// Hack to draw some HUDs even when hud_draw is 0.
		if (!Bench_Active()
			&& !intermission
			&& !(m_iHideHUDDisplay & HIDEHUD_ALL))
		{
			if (m_Crosshairs.m_iFlags & HUD_ACTIVE)
				m_Crosshairs.Draw(flTime);

			if (m_Speedometer.m_iFlags & HUD_ACTIVE)
				m_Speedometer.Draw(flTime);
				
			if (m_StrafeGuide.m_iFlags & HUD_ACTIVE)
				m_StrafeGuide.Draw(flTime);

			if (m_Jumpspeed.m_iFlags & HUD_ACTIVE)
				m_Jumpspeed.Draw(flTime);

			if (gHUD.m_pCvarDrawDeathNoticesAlways->value != 0.0f
				&& m_DeathNotice.m_iFlags & HUD_ACTIVE)
				m_DeathNotice.Draw(flTime);
				
			if (gHUD.m_pCvarDrawMessagesAlways->value != 0.0f
				&& m_Message.m_iFlags & HUD_ACTIVE)
				m_Message.Draw(flTime);
		}
	}

	// are we in demo mode? do we need to draw the logo in the top corner?
	if (m_iLogo)
	{
		int x, y, i;

		if (m_hsprLogo == 0)
			m_hsprLogo = LoadSprite("sprites/%d_logo.spr");

		SPR_Set(m_hsprLogo, 250, 250, 250 );
		
		x = SPR_Width(m_hsprLogo, 0);
		x = ScreenWidth - x;
		y = SPR_Height(m_hsprLogo, 0)/2;

		// Draw the logo at 20 fps
		int iFrame = (int)(flTime * 20) % MAX_LOGO_FRAMES;
		i = grgLogoFrame[iFrame] - 1;

		SPR_DrawAdditive(i, x, y, NULL);
	}

	/*
	if ( g_iVisibleMouse )
	{
		void IN_GetMousePos( int *mx, int *my );
		int mx, my;

		IN_GetMousePos( &mx, &my );
		
		if (m_hsprCursor == 0)
		{
			char sz[256];
			sprintf( sz, "sprites/cursor.spr" );
			m_hsprCursor = SPR_Load( sz );
		}

		SPR_Set(m_hsprCursor, 250, 250, 250 );
		
		// Draw the logo at 20 fps
		SPR_DrawAdditive( 0, mx, my, NULL );
	}
	*/

	return 1;
}

void ScaleColors( int &r, int &g, int &b, int a )
{
	float x = (float)a / 255;
	r = (int)(r * x);
	g = (int)(g * x);
	b = (int)(b * x);
}

void CHud::UpdateDefaultHUDColor()
{
	int r, g, b;

	if (sscanf(m_pCvarColor->string, "%d %d %d", &r, &g, &b) == 3) {
		r = max(r, 0);
		g = max(g, 0);
		b = max(b, 0);

		r = min(r, 255);
		g = min(g, 255);
		b = min(b, 255);

		m_iDefaultHUDColor = (r << 16) | (g << 8) | b;
	} else {
		m_iDefaultHUDColor = RGB_YELLOWISH;
	}
}

int CHud :: DrawHudString(int xpos, int ypos, int iMaxX, const char *szIt, int r, int g, int b )
{
	return xpos + gEngfuncs.pfnDrawString( xpos, ypos, szIt, r, g, b);
}

int CHud :: DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b )
{
	char szString[32];
	sprintf( szString, "%d", iNumber );
	return DrawHudStringReverse( xpos, ypos, iMinX, szString, r, g, b );

}

int CHud :: DrawHudNumberStringFixed( int xpos, int ypos, int iNumber, int r, int g, int b )
{
	char szString[32];
	sprintf( szString, "%d", iNumber );
	return DrawHudStringRightAligned( xpos, ypos, szString, r, g, b );
}

// draws a string from right to left (right-aligned)
int CHud :: DrawHudStringReverse( int xpos, int ypos, int iMinX, const char *szString, int r, int g, int b )
{
	return xpos - gEngfuncs.pfnDrawStringReverse( xpos, ypos, szString, r, g, b);
}

int CHud :: DrawHudNumber( int x, int y, int iFlags, int iNumber, int r, int g, int b)
{
	int iWidth = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;
	int k;
	
	if (iNumber > 0)
	{
		// SPR_Draw 100's
		if (iNumber >= 100)
		{
			 k = iNumber/100;
			SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_3DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw 10's
		if (iNumber >= 10)
		{
			k = (iNumber % 100)/10;
			SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		k = iNumber % 10;
		SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
		SPR_DrawAdditive(0,  x, y, &GetSpriteRect(m_HUD_number_0 + k));
		x += iWidth;
	} 
	else if (iFlags & DHN_DRAWZERO) 
	{
		SPR_Set(GetSprite(m_HUD_number_0), r, g, b );

		// SPR_Draw 100's
		if (iFlags & (DHN_3DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		
		SPR_DrawAdditive( 0,  x, y, &GetSpriteRect(m_HUD_number_0));
		x += iWidth;
	}

	return x;
}

static constexpr int ten_powers[] = {
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	1000000000
};

int CHud::DrawHudNumber(int x, int y, int number, int r, int g, int b)
{
	auto digit_width = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;
	auto digit_count = count_digits(number);

	for (int i = digit_count; i > 0; --i) {
		int digit = number / ten_powers[i - 1];

		SPR_Set(GetSprite(m_HUD_number_0 + digit), r, g, b);
		SPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + digit));
		x += digit_width;

		number -= digit * ten_powers[i - 1];
	}

	return x;
}

int CHud::DrawHudNumberCentered(int x, int y, int number, int r, int g, int b)
{
	auto digit_width = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;
	auto digit_count = count_digits(number);

	return DrawHudNumber(x - (digit_width * digit_count) / 2, y, number, r, g, b);
}

int CHud::GetNumWidth( int iNumber, int iFlags )
{
	if (iFlags & (DHN_3DIGITS))
		return 3;

	if (iFlags & (DHN_2DIGITS))
		return 2;

	if (iNumber <= 0)
	{
		if (iFlags & (DHN_DRAWZERO))
			return 1;
		else
			return 0;
	}

	if (iNumber < 10)
		return 1;

	if (iNumber < 100)
		return 2;

	return 3;

}

int CHud::DrawHudStringCentered(int x, int y, const char* string, int r, int g, int b)
{
	auto width = GetHudStringWidth(string);
	return x + gEngfuncs.pfnDrawString(x - width / 2, y, string, r, g, b);
}

int CHud::DrawHudStringRightAligned(int x, int y, const char* string, int r, int g, int b)
{
	auto width = GetHudStringWidth(string);
	gEngfuncs.pfnDrawString(x - width, y, string, r, g, b);
	return x;
}

int CHud::GetHudStringWidth(const char* string)
{
	return gEngfuncs.pfnDrawString(0, 0, string, 0, 0, 0);
}

int CHud::DrawHudStringWithColorTags(int x, int y, char* string, int default_r, int default_g, int default_b)
{
	color_tags::for_each_colored_substr(string, [=, &x](const char* string, bool custom_color, int r, int g, int b) {
		if (!custom_color) {
			r = default_r;
			g = default_g;
			b = default_b;
		}

		x += gEngfuncs.pfnDrawString(x, y, string, r, g, b);
	});

	return x;
}

int CHud::DrawHudStringCenteredWithColorTags(int x, int y, char* string, int r, int g, int b)
{
	auto width = GetHudStringWidthWithColorTags(string);
	return DrawHudStringWithColorTags(x - width / 2, y, string, r, g, b);
}

int CHud::GetHudStringWidthWithColorTags(const char* string)
{
	return gEngfuncs.pfnDrawString(0, 0, color_tags::strip_color_tags_thread_unsafe(string), 0, 0, 0);
}

int CHud::DrawConsoleStringWithColorTags(int x, int y, char* string, bool use_default_color, float default_r, float default_g, float default_b)
{
	color_tags::for_each_colored_substr(string, [=, &x](const char* string, bool custom_color, int r, int g, int b) {
		if (custom_color)
			gEngfuncs.pfnDrawSetTextColor(r / 255.0f, g / 255.0f, b / 255.0f);
		else if (use_default_color)
			gEngfuncs.pfnDrawSetTextColor(default_r, default_g, default_b);

		x = gEngfuncs.pfnDrawConsoleString(x, y, string);
	});

	return x;
}

void CHud::GetConsoleStringSizeWithColorTags(char* string, int& width, int& height)
{
	gEngfuncs.pfnDrawConsoleStringLen(color_tags::strip_color_tags_thread_unsafe(string), &width, &height);
}

