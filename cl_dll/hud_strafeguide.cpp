#define _USE_MATH_DEFINES
#include <cmath>
#include <complex>

#include "hud.h"
#include "cl_util.h"

#include "pm_defs.h"
#include "pm_movevars.h"

enum border {
	RED_GREEN,
	GREEN_WHITE,
	WHITE_GREEN,
	GREEN_RED
};

int CHudStrafeGuide::Init()
{
	m_iFlags = HUD_ACTIVE;

	hud_strafeguide = CVAR_CREATE("hud_strafeguide", "0", FCVAR_ARCHIVE);
	hud_strafeguide_zoom = CVAR_CREATE("hud_strafeguide_zoom", "1", FCVAR_ARCHIVE);
	hud_strafeguide_height = CVAR_CREATE("hud_strafeguide_height", "0", FCVAR_ARCHIVE);
	hud_strafeguide_size = CVAR_CREATE("hud_strafeguide_size", "0", FCVAR_ARCHIVE);

	gHUD.AddHudElem(this);
	return 0;
}

int CHudStrafeGuide::VidInit()
{
	return 1;
}

int CHudStrafeGuide::Draw(float time)
{
	if (hud_strafeguide->value == 0)
		return 0;
	
	double fov  = gHUD.default_fov->value / 180 * M_PI / 2;
	double zoom = hud_strafeguide_zoom->value;
	
	int size = gHUD.m_iFontHeight;
	int height = ScreenHeight / 2 - 2*size;
	
	if (hud_strafeguide_size->value != 0)
		size = hud_strafeguide_size->value;
	
	if (hud_strafeguide_height->value != 0)
		height = hud_strafeguide_height->value;
	
	for (int i = 0; i < 4; ++i) {
		int r, g, b;
		switch (i) {
			case RED_GREEN: case WHITE_GREEN:
				r = 0; g = 255; b = 0; break;
			case GREEN_WHITE:
				r = 255; g = 255; b = 255; break;
			case GREEN_RED:
				r = 255; g = 0; b = 0; break;
		}
		
		double boxLeftBase  = -angles[i];
		double boxRightBase = -angles[(i+1)%4];
		
		if (std::abs(boxLeftBase - boxRightBase) < 1e-10)
			continue;
		if (boxLeftBase >= boxRightBase)
			boxRightBase += 2 * M_PI;
		if (std::abs(boxLeftBase - boxRightBase) < 1e-10)
			continue;
		
		for (int iCopy = -8; iCopy <= 8; ++iCopy) {
			double boxLeft  = boxLeftBase  + iCopy * 2 * M_PI;
			double boxRight = boxRightBase + iCopy * 2 * M_PI;
			boxLeft  *= zoom;
			boxRight *= zoom;
			
			if (std::abs(boxLeft) > fov && std::abs(boxRight) > fov && boxRight * boxLeft > 0)
				continue;
			
			boxLeft  = boxLeft  > fov ? fov : boxLeft  < -fov ? -fov : boxLeft;
			boxRight = boxRight > fov ? fov : boxRight < -fov ? -fov : boxRight;
			
			boxLeft  = std::tan(boxLeft ) / std::tan(fov);
			boxRight = std::tan(boxRight) / std::tan(fov);
			
			int boxLeftI  = boxLeft / 1 * ScreenWidth / 2;
			int boxRightI = boxRight/ 1 * ScreenWidth / 2;
			boxLeftI  += ScreenWidth / 2;
			boxRightI += ScreenWidth / 2;
			
			FillRGBA(boxLeftI, height, boxRightI-boxLeftI, size, r, g, b, 60);
		}
	}
	
	return 0;
}

static double angleReduce(double a)
{
	double tmp = std::fmod(a, 2*M_PI);
	if (tmp < 0) tmp += 2*M_PI;
	if (tmp > M_PI) tmp -= 2*M_PI;
	return tmp;
}

void CHudStrafeGuide::Update(struct ref_params_s *pparams)
{
	double frameTime = pparams->frametime;
	auto input = std::complex<double>(pparams->cmd->forwardmove, pparams->cmd->sidemove);
	double viewAngle = pparams->viewangles[1] / 180 * M_PI;
	
	if (std::norm(input) == 0) {
		for (int i = 0; i < 4; ++i) {
			if (i < 2)
				angles[i] = M_PI;
			else
				angles[i] = -M_PI;
		}
		return;
	}

	std::complex<double> velocity = lastSimvel;
	lastSimvel = std::complex<double>(pparams->simvel[0], pparams->simvel[1]);

	bool onground = pparams->onground;
	double accelCoeff = onground ? pparams->movevars->accelerate : pparams->movevars->airaccelerate;
	//TODO: grab the entity friction from somewhere. pparams->movevars->friction is sv_friction
	//just use the default 1 for now
	double frictionCoeff = 1;
	
	double inputAbs = std::abs(input);
	if (onground)
		inputAbs = min(inputAbs, pparams->movevars->maxspeed);
	else
		inputAbs = min(inputAbs, 30);
	
	input *= inputAbs / std::abs(input);
	
	double uncappedAccel = accelCoeff * frictionCoeff * inputAbs * frameTime;
	double velocityAbs = std::abs(velocity);
	
	if (uncappedAccel >= 2 * velocityAbs)
		angles[RED_GREEN] = M_PI;
	else
		angles[RED_GREEN] = std::acos(-uncappedAccel / velocityAbs / 2);
	
	if (velocityAbs <= inputAbs)
		angles[GREEN_WHITE] = 0;
	else
		angles[GREEN_WHITE] = std::acos(inputAbs / velocityAbs);
	
	angles[GREEN_RED] = -angles[RED_GREEN];
	angles[WHITE_GREEN] = -angles[GREEN_WHITE];
	
	double inputAngle = std::log(input).imag();
	double velocityAngle;
	
	if (velocityAbs == 0)
		velocityAngle = 0;
	else
		velocityAngle = std::log(velocity).imag();
	
	for (int i = 0; i < 4; ++i) {
		angles[i] += velocityAngle + inputAngle - viewAngle;
		angles[i] = angleReduce(angles[i]);
	}
}
