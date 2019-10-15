#include <EEPROM.h>
#include "U8glib.h"
#include <Wire.h>
#include "RTClib.h"


// Configurar aqui o inicio do ciclo. Ligar no GND pra iniciar.
#define PORTA_IDADE         41
#define EEPROM_IDADE_ADDR   1024

#define EEPROM_LEN          1000

// Porta RESET do ESP01
#define RESET_ESP01           24

// Configurar aqui a quantidade de erros que disparam um RESET do GSM
#define QTDE_ERROS_PARA_RESET 2

// Configurar aqui o tempo de ausência de informações do ESP07
#define TEMPO_SEM_DADOS_ESP07     45000 // em milissegundos

// Configurar aqui a quantidade de tempo para acusar um erro de falta de serial
#define TEMPO_ERRO_SERIAL 300000 // 5 minutos

// Configurar aqui o código do barracão
#define CODIGO_BARRACAO  "10";

// Configurar aqui o código do sensor
//#define CODIGO_SENSOR  "1"; // Colmeia
#define CODIGO_SENSOR  "2"; // Pinteria
//#define CODIGO_SENSOR  "3"; // Ventilaçao

// Configurar aqui o intervalo de envio sem alerta
#define INTERVALO_ENVIO_NORMAL       1200000   // 20 minutos
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

// Definir aqui suas configuracoes
#define RESET_GSM           36   // configurar aqui o pino de RESET do GSM
#define POWER_GSM           9   // configurar aqui o pino de Power do GSM
#define BAUD_GSM            9600 // mudar aqui a velocidade da serial do GSM
#define APN_URL             "zap.vivo.com.br"
#define APN_USER            "vivo"
#define APN_PWD             "vivo"

// Definir aqui a mensagem de retorno do PHP
#define GPRS_OK_MSG         "+HTTPACTION:1,"
#define GPRS_OK_MSG_LEN     14

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
#define BUFFER_SIZE         350
#define TAMANHO_FILTRO      6
#define LED_DEBUG           6

// Configurações de Tempo
//#define INTERVALO_ALERTA    300000UL // 5 minutos
#define INTERVALO_ALERTA    30000UL // 30 segundos
#define INTERVALO_LCD       10000UL // 10 segundos
#define TEMPO_LIGANDO       90000UL // 1m30s

// Configuração de depuração
// Somente ligar quando estiver testando no PC
#define DEBUG 0

// Porta do LEDL
#define LEDL                  13

// Velocidades de Baud Rate do Módulo GSM
// Devemos achar a velocidade atual do módulo e configurar ele pra 4800 BPS
const int qtdeVelocidades = 8;
const unsigned long velocidades[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };

// Declarando objeto do LCD (Enable, RW, RS, RESET)
U8GLIB_ST7920_128X64_1X u8g(46, 47, 48, 49);

// Variavel de tempo do LCD
unsigned long tempo_lcd = 0UL;

unsigned long tempo1vez_tempmax = 0UL;
unsigned long tempo1vez_tempmin = 0UL;
unsigned long tempo1vez_umidmin = 0UL;
unsigned long tempo1vez_umidmax = 0UL;
unsigned long tempo1vez_amonmin = 0UL;

boolean ligar1vez_umidmin = false;
boolean ligar1vez_umidmax = false;
boolean ligar1vez_tempmax = false;
boolean ligar1vez_tempmin = false;
boolean ligar1vez_amonmin = false;

boolean recebeu_dados = false;
boolean terminou_dados = false;

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

// Indice de dias será recebido pela serial do Arduino dos Reles
int indice_dias = 0; // dias 0-3
int idade_dias = 0;

unsigned long tempo_envio_normal  = 0UL;
unsigned long tempo_envio_alerta  = 0UL;

int contador = 0;
volatile boolean serialTerminou = false;
boolean serialRecebeu = false;

// Variaveis de controle da serial
unsigned long tempo_reset_serial = 0UL;

unsigned long ultimavez_dados_esp07 = 0UL;

char buffer[BUFFER_SIZE];
char buffer2[BUFFER_SIZE];
const char *filtro = "esp01:";
char tipo[] = { 0, 0, 0 };
char valor[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int qtdeLeitura = 0;

float a_t[] = { 0.0, 0.0, 0.0 };
float a_um[] = { 0.0, 0.0, 0.0 };
float a_pe[] = { 0.0, 0.0, 0.0 };

boolean started = false;

volatile boolean ocupado = false;

int contadorErroEnvio = 0;

boolean controleDuplicidade = false;

// string de teste
// esp01:an=004,esp01:tp=15.9,esp01:um=345.5,esp01:am=150,esp01:co=003,esp01:lu=010,

int am, ce, lu, bl;
float an, tp, um, pe;

// NOVA TABELA DE DIAS
const int dias_indice[] = { 0 ,1, 2, 3, 4, 5, 6 };
const int dias_maior[] = { 0, 4, 8, 15, 22, 29, 36 };
const int dias_menor[] = { 3, 7, 14, 21, 28, 35, -1 };

// Configurar aqui a data inicial do lote
//                             ano,mes,dia,hr,min,seg
DateTime dtini_lote = DateTime(2019, 10, 13, 23, 52, 0);

RTC_DS1307 rtc;

void setup() 
{
  // Limpando EEPROM
  //for (int i = 0; i < EEPROM.length(); i++) { EEPROM.write(i, 255); delay(1); }
  
  // Iniciando serial
  Serial.begin(9600);
  delay(200);
  if (DEBUG) Serial.println("Serial OK!");

  // Iniciando a Serial do ESP01
  if (!DEBUG) Serial1.begin(9600);
  if (!DEBUG) delay(200);
  if (DEBUG) Serial.println("ESP01 OK!");

  // Iniciando LED L
  pinMode(LEDL, OUTPUT);
  digitalWrite(LEDL, LOW);

  // Colocando porta do RESET em estado de alta impedância
  pinMode(RESET_ESP01, INPUT);

  // Iniciando RTC
  Wire.begin();
  rtc.begin();
  if (!rtc.isrunning()) 
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  if (DEBUG) Serial.println("RTC OK!");

  // Configura idade
  configIdade();
  if (DEBUG) Serial.println("IDADE OK!");

  // configurando dia do ciclo
  idade_dias = getIdade();
  indice_dias = getIndiceDias();
  if (DEBUG) Serial.println("DIAS OK!");
  if (DEBUG) { Serial.print("idade_dias: "); Serial.println(idade_dias); }
  if (DEBUG) { Serial.print("indice_dias: "); Serial.println(indice_dias); }

  // Configurando LCD
  u8g.setColorIndex(1);
  Serial.println("LCD OK");

  // Iniciando GPRS
  while (!initGPRS())
  {
    // Se não iniciou, liga o módulo
    Serial.println("Ligando GRPS...");
    powerUpOrDown();
    delay(3000);
  }
  Serial.println(F("GPRS Iniciado OK!"));

  // Limpa buffer de recebimento
  limpaBuffers();

  // Intervalo de envio dos dados para a Internet
  tempo_envio_normal = millis() + INTERVALO_ENVIO_NORMAL;
  tempo_envio_alerta = millis() + INTERVALO_ENVIO_ALERTA;

  // Led de debug
  pinMode(LED_DEBUG, OUTPUT);

  // Atualiza LCD
  atualizaTelaLCD(2);
  tempo_lcd = millis() + INTERVALO_LCD;

  
  for (int i = 0; i < EEPROM_LEN; i++) { EEPROM.write(i, 255); delay(1); }
}

void loop() 
{
  boolean available = false;
  if (DEBUG) available = Serial.available();
  else available = Serial1.available();
  if (available && !serialTerminou)
  {
    //serialRecebeu = false;
    
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
      if (DEBUG) Serial.println("Serial Terminou");
      if (DEBUG) { Serial.print("Contador: "); Serial.println(contador); }
      if (DEBUG) Serial.println(buffer);
    }
  }

  if (serialTerminou && contador > 0)
  {

    if (DEBUG) { Serial.print("Contador2: "); Serial.println(contador); }
    
    // Tratando dados da serial
    if (DEBUG) Serial.println("tratarSerial");
    tratarSerial();

    // reiniciando variáveis da serial
    if (DEBUG) Serial.println("serialTerminou");
    contador = 0;
    serialTerminou = false;

    // Avisando que dados estão disponíveis da serial
    if (DEBUG) Serial.println("serialRecebeu");
    serialRecebeu = true;

    // Enviando dados para o Arduino Principal
    if (DEBUG) Serial.println("enviarSerial");
    enviarSerial();

    // reiniciando buffers
    for (int i = 0; i < BUFFER_SIZE; i++) buffer[i] = 0;
  }
  
  // Se passou tempo, enviar dados pelo GPRS
  // Verifica primeiro se tem alerta
  if (tempo_envio_alerta < millis())
  {
    // Ativando led de debug
    digitalWrite(LED_DEBUG, HIGH);

    Serial.print("Qtde a enviar: ");
    Serial.println(qtdeMsgPos());

    if (!DEBUG) {
      
      // Verificando se existem registros
      if (isMsgPos())
      {
        // Verifica se GSM esta ligado
        if (isConnected()) 
        {
          // Pega registro do SD Card
          // e envia para o servidor web
          char bMsg[BUFFER_SIZE];
          unsigned int len = readMsgPos(&bMsg[0]);
          
          // Debug
          Serial.print("Enviando do loop: ");
          Serial.println(bMsg);
  
          Serial.print("Len: ");
          Serial.println(len);
  
          //Serial.print("Tratado: ");
          //Serial.println(trataRegistro(bMsg));
  
          ocupado = true;
  
          boolean envia = false;
          String msg = trataRegistro(bMsg);
  
          if (msg == "erro")
          {
            Serial.println("Erro generico de msg!");
          }
          else if (msg == "erro campos")
          {
            Serial.println("Erro de campos da msg!");
          }
          else if (msg == "erro conta")
          {
            Serial.println("Erro de qtde de campos da msg!");
          }
          else
          {
            Serial.println("Msg OK!");
  
            for (int i = 0; i < BUFFER_SIZE; i++) buffer2[i] = 0;
            msg.toCharArray(buffer2, BUFFER_SIZE);
            
            // verifica se tem alerta, se sim, envia para o servidor
            if (ligar1vez_amonmin || ligar1vez_umidmin || ligar1vez_tempmin || ligar1vez_tempmax)
            {
              // tem alerta então envia!
              // mas apenas se controle de duplicidade está ligado
              if (controleDuplicidade)
              {
                envia = true;
                controleDuplicidade = false;
              }
              // Senão espera por outra mensagem chegar
            }
            else
            {
              // se nao tem alerta, verifica se passou tempo normal de envio
              if (tempo_envio_normal < millis())
              {
                envia = true;
                // Resetando tempo de envio normal
                tempo_envio_normal = millis() + INTERVALO_ENVIO_NORMAL;
              }
            }
    
            // verifica se é pra enviar para o servidor
            if (envia)
            {
              // enviando
              Serial.print("Enviando do loop2: ");
              Serial.println(msg);
    
              // Verificando se SAPBR está conectado
              if (!isSAPBR()) 
              { 
                // Se não estiver, inicia
                if (!initSAPBR()) 
                {
                  // Se der erro, termina e tenta de novo depois
                  endSAPBR();
                  delay(3000);
                  ocupado = false;
                  return; 
                }
              }
              
              if (enviarMsg(buffer2, msg.length()))
              {
                // Se o envio deu certo, apaga o registro
                delMsgPos();
  
                delay(500);
                Serial.println("Cheguei aqui. Codigo: 1H2J3K4L");
    
                // Se deu certo limpa contador de erros
                contadorErroEnvio = 0;
              }
              else
              {
                // Se deu erro incrementa contador de erros
                contadorErroEnvio++;
              }
            }
            else
            {
              Serial.print("Apagando msg: ");
              Serial.println(msg);
              
              // senão apenas apaga o registro
              delMsgPos();
            }
            
            delay(TEMPO_APOS_ENVIO_SERVIDOR);
  
          }
  
          ocupado = false;
  
        }
  
        // Limpando EEPROM
        //for (int i = 0; i < EEPROM.length(); i++) { EEPROM.write(i, 255); delay(1); }
      }
    }
    
    // Desativando led de debug
    digitalWrite(LED_DEBUG, LOW);
    
    // Resetando tempo de envio com alerta
    tempo_envio_alerta = millis() + INTERVALO_ENVIO_ALERTA;

    // Limpando serial
    Serial.print("Limpando serial...");
    while(Serial1.available())
    {
      char limpa = (char)Serial1.read();
      Serial.print('.');
    }
    Serial.println("ok!");
    
    Serial.print("Ocupado? ");
    Serial.println((ocupado ? "sim" : "nao"));
  }

  if (!recebeu_dados && tempo_lcd < millis())
  {
    atualizaTelaLCD(2);
    tempo_lcd = millis() + INTERVALO_LCD;
  }

  // Verificando se o GSM precisa de um reset
  if (contadorErroEnvio >= QTDE_ERROS_PARA_RESET)
  {
    // Efetuando reset
    efetuarResetGSM();

    if (DEBUG) Serial.println("Efetuei reset");
    if (DEBUG) { Serial.print("serialTerminou? "); Serial.println(serialTerminou ? "sim" : "nao"); }
    serialTerminou = false;

    // Limpando SDCard
    //while (isMsgPos()) { delMsgPos(); delay(300); }
    // Limpando EEPROM
    //for (int i = 0; i < EEPROM.length(); i++) { EEPROM.write(i, 255); delay(1); }

    // Reiniciando contador
    contadorErroEnvio = 0;
  }
}
