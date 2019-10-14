boolean initGPRS()
{  
  // Configurando o pino de RESET
  pinMode(RESET_GSM, OUTPUT);
  digitalWrite(RESET_GSM, HIGH);
  
  // Descobrindo velocidade atual e configurando BPS
  configVelocidade();
  delay(2000);

  // Iniciando Serial do GPRS 
  Serial3.begin(BAUD_GSM);
  delay(300);

  // Limpando buffer da Serial do GPRS
  gprs_clear(10);
  
  // Enviando comando AT para confirmar que está tudo OK!
  Serial3.println(F("AT"));
  delay(2000);

  return verificaResposta();
}

void powerUpOrDown()
{
  //Liga o GSM Shield
  Serial.print(F("Liga GSM..."));

  pinMode(POWER_GSM, OUTPUT);
  digitalWrite(POWER_GSM, LOW);
  delay(1000);
  digitalWrite(POWER_GSM, HIGH);

  Serial.println(F("OK!"));
  delay(3000);
}

void configVelocidade()
{
  for (int i = 0; i < qtdeVelocidades; i++)
  {
    // Configura uma velocidade 
    Serial3.begin(velocidades[i]);
    delay(300);

    Serial.print(F("Testando Velocidade: "));
    Serial.println(velocidades[i]);

    // Envia um comando AT
    Serial3.println(F("AT"));
    Serial.println(F("AT"));
    delay(2000);

    // Verifica resposta
    // Achou a velocidade certa
    if (verificaResposta())
    {
      if (velocidades[i] != BAUD_GSM)
      {
        // Configurando a velocidade meta
        Serial3.print(F("AT+IPR="));
        Serial3.println(BAUD_GSM);
        delay(2000);
        
        // Verifica resposta
        verificaResposta();

        Serial3.begin(BAUD_GSM);
        delay(1000);
        
        Serial3.println("AT&W");
        Serial.println("AT&W");
        delay(2000);
  
        // Verifica resposta
        verificaResposta();
        
        // Reiniciando o módulo
        Serial.println(F("Reiniciando modulo"));
        efetuarResetGSM();
        //powerUpOrDown();
      }
      break;
    }
  }
}

void efetuarResetGSM()
{
  // Disparando o RESET do Módulo
  //digitalWrite(RESET_GSM, LOW);
  //delay(1000);
  //digitalWrite(RESET_GSM, HIGH);
  //delay(5000);
  
  gprs_send("AT+CPOWD=1\r\n");
  delay(5000);

  // Verifica resposta
  verificaResposta();

  // Religa o módulo
  powerUpOrDown();
}

boolean isConnected()
{
  boolean registrado = false;
  unsigned long tempo = millis() + TEMPO_LIGANDO;
  Serial.println(F("Verificando registro na rede!"));
  while (!registrado && tempo > millis())
  {
    gprs_send("AT+CREG?\r\n");
    if(check_gprs(5000, "+CREG: 0,1\r\n", 12)) registrado = true;
    if (!registrado)
    {
      gprs_send("AT+CREG?\r\n");
      if(check_gprs(5000, "+CREG: 0,5\r\n", 12)) registrado = true;
    }
  }
  if (registrado)
    Serial.println(F("OK! Registrado na rede!"));
  else
  {
    Serial.println(F("ERRO sem rede!"));
    //while(1);
  }
  return registrado;
}


bool initSAPBR()
{
  // Iniciando SAPBR
  bool ret = false;
  gprs_send("AT\r\n");
  if (check_gprs(500, "OK\r\n", 4))
  {
    gprs_send("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n");
    if (check_gprs(5000, "OK\r\n", 4))
    {
      gprs_send("AT+SAPBR=3,1,\"APN\",\"");
      Serial3.print(APN_URL);
      gprs_send("\"\r\n");
      if (check_gprs(5000, "OK\r\n", 4))
      {
        gprs_send("AT+SAPBR=3,1,\"USER\",\"");
        Serial3.print(APN_USER);
        gprs_send("\"\r\n");
        if (check_gprs(5000, "OK\r\n", 4))
        {
          gprs_send("AT+SAPBR=3,1,\"PWD\",\"");
          Serial3.print(APN_PWD);
          gprs_send("\"\r\n");
          if (check_gprs(5000, "OK\r\n", 4))
          {
            gprs_send("AT+SAPBR=1,1\r\n");
            if (check_gprs(30000, "OK\r\n", 4))
            {
              gprs_send("AT+SAPBR=2,1\r\n");
              if (check_gprs(5000, "+SAPBR: 1,1", 11))
              {
                ret = true;
              }
            }
          }
        }
      }
    }
  }
  return ret;
}

void endSAPBR()
{
  gprs_send("AT+HTTPTERM\r\n");
  check_gprs(2000, "OK\r\n", 4);
  gprs_send("AT+SAPBR=0,1\r\n");
  check_gprs(2000, "OK\r\n", 4);
}

bool isSAPBR()
{
  // Verificando SAPBR
  bool ret = false;
  gprs_send("AT+SAPBR=2,1\r\n");
  if (check_gprs(5000, "+SAPBR: 1,1", 11))
  {
    check_gprs(2000, "OK\r\n", 4);
    ret = true;
  }
  return ret;
}


bool enviarMsg(char *msg, unsigned int len)
{
  // Enviando Msg
  bool ret = false;
  gprs_send("AT+HTTPINIT\r\n");
  if (check_gprs(5000, "OK\r\n", 4))
  {
    gprs_send("AT+HTTPPARA=\"CID\",1\r\n");
    if (check_gprs(5000, "OK\r\n", 4))
    {
      //gprs_send("AT+HTTPPARA=\"URL\",\"http://www.systemsegcia.com.br/informacoes/add\"\r\n");
      gprs_send("AT+HTTPPARA=\"URL\",\"http://www.seriallink.com.br/systemseg/add.php\"\r\n");
      if (check_gprs(5000, "OK\r\n", 4))
      {
        gprs_send("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r\n");
        if (check_gprs(5000, "OK\r\n", 4))
        {
          gprs_send("AT+HTTPDATA=");
          Serial3.print(len, DEC);
          gprs_send(",30000\r\n");
          if (check_gprs(5000, "DOWNLOAD\r\n", 10))
          {
            for (int i = 0; i < len; i++) 
              Serial.print(msg[i]);
              
            for (int i = 0; i < len; i++) 
              Serial3.print(msg[i]);
            
            if (check_gprs(10000, "OK\r\n", 4))
            {
              gprs_send("AT+HTTPACTION=1\r\n");
              if (check_gprs_http(30000, GPRS_OK_MSG, GPRS_OK_MSG_LEN))
              {
                ret = true;
              }
            }
          }
        }
      }
    }
  }
  gprs_send("AT+HTTPTERM\r\n");
  check_gprs(2000, "OK\r\n", 4);
  return ret;
}


bool enviarTelegram(char *msg)
{
  unsigned int len = strlen(msg) + 4;
  // Enviando Telegram
  bool ret = false;
  gprs_send("AT+HTTPINIT\r\n");
  if (check_gprs(5000, "OK\r\n", 4))
  {
    gprs_send("AT+HTTPPARA=\"CID\",1\r\n");
    if (check_gprs(5000, "OK\r\n", 4))
    {
      gprs_send("AT+HTTPPARA=\"URL\",\"http://www.seriallink.com.br/systemseg/telegram.php\"\r\n");
      if (check_gprs(5000, "OK\r\n", 4))
      {
        gprs_send("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r\n");
        if (check_gprs(5000, "OK\r\n", 4))
        {
          gprs_send("AT+HTTPDATA=");
          Serial3.print(len, DEC);
          gprs_send(",30000\r\n");
          if (check_gprs(5000, "DOWNLOAD\r\n", 10))
          {
            Serial.print("msg=");
            for (int i = 0; i < len; i++) 
              Serial.print(msg[i]);

            Serial3.print("msg=");
            for (int i = 0; i < len; i++) 
              Serial3.print(msg[i]);
            
            if (check_gprs(10000, "OK\r\n", 4))
            {
              gprs_send("AT+HTTPACTION=1\r\n");
              if (check_gprs_http(30000, GPRS_OK_MSG, GPRS_OK_MSG_LEN))
              {
                ret = true;
              }
            }
          }
        }
      }
    }
  }
  gprs_send("AT+HTTPTERM\r\n");
  check_gprs(2000, "OK\r\n", 4);
  return ret;
}







boolean efetuarLigacao()
{
  boolean ret = false;
  for (int i = 0; i < qtdeCelular; i++)
  {
    if (qual_ligar[i] == 1)
    {
      gprs_send("ATD");
      Serial3.print(&celular[i][2]);
      gprs_send(";\r\n");
      if (check_gprs(30000, "OK\r\n", 4))
      {
        ret = true;
        delay(TEMPO_LIGACAO_ESPERA_ATENDER);
        gprs_send("ATH\r\n");
      }
    }
    // Delay entre as chamadas para dar tempo de 
    // desligar de uma pra ligar em outro telefone
    delay(TEMPO_ENTRE_LIGACOES); 
  }
  return false;
}

boolean enviarSMS(const char* alertamsg)
{
  boolean ret = false;
  char ctrlz = 0x1A;
  for (int i = 0; i < qtdeCelular; i++)
  {
    if (qual_sms[i] == 1)
    {
      gprs_send("AT+CMGF=1\r\n");
      if (check_gprs(5000, "OK\r\n", 4))
      {
        gprs_send("AT+CMGS=\"");
        Serial3.print(celular[i]);
        gprs_send("\"\r\n");
        if (check_gprs(5000, ">", 1))
        {
          //Serial.println(alertamsg);
          Serial3.print(alertamsg);
          Serial3.print(ctrlz);
          if (check_gprs(15000, "OK", 2))
          {
            ret = true;
          } 
        }
      }
    }
    // Delay entre um envio de SMS 
    // entre um número e outro
    delay(TEMPO_ENTRE_SMS);
  }
  return ret;
}

// Limpa serial do GPRS
void gprs_clear(int vezes)
{
  for (int i = 0; i < vezes; i++)
  {
    delay(1);
    while(Serial3.available()) Serial3.read();
  }
}

void gprs_send(const char *cmd)
{
  while(Serial3.available()) Serial3.read();
  //Serial.print(cmd);
  Serial3.print(cmd);
  delay(30);
}

bool check_gprs(unsigned long intervalo, char *resp, int len)
{
  char buff[BUFFER_SIZE];
  for (int i = 0; i < BUFFER_SIZE; i++) buff[i] = 0;
  bool ret = false;
  int conta = 0;
  int cc = 0;
  unsigned long t = millis() + intervalo;
  do 
  {
    cc = Serial3.read();
    delay(1);
    if (cc != -1) 
    {
      Serial.print((char)cc); 
      if (conta < BUFFER_SIZE) buff[conta++] = (char)cc; 
    }
    if (conta >= len)
    {
      for (int i = 0; i < conta; i++)
      {
        if (strncmp(&buff[i], resp, len) == 0)
        {
          ret = true;
          break;
        }
      }
    }
  } while ( t > millis() && !ret );
  while(Serial3.available()) Serial3.read();
  Serial.println();
  return ret;
}

bool check_gprs_http(unsigned long intervalo, char *resp, int len)
{
  char buff[BUFFER_SIZE];
  char ibuf[20];
  memset(ibuf, 0, 20);
  for (int i = 0; i < BUFFER_SIZE; i++) buff[i] = 0;
  bool ret = false;
  int conta = 0;
  int cc = 0;
  int ivar = 0;
  unsigned long t = millis() + intervalo;
  do 
  {
    cc = Serial3.read();
    delay(1);
    if (cc != -1) 
    {
      Serial.print((char)cc); 
      if (conta < BUFFER_SIZE) buff[conta++] = (char)cc; 
    }
    if (conta >= len)
    {
      for (int i = 0; i < conta; i++)
      {
        if (strncmp(&buff[i], resp, len) == 0)
        {
          for (int j = 0; j < 3; j++) 
          {
            cc = Serial3.read();
            delay(1);
            if (cc != -1) 
            {
              Serial.print((char)cc); 
              ibuf[j] = (char)cc; 
            }
          }
          ivar = atoi(ibuf);
          if (ivar >= 200 && ivar < 400) ret = true;
          break;
        }
      }
    }
  } while ( t > millis() && !ret );
  while(Serial3.available()) Serial3.read();
  Serial.println();
  return ret;
}

bool check_gprs_mask(unsigned long intervalo, char resp, int times)
{
  bool ret = false;
  int conta = 0;
  int cc = 0;
  unsigned long t = millis() + intervalo;
  do 
  {
    cc = Serial3.read();
    if (cc != -1) 
    {
      Serial.print((char)cc); 
      if ((char)cc == resp) conta++; 
    }
    if (conta >= times && (char)cc == '\n') ret = true;
  } while ( t > millis() && !ret );
  while(Serial3.available()) Serial3.read();
  Serial.println();
  return ret;
}

bool verificaResposta()
{
  boolean ok1 = false;
  boolean ok2 = false;
  // Verifica resposta
  while(Serial3.available())
  {
    int c = Serial3.read();
    if (c == 'O') ok1 = true;
    if (c == 'K' && ok1) ok2 = true;
    Serial.print((char)c);
  }
  return (ok1 && ok2);
}
