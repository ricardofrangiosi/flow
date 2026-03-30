#include <LiquidCrystal.h>

// Pinos do display LCD paralelo
LiquidCrystal lcd(12, 11, 5, 4, 3, 7);  // RS, EN, D4, D5, D6, D7

// Pinos do Sensor Ultrassônico HC-SR04
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;

// Constantes Físicas do Tanque e do Sistema
const float ALTURA_TANQUE_M = 1.00; // O sensor está a 1 metro de altura (100 cm)
const float RAIO_M = 0.50;          // Raio do tanque cilíndrico (50 cm)
// Área da base = Pi * r²
const float AREA_TANQUE_M2 = 3.14159 * (RAIO_M * RAIO_M);

// Variáveis para rastreamento de volume ao longo do tempo
float volumeAnterior_L = 0.0;
unsigned long tempoAnterior = 0;
const unsigned long INTERVALO_ATUALIZACAO = 5000; // Analisa escoamento a cada 5 segundos (5000 ms)

float nivelAtual_M = 0;
float volumeAtual_L = 0;

// Função auxiliar para disparar o pulso ultrassônico e medir distância
float medirDistanciaMetros() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Duração do pulso de retorno
  long duracao = pulseIn(ECHO_PIN, HIGH);
  
  // Distância (cm) = duração * 0.0343 (vel. som cm/us) / 2
  float dist_cm = (duracao * 0.0343) / 2.0;
  return dist_cm / 100.0; // retorna em metros
}

void setup() {
  lcd.begin(16, 2);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Calibração inicial: define o volume de partida
  float dist_m = medirDistanciaMetros();
  float nivel_m = ALTURA_TANQUE_M - dist_m;
  if(nivel_m < 0) nivel_m = 0;
  // Volume m³ = Área * Altura | Volume Litros = Volume m³ * 1000
  volumeAnterior_L = AREA_TANQUE_M2 * nivel_m * 1000.0; 
  
  lcd.setCursor(0, 0);
  lcd.print("Iniciando CAESB");
  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long tempoAtual = millis();

  // 1. LER DISTÂNCIA (Nível do Tanque)
  float dist_m = medirDistanciaMetros();
  nivelAtual_M = ALTURA_TANQUE_M - dist_m;
  if (nivelAtual_M < 0) nivelAtual_M = 0; // Proteção contra medidas negativas
  
  // Calcula volume atual do tanque
  volumeAtual_L = (AREA_TANQUE_M2 * nivelAtual_M) * 1000.0;

  // Atualiza Nível instantâneo no display
  lcd.setCursor(0, 0);
  lcd.print("Nivel: ");
  lcd.print(nivelAtual_M * 100.0, 1); // Exibe em cm
  lcd.print(" cm  ");

  // 2. CALCULAR VAZÃO A CADA 5 SEGUNDOS
  if (tempoAtual - tempoAnterior >= INTERVALO_ATUALIZACAO) {
    
    // Delta de Tempo (Δt)
    float deltaTempo_s = (tempoAtual - tempoAnterior) / 1000.0;
    
    // Variação de Volume (ΔV): Quanto esvaziou no período
    float deltaVolume_L = volumeAnterior_L - volumeAtual_L;
    
    // Calcula vazão de escoamento (Se volumeAnterior > atual = esvaziando = vazão de saída)
    float vazao_Ls = 0.0;
    if (deltaVolume_L > 0) {
      vazao_Ls = deltaVolume_L / deltaTempo_s; // Litros por segundo
    }

    // Exibe a vazão no LCD
    lcd.setCursor(0, 1);
    lcd.print("Vaz: ");
    lcd.print(vazao_Ls, 2);
    lcd.print(" L/s  ");

    // Prepara para a próxima janela de 5s
    volumeAnterior_L = volumeAtual_L;
    tempoAnterior = tempoAtual;
  }
  
  delay(250); // Delay curto para o simulador não sobrecarregar
}
