#pragma once
#include <string>

class CHudDebug : public CHudBase
{
	cvar_t* hud_debug;
	cvar_t* hud_debug_filename;
	FILE* output;

	std::string additional_data;

	void DumpEntityData(const cl_entity_t* ent, float time);

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	void AddString(const std::string& s);
};
