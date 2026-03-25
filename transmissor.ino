#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inicializa o display LCD no endereço I2C 0x27 (padrão no Tinkercad)
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int PIN_SENSOR = A0; // Pino onde o potenciômetro está conectado

// Função personalizada mapFloat() 
// O map() nativo do Arduino só trabalha com inteiros. Para maior precisão
// em instrumentação (conversão de sinais de 4-20mA), criamos uma versão Float.
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Pratica 1:");
  lcd.setCursor(0, 1);
  lcd.print("Transmissor");
  delay(2000);
  lcd.clear();
}

void loop() {
  // 1. Lê o sinal do "sensor" (potenciômetro)
  // O ADC do Arduino converte a tensão de 0 a 5V em valores de 0 a 1023.
  int leituraADC = analogRead(PIN_SENSOR);

  // 2. Mapeia ADC (0 - 1023) para a escala de Corrente (4.0 - 20.0 mA)
  // Ex: 0 -> 4.0 mA | 511 -> 12.0 mA | 1023 -> 20.0 mA
  float corrente_mA = mapFloat(leituraADC, 0, 1023, 4.0, 20.0);

  // 3. Mapeia Corrente (4.0 - 20.0 mA) para a escala de Vazão (0.0 - 150.0 m³/h)
  float vazao_m3h = mapFloat(corrente_mA, 4.0, 20.0, 0.0, 150.0);

  // 4. Exibe os resultados no LCD
  lcd.setCursor(0, 0);
  lcd.print("Corrente: ");
  lcd.print(corrente_mA, 1); // 1 casa decimal
  lcd.print(" mA  "); // Espaços para limpar dígitos anteriores
  
  lcd.setCursor(0, 1);
  lcd.print("Vazao: ");
  lcd.print(vazao_m3h, 1); // 1 casa decimal
  lcd.print(" m3/h");
  
  delay(250); // Breve atraso para suavizar a leitura no display
}
