void enviaGSM () {
    
  // Verifica se GSM esta ligado
  if (isConnected()) 
  { 
    // Pega registro da struct
    // e envia para o servidor web
    String msg = montaRegistro();
    unsigned int len = msg.length();
    
    // Debug
    if (DEBUG || INFO) { Serial.print(F("Enviando do loop: ")); Serial.println(msg); }
    if (DEBUG || INFO) { Serial.print(F("Len: ")); Serial.println(len); }
  
    boolean envia = false;

    // se nao tem alerta, verifica se passou tempo normal de envio
    if (tempo_envio_normal < millis())
    {
      envia = true;
      // Resetando tempo de envio normal
      tempo_envio_normal = millis() + INTERVALO_ENVIO_NORMAL;
    } 
  
    // verifica se é pra enviar para o servidor
    if (envia)
    {
      // Verificando se SAPBR está conectado
      if (!isSAPBR()) 
      { 
        // Se não estiver, inicia
        if (!initSAPBR()) 
        {
          // Se der erro, termina e tenta de novo depois
          endSAPBR();
          delay(3000);
          return; 
        }
      }
      
      if (enviarMsg(msg.c_str(), msg.length()))
      {
        delay(500);
        if (DEBUG || INFO) Serial.println(F("Cheguei aqui. Codigo: 1H2J3K4L"));
    
        // Se deu certo limpa contador de erros
        contadorErroEnvio = 0;
      }
      else
      {
        if (DEBUG || INFO) Serial.println(F("Deu erro no envio"));
        // Se deu erro incrementa contador de erros
        contadorErroEnvio++;
      }

      // Limpando a variável de recebimento de dados
      recebeu_dados = false;
    }
    
    delay(TEMPO_APOS_ENVIO_SERVIDOR);    
  }
}


String montaRegistro() {
  
  String ret = "barracao_id=";
  ret += CODIGO_BARRACAO;
  ret += "&sensor=";
  ret += CODIGO_SENSOR;
  ret += "&pressaoatm=0";

  ret += "&velocidadevento=" + String(dados.anem);
  ret += "&temperatura=" + String(dados.temp, 2);
  ret += "&umidade=" + String(dados.umid, 2);
  ret += "&amonia=" + String(dados.amonia);
  ret += "&correnteeletrica=" + String(dados.corrente);
  ret += "&luminosidade=" + String(dados.lux);
  ret += "&datahora=" + dateToStringWeb(dados.dt);
  ret += "&indicedias=" + String(dados.indicedias);
  ret += "&idade=" + String(dados.idade);
  ret += "&peso=" + String(dados.peso, 3);
  ret += "&pressaoatm=" + String(dados.pressao);
  ret += "&pressao=" + String(dados.pressao);
  ret += "&sensacaotermica=" + String(calcularSensacaoTermica(dados.temp, dados.umid), 2);
  ret += "&balanca=" + String(dados.balanca);
  
  return ret;
}
