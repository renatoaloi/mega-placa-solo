char* incluirTemp(const char* motivo, float t)
{
  char c[35];
  for (int i = 0; i < 35; i++) c[i]=0;
  String s = "                                   ";
  s = "";
  s.concat(motivo);
  s.concat(", Temp="); 
  s.concat(String(t));
  s.toCharArray(c, 35);
  return c;
}

float calcularSensacaoTermica(float t, float um)
{
  if (t >= 80.0)
  {
    return (2.04901523 * t) + (10.14333127 * um) 
         - (0.22475541 * t * um) - (6.83783 * 0.001 * (t * t)) 
         - (5.481717 * 0.01 * (um * um)) + (1.22874 * 0.001 * (t * t) * um) 
         + (8.5282 * 0.0001 * t * (um * um)) - (1.99 * 0.000001 * (t * t) * (um * um))
         - 42.379;
  }
  else return t;
}

float converteFtoC(float f)
{
  return (5.0 / 9.0) * (f - 32.0);
}

float converteCtoF(float c)
{
  return (9.0 / 5.0) * c + 32.0;
}

int acharPonto(char *__buffer, int idx)
{
  int i = idx;
  while (__buffer[i + TAMANHO_FILTRO + 3] != '.' && i < BUFFER_SIZE) i++;
  return ((i - idx) + 2);
}

void limpaBuffers()
{
  // reiniciando buffer e estruturas de dados
  for (int i = 0; i < BUFFER_SIZE; i++) buffer[i] = 0;
  contador = 0;
  //qtdeLeitura = 0;
}

unsigned int RPMcalc(unsigned long contador)
{
  return ((contador) * 60) / (5000 / 1000); 
}

float SpeedCalcMs(unsigned long contador)
{
  float pi = 3.14159265;
  int radius = 147;
  return ((4.0 * pi * radius * RPMcalc(contador)) / 60.0) / 1000.0;
}

float SpeedCalcKm(unsigned long contador)
{
  return SpeedCalcMs(contador) * 3.6;
}

void configIdade(){
  pinMode(PORTA_IDADE, INPUT);
  digitalWrite(PORTA_IDADE, HIGH);
  delay(300);

  if (digitalRead(PORTA_IDADE) == LOW)
  {
    unsigned long start_lote = dtini_lote.unixtime();
    EEPROMWritelong(EEPROM_IDADE_ADDR, start_lote);
    delay(300);
    atualizaTelaLCD(3);
    while(1);
  }
}

int getIdade()
{
  unsigned long start_lote = EEPROMReadlong(EEPROM_IDADE_ADDR);
  if (DEBUG) { Serial.print(F("Start lote: ")); Serial.println(start_lote, DEC); }
  DateTime dtnow = rtc.now();
  unsigned long now_lote = dtnow.unixtime();
  if (DEBUG) { Serial.print(F("Now lote: ")); Serial.println(now_lote, DEC); }
  int idade = (now_lote - start_lote) / 86400.0;
  return idade;
}

int getIndiceDias()
{
  int indice = 0;
  int idade = getIdade();

  int dias_len = sizeof(dias_indice) / sizeof(int);
  for (int i = 0; i < dias_len; i++)
  {
    if (dias_menor[i] == -1)
    {
      if (idade >= dias_maior[i])
      {
        indice = dias_indice[i];
        break;
      }
    }
    else
    {
      if (idade >= dias_maior[i] && idade <= dias_menor[i])
      {
        indice = dias_indice[i];
        break;
      }
    }
  }

  return indice;
}

void EEPROMWritelong(int address, unsigned long value)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  
  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

unsigned long EEPROMReadlong(int address)
{
  //Read the 4 bytes from the eeprom memory.
  unsigned long four = EEPROM.read(address);
  unsigned long three = EEPROM.read(address + 1);
  unsigned long two = EEPROM.read(address + 2);
  unsigned long one = EEPROM.read(address + 3);
  
  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

char * dateToString(DateTime now) {
  String sData = "";
  sData += String(now.year());
  if (now.month() < 10) sData += "0";
  sData += String(now.month());
  if (now.day() < 10) sData += "0";
  sData += String(now.day());
  sData += " ";
  if (now.hour() < 10) sData += "0";
  sData += String(now.hour());
  sData += ":";
  if (now.minute() < 10) sData += "0";
  sData += String(now.minute());
  sData += ":";
  if (now.second() < 10) sData += "0";
  sData += String(now.second()); 
  return sData.c_str();
}

String dateToStringWeb(DateTime now) {
  String sData = "";
  sData += String(now.year());
  sData += "-";
  if (now.month() < 10) sData += "0";
  sData += String(now.month());
  sData += "-";
  if (now.day() < 10) sData += "0";
  sData += String(now.day());
  sData += "+";
  if (now.hour() < 10) sData += "0";
  sData += String(now.hour());
  sData += "%3A";
  if (now.minute() < 10) sData += "0";
  sData += String(now.minute());
  sData += "%3A";
  if (now.second() < 10) sData += "0";
  sData += String(now.second()); 
  return sData;
}
