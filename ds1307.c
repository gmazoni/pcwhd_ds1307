/////////////////////////////////////////////////////////////////////////////////
///                               DS1307.C                                    ///
///                                                                           ///
/// ds1307_init() -Habilita o relógio sem alterar o estado dos registradores  ///
///               -Saída de onda quadrada desativada por padrão               ///
///                                                                           ///
/// ds1307_set_date_time(dia,mes,ano,hor,min,seg)                             ///
///               -Altera data e hora                                         ///
/// Exemplo: (24/07/2012 08:16:05)                                            ///
///        ds1307_set_date_time(24,07,12,8,16,5);                             ///
///                                                                           ///
/// ds1307_get_date()                                                         ///
///               -Obtém a data e o dia da semana                             ///
///                                                                           ///
/// ds1307_get_time()                                                         ///
///               -Obtém a hora                                               ///
///                                                                           ///
/////////////////////////////////////////////////////////////////////////////////
///                                                                           ///
///                        2012 - Guilherme Mazoni                            ///
///                                                                           ///
/////////////////////////////////////////////////////////////////////////////////
 
#use i2c(MASTER, SLOW, sda=RTC_SDA, scl=RTC_SCL)
 
BYTE seg, min, hor, dds, dia, mes, ano;
 
BYTE bin2bcd(BYTE bin);
BYTE bcd2bin(BYTE bcd);
int8 calc_dds(int8 _dia, int8 _mes, int16 _ano);
 
void ds1307_init(void)
{
 BYTE segs = 0;
 
i2c_start();
 i2c_write(0xD0);
 i2c_write(0x00);
 i2c_stop();
 i2c_start();
 i2c_write(0xD1);
 segs = bcd2bin(i2c_read(0));
 delay_us(3);
 i2c_stop();
 segs &= 0x7F;
 delay_us(3);
 i2c_start();
 i2c_write(0xD0);
 i2c_write(0x00);
 i2c_write(bin2bcd(segs));
 i2c_stop();
 i2c_start();
 i2c_write(0xD0);
 i2c_write(0x07);
 i2c_write(0x80); //Desabilita a saída de onda quadrada
 i2c_stop();
}
 
void ds1307_set_date_time(BYTE dia, BYTE mes, BYTE ano, BYTE hora, BYTE min, BYTE seg)
{
 BYTE dds;
 dds = calc_dds(dia,mes,ano);
 seg &= 0x7F;
 hora &= 0x3F;
 
i2c_start();
 i2c_write(0xD0);
 i2c_write(0x00);
 i2c_write(bin2bcd(seg)); // REG 0
 i2c_write(bin2bcd(min)); // REG 1
 i2c_write(bin2bcd(hora)); // REG 2
 i2c_write(bin2bcd(dds)); // REG 3 - Dia da semana
 i2c_write(bin2bcd(dia)); // REG 4
 i2c_write(bin2bcd(mes)); // REG 5
 i2c_write(bin2bcd(ano)); // REG 6
 i2c_write(0x80); // REG 7 - Desabilita a saída de onda quadrada
 i2c_stop();
}
 
void ds1307_get_date()
{
 i2c_start();
 i2c_write(0xD0);
 i2c_write(0x03);
 i2c_start();
 i2c_write(0xD1);
 dds = bcd2bin(i2c_read() & 0x7f); // REG 3
 dia = bcd2bin(i2c_read() & 0x3f); // REG 4
 mes = bcd2bin(i2c_read() & 0x1f); // REG 5
 ano = bcd2bin(i2c_read(0)); // REG 6
 i2c_stop();
}
 
void ds1307_get_time()
{
 i2c_start();
 i2c_write(0xD0);
 i2c_write(0x00);
 i2c_start();
 i2c_write(0xD1);
 seg = bcd2bin(i2c_read() & 0x7f);
 min = bcd2bin(i2c_read() & 0x7f);
 hor = bcd2bin(i2c_read(0) & 0x3f);
 i2c_stop();
 
}
 
BYTE bin2bcd(BYTE bin)
{
 BYTE temp;
 BYTE aux;
 
temp = bin;
 aux = 0;
 
while(1)
 {
 if(temp >= 10)
 {
 temp -= 10;
 aux += 0x10;
 }
 else
 {
 aux += temp;
 break;
 }
 }
 
return(aux);
}
 
BYTE bcd2bin(BYTE bcd) // Margem de 00 a 99.
{
 BYTE temp;
 
temp = bcd;
 temp >>= 1;
 temp &= 0x78;
 
return(temp + (temp >> 2) + (bcd & 0x0f));
}
 
int8 calc_dds(int8 _dia, int8 _mes, int16 _ano)
{
 int8 ix, tx, vx;
 _ano = _ano + 2000;
 
 switch(_mes)
 {
 case 2 :
 case 6 : vx = 0; break;
 case 8 : vx = 4; break;
 case 10 : vx = 8; break;
 case 9 :
 case 12 : vx = 12; break;
 case 3 :
 case 11 : vx = 16; break;
 case 1 :
 case 5 : vx = 20; break;
 case 4 :
 case 7 : vx = 24; break;
 }
 if (_ano > 1900) _ano -= 1900;
 ix = (int16)((_ano - 21) % 28) + vx + (_mes > 2);
 tx = (ix + (ix / 4)) % 7 + _dia;
 return ((tx % 7) + 1);
}
