void enviarSerial()
{ 
    // Montando data para adicionar no envio
    String sData = "";
    DateTime now = rtc.now();
    sData += "esp01:dt=";
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

    // Atualizando idade e indice de dias
    idade_dias = getIdade();
    indice_dias = getIndiceDias();

    if (DEBUG) { Serial.print("idade_dias: "); Serial.println(idade_dias); }
    if (DEBUG) { Serial.print("indice_dias: "); Serial.println(indice_dias); }

    Serial.print("buffer antes: ");
    Serial.println(buffer);

    // Acrescentando indice de dias
    String sDados = buffer;
    sDados += sData;
    sDados += ",esp01:id=00";
    sDados += indice_dias;

    // Acrescentando idade do lote
    sDados += ",esp01:dn=0";
    if (idade_dias < 10) sDados += "0";
    sDados += idade_dias;

    // Limpando buffers
    limpaBuffers();

    contador = 0;
    bool acheiEnter = false;
    int len = sDados.length();
    for (int i = 0; i < len; i++) {
      if (contador < BUFFER_SIZE) {
        char dadosAt = sDados.charAt(i);
        if (dadosAt == 13 && !acheiEnter) acheiEnter = true;
        buffer[contador++] = dadosAt;
      }
    }

    if (!acheiEnter) {
      buffer[contador++] = 13;
      Serial.println("Nao achei enter!");
    }
    else {
      Serial.println("Achei enter!");
    }

    Serial.print("enviarSerial: ");
    Serial.println(sDados);


    // Fingindo que enviou/recebeu dados da I2C
    terminou_dados = true;
    recebeI2C();
}


// esp01:an=004,esp01:tp=26.9,esp01:um=34.5,esp01:am=150,esp01:co=003,esp01:lu=010,esp01:pe=2.901,esp01:pr=900,esp01:bl=1,
// esp01:an=004,esp01:tp=15.9,esp01:um=345.5,esp01:am=150,esp01:co=003,esp01:lu=010,
// esp01:an=004,esp01:tp=26.9,esp01:um=34.5,esp01:am=150,esp01:co=003,esp01:lu=010,
void tratarSerial()
{
  for (int j = 0; j < BUFFER_SIZE - TAMANHO_FILTRO; j++)
  {
    if (strncmp(&buffer[j], filtro, TAMANHO_FILTRO) == 0)
    {
      tipo[0] = buffer[j + TAMANHO_FILTRO];
      tipo[1] = buffer[j + TAMANHO_FILTRO + 1];

      qtdeLeitura = 3;
      if (tipo[0] == 't' && tipo[1] == 'p') 
        qtdeLeitura = acharPonto(buffer, j);
      else if (tipo[0] == 'u' && tipo[1] == 'm') 
        qtdeLeitura = acharPonto(buffer, j);
      else if (tipo[0] == 'p' && tipo[1] == 'e') 
        qtdeLeitura = acharPonto(buffer, j) + 2;

      int i = 0;
      bool tpOk = true;
      while ((i < qtdeLeitura)) 
      { 
        valor[i] = buffer[j + (TAMANHO_FILTRO + 3) + i]; 
        if (tipo[0] == 't' && tipo[1] == 'p')
        {
          if (valor[i] != '0' && valor[i] != '1' && valor[i] != '2' && valor[i] != '3' && valor[i] != '4' && valor[i] != '5' &&
              valor[i] != '6' && valor[i] != '7' && valor[i] != '8' && valor[i] != '9' && valor[i] != '.' && valor[i] != 0)
          {
            tpOk = false;
            break;
          }
        }
        i++; 
      } 

      // Registrando que recebeu dados com sucesso!
      ultimavez_dados_esp07 = millis() + TEMPO_SEM_DADOS_ESP07;

      // Dados para calculo da sensacao termica e alertas
      if (tipo[0] == 't' && tipo[1] == 'p') tp = atof(valor);
      if (tipo[0] == 'u' && tipo[1] == 'm') um = atof(valor);
      if (tipo[0] == 'p' && tipo[1] == 'e') pe = atof(valor);
      
      if (tipo[0] == 'a' && tipo[1] == 'n')
      {
        unsigned int iValor = atoi(valor);
        an = SpeedCalcMs(iValor);
      }
      else if (tipo[0] == 'a' && tipo[1] == 'm')
      {
        unsigned int iValor = atoi(valor);
        am = constrain(map(iValor, AMONIA_ENTRADA_MIN, AMONIA_ENTRADA_MAX, AMONIA_SAIDA_MIN, AMONIA_SAIDA_MAX), AMONIA_SAIDA_MIN, AMONIA_SAIDA_MAX);
      }
      else if (tipo[0] == 'l' && tipo[1] == 'u')
      {
        unsigned int iValor = atoi(valor);
        lu = constrain(map(iValor, LUX_ENTRADA_MIN, LUX_ENTRADA_MAX, LUX_SAIDA_MIN, LUX_SAIDA_MAX), LUX_SAIDA_MIN, LUX_SAIDA_MAX);
      }
      else if (tipo[0] == 'b' && tipo[1] == 'l')
      {
        unsigned int iValor = atoi(valor);
        bl = iValor;
      }

      // Limpando arrays
      for (int i = 0; i < 3; i++) tipo[i] = 0;
      for (int i = 0; i < 18; i++) valor[i] = 0;
    }
  }
}
