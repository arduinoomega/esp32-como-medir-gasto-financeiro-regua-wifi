#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
BlynkTimer timer;

char auth[] = "Token de Autenticação gerado"; //esse codigo vc receberá por email no cadastro no blynk

//-------- Configurações de Wi-Fi -----------
char ssid[] = "sua rede"; //insira aqui a senha do seu wifi
char pass[] = "sua senha"; //insira aqui a rede do seu wifi

float vetCorrente[300]; //numero de amostras para fazer a media da corrente
float valor_medio;
float valor_Corrente;
float tariff = 0.75, account, valueAccount; //variaveis referente ao calculo do consumo da conta
unsigned long timeNow, timeReadConsumption = 0; //variaveis para controlar o tempo de verificar o consumo

BLYNK_CONNECTED()
{ // Se conectar com Blynk
  Blynk.syncVirtual(V3);    // Sincroniza com o pino V3
}

void sendSensor()
{
  double maior_Valor = 0;
  double valor_Corrente = 0;

  float tensao = 220; //Coloque aqui a tensão da rede de energia de sua cidade
  float potencia = 0;

  for (int i = 0; i < 300; i++) //laço que faz a leitura da corrente
  {
    vetCorrente[i] = analogRead(39);
    delayMicroseconds(600);
  }

  int somaTotal = 0;

  for (int i = 0; i < 300; i++)
  {
    somaTotal = vetCorrente[i] + somaTotal;
  }
  valor_medio = somaTotal / 300; //Calculo do valor medio da corrente
  valor_medio = valor_medio * 0.0012; //ajustando o valor medio com o fator de erro
  valor_Corrente = (valor_medio - 2.5); 
  valor_Corrente = valor_Corrente * 1000; //multiplicando a corrrente por 1000 pois estava em miliamperes
  valor_Corrente = valor_Corrente / 66; //sensibilidade : 66mV/A para ACS712 30A

  //Impressão dos resultados
  Serial.print("Corrente = ");
  Serial.print(valor_Corrente);
  Serial.println(" A");
  potencia = valor_Corrente * tensao;
  Serial.print("Potência = ");
  Serial.print(potencia);
  Serial.println(" W");
  Serial.print(".");

  //Conferencia de o tempo de 01 minuto passou para verificar o valor da conta
  timeReadConsumption = millis() - timeNow;
  if (timeReadConsumption > 60000) {
  //valor da conta = potencia em W * converte para KW * 1 minuto em fraçao de hora * tarifa da sua região
    account = potencia * 0, 001 * 0.0166 * tariff;
    valueAccount = valueAccount + account;
    Serial.print("Valor da conta: R$ ");
    Serial.print(valueAccount);
    Serial.println(" !");
    timeNow = millis();
  }

  delay(500);
  Serial.println("");
  Blynk.virtualWrite(V3, valor_Corrente);
  delay(100);
}
void setup()
{
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(27, OUTPUT);
  Blynk.begin(auth, ssid, pass); // TOKEN+REDE+SENHA
  timer.setInterval(1000L, sendSensor);
  timeNow = millis();
}
void loop()
{
  timer.run();
  Blynk.run();
}
