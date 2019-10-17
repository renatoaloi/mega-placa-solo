void enviaGSM () {
    
  // Verifica se GSM esta ligado
  if (isConnected()) 
  { 
    // Pega registro da struct
    // e envia para o servidor web
    String msg = montaRegistro();
    unsigned int len = msg.length();
    
    // Debug
    if (DEBUG) { Serial.print(F("Enviando do loop: ")); Serial.println(msg); }
    if (DEBUG) { Serial.print(F("Len: ")); Serial.println(len); }
  
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
        Serial.println(F("Cheguei aqui. Codigo: 1H2J3K4L"));
    
        // Se deu certo limpa contador de erros
        contadorErroEnvio = 0;
      }
      else
      {
        Serial.println(F("Deu erro no envio"));
        // Se deu erro incrementa contador de erros
        contadorErroEnvio++;
      }
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
  ret += "&umidade=" + String(dados.umid);
  ret += "&amonia=" + String(dados.amonia);
  ret += "&correnteeletrica=" + String(dados.corrente);
  ret += "&luminosidade=" + String(dados.lux);
  ret += "&datahora=" + dateToStringWeb(dados.dt);
  ret += "&indicedias=" + String(dados.indicedias);
  ret += "&idade=" + String(dados.idade);
  ret += "&peso=" + String(dados.peso, 3);
  ret += "&pressao=" + String(dados.pressao);
  ret += "&balanca=" + String(dados.balanca);
  
  return ret;
}
