void configIdade(){
  pinMode(PORTA_IDADE, INPUT);
  digitalWrite(PORTA_IDADE, HIGH);
  delay(300);

  if (digitalRead(PORTA_IDADE) == LOW)
  {
    unsigned long start_lote = dtini_lote.unixtime();
    EEPROMWritelong(EEPROM_IDADE_ADDR, start_lote);
    delay(300);
    //atualizaTelaLCD(3);
    while(1);
  }
}

int getIdade()
{
  unsigned long start_lote = EEPROMReadlong(EEPROM_IDADE_ADDR);
  Serial.print("Start lote: ");
  Serial.println(start_lote, DEC);
  DateTime dtnow = rtc.now();
  unsigned long now_lote = dtnow.unixtime();
  Serial.print("Now lote: ");
  Serial.println(now_lote, DEC);
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
