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

    // Faz o movimento do servo motor
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
    motorSpeed = map(potValue, 0, 1023, 0, 255);  // Mapeia o valor lido para a faixa de 0 a 255

    analogWrite(motorPin, motorSpeed);  // Define a velocidade do motor com base no valor lido
  } else {
    // Lógica para desligar o motor
    motorSpeed = 0;  // Define a velocidade mínima do motor
    analogWrite(motorPin, motorSpeed);
  }

  // Controla o LED 1
  if (isLedOn) {
    // Lógica para ligar o LED 1
    digitalWrite(ledPin, HIGH);
  } else {
    // Lógica para desligar o LED 1
    digitalWrite(ledPin, LOW);
  }

  // Controla o LED 2 e o servo motor
  if (isLed2On) {
    // Lógica para ligar o LED 2
    digitalWrite(led2Pin, HIGH);

    // Faz o movimento do servo motor
    if (servoPos < 120) {
      servoPos += 1;
    } else {
      servoPos = 0;
    }
    // Envia o sinal para o servo motor
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(100 + servoPos * 50);
    digitalWrite(servoPin, LOW);
    delay(200);
  } else {
    // Lógica para desligar o LED 2
    digitalWrite(led2Pin, LOW);
  }

  // Leitura do comando pela porta serial
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "b1") {
      // Liga ou desliga o botão 1
      isMotorOn = !isMotorOn;
      isLedOn = !isLedOn;
    } else if (command == "b2") {
      // Liga ou desliga o botão 2
      isLed2On = !isLed2On;
      if (isLed2On) {
        servoPos = 0;
      }
    } else if (command == "v") {
      // Envia a velocidade atual do motor pela porta serial
      Serial.println(motorSpeed);
    }
  }
}
