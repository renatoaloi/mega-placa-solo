void recebeI2C()
{
    if (terminou_dados)
    {
      terminou_dados = false;
      
      // Tratando dados da serial
      tratarMensagem();
    
      // Avisando que recebeu dados
      Serial.println("Recebido!");
    
      // Avisando que dados estão disponíveis da serial
      recebeu_dados = true;

      // Ligando o controle de duplicidade
      controleDuplicidade = true;
      
      // Limpando buffers
      limpaBuffers();
    }
}

void tratarMensagem()
{
  // Grava mensagem
  saveMsgPos(&buffer[0]);
  
  // Apresenta na tela de LCD
  atualizaTela(); 
}

String trataRegistro(char *__buffer)
{
  String sValor;
  String ret = "barracao_id=";
  ret += CODIGO_BARRACAO;
  ret += "&sensor=";
  ret += CODIGO_SENSOR;
  ret += "&pressaoatm=0";
  float t, um, pe;
  int am;

  char b_an = 0, b_tp = 0, b_um = 0, b_am = 0, b_co = 0, 
       b_lu = 0, b_pe = 0, b_pr = 0, b_bl = 0, b_dt = 0, 
       b_id = 0, b_dn = 0;

  //Serial.print("__Buffer: ");
  //Serial.println(__buffer);
  //  esp01:an=004,esp01:tp=15.9,esp01:um=345.5,esp01:am=150,esp01:co=003,esp01:lu=010,
  //Enviando: esp01:an=004,esp01:tp=15.9,esp01:um=345.5,esp01:am=150,esp01:co=003,esp01:lu=010,esp01:dt=20161213 00:25:00,esp01:id=000

  bool camposOk = true;
  int contaCampos = 0;
  for (int j = 0; j < BUFFER_SIZE - TAMANHO_FILTRO; j++)
  {
    if (strncmp(&__buffer[j], filtro, TAMANHO_FILTRO) == 0)
    {
      tipo[0] = __buffer[j + TAMANHO_FILTRO];
      tipo[1] = __buffer[j + TAMANHO_FILTRO + 1];

      if (b_an > 1 || b_tp > 1 || b_um > 1 || b_am > 1 || b_co > 1 ||  
          b_lu > 1 || b_pe > 1 || b_pr > 1 || b_bl > 1 || b_dt > 1 || 
          b_id > 1 || b_dn > 1)
      {
        Serial.println("Tirando mais de um campo por envio...");
        Serial.print("b_an: "); Serial.println(b_an);
        Serial.print("b_tp: "); Serial.println(b_tp);
        Serial.print("b_um: "); Serial.println(b_um);
        Serial.print("b_am: "); Serial.println(b_am);
        Serial.print("b_co: "); Serial.println(b_co);
        Serial.print("b_lu: "); Serial.println(b_lu);
        Serial.print("b_pe: "); Serial.println(b_pe);
        Serial.print("b_pr: "); Serial.println(b_pr);
        Serial.print("b_bl: "); Serial.println(b_bl);
        Serial.print("b_dt: "); Serial.println(b_dt);
        Serial.print("b_id: "); Serial.println(b_id);
        Serial.print("b_dn: "); Serial.println(b_dn);
        continue;
      }


      ret += "&";
      if (tipo[0] == 'a' && tipo[1] == 'n') { b_an++; contaCampos++; ret += "velocidadevento"; }
      if (tipo[0] == 't' && tipo[1] == 'p') { b_tp++; contaCampos++; ret += "temperatura"; }
      if (tipo[0] == 'u' && tipo[1] == 'm') { b_um++; contaCampos++; ret += "umidade"; }
      if (tipo[0] == 'a' && tipo[1] == 'm') { b_am++; contaCampos++; ret += "amonia"; }
      if (tipo[0] == 'c' && tipo[1] == 'o') { b_co++; contaCampos++; ret += "correnteeletrica"; }
      if (tipo[0] == 'l' && tipo[1] == 'u') { b_lu++; contaCampos++; ret += "luminosidade"; }
      if (tipo[0] == 'd' && tipo[1] == 't') { b_dt++; contaCampos++; ret += "datahora"; }
      if (tipo[0] == 'i' && tipo[1] == 'd') { b_id++; contaCampos++; ret += "indicedias"; }
      if (tipo[0] == 'd' && tipo[1] == 'n') { b_dn++; contaCampos++; ret += "idade"; }
      if (tipo[0] == 'p' && tipo[1] == 'e') { b_pe++; contaCampos++; ret += "peso"; }
      if (tipo[0] == 'p' && tipo[1] == 'r') { b_pr++; contaCampos++; ret += "pressao"; }
      if (tipo[0] == 'b' && tipo[1] == 'l') { b_bl++; contaCampos++; ret += "balanca"; }
      //ret += tipo;
      ret += "=";

      Serial.print("Tipo: ");
      Serial.println(tipo);

      qtdeLeitura = 3;
      if (tipo[0] == 't' && tipo[1] == 'p') 
        qtdeLeitura = acharPonto(__buffer, j);
      else if (tipo[0] == 'u' && tipo[1] == 'm') 
        qtdeLeitura = acharPonto(__buffer, j);
      else if (tipo[0] == 'p' && tipo[1] == 'e') 
        qtdeLeitura = acharPonto(__buffer, j) + 2;
      else if (tipo[0] == 'p' && tipo[1] == 'r') 
        qtdeLeitura = acharPonto(__buffer, j) + 2;
      else if (tipo[0] == 'd' && tipo[1] == 't') 
        qtdeLeitura = 17;

      Serial.print("Qtde: ");
      Serial.println(qtdeLeitura);
        
      int i = 0;
      camposOk = true;
      while ((i < qtdeLeitura)) 
      { 
        valor[i] = __buffer[j + (TAMANHO_FILTRO + 3) + i]; 

        // Verificando valores float
        if ((tipo[0] == 't' && tipo[1] == 'p') 
            || (tipo[0] == 'u' && tipo[1] == 'm')
            || (tipo[0] == 'p' && tipo[1] == 'e'))
        {
          if (valor[i] != '0' && valor[i] != '1' && valor[i] != '2' && valor[i] != '3' && valor[i] != '4' && valor[i] != '5' &&
              valor[i] != '6' && valor[i] != '7' && valor[i] != '8' && valor[i] != '9' && valor[i] != '.' && valor[i] != 0)
          {
            //Serial.print("Falhou float: ");
            //Serial.println(valor[i], DEC);
            camposOk = false;
            break;
          }
        }
        // Verificando valores inteiros
        else if ((tipo[0] == 'a' && tipo[1] == 'n') 
            || (tipo[0] == 'a' && tipo[1] == 'm') 
            || (tipo[0] == 'l' && tipo[1] == 'u')
            || (tipo[0] == 'b' && tipo[1] == 'l'))
        {
          if (valor[i] != '0' && valor[i] != '1' && valor[i] != '2' && valor[i] != '3' && valor[i] != '4' && valor[i] != '5' &&
              valor[i] != '6' && valor[i] != '7' && valor[i] != '8' && valor[i] != '9' && valor[i] != 0)
          {
            //Serial.print("Falhou int: ");
            //Serial.println(valor[i], DEC);
            camposOk = false;
            break;
          }
        }
        
        i++; 
      } 

      // Verificando se campos tem um valores válidos
      if (!camposOk) break;

      // pegando valores de idade e ciclo
      if (tipo[0] == 'i' && tipo[1] == 'd') { 
        indice_dias = atoi(valor);
        sValor = String(indice_dias);
      }
      if (tipo[0] == 'd' && tipo[1] == 'n') {
        idade_dias = atoi(valor);
        sValor = String(idade_dias);
      }
      //

      // Dados para calculo da sensacao termica e alertas
      if (tipo[0] == 't' && tipo[1] == 'p') t = atof(valor);
      if (tipo[0] == 'u' && tipo[1] == 'm') um = atof(valor);
      if (tipo[0] == 'p' && tipo[1] == 'e') pe = atof(valor);
      
      if (tipo[0] == 'a' && tipo[1] == 'n')
      {
        unsigned int iValor = atoi(valor);
        float mts = SpeedCalcMs(iValor);
        sValor = String(mts);
      }
      else if (tipo[0] == 'a' && tipo[1] == 'm')
      {
        unsigned int iValor = atoi(valor);
        am = constrain(map(iValor, AMONIA_ENTRADA_MIN, AMONIA_ENTRADA_MAX, AMONIA_SAIDA_MIN, AMONIA_SAIDA_MAX), AMONIA_SAIDA_MIN, AMONIA_SAIDA_MAX);
        sValor = String(am);
      }
      else if (tipo[0] == 'l' && tipo[1] == 'u')
      {
        unsigned int iValor = atoi(valor);
        int tVal = constrain(map(iValor, LUX_ENTRADA_MIN, LUX_ENTRADA_MAX, LUX_SAIDA_MIN, LUX_SAIDA_MAX), LUX_SAIDA_MIN, LUX_SAIDA_MAX);
        sValor = String(tVal);
      }
      else if (tipo[0] == 'b' && tipo[1] == 'l')
      {
        unsigned int iValor = atoi(valor);
        bl = iValor;
        sValor = String(bl);
      }
      else
      {
        if (tipo[0] == 'd' && tipo[1] == 't')
        {
          sValor = "";
          sValor.concat(valor[0]);
          sValor.concat(valor[1]);
          sValor.concat(valor[2]);
          sValor.concat(valor[3]);
          sValor.concat('-');
          sValor.concat(valor[4]);
          sValor.concat(valor[5]);
          sValor.concat('-');
          sValor.concat(valor[6]);
          sValor.concat(valor[7]);
          sValor.concat('+');
          sValor.concat(valor[9]);
          sValor.concat(valor[10]);
          sValor.concat("%3A");
          sValor.concat(valor[12]);
          sValor.concat(valor[13]);
        }
        else sValor = String(valor);
      }
      ret += sValor;
      
      //Serial.print("Achei: ");
      //Serial.println(sValor);

      for (int i = 0; i < 3; i++) tipo[i] = 0;
      for (int i = 0; i < 18; i++) valor[i] = 0;
    }
  }

  // Verificando se campos tem um valores válidos
  if (!camposOk) return "erro campos";
  //if (contaCampos != 8) return "erro conta";

  Serial.print("Conta campos = ");
  Serial.println(contaCampos);

  //Serial.println(ret);
  //char __buf[BUFFER_SIZE];
  //for (int i = 0; i < BUFFER_SIZE; i++) __buf[i] = 0;
  //ret.toCharArray(__buf, BUFFER_SIZE);
  //return __buf;

  float st = converteFtoC(calcularSensacaoTermica(converteCtoF(t), um));
  ret += "&sensacaotermica=";
  ret += String(st);

  Serial.print("TempC=");
  Serial.print(t, 1);
  Serial.print(", TempF=");
  Serial.print(converteCtoF(t), 1);
  Serial.print(", Sensacao Termica=");
  Serial.println(st, 1);

  // Verificando limites e alertas
  // Amonia acima de 25 ppm
  if (am > 25 && !ligar1vez_amonmin)
  {
    Serial.print("Amonia acima de 25 ppm...");
    tempo1vez_amonmin = millis() + INTERVALO_ALERTA;
    ligar1vez_amonmin = true;
    enviarSMS(motivos[2]);
    enviarTelegram(motivos[2]);
    efetuarLigacao();
    delay(TEMPO_APOS_LIGACAO_E_SMS);
    Serial.println("alerta ok!");
  }
  else if (am <= 25)
  {
    if (tempo1vez_amonmin < millis()) ligar1vez_amonmin = false;
  }
  // Umidade
  if (um < umid_min[indice_dias] && !ligar1vez_umidmin)
  {
    Serial.print("Alerta de Umidade baixa...");
    tempo1vez_umidmin = millis() + INTERVALO_ALERTA;
    ligar1vez_umidmin = true;
    enviarSMS(motivos[4]);
    enviarTelegram(motivos[4]);
    efetuarLigacao();
    delay(TEMPO_APOS_LIGACAO_E_SMS);
    Serial.println("alerta ok!");
  }
  else if (um > umid_max[indice_dias] && !ligar1vez_umidmax)
  {
    Serial.print("Alerta de Umidade alta...");
    tempo1vez_umidmax = millis() + INTERVALO_ALERTA;
    ligar1vez_umidmax = true;
    enviarSMS(motivos[3]);
    enviarTelegram(motivos[3]);
    efetuarLigacao();
    delay(TEMPO_APOS_LIGACAO_E_SMS);
    Serial.println("alerta ok!");
  }
  else if (um >= umid_min[indice_dias] && um <= umid_max[indice_dias])
  {
    if (tempo1vez_umidmin < millis()) ligar1vez_umidmin = false;
    if (tempo1vez_umidmax < millis()) ligar1vez_umidmax = false;
  }
  // Temperatura
  if (t < temp_min[indice_dias] && !ligar1vez_tempmin)
  {
    Serial.print("Alerta de Temperatura baixa...");
    tempo1vez_tempmin = millis() + INTERVALO_ALERTA;
    ligar1vez_tempmin = true;
    //enviarSMS(incluirTemp(motivos[0], t));
    enviarSMS(motivos[0]);
    enviarTelegram(motivos[0]);
    efetuarLigacao();
    delay(TEMPO_APOS_LIGACAO_E_SMS);
    Serial.println("alerta ok!");
  }
  else if (t > temp_max[indice_dias] && !ligar1vez_tempmax)
  {
    Serial.print("Alerta de Temperatura alta...");
    tempo1vez_tempmax = millis() + INTERVALO_ALERTA;
    ligar1vez_tempmax = true;
    //enviarSMS(incluirTemp(motivos[1], t));
    enviarSMS(motivos[1]);
    enviarTelegram(motivos[1]);
    efetuarLigacao();
    delay(TEMPO_APOS_LIGACAO_E_SMS);
    Serial.println("alerta ok!");
  }
  else if (t >= temp_min[indice_dias] && t <= temp_max[indice_dias])
  {
    if (tempo1vez_tempmin < millis()) ligar1vez_tempmin = false;
    if (tempo1vez_tempmax < millis()) ligar1vez_tempmax = false;
  }
  //

  return ret;
}
