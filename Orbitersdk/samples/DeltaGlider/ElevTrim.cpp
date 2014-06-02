// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// ElevTrim.cpp
// Elevator trim control
// ==============================================================

#define STRICT 1
#include "ElevTrim.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"
#include "meshres_vc.h"

// ==============================================================

// constants for texture coordinates
static const float texw = (float)PANEL2D_TEXW; // texture width
static const float texh = (float)PANEL2D_TEXH; // texture height
static const float tx_x0 = 1138.0f;
static const float tx_y0 = texh-580.0f;
// constants for panel coordinates
static const float bb_x0 = 1242.5f;
static const float bb_y0 =  161.5f;
static const float bb_dx =   21.0f;
static const float bb_dy =    7.0f;

// ==============================================================

ElevatorTrim::ElevatorTrim (VESSEL3 *v): PanelElement (v)
{
}

// ==============================================================

void ElevatorTrim::Reset2D (MESHHANDLE hMesh)
{
	trimpos2D = 0.0;
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 60;
}

// ==============================================================

void ElevatorTrim::ResetVC (DEVMESHHANDLE hMesh)
{
	trimposVC = 0.0;
}

// ==============================================================

bool ElevatorTrim::Redraw2D (SURFHANDLE surf)
{
	double level = vessel->GetControlSurfaceLevel (AIRCTRL_ELEVATORTRIM);
	if (level != trimpos2D) {
		static const float yp[4] = {bb_y0, bb_y0, bb_y0+bb_dy, bb_y0+bb_dy};
		float yshift = (float)(level*24.0);
		for (int i = 0; i < 4; i++)
			grp->Vtx[vtxofs+i].y = yp[i]+yshift;
		trimpos2D = level;
	}
	return false;
}

// ==============================================================

bool ElevatorTrim::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	if (!hMesh) return false;

	double level = vessel->GetControlSurfaceLevel (AIRCTRL_ELEVATORTRIM);
	if (level != trimposVC) {
		const DWORD nvtx = 3;
		WORD vidx[3] = {4,5,6};
		NTVERTEX vtx[nvtx];
		GROUPEDITSPEC ges;
		ges.flags = GRPEDIT_VTXCRDY|GRPEDIT_VTXCRDZ;
		ges.nVtx = nvtx;
		ges.Vtx = vtx;
		ges.vIdx = vidx;

		static double y0[nvtx] = {0.9183, 0.9208, 0.9232};
		static double z0[nvtx] = {7.1390, 7.1408, 7.1425};
		static double range = 0.032;
		static double tilt = 0.6222;
		static double dy = -range*cos(tilt), dz = -range*sin(tilt);
		for (DWORD i = 0; i < nvtx; i++) {
			vtx[i].y = y0[i] + level*dy;
			vtx[i].z = z0[i] + level*dz;
		}
		oapiEditMeshGroup (hMesh, GRP_LIT_SURFACES_VC, &ges);

		DeltaGlider *dg = (DeltaGlider*)vessel;
		double v;
		vessel->SetAnimation (dg->anim_vc_trimwheel, modf((1-level)*20, &v));

		trimposVC = level;
	}
	return false;
}

// ==============================================================

bool ElevatorTrim::ProcessMouse2D (int event, int mx, int my)
{
	double tgtlvl = vessel->GetControlSurfaceLevel (AIRCTRL_ELEVATORTRIM);
	tgtlvl += oapiGetSimStep() * (my < 30 ? -0.2:0.2);
	tgtlvl = max (-1.0, min (1.0, tgtlvl));
	vessel->SetControlSurfaceLevel (AIRCTRL_ELEVATORTRIM, tgtlvl);
	return true;
}

// ==============================================================

bool ElevatorTrim::ProcessMouseVC (int event, VECTOR3 &p)
{
	//DeltaGlider *dg = (DeltaGlider*)vessel;

	double dtrim = oapiGetSimStep() * (p.y < 0.5 ? 0.2:-0.2);
	double trim0 = vessel->GetControlSurfaceLevel (AIRCTRL_ELEVATORTRIM);
	double trim1 = max(-1.0, min(1.0, trim0+dtrim));

	if (trim0 != trim1) {
		vessel->SetControlSurfaceLevel (AIRCTRL_ELEVATORTRIM, trim1);
		//double v;
		//vessel->SetAnimation (dg->anim_vc_trimwheel, modf((1-trim1)*20, &v));
		return true;
	}
	return false;
}
