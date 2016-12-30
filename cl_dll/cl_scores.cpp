// +msk
#include <iostream>
#include <algorithm> // reverse,sort,min
#include <vector>
#include <string>
#include <sstream>
#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include <cctype> // std::isdigit


extern int iNumberOfTeamColors; // in vgui_TeamFortressViewport.cpp
extern int iTeamColors[5][3]; // in vgui_TeamFortressViewport.cpp
extern hud_player_info_t   g_PlayerInfoList [MAX_PLAYERS + 1];
extern extra_player_info_t g_PlayerExtraInfo[MAX_PLAYERS + 1];
extern team_info_t g_TeamInfo[MAX_TEAMS + 1];

namespace ScoreTabs
{

  int ScoreList::Init()
  {
    m_iFlags = 0;

    gHUD.AddHudElem(this);

    cl_scores=CVAR_CREATE("cl_scores","0",FCVAR_ARCHIVE); // msk: 0, 1 , 2 , ...
    cl_scores_x=CVAR_CREATE("cl_scores_x","0",FCVAR_ARCHIVE); // msk: posi
    cl_scores_y=CVAR_CREATE("cl_scores_y","0",FCVAR_ARCHIVE);

    cl_scores_width=CVAR_CREATE("cl_scores_width","0",FCVAR_ARCHIVE); // non uniform if zéro
    cl_scores_brightness=CVAR_CREATE("cl_scores_brightness","30",FCVAR_ARCHIVE); // ew
    cl_scores_hpad=CVAR_CREATE("cl_scores_hpad","5",FCVAR_ARCHIVE); // ew
    cl_scores_vpad=CVAR_CREATE("cl_scores_vpad","0",FCVAR_ARCHIVE); // ew
    return 0;
  }

  

  int ScoreList::VidInit()
  {
    m_iFlags |= HUD_ACTIVE;
    return 1;
  }

  static int count_color_tags(const std::string & arg)
  {
    int string_length = arg.size();
    bool checktag = false;
    char current_character = '\0';
    int tags_count=0;
    for(int i = 0; i < string_length ; ++i)
      {
	current_character = arg[i];
	if(current_character == '^')
	  {
	    checktag=true;
	    continue;
	  }
	if(checktag)
	  {
	    if(std::isdigit(current_character))
	      ++tags_count;
	  }
	checktag=false;
      }
    return tags_count;
  }

  void ScoreList::DrawSortedTabs()
  {
    bool bTeamplay = gHUD.m_Teamplay;
    int brightness = static_cast<int>(cl_scores_brightness->value);
    int width = static_cast<int>(cl_scores_width->value);
    
    
    std::string str;
    std::vector<char> cstr; // drawhudstring want c-string so give a c-string to drawhudstring ok
    int count = 0;
    int depth = static_cast<int>(cl_scores->value);
    int hpaddingsize = static_cast<int>(cl_scores_hpad->value);
    int vpaddingsize = static_cast<int>(cl_scores_vpad->value);
    if(tabs.size() != 0)
      {
	std::reverse(tabs.begin(),tabs.end()); // best team/player first
	
	for(auto it = tabs.begin(); it != tabs.end(); ++it)
	  {
	    int caracteres =  0;
	    std::ostringstream oss;
	    oss.str("");
	    oss.clear();
	    str.clear();
	    cstr.clear();

	    tab_t cur = *it;
	    oss << cur.score << "   " << cur.name;
	    str=oss.str();

	    if(width == 0)
	      caracteres = str.size() - 2*count_color_tags(str);
	    else
	      caracteres = width;

	    int copied = 0;

	    int i = 0;
	    cstr=std::vector<char>{};
	    while(copied < caracteres) // fill cstring until no space left. ignore tags 
	      {
		if(str[i] == '^' && ((str[i+1] <= '9') && (str[i+1] >= '0'))) // quest: store this info in count_color_tags
		  {
		    cstr.push_back(str[i]);
		    cstr.push_back(str[i+1]);
		    i = i + 2;
		  }
		else
		  {
		    copied = copied + 1;
		    cstr.push_back(str[i]);
		    i = i + 1;
		  }

	      }
	    cstr.push_back('\0');
	    
	    /* // old code men)
	       cstr= std::vector<char> {str.begin(), str.end()};
	       cstr.push_back('\0'); 
	    */

	    int largeur = gHUD.m_scrinfo.charWidths['a'];
	    int hauteurcarac=gHUD.m_scrinfo.iCharHeight;
	    float x = cl_scores_x->value;
	    float y = cl_scores_y->value + count*(hauteurcarac + 2*vpaddingsize);
	    
	    int r,g,b;
	    if(bTeamplay)
	      {
		r = iTeamColors[cur.teamnum % iNumberOfTeamColors][0];
		g = iTeamColors[cur.teamnum % iNumberOfTeamColors][1];
		b = iTeamColors[cur.teamnum % iNumberOfTeamColors][2];
	      }
	    else
	      UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);
	    
	    gEngfuncs.pfnFillRGBA(x - hpaddingsize,y - vpaddingsize,(caracteres-1)*largeur + 2*hpaddingsize,hauteurcarac + 2*vpaddingsize,r,g,b,brightness);
	    ScaleColors(r,g,b,135);
	    gHUD.DrawHudStringWithColorTags(x,y,&cstr[0],r,g,b);

	    count=count+1;
	    if(count>(depth-1))
	      return;
	  }
      }
    
  }

  int ScoreList::Draw(float flTime)
  {
    // GREY QUEST: dont do anything if cl scores value is zero
    if(cl_scores->value != 0) {
      bool bTeamplay = gHUD.m_Teamplay;
      tabs.clear();

      std::string name;
      int frags;
      int teamnum;

      if (bTeamplay)
	{
	  for(int i = 1; i <= MAX_TEAMS; ++i)
	    {
	      if(g_TeamInfo[i].name == nullptr || g_TeamInfo[i].name[0] == '\0')
		continue;
	      else
		{
		  name=std::string(g_TeamInfo[i].name); // do not strip color tags now
		  frags=g_TeamInfo[i].frags;
		  teamnum=g_PlayerExtraInfo[i].teamnumber; // for team color)
		  tabs.push_back({frags, teamnum, name});
		}
	    }
	}

      else
	{ 
	  for(int i = 1; i <= MAX_PLAYERS; ++i) 
	    {
	      if(g_PlayerInfoList[i].name == nullptr)
		continue;
	      name=std::string(g_PlayerInfoList[i].name);

	      frags=g_PlayerExtraInfo[i].frags;
	      teamnum=-1;
	      tabs.push_back({frags, teamnum, name});
	    }

	}

      std::sort(tabs.begin(), tabs.end());
      DrawSortedTabs();
      return 0;
    }
  }
}
// msk-
