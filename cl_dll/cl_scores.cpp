// +msk
#include <iostream> // debug
#include <algorithm> // std::reverse,std::sort,std::min
#include <vector> // std::vector
#include <string> // std::string
#include <sstream> // std::ostringstream
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
  enum class DrawType{AUTO,MANUAL, NO_UNIFORM}; 
  int ScoreList::Init()
  {
    m_iFlags = 0;

    gHUD.AddHudElem(this);

    cl_scores=CVAR_CREATE("cl_scores","0",FCVAR_ARCHIVE); 
    cl_scores_x=CVAR_CREATE("cl_scores_x","0",FCVAR_ARCHIVE);
    cl_scores_y=CVAR_CREATE("cl_scores_y","0",FCVAR_ARCHIVE);

    cl_scores_width=CVAR_CREATE("cl_scores_width","0",FCVAR_ARCHIVE); 
    cl_scores_brightness=CVAR_CREATE("cl_scores_brightness","30",FCVAR_ARCHIVE);
    cl_scores_hpad=CVAR_CREATE("cl_scores_hpad","0",FCVAR_ARCHIVE); // horizontal padding
    cl_scores_vpad=CVAR_CREATE("cl_scores_vpad","0",FCVAR_ARCHIVE); // vertical padding
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

  static int longest_line_chars(const std::vector<tab_t> & t)
  {
    int long_size=0;
    for(auto i : t)
      {
	std::ostringstream oss;
	oss.str("");
	oss.clear();
	std::string curline;
	curline.clear();

	oss << i.score << "   " << i.name;
	curline = oss.str();

	int size = curline.size() - 2*count_color_tags(curline);
	if(size>long_size)
	  long_size = size;
      }
    return long_size;
  }

  void ScoreList::DrawSortedTabs()
  {
    const bool bTeamplay = gHUD.m_Teamplay;
    const int hpaddingsize = static_cast<int>(cl_scores_hpad->value);
    const int vpaddingsize = static_cast<int>(cl_scores_vpad->value);
    const int brightness = static_cast<int>(cl_scores_brightness->value);

    DrawType drawing_mode {DrawType::NO_UNIFORM};

    int width = 0;
    if(!strncmp(cl_scores_width->string, "auto", 8))
      drawing_mode = DrawType::AUTO; // width will be determined automatically
    else if(cl_scores_width->value == 0)
      drawing_mode = DrawType::NO_UNIFORM;  // disregard width, cells size not uniform
    else
      {
	drawing_mode = DrawType::MANUAL; // user set width (clamp or extend)
	width = static_cast<int>(cl_scores_width->value);
      } 
   
    std::string current_line; // will store each tabs line
    std::vector<char> cstr; //  c-string for drawhudstring 
    int depth = static_cast<int>(cl_scores->value); // how many items should we draw
    int count = 0; // how many have we drawn   
    int longest_line = 0; // longest (modulo colortags) line size of tabs 

    if(tabs.size() != 0)
      {
	std::reverse(tabs.begin(),tabs.end()); // highest score to lowest score order (top->down)

	if(drawing_mode == DrawType::AUTO)
	  longest_line = longest_line_chars(tabs);

	for(auto it = tabs.begin(); it != tabs.end(); ++it)
	  {
	    int characters =  0; // how many characters will be drawn
	    std::ostringstream oss;
	    oss.str("");
	    oss.clear();
	    current_line.clear();
	    cstr.clear();

	    tab_t cur = *it; 
	    oss << cur.score << "   " << cur.name;
	    current_line=oss.str();

	    if(drawing_mode == DrawType::NO_UNIFORM)
	      characters = current_line.size() - 2*count_color_tags(current_line);
	    else if(drawing_mode == DrawType::MANUAL)
	      characters = width;
	    else
	      characters = longest_line;

	    int copied = 0; // how many characters (modulo colortags) have we copied
	    int i = 0; 
	    cstr=std::vector<char>{};
	    int cursize = current_line.size();
	    while((copied < characters) && (i < cursize) ) // fill cstring until no space left. ignore tags 
	      {
		if(current_line[i] == '^' && ((current_line[i+1] <= '9') && (current_line[i+1] >= '0'))) 
		  {
		    cstr.push_back(current_line[i]);
		    cstr.push_back(current_line[i+1]);
		    i = i + 2;
		  }
		else
		  {
		    copied = copied + 1;
		    cstr.push_back(current_line[i]);
		    i = i + 1;
		  }

	      }
	    cstr.push_back('\0');

	    int height_of_a_character=gHUD.m_scrinfo.iCharHeight;
	    float x = cl_scores_x->value;
	    float y = cl_scores_y->value + count*(height_of_a_character + 2*vpaddingsize);
	    
	    int r,g,b;
	    if(bTeamplay)
	      {
		r = iTeamColors[cur.teamnum % iNumberOfTeamColors][0];
		g = iTeamColors[cur.teamnum % iNumberOfTeamColors][1];
		b = iTeamColors[cur.teamnum % iNumberOfTeamColors][2];
	      }
	    else
	      UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	    int endx = 0;
	    ScaleColors(r,g,b,135);
	    endx = gHUD.DrawHudStringWithColorTags(x,y,&cstr[0],r,g,b);
	    gEngfuncs.pfnFillRGBA(x - hpaddingsize,y - vpaddingsize,2*hpaddingsize + endx,height_of_a_character + 2*vpaddingsize,r,g,b,brightness);

	    count=count+1; // don't draw too many
	    if(count>(depth-1))
	      return;
	  }
      }
    
  }

  int ScoreList::Draw(float flTime)
  {
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
		  name=std::string(g_TeamInfo[i].name); 
		  frags=g_TeamInfo[i].frags;
		  teamnum=g_PlayerExtraInfo[i].teamnumber;
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
