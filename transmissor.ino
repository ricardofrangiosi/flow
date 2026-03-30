// ============================================================
//  PRÁTICA 1 – TRANSMISSOR ANALÓGICO (4-20 mA)
//  Disciplina: Medição de Vazão | Plataforma: Tinkercad
//
//  CORREÇÃO: Substituído LCD I2C (incompatível c/ Tinkercad)
//            por LCD paralelo 16×2 (padrão nativo do Tinkercad)
//
//  Pinos LCD Paralelo:
//    RS → 12 | EN → 11 | D4 → 6 | D5 → 5 | D6 → 4 | D7 → 3
//
//  Pino Potenciômetro (simula transmissor 4-20 mA):
//    Wiper (pino central) → A0
// ============================================================

#include <LiquidCrystal.h>

// RS, EN, D4, D5, D6, D7 — pinagem padrão Tinkercad
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

const int PIN_SENSOR = A0; // Potenciômetro simula o sinal 4-20 mA condicionado

// ── mapFloat() ──────────────────────────────────────────────
// O map() nativo do Arduino só trabalha com inteiros.
// Para instrumentação (4-20 mA com casas decimais) usamos float.
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Pratica 1:");
  lcd.setCursor(0, 1);
  lcd.print("Transmissor");
  delay(2000);
  lcd.clear();
}

void loop() {
  // 1. Lê o sinal do "sensor" (potenciômetro)
  // ADC do Arduino converte 0–5V → 0–1023
  int leituraADC = analogRead(PIN_SENSOR);

  // 2. Mapeia ADC (0–1023) → Corrente (4.0–20.0 mA)
  //    0   → 4.0 mA  (Zero Vivo: tubulação integra, mas vazia)
  //    511 → 12.0 mA (50% da escala = 75 m³/h)
  //    1023 → 20.0 mA (fundo de escala = 150 m³/h)
  float corrente_mA = mapFloat(leituraADC, 0, 1023, 4.0, 20.0);

  // 3. Mapeia Corrente (4.0–20.0 mA) → Vazão (0.0–150.0 m³/h)
  float vazao_m3h = mapFloat(corrente_mA, 4.0, 20.0, 0.0, 150.0);

  // 4. Exibe no LCD
  lcd.setCursor(0, 0);
  lcd.print("I: ");
  lcd.print(corrente_mA, 1); // 1 casa decimal
  lcd.print(" mA    ");      // espaços para limpar dígitos anteriores

  lcd.setCursor(0, 1);
  lcd.print("Q: ");
  lcd.print(vazao_m3h, 1);
  lcd.print(" m3/h  ");

  delay(250);
}
