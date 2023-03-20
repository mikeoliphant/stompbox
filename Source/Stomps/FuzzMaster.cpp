#include <math.h>
#include <algorithm>

#include "FuzzMaster.h"
#include "KT88.cc"
#include "7199P.cc"

static double mydsp_faustpower2_f(double value) {
	return (value * value);

}

static double mydsp_faustpower3_f(double value) {
	return ((value * value) * value);

}
static double mydsp_faustpower4_f(double value) {
	return (((value * value) * value) * value);

}

table1d* tranytab[TRANY_TABLE_SIZE] = {
	&static_cast<table1d&>(tubetable_KT88[0]),
	&static_cast<table1d&>(tubetable_KT88[1]),
	&static_cast<table1d&>(tubetable_7199P[0]),
	&static_cast<table1d&>(tubetable_7199P[1]),
};

static double Ftrany(int table, double Vgk) {
	const table1d& tab = *tranytab[table];
	double f = (Vgk - tab.low) * tab.istep;
	int i = static_cast<int>(f);
	if (i < 0)
		return tab.data[0];
	if (i >= tab.size - 1)
		return tab.data[tab.size - 1];
	f -= i;
	return tab.data[i] * (1 - f) + tab.data[i + 1] * f;
}

FuzzMaster::FuzzMaster()
{
	Name = "FuzzMaster";

	fVslider0 = FAUSTFLOAT(0.5);
	fVslider1 = FAUSTFLOAT(0.5);

	NumParameters = FUZZMASTER_NUMPARAMETERS;
	Parameters = new GuitarSimParameter[NumParameters];

	Parameters[FUZZMASTER_TONE].Name = "Tone";
	Parameters[FUZZMASTER_TONE].SourceVariable = &fVslider0;
	Parameters[FUZZMASTER_TONE].DefaultValue = fVslider0;

	Parameters[FUZZMASTER_VOLUME].Name = "Volume";
	Parameters[FUZZMASTER_VOLUME].SourceVariable = &fVslider1;
	Parameters[FUZZMASTER_VOLUME].DefaultValue = fVslider1;
}

void FuzzMaster::init(int samplingFreq)
{
	GuitarSimComponent::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void FuzzMaster::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = tan((97.389372261283583 / fConst0));
	fConst2 = (1.0 / fConst1);
	fConst3 = (fConst2 + 1.0);
	fConst4 = (1.0 / (fConst1 * fConst3));
	fConst5 = (1.0 / tan((270.1769682087222 / fConst0)));
	fConst6 = (1.0 / (fConst5 + 1.0));
	fConst7 = (1.0 - fConst5);
	fConst8 = (0.93028479253239138 * mydsp_faustpower4_f(fConst0));
	fConst9 = mydsp_faustpower2_f(fConst0);
	fConst10 = (6.0569805086158998e-28 * fConst0);
	fConst11 = ((fConst0 * ((fConst9 * ((fConst0 * (-1.28205547569977e-24 - fConst10)) + -1.19076272781387e-21)) + 1.50574620515751e-16)) + 1.57965086882498e-14);
	fConst12 = (9.0101081625029807e-31 * fConst0);
	fConst13 = ((fConst0 * ((fConst9 * ((fConst0 * (fConst12 + -1.9849418983458499e-25)) + -2.5583534022328998e-22)) + 1.02282867089281e-16)) + 4.3394989686302998e-15);
	fConst14 = (6.0479704004534004e-28 * fConst0);
	fConst15 = (fConst0 * ((fConst9 * ((fConst0 * (fConst14 + 1.0367014655351399e-24)) + 8.7051846019280894e-22)) + -4.3394989686303002e-18));
	fConst16 = (1.5142451271539699e-27 * fConst0);
	fConst17 = ((fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (fConst16 + 1.60256934462471e-24)) + -5.9538136390693501e-22)) + -1.11633243019242e-18)) + -7.5287310257875699e-17)) + 1.9745635860312201e-14);
	fConst18 = (2.2525270406257401e-30 * fConst0);
	fConst19 = ((fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (2.4811773729323101e-25 - fConst18)) + -1.2791767011164499e-22)) + -6.4996736249717003e-19)) + -5.1141433544640603e-17)) + 5.4243737107878799e-15);
	fConst20 = (1.5119926001133501e-27 * fConst0);
	fConst21 = (fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (-1.29587683191893e-24 - fConst20)) + 4.3525923009640504e-22)) + 3.0775065931025701e-19)) + 2.1697494843151501e-18));
	fConst22 = ((fConst9 * (2.38152545562774e-21 - (2.01899350287197e-27 * fConst9))) + -3.0114924103150299e-16);
	fConst23 = ((fConst9 * ((3.0033693875009901e-30 * fConst9) + 5.1167068044657902e-22)) + -2.0456573417856199e-16);
	fConst24 = ((fConst9 * ((2.01599013348447e-27 * fConst9) + -1.7410369203856201e-21)) + 8.6789979372606003e-18);
	fConst25 = ((fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (fConst16 + -1.60256934462471e-24)) + -5.9538136390693501e-22)) + 1.11633243019242e-18)) + -7.5287310257875699e-17)) + -1.9745635860312201e-14);
	fConst26 = ((fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (-2.4811773729323101e-25 - fConst18)) + -1.2791767011164499e-22)) + 6.4996736249717003e-19)) + -5.1141433544640603e-17)) + -5.4243737107878799e-15);
	fConst27 = (fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (1.29587683191893e-24 - fConst20)) + 4.3525923009640504e-22)) + -3.0775065931025701e-19)) + 2.1697494843151501e-18));
	fConst28 = ((fConst0 * ((fConst9 * ((fConst0 * (1.28205547569977e-24 - fConst10)) + -1.19076272781387e-21)) + 1.50574620515751e-16)) + -1.57965086882498e-14);
	fConst29 = ((fConst0 * ((fConst9 * ((fConst0 * (fConst12 + 1.9849418983458499e-25)) + -2.5583534022328998e-22)) + 1.02282867089281e-16)) + -4.3394989686302998e-15);
	fConst30 = (fConst0 * ((fConst9 * ((fConst0 * (fConst14 + -1.0367014655351399e-24)) + 8.7051846019280894e-22)) + -4.3394989686303002e-18));
	fConst31 = (1.00949675143598e-28 * fConst0);
	fConst32 = ((fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (fConst31 + -3.2051386892494098e-25)) + 5.9538136390693501e-22)) + -3.7211081006413802e-19)) + 7.5287310257875699e-17)) + -3.9491271720624398e-15);
	fConst33 = (1.5016846937505001e-31 * fConst0);
	fConst34 = ((fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (-4.9623547458646202e-26 - fConst33)) + 1.2791767011164499e-22)) + -2.1665578749905701e-19)) + 5.1141433544640603e-17)) + -1.0848747421575801e-15);
	fConst35 = (1.00799506674223e-28 * fConst0);
	fConst36 = (fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (2.5917536638378599e-25 - fConst35)) + -4.3525923009640504e-22)) + 1.02583553103419e-19)) + -2.1697494843151501e-18));
	fConst37 = ((fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (fConst31 + 3.2051386892494098e-25)) + 5.9538136390693501e-22)) + 3.7211081006413802e-19)) + 7.5287310257875699e-17)) + 3.9491271720624398e-15);
	fConst38 = ((fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (4.9623547458646202e-26 - fConst33)) + 1.2791767011164499e-22)) + 2.1665578749905701e-19)) + 5.1141433544640603e-17)) + 1.0848747421575801e-15);
	fConst39 = (fConst0 * ((fConst0 * ((fConst0 * ((fConst0 * (-2.5917536638378599e-25 - fConst35)) + -4.3525923009640504e-22)) + -1.02583553103419e-19)) + -2.1697494843151501e-18));
	fConst40 = (1.4177527051631099e-29 * fConst0);
	fConst41 = ((fConst0 * (fConst40 + -4.7099982573797203e-24)) + -2.5947877434919699e-21);
	fConst42 = (1.41775270516311e-27 * fConst0);
	fConst43 = ((fConst0 * (fConst42 + -4.7099982573797198e-22)) + -2.59478774349197e-19);
	fConst44 = (4.7177958972581202e-24 - fConst40);
	fConst45 = (4.7177958972581202e-22 - fConst42);
	fConst46 = (8.5065162309786795e-29 * fConst0);
	fConst47 = ((fConst0 * (1.8839993029518899e-23 - fConst46)) + 5.18957548698393e-21);
	fConst48 = (8.5065162309786799e-27 * fConst0);
	fConst49 = ((fConst0 * (1.8839993029518902e-21 - fConst48)) + 5.1895754869839304e-19);
	fConst50 = (fConst46 + -1.8871183589032501e-23);
	fConst51 = (fConst48 + -1.88711835890325e-21);
	fConst52 = (2.1266290577446699e-28 * fConst0);
	fConst53 = ((fConst0 * (fConst52 + -2.3549991286898599e-23)) + 2.5947877434919699e-21);
	fConst54 = (2.1266290577446701e-26 * fConst0);
	fConst55 = ((fConst0 * (fConst54 + -2.3549991286898602e-21)) + 2.59478774349197e-19);
	fConst56 = (2.3588979486290599e-23 - fConst52);
	fConst57 = (2.3588979486290601e-21 - fConst54);
	fConst58 = (-1.0379150973967901e-20 - (2.8355054103262301e-28 * fConst9));
	fConst59 = (-1.0379150973967899e-18 - (2.8355054103262299e-26 * fConst9));
	fConst60 = ((fConst0 * (fConst52 + 2.3549991286898599e-23)) + 2.5947877434919699e-21);
	fConst61 = (-2.3588979486290599e-23 - fConst52);
	fConst62 = (-2.3588979486290601e-21 - fConst54);
	fConst63 = ((fConst0 * (fConst54 + 2.3549991286898602e-21)) + 2.59478774349197e-19);
	fConst64 = ((fConst0 * (-1.8839993029518899e-23 - fConst46)) + 5.18957548698393e-21);
	fConst65 = ((fConst0 * (-1.8839993029518902e-21 - fConst48)) + 5.1895754869839304e-19);
	fConst66 = (fConst46 + 1.8871183589032501e-23);
	fConst67 = (fConst48 + 1.88711835890325e-21);
	fConst68 = ((fConst0 * (fConst40 + 4.7099982573797203e-24)) + -2.5947877434919699e-21);
	fConst69 = ((fConst0 * (fConst42 + 4.7099982573797198e-22)) + -2.59478774349197e-19);
	fConst70 = (-4.7177958972581202e-24 - fConst40);
	fConst71 = (-4.7177958972581202e-22 - fConst42);
	fConst72 = (0.0 - fConst4);
	fConst73 = ((1.0 - fConst2) / fConst3);
}

void FuzzMaster::instanceClear() {
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		fRec6[l0] = 0.0;

	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		fRec11[l1] = 0.0;

	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		fRec15[l2] = 0.0;

	}
	for (int l3 = 0; (l3 < 7); l3 = (l3 + 1)) {
		fRec14[l3] = 0.0;

	}
	for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
		fRec16[l4] = 0.0;

	}
	for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
		fVec0[l5] = 0.0;

	}
	for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
		fRec13[l6] = 0.0;

	}
	for (int l7 = 0; (l7 < 3); l7 = (l7 + 1)) {
		fRec12[l7] = 0.0;

	}
	for (int l8 = 0; (l8 < 3); l8 = (l8 + 1)) {
		fRec10[l8] = 0.0;

	}
	for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
		fRec9[l9] = 0.0;

	}
	for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
		fRec8[l10] = 0.0;

	}
	for (int l11 = 0; (l11 < 3); l11 = (l11 + 1)) {
		fRec7[l11] = 0.0;

	}
	for (int l12 = 0; (l12 < 3); l12 = (l12 + 1)) {
		fRec5[l12] = 0.0;

	}
	for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
		fRec4[l13] = 0.0;

	}
	for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
		fRec3[l14] = 0.0;

	}
	for (int l15 = 0; (l15 < 3); l15 = (l15 + 1)) {
		fRec2[l15] = 0.0;

	}
	for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
		fRec17[l16] = 0.0;

	}
	for (int l17 = 0; (l17 < 3); l17 = (l17 + 1)) {
		fRec1[l17] = 0.0;

	}
	for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
		fRec0[l18] = 0.0;

	}
}

void FuzzMaster::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double fSlow0 = (0.0070000000000000062 * double(fVslider0));
	double fSlow1 = (0.00036676987543879196 * (exp((3.0 * double(fVslider1))) + -1.0));
	for (int i = 0; (i < count); i = (i + 1)) {
		fRec6[0] = (fConst6 * ((0.027 * (fRec5[1] + fRec5[2])) - (fConst7 * fRec6[1])));
		fRec11[0] = (fConst6 * ((0.027 * (fRec10[1] + fRec10[2])) - (fConst7 * fRec11[1])));
		fRec15[0] = (fSlow0 + (0.99299999999999999 * fRec15[1]));
		double fTemp0 = ((fConst0 * (fConst37 + (fRec15[0] * (fConst38 + (fConst39 * fRec15[0]))))) + 5.4243737107878698e-14);
		fRec14[0] = (double(input0[i]) - (((((((fRec14[1] * ((fConst0 * (fConst11 + (fRec15[0] * (fConst13 + (fConst15 * fRec15[0]))))) + 3.2546242264727301e-13)) + (fRec14[2] * ((fConst0 * (fConst17 + (fRec15[0] * (fConst19 + (fConst21 * fRec15[0]))))) + 8.1365605661818103e-13))) + (fRec14[3] * ((fConst9 * (fConst22 + (fRec15[0] * (fConst23 + (fConst24 * fRec15[0]))))) + 1.0848747421575801e-12))) + (fRec14[4] * ((fConst0 * (fConst25 + (fRec15[0] * (fConst26 + (fConst27 * fRec15[0]))))) + 8.1365605661818103e-13))) + (fRec14[5] * ((fConst0 * (fConst28 + (fRec15[0] * (fConst29 + (fConst30 * fRec15[0]))))) + 3.2546242264727301e-13))) + (fRec14[6] * ((fConst0 * (fConst32 + (fRec15[0] * (fConst34 + (fConst36 * fRec15[0]))))) + 5.4243737107878698e-14))) / fTemp0));
		fRec16[0] = (fSlow1 + (0.99299999999999999 * fRec16[1]));
		double fTemp1 = ((((((((fRec14[0] * (fConst41 + ((fConst43 * fRec16[0]) + (fConst0 * (fRec15[0] * (fConst44 + (fConst45 * fRec16[0]))))))) + (fRec14[1] * (fConst47 + ((fConst49 * fRec16[0]) + (fConst0 * (fRec15[0] * (fConst50 + (fConst51 * fRec16[0])))))))) + (fRec14[2] * (fConst53 + ((fConst55 * fRec16[0]) + (fConst0 * (fRec15[0] * (fConst56 + (fConst57 * fRec16[0])))))))) + (fRec14[3] * (fConst58 + ((fConst59 * fRec16[0]) + (fConst9 * (fRec15[0] * ((2.8355054103262299e-26 * fRec16[0]) + 2.8355054103262301e-28))))))) + (fRec14[4] * (fConst60 + ((fConst0 * (fRec15[0] * (fConst61 + (fConst62 * fRec16[0])))) + (fConst63 * fRec16[0]))))) + (fRec14[5] * (fConst64 + ((fConst65 * fRec16[0]) + (fConst0 * (fRec15[0] * (fConst66 + (fConst67 * fRec16[0])))))))) + (fRec14[6] * (fConst68 + ((fConst69 * fRec16[0]) + (fConst0 * (fRec15[0] * (fConst70 + (fConst71 * fRec16[0])))))))) / fTemp0);
		fVec0[0] = fTemp1;
		fRec13[0] = ((fConst8 * (fTemp1 + fVec0[1])) - (0.86056958506478287 * fRec13[1]));
		fRec12[0] = (fRec13[0] - ((0.86129424393186271 * fRec12[2]) + (1.8405051250752198 * fRec12[1])));
		fRec10[0] = (double(Ftrany(int(TRANY_TABLE_7199P_68k), double((((fRec11[0] + (0.92544984225177063 * (fRec12[0] + fRec12[2]))) + (1.8508996845035413 * fRec12[1])) + -3.5719810000000001)))) + -117.70440740740739);
		fRec9[0] = ((0.025000000000000001 * ((fConst4 * fRec10[0]) + (fConst72 * fRec10[1]))) - (fConst73 * fRec9[1]));
		fRec8[0] = ((0.93028479253239138 * (fRec9[0] + fRec9[1])) - (0.86056958506478287 * fRec8[1]));
		fRec7[0] = (fRec8[0] - ((0.86129424393186271 * fRec7[2]) + (1.8405051250752198 * fRec7[1])));
		fRec5[0] = (double(Ftrany(int(TRANY_TABLE_7199P_68k), double((((fRec6[0] + (1.8508996845035413 * fRec7[1])) + (0.92544984225177063 * (fRec7[0] + fRec7[2]))) + -3.5719810000000001)))) + -117.70440740740739);
		fRec4[0] = ((0.025000000000000001 * ((fConst4 * fRec5[0]) + (fConst72 * fRec5[1]))) - (fConst73 * fRec4[1]));
		fRec3[0] = ((0.93028479253239138 * (fRec4[0] + fRec4[1])) - (0.86056958506478287 * fRec3[1]));
		fRec2[0] = (fRec3[0] - ((0.86129424393186271 * fRec2[2]) + (1.8405051250752198 * fRec2[1])));
		fRec17[0] = (fConst6 * ((0.027 * (fRec1[2] + fRec1[1])) - (fConst7 * fRec17[1])));
		fRec1[0] = (double(Ftrany(int(TRANY_TABLE_7199P_68k), double((((0.92544984225177063 * (fRec2[0] + fRec2[2])) + (fRec17[0] + (1.8508996845035413 * fRec2[1]))) + -3.5719810000000001)))) + -117.70440740740739);
		fRec0[0] = ((0.025000000000000001 * ((fConst4 * fRec1[0]) + (fConst72 * fRec1[1]))) - (fConst73 * fRec0[1]));
		output0[i] = FAUSTFLOAT(fRec0[0]);
		fRec6[1] = fRec6[0];
		fRec11[1] = fRec11[0];
		fRec15[1] = fRec15[0];
		for (int j0 = 6; (j0 > 0); j0 = (j0 - 1)) {
			fRec14[j0] = fRec14[(j0 - 1)];

		}
		fRec16[1] = fRec16[0];
		fVec0[1] = fVec0[0];
		fRec13[1] = fRec13[0];
		fRec12[2] = fRec12[1];
		fRec12[1] = fRec12[0];
		fRec10[2] = fRec10[1];
		fRec10[1] = fRec10[0];
		fRec9[1] = fRec9[0];
		fRec8[1] = fRec8[0];
		fRec7[2] = fRec7[1];
		fRec7[1] = fRec7[0];
		fRec5[2] = fRec5[1];
		fRec5[1] = fRec5[0];
		fRec4[1] = fRec4[0];
		fRec3[1] = fRec3[0];
		fRec2[2] = fRec2[1];
		fRec2[1] = fRec2[0];
		fRec17[1] = fRec17[0];
		fRec1[2] = fRec1[1];
		fRec1[1] = fRec1[0];
		fRec0[1] = fRec0[0];

	}
}

