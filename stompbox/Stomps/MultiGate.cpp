#include <math.h>
#include <algorithm>

#include "MultiGate.h"

MultiGate::MultiGate()
{
	Name = "MultiGate";

	threshold = -60;
	attackMS = 5;
	holdMS = 10;
	releaseMS = 50;

	NumParameters = MULTIGATE_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[MULTIGATE_THRESHOLD].Name = "Thresh";
	Parameters[MULTIGATE_THRESHOLD].MinValue = -100;
	Parameters[MULTIGATE_THRESHOLD].MaxValue = 0;
	Parameters[MULTIGATE_THRESHOLD].SourceVariable = &threshold;
	Parameters[MULTIGATE_THRESHOLD].DefaultValue = threshold;
	Parameters[MULTIGATE_THRESHOLD].CanSyncToHostBPM = true;
	Parameters[MULTIGATE_THRESHOLD].DisplayFormat = "{0:0}db";

	Parameters[MULTIGATE_ATTACK].Name = "Attack";
	Parameters[MULTIGATE_ATTACK].MinValue = 1;
	Parameters[MULTIGATE_ATTACK].MaxValue = 100;
	Parameters[MULTIGATE_ATTACK].SourceVariable = &attackMS;
	Parameters[MULTIGATE_ATTACK].DefaultValue = attackMS;
	Parameters[MULTIGATE_ATTACK].DisplayFormat = "{0:0}ms";
	Parameters[MULTIGATE_ATTACK].IsAdvanced = true;

	Parameters[MULTIGATE_HOLD].Name = "Hold";
	Parameters[MULTIGATE_HOLD].MinValue = 1;
	Parameters[MULTIGATE_HOLD].MaxValue = 100;
	Parameters[MULTIGATE_HOLD].SourceVariable = &holdMS;
	Parameters[MULTIGATE_HOLD].DefaultValue = holdMS;
	Parameters[MULTIGATE_HOLD].DisplayFormat = "{0:0}ms";
	Parameters[MULTIGATE_HOLD].IsAdvanced = true;

	Parameters[MULTIGATE_RELEASE].Name = "Release";
	Parameters[MULTIGATE_RELEASE].MinValue = 1;
	Parameters[MULTIGATE_RELEASE].MaxValue = 100;
	Parameters[MULTIGATE_RELEASE].SourceVariable = &releaseMS;
	Parameters[MULTIGATE_RELEASE].DefaultValue = releaseMS;
	Parameters[MULTIGATE_RELEASE].DisplayFormat = "{0:0}ms";
	Parameters[MULTIGATE_RELEASE].IsAdvanced = true;
}

void MultiGate::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void MultiGate::instanceConstants(int samplingFreq)
{
	fSampleRate = samplingFreq;

	fConst0 = std::min<double>(1.92e+05, std::max<double>(1.0, double(fSampleRate)));
	fConst1 = std::tan(3141.592653589793 / fConst0);
	fConst2 = fConst1 * (fConst1 + 1.4142135623730951) + 1.0;
	fConst3 = 2.0 / fConst2;
	fConst4 = fConst1 / fConst2;
	fConst5 = 1.0 / fConst2;
	fConst6 = std::tan(402.1238596594935 / fConst0);
	fConst7 = fConst6 * (fConst6 + 1.4142135623730951) + 1.0;
	fConst8 = 2.0 / fConst7;
	fConst9 = fConst6 / fConst7;
	fConst10 = 1.0 / fConst7;
	fConst11 = std::tan(25132.741228718343 / fConst0);
	fConst12 = fConst11 * (fConst11 + 1.4142135623730951) + 1.0;
	fConst13 = 2.0 / fConst12;
	fConst14 = fConst11 / fConst12;
	fConst15 = 1.0 / fConst12;
	fConst16 = 1.0 / fConst0;
	fConst17 = 0.001 * fConst0;
}

void MultiGate::instanceClear() {
	for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
		fRec7[l0] = 0.0;
	}
	for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
		fRec8[l1] = 0.0;
	}
	for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
		fRec3[l2] = 0.0;
	}
	for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
		fRec4[l3] = 0.0;
	}
	for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
		fRec0[l4] = 0.0;
	}
	for (int l5 = 0; l5 < 2; l5 = l5 + 1) {
		fRec1[l5] = 0.0;
	}
	for (int l6 = 0; l6 < 2; l6 = l6 + 1) {
		fRec11[l6] = 0.0;
	}
	for (int l7 = 0; l7 < 2; l7 = l7 + 1) {
		fRec12[l7] = 0.0;
	}
	for (int l8 = 0; l8 < 2; l8 = l8 + 1) {
		fRec15[l8] = 0.0;
	}
	for (int l9 = 0; l9 < 2; l9 = l9 + 1) {
		fRec16[l9] = 0.0;
	}
	for (int l10 = 0; l10 < 2; l10 = l10 + 1) {
		fRec20[l10] = 0.0;
	}
	for (int l11 = 0; l11 < 2; l11 = l11 + 1) {
		iVec0[l11] = 0;
	}
	for (int l12 = 0; l12 < 2; l12 = l12 + 1) {
		iRec21[l12] = 0;
	}
	for (int l13 = 0; l13 < 2; l13 = l13 + 1) {
		fRec19[l13] = 0.0;
	}
	for (int l14 = 0; l14 < 2; l14 = l14 + 1) {
		fRec24[l14] = 0.0;
	}
	for (int l15 = 0; l15 < 2; l15 = l15 + 1) {
		fRec25[l15] = 0.0;
	}
	for (int l16 = 0; l16 < 2; l16 = l16 + 1) {
		fRec23[l16] = 0.0;
	}
	for (int l17 = 0; l17 < 2; l17 = l17 + 1) {
		iVec1[l17] = 0;
	}
	for (int l18 = 0; l18 < 2; l18 = l18 + 1) {
		iRec27[l18] = 0;
	}
	for (int l19 = 0; l19 < 2; l19 = l19 + 1) {
		fRec22[l19] = 0.0;
	}
	for (int l20 = 0; l20 < 2; l20 = l20 + 1) {
		fRec31[l20] = 0.0;
	}
	for (int l21 = 0; l21 < 2; l21 = l21 + 1) {
		fRec32[l21] = 0.0;
	}
	for (int l22 = 0; l22 < 2; l22 = l22 + 1) {
		fRec28[l22] = 0.0;
	}
	for (int l23 = 0; l23 < 2; l23 = l23 + 1) {
		fRec29[l23] = 0.0;
	}
	for (int l24 = 0; l24 < 2; l24 = l24 + 1) {
		fRec34[l24] = 0.0;
	}
	for (int l25 = 0; l25 < 2; l25 = l25 + 1) {
		fRec35[l25] = 0.0;
	}
	for (int l26 = 0; l26 < 2; l26 = l26 + 1) {
		fRec38[l26] = 0.0;
	}
	for (int l27 = 0; l27 < 2; l27 = l27 + 1) {
		fRec39[l27] = 0.0;
	}
	for (int l28 = 0; l28 < 2; l28 = l28 + 1) {
		fRec43[l28] = 0.0;
	}
	for (int l29 = 0; l29 < 2; l29 = l29 + 1) {
		iVec2[l29] = 0;
	}
	for (int l30 = 0; l30 < 2; l30 = l30 + 1) {
		iRec44[l30] = 0;
	}
	for (int l31 = 0; l31 < 2; l31 = l31 + 1) {
		fRec42[l31] = 0.0;
	}
	for (int l32 = 0; l32 < 2; l32 = l32 + 1) {
		fRec47[l32] = 0.0;
	}
	for (int l33 = 0; l33 < 2; l33 = l33 + 1) {
		fRec48[l33] = 0.0;
	}
	for (int l34 = 0; l34 < 2; l34 = l34 + 1) {
		fRec46[l34] = 0.0;
	}
	for (int l35 = 0; l35 < 2; l35 = l35 + 1) {
		iVec3[l35] = 0;
	}
	for (int l36 = 0; l36 < 2; l36 = l36 + 1) {
		iRec50[l36] = 0;
	}
	for (int l37 = 0; l37 < 2; l37 = l37 + 1) {
		fRec45[l37] = 0.0;
	}
}

void MultiGate::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double fSlow0 = std::pow(1e+01, 0.05 * threshold);
	double fSlow1 = 0.001 * attackMS;
	double fSlow2 = 0.001 * releaseMS;
	double fSlow3 = std::min<double>(fSlow1, fSlow2);
	int iSlow4 = std::fabs(fSlow3) < 2.220446049250313e-16;
	double fSlow5 = ((iSlow4) ? 0.0 : std::exp(-(fConst16 / ((iSlow4) ? 1.0 : fSlow3))));
	double fSlow6 = 1.0 - fSlow5;
	int iSlow7 = int(fConst17 * holdMS);
	int iSlow8 = std::fabs(fSlow2) < 2.220446049250313e-16;
	double fSlow9 = ((iSlow8) ? 0.0 : std::exp(-(fConst16 / ((iSlow8) ? 1.0 : fSlow2))));
	int iSlow10 = std::fabs(fSlow1) < 2.220446049250313e-16;
	double fSlow11 = ((iSlow10) ? 0.0 : std::exp(-(fConst16 / ((iSlow10) ? 1.0 : fSlow1))));
	for (int i0 = 0; i0 < count; i0 = i0 + 1) {
		double fTemp0 = double(input0[i0]);
		double fTemp1 = fRec7[1] + fConst1 * (fTemp0 - fRec8[1]);
		fRec7[0] = fConst3 * fTemp1 - fRec7[1];
		double fTemp2 = fRec8[1] + fConst4 * fTemp1;
		fRec8[0] = 2.0 * fTemp2 - fRec8[1];
		double fRec9 = fConst5 * fTemp1;
		double fRec10 = fTemp2;
		double fTemp3 = fRec10 + 1.4142135623730951 * fRec9;
		double fTemp4 = fRec3[1] + fConst1 * (fTemp0 - (fTemp3 + fRec4[1]));
		fRec3[0] = fConst3 * fTemp4 - fRec3[1];
		double fTemp5 = fRec4[1] + fConst4 * fTemp4;
		fRec4[0] = 2.0 * fTemp5 - fRec4[1];
		double fRec5 = fConst5 * fTemp4;
		double fRec6 = fTemp5;
		double fTemp6 = fRec9 + fRec5;
		double fTemp7 = 1.4142135623730951 * fTemp6;
		double fTemp8 = fTemp7 + fRec10 + fRec6;
		double fTemp9 = fRec0[1] + fConst6 * (fTemp0 - (fTemp8 + fRec1[1]));
		fRec0[0] = fConst8 * fTemp9 - fRec0[1];
		double fTemp10 = fRec1[1] + fConst9 * fTemp9;
		fRec1[0] = 2.0 * fTemp10 - fRec1[1];
		double fRec2 = fConst10 * fTemp9;
		double fTemp11 = fRec6 + fRec10 + 2.8284271247461903 * fRec2;
		double fTemp12 = fTemp7 + fTemp11;
		double fTemp13 = fRec11[1] + fConst11 * (fTemp0 - (fTemp12 + fRec12[1]));
		fRec11[0] = fConst13 * fTemp13 - fRec11[1];
		double fTemp14 = fRec12[1] + fConst14 * fTemp13;
		fRec12[0] = 2.0 * fTemp14 - fRec12[1];
		double fRec13 = fConst15 * fTemp13;
		double fRec14 = fTemp14;
		double fTemp15 = fRec14 + fTemp11;
		double fTemp16 = fRec13 + fTemp6;
		double fTemp17 = 1.4142135623730951 * fTemp16 + fTemp15;
		double fTemp18 = fRec15[1] + fConst11 * (fTemp0 - (fTemp17 + fRec16[1]));
		fRec15[0] = fConst13 * fTemp18 - fRec15[1];
		double fTemp19 = fRec16[1] + fConst14 * fTemp18;
		fRec16[0] = 2.0 * fTemp19 - fRec16[1];
		double fRec17 = fConst15 * fTemp18;
		double fRec18 = fTemp19;
		double fTemp20 = fTemp0 - (1.4142135623730951 * (fRec17 + fTemp16) + fRec18 + fTemp15);
		fRec20[0] = fSlow6 * std::fabs(fTemp20) + fSlow5 * fRec20[1];
		int iTemp21 = fRec20[0] > fSlow0;
		iVec0[0] = iTemp21;
		iRec21[0] = std::max<int>(iSlow7 * (iTemp21 < iVec0[1]), iRec21[1] + -1);
		double fTemp22 = std::fabs(std::max<double>(double(iTemp21), double(iRec21[0] > 0)));
		double fTemp23 = ((fTemp22 > fRec19[1]) ? fSlow11 : fSlow9);
		fRec19[0] = fTemp22 * (1.0 - fTemp23) + fRec19[1] * fTemp23;
		double fTemp24 = fRec24[1] + fConst11 * (fRec14 - fRec25[1]);
		fRec24[0] = fConst13 * fTemp24 - fRec24[1];
		double fTemp25 = fRec25[1] + fConst14 * fTemp24;
		fRec25[0] = 2.0 * fTemp25 - fRec25[1];
		double fRec26 = fTemp25;
		fRec23[0] = fSlow6 * std::fabs(fRec26) + fSlow5 * fRec23[1];
		int iTemp26 = fRec23[0] > fSlow0;
		iVec1[0] = iTemp26;
		iRec27[0] = std::max<int>(iSlow7 * (iTemp26 < iVec1[1]), iRec27[1] + -1);
		double fTemp27 = std::fabs(std::max<double>(double(iTemp26), double(iRec27[0] > 0)));
		double fTemp28 = ((fTemp27 > fRec22[1]) ? fSlow11 : fSlow9);
		fRec22[0] = fTemp27 * (1.0 - fTemp28) + fRec22[1] * fTemp28;
		double fTemp29 = fRec31[1] + fConst1 * (fRec10 - fRec32[1]);
		fRec31[0] = fConst3 * fTemp29 - fRec31[1];
		double fTemp30 = fRec32[1] + fConst4 * fTemp29;
		fRec32[0] = 2.0 * fTemp30 - fRec32[1];
		double fRec33 = fTemp30;
		double fTemp31 = fRec28[1] + fConst11 * (fRec33 - fRec29[1]);
		fRec28[0] = fConst13 * fTemp31 - fRec28[1];
		double fTemp32 = fRec29[1] + fConst14 * fTemp31;
		fRec29[0] = 2.0 * fTemp32 - fRec29[1];
		double fRec30 = fConst15 * fTemp31;
		double fTemp33 = 2.8284271247461903 * fRec30;
		double fTemp34 = fRec34[1] + fConst6 * (fRec33 - (fTemp33 + fRec35[1]));
		fRec34[0] = fConst8 * fTemp34 - fRec34[1];
		double fTemp35 = fRec35[1] + fConst9 * fTemp34;
		fRec35[0] = 2.0 * fTemp35 - fRec35[1];
		double fRec36 = fConst10 * fTemp34;
		double fRec37 = fTemp35;
		double fTemp36 = fRec37 + fTemp33 + 1.4142135623730951 * fRec36;
		double fTemp37 = fRec38[1] + fConst6 * (fRec33 - (fTemp36 + fRec39[1]));
		fRec38[0] = fConst8 * fTemp37 - fRec38[1];
		double fTemp38 = fRec39[1] + fConst9 * fTemp37;
		fRec39[0] = 2.0 * fTemp38 - fRec39[1];
		double fRec40 = fConst10 * fTemp37;
		double fRec41 = fTemp38;
		double fTemp39 = fRec33 - (1.4142135623730951 * (fRec36 + fRec40) + fRec41 + fRec37 + fTemp33);
		fRec43[0] = fSlow6 * std::fabs(fTemp39) + fSlow5 * fRec43[1];
		int iTemp40 = fRec43[0] > fSlow0;
		iVec2[0] = iTemp40;
		iRec44[0] = std::max<int>(iSlow7 * (iTemp40 < iVec2[1]), iRec44[1] + -1);
		double fTemp41 = std::fabs(std::max<double>(double(iTemp40), double(iRec44[0] > 0)));
		double fTemp42 = ((fTemp41 > fRec42[1]) ? fSlow11 : fSlow9);
		fRec42[0] = fTemp41 * (1.0 - fTemp42) + fRec42[1] * fTemp42;
		double fTemp43 = fRec47[1] + fConst6 * (fRec37 - fRec48[1]);
		fRec47[0] = fConst8 * fTemp43 - fRec47[1];
		double fTemp44 = fRec48[1] + fConst9 * fTemp43;
		fRec48[0] = 2.0 * fTemp44 - fRec48[1];
		double fRec49 = fTemp44;
		fRec46[0] = std::fabs(fRec49) * fSlow6 + fRec46[1] * fSlow5;
		int iTemp45 = fRec46[0] > fSlow0;
		iVec3[0] = iTemp45;
		iRec50[0] = std::max<int>(iSlow7 * (iTemp45 < iVec3[1]), iRec50[1] + -1);
		double fTemp46 = std::fabs(std::max<double>(double(iTemp45), double(iRec50[0] > 0)));
		double fTemp47 = ((fTemp46 > fRec45[1]) ? fSlow11 : fSlow9);
		fRec45[0] = fTemp46 * (1.0 - fTemp47) + fRec45[1] * fTemp47;
		output0[i0] = FAUSTFLOAT(fRec49 * fRec45[0] + fRec42[0] * fTemp39 + fRec26 * fRec22[0] + fRec19[0] * fTemp20);
		fRec7[1] = fRec7[0];
		fRec8[1] = fRec8[0];
		fRec3[1] = fRec3[0];
		fRec4[1] = fRec4[0];
		fRec0[1] = fRec0[0];
		fRec1[1] = fRec1[0];
		fRec11[1] = fRec11[0];
		fRec12[1] = fRec12[0];
		fRec15[1] = fRec15[0];
		fRec16[1] = fRec16[0];
		fRec20[1] = fRec20[0];
		iVec0[1] = iVec0[0];
		iRec21[1] = iRec21[0];
		fRec19[1] = fRec19[0];
		fRec24[1] = fRec24[0];
		fRec25[1] = fRec25[0];
		fRec23[1] = fRec23[0];
		iVec1[1] = iVec1[0];
		iRec27[1] = iRec27[0];
		fRec22[1] = fRec22[0];
		fRec31[1] = fRec31[0];
		fRec32[1] = fRec32[0];
		fRec28[1] = fRec28[0];
		fRec29[1] = fRec29[0];
		fRec34[1] = fRec34[0];
		fRec35[1] = fRec35[0];
		fRec38[1] = fRec38[0];
		fRec39[1] = fRec39[0];
		fRec43[1] = fRec43[0];
		iVec2[1] = iVec2[0];
		iRec44[1] = iRec44[0];
		fRec42[1] = fRec42[0];
		fRec47[1] = fRec47[0];
		fRec48[1] = fRec48[0];
		fRec46[1] = fRec46[0];
		iVec3[1] = iVec3[0];
		iRec50[1] = iRec50[0];
		fRec45[1] = fRec45[0];
	}
}
