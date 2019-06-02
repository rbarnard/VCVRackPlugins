//----------------------------------------------------------------------------
//	Count Modula - Gate Processor
//	A standardised gate input processor
//----------------------------------------------------------------------------
#pragma once
#include "dsp/digital.hpp"

class GateProcessor {
	private:
		SchmittTrigger st;
		bool prevState = false;
		bool currentState = false;
		
	public:
		// set the gate with the given value
		bool set(float value) {
			st.process(rescale(value, 0.1f, 2.f, 0.f, 1.f));
			prevState = currentState;
			currentState = st.isHigh();
			
			return currentState;
		}
		
		// reset the gate processor
		void reset() {
			st.reset();
			prevState = currentState = false;		
		}
		
		// gate high indicator
		bool high() {
			return currentState;
		}
		
		// gate low indicator
		bool low() {
			return !currentState;
		}
		
		// gate state indicator
		bool state() {
			return currentState;
		}

		// indicates if the latest value cause a leading edge
		bool leadingEdge() {
			return currentState && !prevState;
		}
		
		// indicates if the latest value cause a trailing edge
		bool trailingEdge() {
			return prevState && !currentState;
		}

		// indicates if the latest value cause any edge
		bool anyEdge () {
			return prevState != currentState;
		}
		
		// gate state value for output
		float value() {
			return currentState ? 10.0f : 0.0f;
		}
		
		float notValue() {
			return currentState ? 0.0f : 10.0f;
		}
		
		// gate state value for display
		float light () {
			return currentState ? 1.0f : 0.0f;	
		}
};