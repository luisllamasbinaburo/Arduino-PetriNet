# Librería PetriNet

La librería PetriNet implementa una red de Petri que puede ejecutarse en un procesador como Arduino.

Más información https://www.luisllamas.es/libreria-de-arduino-petrinet/

## Instrucciones de uso

La red de Petri se inicializa indicando el número de estados y transiciones. Ambos, estados y transiciones, están identificados por un entero de 8bits (0 a 255).

Para crear las transiciones entre estados se emplea la función `SetTransition()`, que recibe como parámetros el número identificador de la transición, los estados de entrada y el número, los estados de salida y su número, la función de disparo de la transición y, opcionalmente, la acción acción asociada a realizar al disparar la transición.

Adicionalmente, estos parámetros de la transición pueden modificarse a posteriori con las funciones `SetTransitionInputs()`, `SetTransitionOutputs()`, `SetTransitionAction()` y `SetTransitionCondition()`.

Para inicializar o modificar el estado de la red de Petri, podemos usar la función SetMarkup(), que establece el número de marcas de un estado. También podemos obtener el número de marcas actual de un estado con la función GetMarkup().

Finalmente, la función `Update()` realiza la actualización de la red de Petri. Calcula las entradas sensibilizadas, las condiciones de disparo y, en caso necesario, actualiza las marcas de los estados y dispara las acciones oportunas.

Las condiciones de las transiciones deben ser lo más ligeras posible, ya que son evaluadas cada vez que se dispara la función `Update()`. Por tanto, conviene evitar realizar cálculos pesados en las mismas, si no evaluar condiciones simples, y realizar los cálculos cuando sea conveniente en el bucle principal.

Por ejemplo, si estamos esperando una comunicación, es mejor realizarla únicamente en el bucle, y evaluar las transiciones con el resultado. De lo contrario realizaríamos la comunicación en cada transición, ralenticiendo innecesariamente la actualización de la red de Petri.

### Constructor
La clase PetriNet se instancia a través de su constructor.
```c++
PetriNet(uint8_t numStates, uint8_t numTransitions);
```

### Uso PetriNet
```c++
  // Actualizar la red de petri
  void Update();

  // Configura las transiciones
  void SetTransition(uint8_t transition, uint8_t* inputs, uint8_t numInputs, uint8_t* outputs, uint8_t numOutputs, PetriNetCondition condition, PetriNetAction action) const;
  void SetTransitionInputs(uint8_t transition, uint8_t* inputs) const;
  void SetTransitionOutputs(uint8_t transition, uint8_t* outputs) const;
  void SetTransitionAction(uint8_t transition, PetriNetAction action) const;
  void SetTransitionCondition(uint8_t transition, PetriNetCondition condition) const;
  
  // Establecer, obtener, o eliminar las marcas de los estados
  void ClearMarks() const;
  void SetMarkup(uint8_t state, uint8_t marks) const;
  uint8_t GetMarkup(uint8_t state) const;
```
## Ejemplos
La librería PetriNet incluye los siguientes ejemplos para ilustrar su uso.

* Simple: Ejemplo que muestra el uso de PetriNet
```c++
#include "PetriNetLib.h"

enum Input
{
	ForwardA = 0,
	ForwardB = 1,
	Unknown = 2
};

Input input;

PetriNet petriNet(8, 7);

void setup()
{
	Serial.begin(9600);
	
	// Definicion de la red de petri del ejemplo
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
	petriNet.SetTransition(0, inputs0, 2, outputs0, 2,
		[]() -> bool {return input == Input::ForwardA || input == Input::ForwardB; },
		[]() {Serial.println("Fired T0"); printMarkup(); });

	petriNet.SetTransition(1, inputs1, 1, outputs1, 1,
		[]() -> bool {return input == Input::ForwardA; },
		[]() {Serial.println("Fired T1"); printMarkup(); });

	petriNet.SetTransition(2, inputs2, 2, outputs2, 2,
		[]() -> bool {return input == Input::ForwardA || input == Input::ForwardB; },
		[]() {Serial.println("Fired T2"); printMarkup(); });

	petriNet.SetTransition(3, inputs3, 1, outputs3, 1,
		[]() -> bool {return input == Input::ForwardA; },
		[]() {Serial.println("Fired T3"); printMarkup(); });

	petriNet.SetTransition(4, inputs4, 1, outputs4, 1,
		[]() -> bool {return input == Input::ForwardB; },
		[]() {Serial.println("Fired T4"); printMarkup(); activateTimer(); });

	petriNet.SetTransition(5, inputs5, 1, outputs5, 1,
		[]() -> bool {return input == Input::ForwardB; },
		[]() {Serial.println("Fired T5");  printMarkup(); });

	petriNet.SetTransition(6, outputs4, 1, inputs4, 1,
		timerExpired,
		[]() {Serial.println("Reseting T6"); printMarkup(); });

	// Marcado inicial
	petriNet.SetMarkup(0, 1);
	petriNet.SetMarkup(4, 1);
	
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
```



* Ejemplo herencia: Ejemplo que muestra el uso de PetriNet mediante una clase derivada.
```c++
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
```
