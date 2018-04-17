/***************************************************
Copyright (c) 2017 Luis Llamas
(www.luisllamas.es)

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
****************************************************/

#include "PetriNetLib.h"

PetriNet::PetriNet(uint8_t numStates, uint8_t numTransitions)
{
	_numStates = numStates;
	_numTransitions = numTransitions;

	_transitions = new Transition[numTransitions];
	_markup = new uint8_t[numStates];
	_prevMarkup = new uint8_t[numStates];

	for (uint8_t index = 0; index < _numStates; index++)
		_markup[index] = 0;
}

void PetriNet::ClearMarks() const
{
	for (uint8_t index = 0; index < _numStates; index++)
		_markup[index] = 0;
}

void PetriNet::Update()
{
	if (!isUpdateRequired()) return;

	memcpy(_prevMarkup, _markup, _numStates * sizeof(uint8_t));
	for (uint8_t index = 0; index < _numTransitions; index++)
	{
		if (_transitions[index].IsEnable && isEnabled(_transitions[index], _prevMarkup))
		{
			addMarks(_transitions[index], _markup);
			removeMarks(_transitions[index], _markup);
			removeMarks(_transitions[index], _prevMarkup);
			if((*_transitions[index].Action) != nullptr) (*_transitions[index].Action)();
		}
	}
}

void PetriNet::SetTransition(uint8_t transition, uint8_t* inputs, uint8_t numInputs, uint8_t* outputs, uint8_t numOutputs,
	PetriNetCondition condition, PetriNetAction action = nullptr) const
{
	_transitions[transition].NumInputs = numInputs;
	_transitions[transition].NumOutputs = numOutputs;
	_transitions[transition].Inputs = inputs;
	_transitions[transition].Outputs = outputs;
	_transitions[transition].Condition = condition;
	_transitions[transition].Action = action;
}

void PetriNet::SetTransitionInputs(uint8_t transition, uint8_t* inputs) const
{
	_transitions[transition].Inputs = inputs;
}

void PetriNet::SetTransitionOutputs(uint8_t transition, uint8_t* outputs) const
{
	_transitions[transition].Outputs = outputs;
}

void PetriNet::SetTransitionAction(uint8_t transition, PetriNetAction action) const
{
	_transitions[transition].Action = action;
}

void PetriNet::SetTransitionCondition(uint8_t transition, PetriNetCondition condition) const
{
	_transitions[transition].Condition = condition;
}

void PetriNet::SetMarkup(uint8_t state, uint8_t marks) const
{
	_markup[state] = marks;
}

uint8_t PetriNet::GetMarkup(uint8_t state) const
{
	return _markup[state];
}

bool PetriNet::isUpdateRequired()
{
	bool updateRequired = false;
	for (uint8_t index = 0; index < _numTransitions; index++)
	{
		_transitions[index].IsEnable = false;
		if (isEnabled(_transitions[index], _markup) && (*_transitions[index].Condition)())
		{
			_transitions[index].IsEnable = true;
			updateRequired = true;
		}
	}
	return updateRequired;
}

bool PetriNet::isEnabled(Transition tran, const uint8_t markup[])
{
	for (uint8_t index = 0; index < tran.NumInputs; index++)
		if (markup[tran.Inputs[index]] < 1)
			return false;

	return true;
}

void PetriNet::addMarks(Transition tran, uint8_t markup[])
{
	for (uint8_t index = 0; index < tran.NumOutputs; index++)
		markup[tran.Outputs[index]]++;
}

void PetriNet::removeMarks(Transition tran, uint8_t markup[])
{
	for(uint8_t index = 0; index < tran.NumInputs; index++)
		if(markup[tran.Inputs[index]] > 0)
			markup[tran.Inputs[index]]--;
}