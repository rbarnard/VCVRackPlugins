//----------------------------------------------------------------------------
//	/^M^\ Count Modula header.
//----------------------------------------------------------------------------
#include "rack.hpp"
using namespace rack;

// Forward-declare the Plugin
extern Plugin *pluginInstance;

// Forward-declare each Model, defined in each module source file
extern Model *modelComparator;
extern Model *modelBinarySequencer;
extern Model *modelVCPolarizer;
extern Model *modelMatrixMixer;
extern Model *modelBooleanAND;
extern Model *modelBooleanOR;
extern Model *modelBooleanXOR;
extern Model *modelCVSpreader;
extern Model *modelAnalogueShiftRegister;
extern Model *modelVoltageInverter;
extern Model *modelVCFrequencyDivider;
extern Model *modelEventArranger;
extern Model *modelAttenuator;
extern Model *modelG2T;
extern Model *modelMinimusMaximus;
extern Model *modelVoltageControlledSwitch;
extern Model *modelMultiplexer;
extern Model *modelGateDelay;
extern Model *modelMorphShaper;
extern Model *modelShepardGenerator;
extern Model *modelSRFlipFlop;
extern Model *modelManualCV;
extern Model *modelManualGate;
extern Model *modelGateDelayMT;
extern Model *modelMuteIple;
extern Model *modelPolyrhythmicGenerator;
extern Model *modelBooleanVCNOT;
extern Model *modelBurstGenerator;
extern Model *modelTriggerSequencer16;
extern Model *modelTriggerSequencer8;
extern Model *modelMixer;
extern Model *modelMute;
extern Model *modelRectifier;
extern Model *modelSampleAndHold;
extern Model *modelTFlipFlop;
extern Model *modelGateModifier;
extern Model *modelStepSequencer8;
extern Model *modelMangler;
extern Model *modelBasicSequencer8;
extern Model *modelSequencerExpanderCV8;
extern Model *modelSequencerExpanderOut8;
extern Model *modelSequencerExpanderTrig8;
extern Model *modelSubHarmonicGenerator;
extern Model *modelPolyrhythmicGeneratorMkII;
extern Model *modelVCFrequencyDividerMkII;
extern Model *modelGatedComparator;
extern Model *modelSlopeDetector;
extern Model *modelSequencerExpanderRM8;
extern Model *modelSequencerExpanderLog8;
// extern Model *modelVCGateDivider;
// extern Model *modelOscilloscope;
extern Model *modelStartupDelay;
extern Model *modelRackEarLeft;
extern Model *modelRackEarRight;
extern Model *modelBlank4HP;
extern Model *modelBlank8HP;
extern Model *modelBlank12HP;
extern Model *modelBlank16HP;

// theme functions
int readDefaultTheme();
void saveDefaultTheme(int theme);

#include "components/CountModulaComponents.hpp"
#include "components/StdComponentPositions.hpp"


