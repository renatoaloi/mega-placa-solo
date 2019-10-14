void saveMsgPos(char * msg)
{
  bool last13 = true;
  for (int i = 0; i < EEPROM_LEN; i++) { EEPROM.write(i, 255); delay(1); }
  
  Serial.print("Save: ");
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    if (msg[i] == 0) {
      if (last13)
      {
        last13 = false;
        EEPROM.write(i, 13);
        Serial.print(13, DEC);
        if (i < BUFFER_SIZE - 1) Serial.print(',');
      }
      else
      {
        EEPROM.write(i, 0);
        break;
      }
    }
    else
    {
      Serial.print(msg[i], DEC);
      if (i < BUFFER_SIZE - 1) Serial.print(',');
      EEPROM.write(i, msg[i]);
    }
  }
  Serial.println();
  EEPROM.write(BUFFER_SIZE-1, 0);
}

int qtdeMsgPos()
{
  int qtde = 0;
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    byte b = EEPROM.read(i);
    if (b == 13)
    {
      qtde++;
      break;
    }
  }
  return qtde;
}

bool isMsgPos()
{
  bool ret = false;
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    byte b = EEPROM.read(i);
    if (b == 13)
    {
      ret = true;
      break;
    }
  }
  return ret;
}

int readMsgPos(char * msg)
{
  int qtde = 0;
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    byte b = EEPROM.read(i);
    if (b == 13)
    {
      msg[qtde] = 0;
      break;
    }
    else
    {
      msg[qtde++] = (char)b;
    }
  }
  return qtde;
}

void delMsgPos()
{
  for (int i = 0; i < EEPROM_LEN; i++)
  {
    EEPROM.write(i, 255);
  }
}
