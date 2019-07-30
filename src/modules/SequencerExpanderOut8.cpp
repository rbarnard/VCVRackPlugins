//----------------------------------------------------------------------------
//	/^M^\ Count Modula - Step Sequencer Module
//  A classic 8 step CV/Gate sequencer
//----------------------------------------------------------------------------
#include "../CountModula.hpp"
#include "../inc/Utility.hpp"
#include "../inc/GateProcessor.hpp"
#include "../inc/SequencerExpanderMessage.hpp"

#define SEQ_NUM_STEPS	8

struct SequencerExpanderOut8 : Module {

	enum ParamIds {
		MODE_PARAM,
		NUM_PARAMS
	};
	
	enum InputIds {
		NUM_INPUTS
	};
	
	enum OutputIds {
		ENUMS(STEP_GATE_OUTPUTS, SEQ_NUM_STEPS),
		NUM_OUTPUTS
	};
	
	enum LightIds {
		ENUMS(STEP_LIGHTS, SEQ_NUM_STEPS),
		ENUMS(CHANNEL_LIGHTS, SEQUENCER_EXP_MAX_CHANNELS),
		NUM_LIGHTS
	};
	
	// Expander
	SequencerExpanderMessage leftMessages[2][1];	// messages from left module (master)
	SequencerExpanderMessage rightMessages[2][1]; // messages to right module (expander)
	SequencerExpanderMessage *messagesFromMaster;
	
	int channelID = -1;
	int prevChannelID = -1;
	
	// 0123
	// RGYB
	int colourMapDefault[4] = {0, 1, 2, 3};	// default colour map - matches the trigger sequencer colours RGYB
	int colourMapBinSeq[4] = {1, 2, 3, 0}; 	// colour map for binary sequencer- Puts the red knob last GYBR
	int colourMapSS[4] = {1, 3, 0, 2}; 		// colour map for step sequencer, matches the first row of knob colours 
	
	int *colourMap = colourMapDefault;
	
	SequencerExpanderOut8() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		
		// from left module (master)
		leftExpander.producerMessage = leftMessages[0];
		leftExpander.consumerMessage = leftMessages[1];		
		
		// to right module (expander)
		rightExpander.producerMessage = rightMessages[0];
		rightExpander.consumerMessage = rightMessages[1];	
		
		// range switch
		configParam(MODE_PARAM, 0.0f, 1.0f, 0.0f, "Mode");
	}

	json_t *dataToJson() override {
		json_t *root = json_object();

		json_object_set_new(root, "moduleVersion", json_string("1.0"));
		
		return root;
	}
	
	void process(const ProcessArgs &args) override {

		// details from master
		bool running = true;
		int count = 0;
		bool clock = false;
		int channelCounters[SEQUENCER_EXP_MAX_CHANNELS] = {0, 0, 0, 0};
		bool clockStates[SEQUENCER_EXP_MAX_CHANNELS] = {false, false, false, false};
		bool runningStates[SEQUENCER_EXP_MAX_CHANNELS] = {false, false, false, false};
		
		colourMap = colourMapDefault;
		
		// grab the detail from the left hand module if we have one
		if (leftExpander.module) {
			if (leftExpander.module->model == modelSequencerExpanderCV8 || leftExpander.module->model == modelSequencerExpanderOut8 || leftExpander.module->model == modelSequencerExpanderTrig8 ||
				leftExpander.module->model == modelTriggerSequencer8 || leftExpander.module->model == modelStepSequencer8 || leftExpander.module->model == modelBinarySequencer || 
				leftExpander.module->model == modelBasicSequencer8 || leftExpander.module->model == modelBurstGenerator) {
					
				messagesFromMaster = (SequencerExpanderMessage *)(leftExpander.consumerMessage);

				switch (messagesFromMaster->masterModule) {
					case SEQUENCER_EXP_MASTER_MODULE_BNRYSEQ:
						colourMap = colourMapBinSeq;
						break;
					case SEQUENCER_EXP_MASTER_MODULE_STEPSEQ:
						colourMap = colourMapSS;
						break;
					case SEQUENCER_EXP_MASTER_MODULE_DEFAULT:
					default:
						colourMap = colourMapDefault;
						break;
				}
				
				// grab the channel id
				channelID = clamp(messagesFromMaster->channelOUT, -1, 3);

				// decode the counter array
				for(int i = 0; i < SEQUENCER_EXP_MAX_CHANNELS; i++) {
					channelCounters[i] = messagesFromMaster->counters[i];
					clockStates[i] = messagesFromMaster->clockStates[i];
					runningStates[i] = messagesFromMaster->runningStates[i];
					
					 if (i == channelID) {
						count = std::max(channelCounters[i], 0);
						clock = clockStates[i];
						running = runningStates[i];
						
						// wrap counters > 8 back around to 1
						while (count > SEQ_NUM_STEPS)
							count -= SEQ_NUM_STEPS;
					}
				}
			}
		}
		else
			channelID = -1;
		
		// determine channel light colour
		int m = 4;
		switch (channelID)  {
			case 0:
			case 1:
			case 2:
			case 3:
				m = colourMap[channelID];
				break;
			default:
				m = 4; // always 4 (grey)
				break;
		}
		
		// now set the light colour
		for (int i = 0; i < 4; i ++)
			lights[CHANNEL_LIGHTS + i].setBrightness(boolToLight(i == m));

		// if in gate mode, the clock is irrelevant
		if (params[MODE_PARAM].getValue() < 0.5f)
			clock = true;
		
		// set step lights and outputs
		for (int c = 0; c < SEQ_NUM_STEPS; c++) {
			bool stepActive = ((c + 1) == count);
			lights[STEP_LIGHTS + c].setBrightness(boolToLight(stepActive));
			outputs[STEP_GATE_OUTPUTS + c].setVoltage(boolToGate(stepActive && clock && running));
		}

		// set up the detail for any secondary expander
		if (rightExpander.module) {
			if (rightExpander.module->model == modelSequencerExpanderCV8 || rightExpander.module->model == modelSequencerExpanderOut8 || rightExpander.module->model == modelSequencerExpanderTrig8) {
				
				SequencerExpanderMessage *messageToExpander = (SequencerExpanderMessage*)(rightExpander.module->leftExpander.producerMessage);
				
				// set next module's channel number
				if (channelID < 0) {
					// we have no left hand module
					messageToExpander->setCVChannel(-1);
					messageToExpander->setTrigChannel(-1);
					messageToExpander->setOutChannel(-1);
					messageToExpander->masterModule = SEQUENCER_EXP_MASTER_MODULE_DEFAULT;
				}
				else {
					messageToExpander->setNextOutChannel(channelID);
					
					// add the channel counters and gate states
					for (int i = 0; i < SEQUENCER_EXP_MAX_CHANNELS ; i++) {
						messageToExpander->counters[i] = channelCounters[i];
						messageToExpander->clockStates[i] = clockStates[i];
						messageToExpander->runningStates[i] = runningStates[i];
					}
					
					// pass through the trigger channel number and master module details
					if (messagesFromMaster) {
						messageToExpander->setTrigChannel(messagesFromMaster->channelTRIG);
						messageToExpander->setCVChannel(messagesFromMaster->channelCV);
						messageToExpander->masterModule = messagesFromMaster->masterModule;
					}
				}

				rightExpander.module->leftExpander.messageFlipRequested = true;
			}
		}			
	}
};

struct SequencerExpanderOut8Widget : ModuleWidget {
	SequencerExpanderOut8Widget(SequencerExpanderOut8 *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SequencerExpanderOut8.svg")));

		addChild(createWidget<CountModulaScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CountModulaScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CountModulaScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CountModulaScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// row lights and knobs
		for (int s = 0; s < SEQ_NUM_STEPS; s++) {
			addChild(createLightCentered<MediumLight<RedLight>>(Vec(STD_COLUMN_POSITIONS[STD_COL2], STD_ROWS8[STD_ROW1 + s]), module, SequencerExpanderOut8::STEP_LIGHTS + s));
			addOutput(createOutputCentered<CountModulaJack>(Vec(STD_COLUMN_POSITIONS[STD_COL3], STD_ROWS8[STD_ROW1 + s]), module, SequencerExpanderOut8::STEP_GATE_OUTPUTS + s));
		}

		// channel light
		addChild(createLightCentered<MediumLight<CountModulaLightRGYB>>(Vec(STD_COLUMN_POSITIONS[STD_COL1], STD_ROWS8[STD_ROW1]), module, SequencerExpanderOut8::CHANNEL_LIGHTS));
	
		// mode control
		addParam(createParamCentered<CountModulaToggle2P>(Vec(STD_COLUMN_POSITIONS[STD_COL1], STD_ROWS8[STD_ROW2]), module, SequencerExpanderOut8::MODE_PARAM));
	}	
};

Model *modelSequencerExpanderOut8 = createModel<SequencerExpanderOut8, SequencerExpanderOut8Widget>("SequencerExpanderOut8");