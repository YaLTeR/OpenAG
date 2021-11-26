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

const float pi = std::acos(-1.);

int CHudStrafeGuide::Init()
{
	m_iFlags = HUD_ACTIVE;

	hud_strafeguide = CVAR_CREATE("hud_strafeguide", "0", FCVAR_ARCHIVE);
	hud_strafeguide_fov = CVAR_CREATE("hud_strafeguide_fov", "140", FCVAR_ARCHIVE);
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
	
	double fov = hud_strafeguide_fov->value / 2 / 180 * pi;
	
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
		
		double boxLeft  = -angles[i];
		double boxRight = -angles[(i+1)%4];
		if (std::abs(boxLeft-boxRight) < 1e-10)
			continue;
		
		if (boxLeft > fov) {
			if (boxRight < -fov || boxRight > boxLeft)
				continue;
			boxLeft = -fov;
		}
		else if (boxRight < -fov) {
			if (boxLeft < boxRight)
				continue;
			boxRight = fov;
		}
		
		
		if (boxLeft  < -fov) boxLeft  = -fov;
		if (boxRight >  fov) boxRight = fov;
		
		int boxLeftI  = boxLeft / fov * ScreenWidth / 2;
		int boxRightI = boxRight/ fov * ScreenWidth / 2;
		boxLeftI  += ScreenWidth / 2;
		boxRightI += ScreenWidth / 2;
		
		FillRGBA(boxLeftI, height, boxRightI-boxLeftI, size, r, g, b, 60);
	}
	
	return 0;
}

static double angleReduce(double a)
{
	double tmp = std::fmod(a, 2*pi);
	if (tmp < 0) tmp += 2*pi;
	if (tmp > M_PI) tmp -= 2*pi;
	return tmp;
}

void CHudStrafeGuide::Update(struct ref_params_s *pparams)
{
	double frameTime = pparams->frametime;
	auto input = std::complex<double>(pparams->cmd->forwardmove, pparams->cmd->sidemove);
	double viewAngle = pparams->viewangles[1] / 180 * pi;
	
	if (std::norm(input) == 0) {
		for (int i = 0; i < 4; ++i) {
			if (i < 2)
				angles[i] = pi;
			else
				angles[i] = -pi;
		}
		return;
	}

	std::complex<double> velocity = lastSimvel;
	lastSimvel = std::complex<double>(pparams->simvel[0], pparams->simvel[1]);

	bool onground = pparams->onground;
	double accelCoeff = onground ? pparams->movevars->accelerate : pparams->movevars->airaccelerate;
	double frictionCoeff = pparams->movevars->friction;
	double speedCap = onground ? pparams->movevars->maxspeed : min(pparams->movevars->maxspeed, 30);
	
	
	double inputAbs = min(std::abs(input), pparams->movevars->maxspeed);
	input *= inputAbs / std::abs(input);
	double uncappedAccel = accelCoeff * frictionCoeff * inputAbs * frameTime;
	double velocityAbs = std::abs(velocity);
	
	if (velocityAbs <= 2 * uncappedAccel)
		angles[RED_GREEN] = pi;
	else
		angles[RED_GREEN] = std::acos(-uncappedAccel / velocityAbs / 2);
	
	if (velocityAbs <= speedCap)
		angles[GREEN_WHITE] = 0;
	else
		angles[GREEN_WHITE] = std::acos(speedCap / velocityAbs);
	
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
