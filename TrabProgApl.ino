// Definição dos pinos
const int buttonPin = 2;            // Pino do primeiro botão
const int ledPin = 13;              // Pino do LED 1
const int motorPin = 9;             // Pino do motor DC
const int button2Pin = 3;           // Pino do segundo botão
const int led2Pin = 12;             // Pino do LED 2
const int servoPin = 10;            // Pino do servo motor
const int potPin = A0;              // Pino do potenciômetro

// Variáveis globais
bool isButtonPressed = false;       // Estado do primeiro botão
bool isButton2Pressed = false;      // Estado do segundo botão
bool isMotorOn = false;             // Estado do motor
bool isLedOn = false;               // Estado do LED
bool isLed2On = false;              // Estado do LED 2
int servoPos = 0;                   // Posição do servo motor
int motorSpeed = 0;                 // Velocidade do motor

void setup() {
  pinMode(buttonPin, INPUT);        // Define o pino do primeiro botão como entrada
  pinMode(button2Pin, INPUT);       // Define o pino do segundo botão como entrada
  pinMode(ledPin, OUTPUT);          // Define o pino do LED 1 como saída
  pinMode(led2Pin, OUTPUT);         // Define o pino do LED 2 como saída
  pinMode(motorPin, OUTPUT);        // Define o pino do motor DC como saída
  pinMode(servoPin, OUTPUT);        // Define o pino do servo motor como saída

  Serial.begin(9600);               // Inicia a comunicação serial
}

void loop() {
  // Verifica o estado do primeiro botão
  if (digitalRead(buttonPin) == HIGH && !isButtonPressed) {
    delay(200);  // Delay para evitar detecção múltipla ao segurar o botão

    // Liga ou desliga o motor
    isMotorOn = !isMotorOn;

    // Liga ou desliga o LED 1
    isLedOn = !isLedOn;

    // Atualiza o estado do botão
    isButtonPressed = true;
  } else if (digitalRead(buttonPin) == LOW) {
    // Atualiza o estado do botão
    isButtonPressed = false;
  }

  // Verifica o estado do segundo botão
  if (digitalRead(button2Pin) == HIGH && !isButton2Pressed) {
    delay(200);  // Delay para evitar detecção múltipla ao segurar o botão

    // Liga ou desliga o LED 2
    isLed2On = !isLed2On;
    if (isLed2On) {
      servoPos = 0;  // Define a posição inicial do servo motor
    }

    // Atualiza o estado do botão
    isButton2Pressed = true;
  } else if (digitalRead(button2Pin) == LOW) {
    // Atualiza o estado do botão
    isButton2Pressed = false;
  }

  // Controla o motor
  if (isMotorOn) {
    // Lógica para ligar o motor
    int potValue = analogRead(potPin);  // Lê o valor do potenciômetro
    motorSpeed = map(potValue, 0, 1023, 0, 255);  // Mapeia o valor lido para a faixa de velocidade do motor
    analogWrite(motorPin, motorSpeed);  // Define a velocidade do motor
  } else {
    // Desliga o motor
    analogWrite(motorPin, 0);
  }

  // Controla o servo motor
  if (isLed2On) {
    // Lógica para mover o servo motor
    if (servoPos < 180) {
      servoPos += 1;  // Incrementa a posição do servo motor
    }
    analogWrite(servoPin, servoPos);  // Define a posição do servo motor
    delay(15);  // Delay para controlar a velocidade de movimento do servo motor
  } else {
    // Desliga o servo motor
    analogWrite(servoPin, 0);
  }

  // Atualiza o estado do LED 1
  digitalWrite(ledPin, isLedOn ? HIGH : LOW);

  // Atualiza o estado do LED 2
  digitalWrite(led2Pin, isLed2On ? HIGH : LOW);

  // Leitura do comando pela porta serial
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "b1") {
      isMotorOn = !isMotorOn;
      isLedOn = !isLedOn;
    } else if (command == "b2") {
      isLed2On = !isLed2On;
      if (isLed2On) {
        servoPos = 0;
      }
    } else if (command == "v") {
      int potValue = analogRead(potPin);
      motorSpeed = map(potValue, 0, 1023, 0, 255);
      analogWrite(motorPin, motorSpeed);
    } else if (command == "status") {
      // Envia o estado das saídas pela porta serial
      Serial.print("A0: ");
      Serial.print(analogRead(potPin));
      Serial.print(" | LED 1: ");
      Serial.print(isLedOn ? "ON" : "OFF");
      Serial.print(" | LED 2: ");
      Serial.print(isLed2On ? "ON" : "OFF");
      Serial.print(" | Servo: ");
      Serial.print(servoPos);
      Serial.print(" | Motor: ");
      Serial.print(isMotorOn ? "ON" : "OFF");
      Serial.print(" | Button 1: ");
      Serial.print(digitalRead(buttonPin) == HIGH ? "HIGH" : "LOW");
      Serial.print(" | Button 2: ");
      Serial.println(digitalRead(button2Pin) == HIGH ? "HIGH" : "LOW");
    }
  }
}
