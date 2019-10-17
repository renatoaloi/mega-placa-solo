void draw(void) 
{
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
}

void atualizaTelaLCD(int qual)
{
  if (DEBUG) Serial.println(F("Montando dados no LCD..."));
  if (DEBUG) { Serial.print(F("Qual = ")); Serial.println(qual, DEC); }
  
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
  if (DEBUG) Serial.println(F("atualizaTelaLCD1"));
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
  float t, um, pe;
  int bl = -1;

  // desenha linhas
  u8g.drawLine(0, 26, 128, 26);
  u8g.drawLine(0, 46, 128, 46);

  // Imprime a data
  char* valor = dateToString(dados.dt);
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

  // Imprime anemometro
  u8g.setPrintPos(colLabel, linha5); 
  u8g.print("V:"); 
  u8g.print(dados.anem, 1);
  u8g.print("m/s");

  // Imprime Lux
  u8g.setPrintPos(colLabel2, linha5); 
  u8g.print("Ilumi.:"); 
  u8g.setPrintPos(colVal2, linha5);
  u8g.print(dados.lux);

  // Imprime pressão
  u8g.setPrintPos(colLabel2 + 20, linha1); 
  u8g.print("Pa:"); 
  u8g.print(dados.pressao);

  // Dados da balança
  if (dados.balanca > 0) {
    a_t[dados.balanca - 1] = dados.temp;
    a_um[dados.balanca - 1] = dados.umid;
    a_pe[dados.balanca - 1] = dados.peso;

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

  // Idade
  u8g.setPrintPos(colLabel, linha6);
  u8g.print("Idade:");
  u8g.setPrintPos(colVal, linha6);
  if (idade_dias < 10) u8g.print("0");
  u8g.print(dados.idade);
  u8g.setPrintPos(colVal + colUnid - 10, linha6); 
  u8g.print("d");

  // Ciclo
  u8g.setPrintPos(colLabel2, linha6);
  u8g.print("Ciclo:");
  u8g.setPrintPos(colVal2, linha6);
  if (dados.idade >= 0 && dados.idade <= 3) u8g.print("01");
  if (dados.idade >= 4 && dados.idade <= 7) u8g.print("02");
  if (dados.idade >= 8 && dados.idade <= 14) u8g.print("03");
  if (dados.idade >= 15 && dados.idade <= 21) u8g.print("04");
  if (dados.idade >= 22 && dados.idade <= 30) u8g.print("05");
  if (dados.idade >= 31               ) u8g.print("06");
  
}
