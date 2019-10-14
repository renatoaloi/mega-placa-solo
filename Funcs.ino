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
  qtdeLeitura = 0;
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
