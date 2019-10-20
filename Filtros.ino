dadosType filtroDados(char *__buffer) {
  char tipo[] = { 0, 0, 0 };
  char valor[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int qtdeLeitura = 0;
  unsigned int iValor = 0;
  dadosType newDados;

  for (int j = 0; j < BUFFER_SIZE - TAMANHO_FILTRO; j++)
  {
    if (strncmp(&__buffer[j], filtro, TAMANHO_FILTRO) == 0)
    {
      tipo[0] = __buffer[j + TAMANHO_FILTRO];
      tipo[1] = __buffer[j + TAMANHO_FILTRO + 1];

      qtdeLeitura = 3;
      if (tipo[0] == 't' && tipo[1] == 'p') 
        qtdeLeitura = acharPonto(&__buffer[0], j);
      else if (tipo[0] == 'u' && tipo[1] == 'm') 
        qtdeLeitura = acharPonto(&__buffer[0], j);
      else if (tipo[0] == 'p' && tipo[1] == 'e') 
        qtdeLeitura = acharPonto(&__buffer[0], j) + 2;
      else if (tipo[0] == 'p' && tipo[1] == 'r') 
        qtdeLeitura = acharPonto(&__buffer[0], j);

      int i = 0;
      while ((i < qtdeLeitura)) 
      { 
        valor[i] = buffer[j + (TAMANHO_FILTRO + 3) + i];
        i++; 
      } 

      if (DEBUG || INFO) { Serial.print(F("Tipo: ")); Serial.print(tipo[0]); Serial.print(tipo[1]); }
      if (DEBUG || INFO) { Serial.print(F("Valor: ")); Serial.print(valor); }

      if (tipo[0] == 't' && tipo[1] == 'p') newDados.temp = atof(valor);
      if (tipo[0] == 'u' && tipo[1] == 'm') newDados.umid = atof(valor);
      if (tipo[0] == 'p' && tipo[1] == 'e') newDados.peso = atof(valor);
      if (tipo[0] == 'p' && tipo[1] == 'r') newDados.pressao = atof(valor);
      if (tipo[0] == 'c' && tipo[1] == 'o') newDados.corrente = atoi(valor);
      if (tipo[0] == 'a' && tipo[1] == 'n') newDados.anem = SpeedCalcMs(atoi(valor));
      if (tipo[0] == 'a' && tipo[1] == 'm') newDados.amonia = constrain(map(atoi(valor), AMONIA_ENTRADA_MIN, AMONIA_ENTRADA_MAX, AMONIA_SAIDA_MIN, AMONIA_SAIDA_MAX), AMONIA_SAIDA_MIN, AMONIA_SAIDA_MAX);
      if (tipo[0] == 'l' && tipo[1] == 'u') newDados.lux = constrain(map(atoi(valor), LUX_ENTRADA_MIN, LUX_ENTRADA_MAX, LUX_SAIDA_MIN, LUX_SAIDA_MAX), LUX_SAIDA_MIN, LUX_SAIDA_MAX);
      if (tipo[0] == 'b' && tipo[1] == 'l') newDados.balanca = atoi(valor);
      
    }  
  }

  // Montando data para adicionar no envio
  newDados.dt = rtc.now();

  // Atualizando idade e indice de dias
  newDados.idade = getIdade();
  newDados.indicedias = getIndiceDias();

  return newDados;
}
