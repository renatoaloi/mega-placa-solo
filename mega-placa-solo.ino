#include <EEPROM.h>
#include "U8glib.h"
#include <Wire.h>
#include "RTClib.h"
#include <MemoryFree.h>


// Configurar aqui o inicio do ciclo. Ligar no GND pra iniciar.
#define PORTA_IDADE         41
#define EEPROM_IDADE_ADDR   4000

// Configurar aqui o código do barracão
#define CODIGO_BARRACAO  "10";

// Configurar aqui o código do sensor
//#define CODIGO_SENSOR  "1"; // Colmeia
#define CODIGO_SENSOR  "2"; // Pinteria
//#define CODIGO_SENSOR  "3"; // Ventilaçao

// Definir aqui suas configuracoes
#define RESET_GSM           36   // configurar aqui o pino de RESET do GSM
#define POWER_GSM           9   // configurar aqui o pino de Power do GSM
#define BAUD_GSM            9600 // mudar aqui a velocidade da serial do GSM
#define APN_URL             "zap.vivo.com.br"
#define APN_USER            "vivo"
#define APN_PWD             "vivo"

// Porta RESET do ESP01
#define RESET_ESP01           24

// Definir aqui a mensagem de retorno do PHP
#define GPRS_OK_MSG         "+HTTPACTION:1,"
#define GPRS_OK_MSG_LEN     14

// Configurar aqui o intervalo de envio sem alerta
//#define INTERVALO_ENVIO_NORMAL       1200000   // 20 minutos
#define INTERVALO_ENVIO_NORMAL       60000
// Configurar aqui o intervalo de envio qndo está com problema e tem alerta pra enviar
//#define INTERVALO_ENVIO_ALERTA       120000    // 2 minutos
#define INTERVALO_ENVIO_ALERTA       15000    // 15 segundos

// Configurar aqui o tempo em que depois de feita a ligação o sistema espera antes de desligar
#define TEMPO_LIGACAO_ESPERA_ATENDER 60000 // 1 minuto
// Definir aqui o tempo entre uma ligação e outra dos telefones cadastrados
#define TEMPO_ENTRE_LIGACOES         20000 // 20 segs
// Definir aqui o tempo entre um envio de SMS e outro dos telefones cadastrados
#define TEMPO_ENTRE_SMS              20000 // 20 segs
// Definir aqui o tempo depois que o sistema efetuou todas as ligações e todos os envios de SMS
#define TEMPO_APOS_LIGACAO_E_SMS     20000 // 20 segs
// Definir aqui o tempo de espera depois que envia msg para o servidor 
#define TEMPO_APOS_ENVIO_SERVIDOR    10000 // 10 segs

// Configurações de Tempo
//#define INTERVALO_ALERTA    300000UL // 5 minutos
#define INTERVALO_ALERTA    30000UL // 30 segundos
#define INTERVALO_LCD       10000UL // 10 segundos
#define TEMPO_LIGANDO       90000UL // 1m30s

//configurar aqui os limites
#define LUX_ENTRADA_MAX     33
#define LUX_ENTRADA_MIN     520
#define LUX_SAIDA_MAX       400
#define LUX_SAIDA_MIN       0
#define AMONIA_ENTRADA_MAX  380
#define AMONIA_ENTRADA_MIN  10
#define AMONIA_SAIDA_MAX    200
#define AMONIA_SAIDA_MIN    0

// Configuracoes internas (NAO MEXER)
#define BUFFER_SIZE         500
#define TAMANHO_FILTRO      6

// Configuração de depuração
// Somente ligar quando estiver testando no PC
#define DEBUG 1


// Velocidades de Baud Rate do Módulo GSM
// Devemos achar a velocidade atual do módulo e configurar ele pra 4800 BPS
const int qtdeVelocidades = 8;
const unsigned long velocidades[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };

const int   qtdeCelular  = 1;
const char* celular[]    = { "44998416732", "11964676136", "11964676136" };
const char  qual_ligar[] = { 1, 0, 0 }; // Configurar 1 para ligar e 0 para não ligar
const char  qual_sms[]   = { 1, 0, 0 }; // Configurar 1 para enviar sms e 0 para não enviar
const char* motivos[]    = { "temperatura baixa", "temperatura alta", "acumulo de amonia", "umidade alta", "umidade baixa" };
const char* temp_dias[]  = { "0-3", "4-7", "8-14", "15-21", "22-28", "29-35", "36-FIM" };
const float temp_min[]   = {  30.0,  28.0,  26.0,   24.5,    23.0,    21.0,    20.0 };
const float temp_max[]   = {  34.0,  32.0,  30.0,   28.5,    27.0,    25.0,    24.0 };
const float umid_min[]   = {  60.0,  60.0,  60.0,   60.0,    60.0,    60.0,    60.0 };
const float umid_max[]   = {  70.0,  70.0,  70.0,   70.0,    70.0,    70.0,    70.0 };

// NOVA TABELA DE DIAS
const int dias_indice[] = { 0 ,1, 2, 3, 4, 5, 6 };
const int dias_maior[] = { 0, 4, 8, 15, 22, 29, 36 };
const int dias_menor[] = { 3, 7, 14, 21, 28, 35, -1 };

// Estrutura de dados dos campos da serial
const char *filtro = "esp01:";
struct dadosType {
  int anem;
  int amonia;
  int corrente;
  int lux;
  int pressao;
  int balanca;
  int indicedias;
  int idade;
  float temp;
  float umid;
  float peso;
  DateTime dt;
};

dadosType dados;

float a_t[] = { 0.0, 0.0, 0.0 };
float a_um[] = { 0.0, 0.0, 0.0 };
float a_pe[] = { 0.0, 0.0, 0.0 };

// Indice de dias será calculado e gravado na eeprom
int indice_dias = 0; // dias 0-3
int idade_dias = 0;

unsigned long tempo_envio_normal  = 0UL;
unsigned long tempo_envio_alerta  = 0UL;

// Variavel de tempo do LCD
unsigned long tempo_lcd = 0UL;

int contador = 0;
boolean serialTerminou = false;
boolean recebeu_dados = false;

int contadorErroEnvio = 0;

char buffer[BUFFER_SIZE];

// Configurar aqui a data inicial do lote
//                             ano,mes,dia,hr,min,seg
DateTime dtini_lote = DateTime(2019, 10, 13, 23, 52, 0);

RTC_DS1307 rtc;

// Declarando objeto do LCD (Enable, RW, RS, RESET)
U8GLIB_ST7920_128X64_1X u8g(46, 47, 48, 49);


void setup() 
{
  // Iniciando serial
  Serial.begin(9600);
  delay(200);
  if (DEBUG) Serial.println(F("Serial OK!"));

  // Iniciando a Serial do ESP01
  if (!DEBUG) Serial1.begin(9600);
  if (!DEBUG) delay(200);
  if (DEBUG) Serial.println(F("ESP01 OK!"));

  // Colocando porta do RESET em estado de alta impedância
  pinMode(RESET_ESP01, INPUT);

  // Iniciando RTC
  Wire.begin();
  rtc.begin();
  if (!rtc.isrunning()) 
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  if (DEBUG) Serial.println(F("RTC OK!"));

  // Configura idade
  configIdade();
  if (DEBUG) Serial.println(F("IDADE OK!"));

  // configurando dia do ciclo
  idade_dias = getIdade();
  indice_dias = getIndiceDias();
  if (DEBUG) Serial.println(F("DIAS OK!"));
  if (DEBUG) { Serial.print(F("idade_dias: ")); Serial.println(idade_dias); }
  if (DEBUG) { Serial.print("indice_dias: "); Serial.println(indice_dias); }

  // Configurando LCD
  u8g.setColorIndex(1);
  Serial.println(F("LCD OK"));

  // Iniciando GPRS
  while (!initGPRS())
  {
    // Se não iniciou, liga o módulo
    Serial.println(F("Ligando GRPS..."));
    powerUpOrDown();
    delay(3000);
  }
  Serial.println(F("GPRS Iniciado OK!"));

  // Limpa buffer de recebimento
  limpaBuffers();

  // Intervalo de envio dos dados para a Internet
  tempo_envio_normal = millis() + INTERVALO_ENVIO_NORMAL;
  tempo_envio_alerta = millis() + INTERVALO_ENVIO_ALERTA;

  // Atualiza LCD
  if (DEBUG) Serial.println(F("atualizaTelaLCD2"));
  atualizaTelaLCD(2);
  tempo_lcd = millis() + INTERVALO_LCD;
}

void loop() 
{
  boolean available = false;
  if (DEBUG) available = Serial.available();
  else available = Serial1.available();
  if (available && !serialTerminou)
  {
    char c = 0;
    if (DEBUG) c = Serial.read();
    else c = Serial1.read();
    
    if (c != 13)
    {
      if (contador < BUFFER_SIZE - 1)
        buffer[contador++] = c;
    }
    else {
      serialTerminou = true;
      if (DEBUG) Serial.println(F("Serial Terminou"));
      if (DEBUG) { Serial.print(F("Contador: ")); Serial.println(contador); }
      if (DEBUG) Serial.println(buffer);
    }
  }

  if (serialTerminou && contador > 0 && !recebeu_dados)
  {
    // reiniciando variáveis da serial
    if (DEBUG) Serial.println(F("serialTerminou"));
    contador = 0;
    serialTerminou = false;

    // filtrando dados da serial
    dados = filtroDados(&buffer[0]);

    // Avisando que dados estão disponíveis da serial
    recebeu_dados = true;
  }

  if (recebeu_dados && tempo_lcd < millis())
  {
    // Atualizando LCD
    if (DEBUG) Serial.println(F("atualizaTelaLCD3"));
    atualizaTelaLCD(1);

    // Reiniciando tempo de atualização do LCD
    tempo_lcd = millis() + INTERVALO_LCD;
  }

  // Se passou tempo, enviar dados pelo GPRS
  // Verifica primeiro se tem alerta
  if (recebeu_dados && tempo_envio_alerta < millis())
  {
    // Limpando a variável de recebimento de dados
    recebeu_dados = false;
    
    // enviar via GSM
    enviaGSM();
  }
}
