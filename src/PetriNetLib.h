/***************************************************
Copyright (c) 2017 Luis Llamas
(www.luisllamas.es)

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
****************************************************/

#ifndef _PetriNetLib_h
#define _PetriNetLib_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class PetriNet
{
	typedef bool(*PetriNetCondition)();
	typedef void(*PetriNetAction)();

	typedef struct {
		uint8_t NumInputs;
		uint8_t NumOutputs;

		uint8_t *Inputs;
		uint8_t *Outputs;
		PetriNetCondition Condition;
		PetriNetAction Action;
		bool IsEnable;
	} Transition;

public:
	PetriNet(uint8_t numStates, uint8_t numTransitions);

	void ClearMarks() const;
	void Update();

	void SetTransition(uint8_t transition, uint8_t* inputs, uint8_t numInputs, uint8_t* outputs, uint8_t numOutputs, PetriNetCondition condition, PetriNetAction action) const;
	void SetTransitionInputs(uint8_t transition, uint8_t* inputs) const;
	void SetTransitionOutputs(uint8_t transition, uint8_t* outputs) const;
	void SetTransitionAction(uint8_t transition, PetriNetAction action) const;
	void SetTransitionCondition(uint8_t transition, PetriNetCondition condition) const;

	void SetMarkup(uint8_t state, uint8_t marks) const;
	uint8_t GetMarkup(uint8_t state) const;

private:
	uint8_t _numStates;
	uint8_t _numTransitions;

	Transition *_transitions;
	uint8_t *_markup;
	uint8_t *_prevMarkup;

	bool isUpdateRequired();
	static bool isEnabled(Transition tran, const uint8_t markup[]);
	static void addMarks(Transition tran, uint8_t markup[]);
	static void removeMarks(Transition tran, uint8_t markup[]);
};

#endif
