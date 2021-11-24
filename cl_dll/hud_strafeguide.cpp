#include <complex>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "hudgl.h"

#include "pm_defs.h"
#include "usercmd.h"
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
	
	double fov = 140./ 2 / 180 * pi;
	
	int fontHeight = gHUD.m_iFontHeight;
	
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
		if (boxLeft == boxRight)
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
		
		FillRGBA(boxLeftI, ScreenHeight*.48, boxRightI-boxLeftI, fontHeight, r, g, b, 60);
	}
	
	ConsolePrint(debug);
	sprintf(debug, "Angles: %f %f %f %f\n", angles[0], angles[1], angles[2], angles[3]);
	ConsolePrint(debug);
	
	return 0;
}


static double angleReduce(double a)
{
	double tmp = std::fmod(a, 2*pi);
	if (tmp < 0) tmp += 2*pi;
	if (tmp > M_PI) tmp -= 2*pi;
	return tmp;
}

void CHudStrafeGuide::Update(struct playermove_s *ppmove)
{
	auto iUnit = std::complex<double>(0., 1.);
	double frameTime = ppmove->frametime;
	auto input = std::complex<double>(ppmove->cmd.forwardmove, ppmove->cmd.sidemove);
	double viewAngle = ppmove->angles[1] / 180 * pi;
	
	if (std::norm(input) == 0) {
		for (int i = 0; i < 4; ++i) {
			if (i < 2)
				angles[i] = pi;
			else
				angles[i] = -pi;
		}
		return;
	}

	//~ sprintf(debug, "angle %f lastangle %f input %f %f ftime %f rotvel %f\n", viewAngle, lastViewAngle, input.real(), input.imag(), frameTime, actualRotVel);
	
	auto velocity = std::complex<double>(ppmove->velocity[0], ppmove->velocity[1]);

	
	bool onground = ppmove->onground;
	double accelCoeff = onground ? ppmove->movevars->accelerate : ppmove->movevars->airaccelerate;
	double frictionCoeff = ppmove->friction;
	double speedCap = onground ? ppmove->maxspeed : min(ppmove->maxspeed, 30);
	
	
	double inputAbs = min(std::abs(input), ppmove->maxspeed);
	input *= inputAbs / std::abs(input);
	double uncappedAccel = accelCoeff * frictionCoeff * inputAbs * frameTime;
	double velocityAbs = std::abs(velocity);
	
	if (velocityAbs == 0)
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
	
	sprintf(debug, "inp %f %f, vel %f %f, view %f\n", input.real(), input.imag(), velocity.real(), velocity.imag(), viewAngle);
}
