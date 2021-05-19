/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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
// saytext.cpp
//
// implementation of CHudSayText class
//

#include "hud.h"
#include <ctime>
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

#include "vgui_TeamFortressViewport.h"
#include "discord_integration.h"

extern float *GetClientColor( int clientIndex );

#define MAX_LINES	5
#define MAX_CHARS_PER_LINE	256  /* it can be less than this, depending on char size */

// allow 20 pixels on either side of the text
#define MAX_LINE_WIDTH  ( ScreenWidth - 40 )
#define LINE_START  10
static float SCROLL_SPEED = 5;

static char g_szLineBuffer[ MAX_LINES + 1 ][ MAX_CHARS_PER_LINE ];
static float *g_pflNameColors[ MAX_LINES + 1 ];
static int g_iNameLengths[ MAX_LINES + 1 ];
static float flScrollTime = 0;  // the time at which the lines next scroll up

static int Y_START = 0;
static int line_height = 0;

DECLARE_MESSAGE( m_SayText, SayText );

int CHudSayText :: Init( void )
{
	gHUD.AddHudElem( this );

	HOOK_MESSAGE( SayText );

	InitHUDData();

	m_HUD_saytext =			gEngfuncs.pfnRegisterVariable( "hud_saytext", "1", 0 );
	m_HUD_saytext_time =	gEngfuncs.pfnRegisterVariable( "hud_saytext_time", "5", FCVAR_ARCHIVE );
	m_HUD_saytext_sound =	gEngfuncs.pfnRegisterVariable( "hud_saytext_sound", "0", FCVAR_ARCHIVE );

	m_iFlags |= HUD_INTERMISSION; // is always drawn during an intermission

	return 1;
}


void CHudSayText :: InitHUDData( void )
{
	memset( g_szLineBuffer, 0, sizeof g_szLineBuffer );
	memset( g_pflNameColors, 0, sizeof g_pflNameColors );
	memset( g_iNameLengths, 0, sizeof g_iNameLengths );
}

int CHudSayText :: VidInit( void )
{
	return 1;
}


int ScrollTextUp( void )
{
	g_szLineBuffer[MAX_LINES][0] = 0;
	memmove( g_szLineBuffer[0], g_szLineBuffer[1], sizeof(g_szLineBuffer) - sizeof(g_szLineBuffer[0]) ); // overwrite the first line
	memmove( &g_pflNameColors[0], &g_pflNameColors[1], sizeof(g_pflNameColors) - sizeof(g_pflNameColors[0]) );
	memmove( &g_iNameLengths[0], &g_iNameLengths[1], sizeof(g_iNameLengths) - sizeof(g_iNameLengths[0]) );
	g_szLineBuffer[MAX_LINES-1][0] = 0;

	if ( g_szLineBuffer[0][0] == ' ' ) // also scroll up following lines
	{
		g_szLineBuffer[0][0] = 2;
		return 1 + ScrollTextUp();
	}

	return 1;
}

int CHudSayText :: Draw( float flTime )
{
	int y = Y_START;

	if ( ( gViewPort && gViewPort->AllowedToPrintText() == FALSE) || !m_HUD_saytext->value )
		return 1;

	// make sure the scrolltime is within reasonable bounds,  to guard against the clock being reset
	flScrollTime = min( flScrollTime, flTime + m_HUD_saytext_time->value );

	// make sure the scrolltime is within reasonable bounds,  to guard against the clock being reset
	flScrollTime = min( flScrollTime, flTime + m_HUD_saytext_time->value );

	if ( flScrollTime <= flTime )
	{
		if ( *g_szLineBuffer[0] )
		{
			flScrollTime = flTime + m_HUD_saytext_time->value;
			// push the console up
			ScrollTextUp();
		}
		else
		{ // buffer is empty,  just disable drawing of this section
			m_iFlags &= ~HUD_ACTIVE;
		}
	}

	for ( int i = 0; i < MAX_LINES; i++ )
	{
		if ( *g_szLineBuffer[i] )
		{
			if ( *g_szLineBuffer[i] == 2 && g_pflNameColors[i] )
			{
				char buf[MAX_PLAYER_NAME_LENGTH + 32];

				// draw the first x characters in the player color
				strncpy( buf, g_szLineBuffer[i], min(g_iNameLengths[i], MAX_PLAYER_NAME_LENGTH + 31) );
				buf[ min(g_iNameLengths[i], MAX_PLAYER_NAME_LENGTH + 31) ] = 0;
				int x = gHUD.DrawConsoleStringWithColorTags(
					LINE_START,
					y,
					buf + 1, // don't draw the control code at the start
					true,
					g_pflNameColors[i][0],
					g_pflNameColors[i][1],
					g_pflNameColors[i][2]
				);

				// color is reset after each string draw
				DrawConsoleString( x, y, g_szLineBuffer[i] + g_iNameLengths[i] );
			}
			else
			{
				// normal draw
				DrawConsoleString( LINE_START, y, g_szLineBuffer[i] );
			}
		}

		y += line_height;
	}

	return 1;
}

int CHudSayText :: MsgFunc_SayText( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int client_index = READ_BYTE();		// the client who spoke the message
	auto message = READ_STRING();
	SayTextPrint( message, iSize - 1,  client_index );

	return 1;
}

/*
 * Copies at most count characters (including the terminating null character)
 * from src to dest, replacing the location tags with the location names.
 * The resulting array is always null-terminated except when count == 0.
 */
static void convert_locations(char* dest, const char* src, size_t count, int player_id)
{
	if (count == 0)
		return;

	if (count == 1) {
		dest[0] = '\0';
		return;
	}

	size_t i = 0;

	for (; *src != '\0'; ++src) {
		if (src[0] == '%' && (src[1] == 'l' || src[1] == 'L' || src[1] == 'd' || src[1] == 'D')) {
			auto loc = gHUD.m_Location.get_player_location(player_id).c_str();
			auto loc_len = strlen(loc);
			auto bytes_to_copy = min(loc_len, count - i - 1);

			memcpy(&dest[i], loc, bytes_to_copy);
			i += bytes_to_copy;

			if (i + 1 == count)
				break;

			++src;
		} else {
			dest[i++] = *src;

			if (i + 1 == count)
				break;
		}
	}

	dest[i] = '\0';
}

void CHudSayText :: SayTextPrint( const char *pszBuf, int iBufSize, int clientIndex )
{
	if ( gViewPort && gViewPort->AllowedToPrintText() == FALSE )
	{
		// Print it straight to the console
		ConsolePrint( pszBuf );
		return;
	}

	int i;
	// find an empty string slot
	for ( i = 0; i < MAX_LINES; i++ )
	{
		if ( ! *g_szLineBuffer[i] )
			break;
	}
	if ( i == MAX_LINES )
	{
		// force scroll buffer up
		ScrollTextUp();
		i = MAX_LINES - 1;
	}

	g_iNameLengths[i] = 0;
	g_pflNameColors[i] = NULL;

	// if it's a say message, search for the players name in the string
	if ( *pszBuf == 2 && clientIndex > 0 )
	{
		gEngfuncs.pfnGetPlayerInfo( clientIndex, &g_PlayerInfoList[clientIndex] );
		const char *pName = g_PlayerInfoList[clientIndex].name;

		if ( pName )
		{
			const char *nameInString = strstr( pszBuf, pName );

			if ( nameInString )
			{
				g_iNameLengths[i] = strlen( pName ) + (nameInString - pszBuf);
				g_pflNameColors[i] = GetClientColor( clientIndex );
			}
		}
	}

	convert_locations( g_szLineBuffer[i], pszBuf, MAX_CHARS_PER_LINE, clientIndex );

	char timestamp[16];
	std::time_t curtime = std::time(nullptr);
	auto written = std::strftime(timestamp, sizeof(timestamp), "[%H:%M:%S] ", std::localtime(&curtime));

	if(written > 0)
		ConsolePrint( timestamp );

	ConsolePrint( g_szLineBuffer[i] );

	// make sure the text fits in one line
	EnsureTextFitsInOneLineAndWrapIfHaveTo( i );

	// Set scroll time
	if ( i == 0 )
	{
		flScrollTime = gHUD.m_flTime + m_HUD_saytext_time->value;
	}

	m_iFlags |= HUD_ACTIVE;

	if (m_HUD_saytext_sound->value != 0.0f)
		PlaySound( "misc/talk.wav", 1 );

	Y_START = ScreenHeight - 60 - ( line_height * (MAX_LINES+2) );
}

void CHudSayText :: EnsureTextFitsInOneLineAndWrapIfHaveTo( int line )
{
	int line_width = 0;
	GetConsoleStringSize( g_szLineBuffer[line], &line_width, &line_height );

	if ( (line_width + LINE_START) > MAX_LINE_WIDTH )
	{ // string is too long to fit on line
		// scan the string until we find what word is too long,  and wrap the end of the sentence after the word
		int length = LINE_START;
		int tmp_len = 0;
		char *last_break = NULL;

		int current_color = 0;
		int color_before_last_break = 0;
		for ( char *x = g_szLineBuffer[line]; *x != 0; x++ )
		{
			// check for a color change, if so skip past it
			if ( x[0] == '/' && x[1] == '(' )
			{
				x += 2;
				// skip forward until past mode specifier
				while ( *x != 0 && *x != ')' )
					x++;

				if ( *x != 0 )
					x++;

				if ( *x == 0 )
					break;
			}

			// Skip past the color tags.
			if (x[0] == '^' && x[1] >= '0' && x[1] <= '9') {
				if (g_szLineBuffer[line][0] != 2 || g_szLineBuffer[line] + g_iNameLengths[line] < x) {
					current_color = x[1] - '0';
					if (current_color == 9)
						current_color = 0;
				}

				x += 2;

				if (*x == 0)
					break;
			}

			// Skip past the control character at the start.
			if (x[0] == 2) {
				++x;

				if (*x == 0)
					break;
			}

			char buf[2];
			buf[1] = 0;

			if (*x == ' ' && x != g_szLineBuffer[line]) {  // store each line break,  except for the very first character
				last_break = x;
				color_before_last_break = current_color;
			}

			buf[0] = *x;  // get the length of the current character
			GetConsoleStringSize( buf, &tmp_len, &line_height );
			length += tmp_len;

			if ( length > MAX_LINE_WIDTH )
			{  // needs to be broken up
				if (!last_break) {
					last_break = x - 1;
					color_before_last_break = current_color;
				}

				x = last_break;

				// find an empty string slot
				int j;
				do 
				{
					for ( j = 0; j < MAX_LINES; j++ )
					{
						if ( ! *g_szLineBuffer[j] )
							break;
					}
					if ( j == MAX_LINES )
					{
						// need to make more room to display text, scroll stuff up then fix the pointers
						int linesmoved = ScrollTextUp();
						line -= linesmoved;
						last_break = last_break - (sizeof(g_szLineBuffer[0]) * linesmoved);
					}
				}
				while ( j == MAX_LINES );

				// copy remaining string into next buffer,  making sure it starts with a space character
				g_szLineBuffer[j][0] = ' ';

				if (color_before_last_break)
					sprintf(g_szLineBuffer[j] + 1, "^%d", color_before_last_break);
				else
					g_szLineBuffer[j][1] = '\0';

				if ( *last_break == ' ' )
				{
					strcat(g_szLineBuffer[j], last_break + 1);
				}
				else
				{
					strcat(g_szLineBuffer[j], last_break);
				}

				*last_break = 0; // cut off the last string

				EnsureTextFitsInOneLineAndWrapIfHaveTo( j );
				break;
			}
		}
	}
}
