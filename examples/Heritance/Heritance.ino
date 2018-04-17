/***************************************************
Copyright (c) 2017 Luis Llamas
(www.luisllamas.es)

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
****************************************************/

//              T0     T1    T2
// StationA 0 ─┐ ┌─ 1 ─>─ 2 ─┐ ┌─ 3 -> 0 T4
//             │─│     T3    │─│
// StationB 4 ─┘ └─ 5 ─>─ 6 ─┘ └─ 7 -> 4 T5 
//                  │  T6 │
//                  └--<-─┘ 

// T0, T2 Condition = InputA || InputB
// T1, T4, T5 Condition = InputA
// T3, T5 Condition = InputB
// T6 Condition = Elapsed 5s

#include "PetriNetLib.h"

enum Input
{
	ForwardA = 0,
	ForwardB = 1,
	Unknown = 2
};

Input input;

// Definicion de la red de petri del ejemplo
class MyPetriNet : public PetriNet
{
public:
	MyPetriNet() : PetriNet(8, 7)
	{
		// Entradas y salidas de los estados
		static uint8_t inputs0[] = { 0, 4 };
		static uint8_t outputs0[] = { 1, 5 };

		static uint8_t inputs1[] = { 1 };
		static uint8_t outputs1[] = { 2 };

		static uint8_t inputs2[] = { 2, 6 };
		static uint8_t outputs2[] = { 3, 7 };

		static uint8_t inputs3[] = { 3 };
		static uint8_t outputs3[] = { 0 };

		static uint8_t inputs4[] = { 5 };
		static uint8_t outputs4[] = { 6 };

		static uint8_t inputs5[] = { 7 };
		static uint8_t outputs5[] = { 4 };

		// Transiciones
		SetTransition(0, inputs0, 2, outputs0, 2,
			[]() -> bool {return input == Input::ForwardA || input == Input::ForwardB; },
			[]() {Serial.println("Fired T0"); printMarkup(); });

		SetTransition(1, inputs1, 1, outputs1, 1,
			[]() -> bool {return input == Input::ForwardA; },
			[]() {Serial.println("Fired T1"); printMarkup(); });

		SetTransition(2, inputs2, 2, outputs2, 2,
			[]() -> bool {return input == Input::ForwardA || input == Input::ForwardB; },
			[]() {Serial.println("Fired T2"); printMarkup(); });

		SetTransition(3, inputs3, 1, outputs3, 1,
			[]() -> bool {return input == Input::ForwardA; },
			[]() {Serial.println("Fired T3"); printMarkup(); });

		SetTransition(4, inputs4, 1, outputs4, 1,
			[]() -> bool {return input == Input::ForwardB; },
			[]() {Serial.println("Fired T4"); printMarkup(); activateTimer(); });

		SetTransition(5, inputs5, 1, outputs5, 1,
			[]() -> bool {return input == Input::ForwardB; },
			[]() {Serial.println("Fired T5"); printMarkup(); });

		SetTransition(6, outputs4, 1, inputs4, 1,
			timerExpired,
			[]() {Serial.println("Reseting T6"); printMarkup(); });

		// Marcado inicial
		SetMarkup(0, 1);
		SetMarkup(4, 1);
	}
};

MyPetriNet petriNet;

void setup()
{
	Serial.begin(9600);
	printMarkup();  // Mostrar el estado inicial
}

void loop()
{
	input = static_cast<Input>(readInput());
	petriNet.Update();
}

// Realiza la lectura de un caracter para el ejemplo
int readInput()
{
	Input currentInput = Input::Unknown;
	if (Serial.available())
	{
		char incomingChar = Serial.read();

		switch (incomingChar)
		{
		case 'A': currentInput = Input::ForwardA; break;
		case 'B': currentInput = Input::ForwardB; break;
		default: break;
		}
	}

	return currentInput;
}

// Para debug del ejemplo
void printMarkup()
{
	Serial.print(petriNet.GetMarkup(0));
	Serial.print(petriNet.GetMarkup(1));
	Serial.print(petriNet.GetMarkup(2));
	Serial.println(petriNet.GetMarkup(3));

	Serial.print(petriNet.GetMarkup(4));
	Serial.print(petriNet.GetMarkup(5));
	Serial.print(petriNet.GetMarkup(6));
	Serial.println(petriNet.GetMarkup(7));
}

// Timer para la transicion 6
unsigned long previousMillis;
bool isTimerON = false;

void activateTimer()
{
	isTimerON = true;
	previousMillis = millis();
}

bool timerExpired()
{
	if (isTimerON == false) return false;

	if ((unsigned long)(millis() - previousMillis) >= 5000)
		return true;
	return false;
}
