#define PWMA 12
#define AIN1 14
#define AIN2 15
#define PWMB 13
#define BIN1 16
#define BIN2 17

void setup() {
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
}

void loop() {
  // Adelante 2 segundos
  avanzar(200);
  delay(2000);

  // Para 1 segundo
  parar();
  delay(1000);

  // Atrás 2 segundos
  retroceder(200);
  delay(2000);

  // Para 1 segundo
  parar();
  delay(1000);

  // Gira derecha 2 segundos
  girarDerecha(200);
  delay(2000);

  // Para 1 segundo
  parar();
  delay(1000);

  // Gira izquierda 2 segundos
  girarIzquierda(200);
  delay(2000);

  // Para 1 segundo
  parar();
  delay(1000);
}

void avanzar(int vel) {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, vel);
  
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, vel);
}

void retroceder(int vel) {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, vel);
  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, vel);
}

void girarDerecha(int vel) {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, vel);
  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, vel);
}

void girarIzquierda(int vel) {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, vel);
  
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, vel);
}

void parar() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}