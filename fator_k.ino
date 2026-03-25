#include <LiquidCrystal.h>

// Pinos do display LCD em modo paralelo (Tinkercad padrão)
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

const int PINO_PULSO = 2; // O pino 2 do Arduino Uno suporta interrupção externa (INT0)
const float FATOR_K = 450.0; // Constante do medidor: 450 pulsos por Litro

// Variável 'volatile' pois é alterada dentro da Interrupção
volatile unsigned long contadorPulsos = 0;

unsigned long tempoAnterior = 0;
const unsigned long INTERVALO_LEITURA = 1000; // Analisa pulsos a cada 1000 ms (1 segundo)

// Função de Interrupção (ISR - Interrupt Service Routine)
// Esta função é chamada automaticamente a cada pulso recebido no pino 2
void contarPulso() {
  contadorPulsos++;
}

void setup() {
  lcd.begin(16, 2);
  
  // Configura o pino de leitura do Gerador de Funções
  pinMode(PINO_PULSO, INPUT_PULLUP);
  
  // Habilita a interrupção no pino 2, disparando na borda de subida (RISING) do sinal
  attachInterrupt(digitalPinToInterrupt(PINO_PULSO), contarPulso, RISING);

  lcd.setCursor(0, 0);
  lcd.print("Fator K: 450");
  lcd.setCursor(0, 1);
  lcd.print("pulsos/Litro");
  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long tempoAtual = millis();
  
  // Quando passa exato 1 segundo (janela de tempo)
  if (tempoAtual - tempoAnterior >= INTERVALO_LEITURA) {
    // 1. Pausa as interrupções para ler e zerar o contador com segurança
    noInterrupts();
    unsigned long pulsos = contadorPulsos;
    contadorPulsos = 0; // Prepara para contar o próximo segundo
    interrupts(); // Reativa interrupções
    
    tempoAnterior = tempoAtual;

    // 2. A Frequência (Hz) em 1 segundo é exatamente o número de pulsos lidos
    float frequencia_Hz = (float)pulsos;

    // 3. Calcula a Vazão em Litros por Minuto (L/min)
    // Vazão = (Frequência em Hz * 60) / Fator_K
    float vazao_Lmin = (frequencia_Hz * 60.0) / FATOR_K;

    // 4. Mostra no LCD
    lcd.setCursor(0, 0);
    lcd.print("Freq: ");
    lcd.print(frequencia_Hz, 0);
    lcd.print(" Hz    ");

    lcd.setCursor(0, 1);
    lcd.print("Vazao:");
    lcd.print(vazao_Lmin, 2); // 2 casas decimais
    lcd.print(" L/min ");
  }
}
