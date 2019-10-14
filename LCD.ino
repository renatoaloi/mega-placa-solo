
void draw(void) 
{
  //u8g.setFont(u8g_font_helvR08);
  //u8g.setFont(u8g_font_courR08);
  u8g.setFont(u8g_font_5x8);
  mostraDadosLCD();
}

void drawReset(void) 
{
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 0, 22, "LOTE INICIADO!");
  u8g.drawStr( 0, 44, "REINICIE ARDUINO");
}

void drawPadrao(void) 
{
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 0, 22, "SYSTEM SEG & CIA");

  /*String sDados = "";
  DateTime now = rtc.now();
  if (now.day() < 10) sDados += "0";
  sDados += String(now.day());
  sDados += "/";
  if (now.month() < 10) sDados += "0";
  sDados += String(now.month());
  sDados += "/";
  sDados += String(now.year());
  sDados += " ";
  if (now.hour() < 10) sDados += "0";
  sDados += String(now.hour());
  sDados += ":";
  if (now.minute() < 10) sDados += "0";
  sDados += String(now.minute());*/

  
  char sDados[20];
  const char *filtroDt = "esp01:dt=";
  for (int i = 0; i < 20; i++) sDados[i] = 0;
  for (int j = 0; j < BUFFER_SIZE - strlen(filtroDt); j++)
  {
    if (strncmp(&buffer[j], filtroDt, strlen(filtroDt)) == 0)
    {
      for (int k = 0; k < 18; k++)
      {
        sDados[k] = buffer[j + strlen(filtroDt) + k];
      }
    }
  }

  u8g.setPrintPos(0, 44); 
  u8g.print(sDados);
}

void atualizaTelaLCD(int qual)
{
  Serial.println("Montando dados no LCD...");
  Serial.print("Qual = ");
  Serial.println(qual, DEC);
  
  u8g.firstPage();
  do 
  {
    if (qual == 1) draw();
    if (qual == 2) drawPadrao();
    if (qual == 3) drawReset();
  } while( u8g.nextPage() );
}

void atualizaTela()
{
  atualizaTelaLCD(1);
}

void mostraDadosLCD()
{
  int linhas[] = { 16, 24, 36 };
  int linha1 = 8;
  int linha2 = 16;
  int linha3 = 24;
  int linha4 = 36;
  int linha5 = 44;
  int linha6 = 57;
  int colLabel = 0;
  int colLabel2 = 70;
  int colVal = 33;
  int colVal2 = 106;
  int colUnid = 24;
  //u8g.drawStr( 0, 10, "Hello World!");
  //u8g.setPrintPos(0, 20); 
  //u8g.print("Hello World!");
  // esp01:an=004,esp01:tp=15.9,esp01:um=345.5,esp01:am=150,esp01:co=003,esp01:lu=010,

  float t, um, pe;
  int bl = -1;

  u8g.drawLine(0, 26, 128, 26);
  u8g.drawLine(0, 46, 128, 46);

  for (int i = 0; i < 3; i++) tipo[i] = 0;
  for (int i = 0; i < 18; i++) valor[i] = 0;
  
  for (int j = 0; j < BUFFER_SIZE - TAMANHO_FILTRO; j++)
  {
    if (strncmp(&buffer[j], filtro, TAMANHO_FILTRO) == 0)
    {
      tipo[0] = buffer[j + TAMANHO_FILTRO];
      tipo[1] = buffer[j + TAMANHO_FILTRO + 1];
      
      //Serial.print("Tipo: ");
      //Serial.println(tipo);

      qtdeLeitura = 3;
      if (tipo[0] == 't' && tipo[1] == 'p') 
        qtdeLeitura = acharPonto(buffer, j);
      else if (tipo[0] == 'u' && tipo[1] == 'm') 
        qtdeLeitura = acharPonto(buffer, j);
      else if (tipo[0] == 'p' && tipo[1] == 'r') 
        qtdeLeitura = acharPonto(buffer, j);
      else if (tipo[0] == 'p' && tipo[1] == 'e') 
        qtdeLeitura = acharPonto(buffer, j) + 2;
      else if (tipo[0] == 'd' && tipo[1] == 't') 
        qtdeLeitura = 17;

      int i = 0;
      while ((i < qtdeLeitura)) 
      { 
        valor[i] = buffer[j + (TAMANHO_FILTRO + 3) + i]; 
        i++; 
      } 
      //sValor = String(valor);
      //ret += sValor;

      // Dados para calculo da sensacao termica
      if (tipo[0] == 't' && tipo[1] == 'p') t = atof(valor);
      if (tipo[0] == 'u' && tipo[1] == 'm') um = atof(valor);
      if (tipo[0] == 'p' && tipo[1] == 'e') pe = atof(valor);
      if (tipo[0] == 'b' && tipo[1] == 'l')
      {
        unsigned int iValor = atoi(valor);
        bl = iValor;
      }
      
      if (tipo[0] == 'd' && tipo[1] == 't')
      {
        u8g.setPrintPos(colLabel, linha1); 
        u8g.print("D: ");
        u8g.print(valor[6]); u8g.print(valor[7]);
        u8g.print('/');
        u8g.print(valor[4]); u8g.print(valor[5]);
        u8g.print('/');
        u8g.print(valor[2]); u8g.print(valor[3]);
        u8g.print(' ');
        u8g.print(valor[9]); u8g.print(valor[10]);
        u8g.print(':');
        u8g.print(valor[12]); u8g.print(valor[13]);
        //u8g.print(':');
        //u8g.print(valor[14]); u8g.print(valor[15]);
      }
      else if (tipo[0] == 'a' && tipo[1] == 'n')
      {
          unsigned int iValor = atoi(valor);
          u8g.setPrintPos(colLabel, linha5); 
          u8g.print("V:"); 
          u8g.print(SpeedCalcMs(iValor), 1);
          u8g.print("m/s");
        
      }
      else if (tipo[0] == 'l' && tipo[1] == 'u')
      {
        u8g.setPrintPos(colLabel2, linha5); 
        u8g.print("Ilumi.:"); 
        u8g.setPrintPos(colVal2, linha5); 
        unsigned int iValor = atoi(valor);
        u8g.print(constrain(map(iValor, LUX_ENTRADA_MIN, LUX_ENTRADA_MAX, LUX_SAIDA_MIN, LUX_SAIDA_MAX), LUX_SAIDA_MIN, LUX_SAIDA_MAX));
      }
      else if (tipo[0] == 'p' && tipo[1] == 'r')
      {
          unsigned int iValor = atoi(valor);
          u8g.setPrintPos(colLabel2 + 20, linha1); 
          u8g.print("Pa:"); 
          u8g.print(iValor);
      }


      if (bl != -1)
      {
        a_t[bl - 1] = t;
        a_um[bl - 1] = um;
        a_pe[bl - 1] = pe;
        
          u8g.setPrintPos(colLabel, linhas[0]); 
          u8g.print("T1:"); 
          u8g.print(a_t[0]);
          u8g.print(" T2:");
          u8g.print(a_t[1]);
          u8g.print(" T3:");
          u8g.print(a_t[2]); 
  
          u8g.setPrintPos(colLabel, linhas[1]); 
          u8g.print("U1:"); 
          u8g.print(a_um[0]);
          u8g.print(" U2:");
          u8g.print(a_um[1]);
          u8g.print(" U3:");
          u8g.print(a_um[2]); 
  
          u8g.setPrintPos(colLabel, linhas[2]); 
          u8g.print("P1:"); 
          u8g.print(a_pe[0], 3);
          u8g.print(" P2:");
          u8g.print(a_pe[1], 3);
          u8g.print(" P3:");
          u8g.print(a_pe[2], 3);  
        
      }

      for (int i = 0; i < 3; i++) tipo[i] = 0;
      for (int i = 0; i < 18; i++) valor[i] = 0;
    }
  }

  u8g.setPrintPos(colLabel, linha6);
  u8g.print("Idade:");
  u8g.setPrintPos(colVal, linha6);
  if (idade_dias < 10) u8g.print("0");
  u8g.print(idade_dias);
  u8g.setPrintPos(colVal + colUnid - 10, linha6); 
  u8g.print("d");

  u8g.setPrintPos(colLabel2, linha6);
  u8g.print("Ciclo:");
  u8g.setPrintPos(colVal2, linha6);
  if (idade_dias >= 0 && idade_dias <= 3) u8g.print("01");
  if (idade_dias >= 4 && idade_dias <= 7) u8g.print("02");
  if (idade_dias >= 8 && idade_dias <= 14) u8g.print("03");
  if (idade_dias >= 15 && idade_dias <= 21) u8g.print("04");
  if (idade_dias >= 22 && idade_dias <= 30) u8g.print("05");
  if (idade_dias >= 31               ) u8g.print("06");
}
