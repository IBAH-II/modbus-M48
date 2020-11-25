#define __MODBUS_C
#include "modbus.h"

static unsigned long TimModbus; //время приема пакета по системному таймеру
static unsigned short CRCmodbus;//текущий CRC
static unsigned char UkPaket;//указатель в массиве, текущий принятый символ 
static unsigned char Sost;//состояние 0/1 прием/передача
static unsigned char PaketRX[ModBusMaxPaketRX];//массив для сохранения пакета

//Инициализация ModBus
void ModBusIni(void)
  {
  TimModbus=ModBusSysTimer;//запомнить таймер
  UkPaket=0;//сбросить указатель пакета
  CRCmodbus=0xFFFF; //установить начальное значение CRC
  Sost=0;//состояние прием
  //Инициализация регистров ModBus
#if ModBusMaxOutBit!=0
  Prg2ModBusOutBit();
#endif  
#if ModBusMaxInBit!=0  
  Prg2ModBusInBit();
#endif  
#if ModBusMaxOutReg!=0  
  Prg2ModBusOutReg();
#endif  
#if ModBusMaxInReg!=0
  Prg2ModBusInReg();
#endif 
  return;
  }

//Функция вычисления CRC
#if ModBusUseTableCRC!=0
static __flash unsigned char TableL[256]= 
  {
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40
  };
static __flash unsigned char TableH[256]= 
  { 
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
  0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
  0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
  0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
  0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
  0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
  0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
  0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
  0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
  0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
  0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
  0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
  0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
  0x40
  }; 

static unsigned short CRCfunc(unsigned short inCRC, unsigned char in)
  {
  unsigned char i=inCRC^in;
  return ((((unsigned short)(TableH[i]))<<8)|((inCRC>>8)^TableL[i]));
  }
#else
static unsigned short CRCfunc(unsigned short inCRC, unsigned char in)
  {
  inCRC=inCRC^in;
  {
  unsigned char i;
  for(i=0;i<8;i++){if(inCRC&1){inCRC=(inCRC>>1)^0xA001U;}else {inCRC=inCRC>>1;}}
  }
  return inCRC;
  }
#endif

//Функция обработки ошибок
#if ModBusUseErrMes!=0
#define ILLEGAL_FUNCTION (1) //Принятый код функции не может быть обработан на подчиненном. 
#define ILLEGAL_DATA_ADDRESS (2) //Адрес данных указанный в запросе не доступен данному подчиненному.
#define ILLEGAL_DATA_VALUE (3) //Величина, содержащаяся в поле данных запроса является не допустимой величиной для подчиненного.
static void MesErrRTU(unsigned char Err)
  {
  //адрес
  ModBusPUT(PaketRX[0]);
  {
  unsigned short TmpCRC=CRCfunc(0xFFFF,PaketRX[0]);
  //код команды    
  ModBusPUT(0x80|PaketRX[1]);
  TmpCRC=CRCfunc(TmpCRC,(0x80|PaketRX[1]));
  //код ошибки
  ModBusPUT(Err);
  TmpCRC=CRCfunc(TmpCRC,Err);
  ModBusPUT(TmpCRC);
  ModBusPUT(TmpCRC>>8);
  }
  }
#endif

//Функция обработки сообщений ModBus
void ModBusRTU(void)
  {
  if(Sost==0)
    {//Состояние прием
    while(!0)
      {//Цикл приема символов
      unsigned short Tmp=ModBusGET(); //читаем символ из входного потока
      if(Tmp==0) return; //если нет данных - возврат 
      //символ принят
      Tmp=Tmp&0xFF;//отбрасываем признак приема байта
      //Проверка временного интервала между символами
      if((ModBusSysTimer-TimModbus)>ModBusMaxPause)
        {//превышен таймаут, начинаем прием нового пакета
        PaketRX[0]=Tmp;//сохранить принятый символ в буфер приема
        UkPaket=1;//установить указатель пакета
        TimModbus=ModBusSysTimer;//сбросить таймер
        //вычисление CRC
        CRCmodbus=CRCfunc(0xFFFF,Tmp);
        continue;//повторный запрос символа
        }
      else
        {//таймаут не превышен, принимаем уже начатый пакет
        TimModbus=ModBusSysTimer;//сбросить таймер
        PaketRX[UkPaket]=Tmp;//сохранить принятый символ
        UkPaket++;//инкремент указателя пакета
        if(UkPaket==ModBusMaxPaketRX)//проверяем количество принятых байт
          {//буфер пакета переполнился
          UkPaket=0;//сбросить указатель пакета
          CRCmodbus=0xFFFF; //установить начальное значение CRC
          return;//ошибка, повторный запрос символа требуется
          }
        CRCmodbus=CRCfunc(CRCmodbus,Tmp);//вычисление CRC
        }
      //Если принято мало данных
      if(UkPaket<8) continue; //повторный запрос символа
      //проверка на принятие пакета
      if(CRCmodbus==0) 
        {//проверка на длинные пакеты
        if(PaketRX[1]==15 || PaketRX[1]==16)
          {//если длинные команды (15,16) , проверяем "Счетчик байт"
          if((PaketRX[6]+9)!=UkPaket) continue;
          }
        break; //Ура! Пакет принят!!!
        }
      }
    //Ура! Пакет принят!!!
    CRCmodbus=0xFFFF; //установить начальное значение CRC
    UkPaket=0;//сбросить указатель пакета    
    //проверка адреса
    if((PaketRX[0]!=ModBusID)&&(PaketRX[0]!=ModBusID_FF))
      {//Не наш адрес
      return;
      }    
    //переходим в состояние передача ответа
    Sost=!0;
#if ModBusMaxPauseResp!=0  
    return;
#endif 
    }
  
  
  if(Sost!=0 
#if ModBusMaxPauseResp!=0     
     && (ModBusSysTimer-TimModbus)>=ModBusMaxPauseResp
#endif     
     )
    {//Состояние передача ответа
    Sost=0;
    //Код функции 01 - чтение статуса Coils (дискретных выходных битов). 
    /*Сообщение-запрос содержит адрес начального бита и количество битов для чтения. 
    Биты нумеруются начиная с 0. 
    В сообщении-ответе каждое значение переменной передается одним битом,
    то есть в одном байте пакуется статус 8 битов переменных. 
    Если количество их не кратно восьми, остальные биты в байте заполняются нулями. 
    Счетчик вмещает количество байт в поле данных.
    01 Чтение статуса выходов 
          ОПИСАНИЕ 
          Читает статуса ON/OFF дискретных выходов в подчиненном. 
          ЗАПРОС 
          Запрос содержит адрес начального выхода и количество выходов для чтения. 
          Выхода адресуются начиная с нуля: выхода 1-16 адресуются как 0-15.
          Ниже приведен пример запроса на чтение выходов 20-56 с подчиненного устройства 17. 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						01	1
          Начальный адрес Hi					00	2
          Начальный адрес Lo					13	3
          Количество Hi						00	4
          Количество Lo						25	5
          Контрольная сумма (CRC или LRC)			--

          ОТВЕТ 
          Статус выходов в ответном сообщении передается как один выход на бит.
          Если возвращаемое количество выходов не кратно восьми, то оставшиеся биты в последнем байте сообщения будут установлены в 0. 
          Счетчик байт содержит количество байт передаваемых в поле данных. 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						01	1
          Счетчик байт						05	2
          Данные(Выхода 27-20)					CD	3
          Данные(Выхода 35-28)					6B	4
          Данные(Выхода 43-36)					B2	5
          Данные(Выхода 51-44)					0E	6
          Данные(Выхода 56-52)					1B	7
          Контрольная сумма (CRC или LRC)			--
    */
#if ModBusUseFunc1!=0       
    if(PaketRX[1]==0x01)
      {
      //вычисление адреса запрашиваемых бит
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление количества запрашиваемых бит
      unsigned short KolvoBit=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5]));
      
      //если неправильный адрес и количество
      if(KolvoBit>ModBusMaxOutBitTX || KolvoBit==0)
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }     
      if((AdresBit+KolvoBit)>ModBusMaxOutBit)
        {//неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      Prg2ModBusOutBit();//Заполнение регистров Модбас (GlobalDate->ModBus)
      
      //формирование пакета ответа
      //адрес
      ModBusPUT(PaketRX[0]);
      {
      unsigned short TmpCRC=CRCfunc(0xFFFF,PaketRX[0]);
      //код команды    
      ModBusPUT(1);
      TmpCRC=CRCfunc(TmpCRC,1);
      //количество полных байт
      ModBusPUT((KolvoBit+7)>>3);
      TmpCRC=CRCfunc(TmpCRC,((KolvoBit+7)>>3));
      
      //копирование битов в пакет ответа
      {
      unsigned char TxByte=0;//текущий байт
      unsigned char Bit=AdresBit&7;//указатель бит в ModBusOutBit[]
      AdresBit=AdresBit>>3;//указатель байт ModBusOutBit[]
      //копирование из регистра ModBusOutBit[] в пакет
      {
      unsigned char i=0;
      while(!0)
        {
        if((ModBusOutBit[AdresBit].byte)&(1<<Bit))
          {
          TxByte=TxByte|(1<<(i&7));
          }
        //инкрементруем указатели 
        Bit++;
        if(Bit==8){Bit=0;AdresBit++;}
        i++;
        if((i&7)==0)
          {//скопирован байт
          ModBusPUT(TxByte);
          TmpCRC=CRCfunc(TmpCRC,TxByte);
          TxByte=0;
          if(i==KolvoBit) break; else continue;
          }
        if(i==KolvoBit) 
          {//скопированы все биты
          ModBusPUT(TxByte);
          TmpCRC=CRCfunc(TmpCRC,TxByte);
          break;
          }
        }
      }
      }
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      }
      //конец
      return;
      }
#endif    
    
    //Код функции 2 - чтение статуса дискретных входов
    /*02 Read Input Status 
          ОПИСАНИЕ 
          Чтение ON/OFF состояния дискретных входов (ссылка 1Х) в подчиненном. 
          ЗАПРОС 
          Запрос содержит номер начального входа и количество входов для чтения. Входа адресуются начиная с 0.
          Ниже приведен пример запроса на чтение входов 10197-10218 с подчиненного устройства 17. 
                  Запрос 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						02	1
          Начальный адрес ст.					00	2
          Начальный адрес мл.					C4	3
          Кол-во входов ст.					00	4
          Кол-во входов мл.					16	5
          Контрольная сумма					--

          ОТВЕТ 
          Статус входов в ответном сообщении передается как один выход на бит.
          Если возвращаемое количество входов не кратно восьми, то оставшиеся биты в последнем байте сообщения будут установлены в 0. 
          Счетчик байт содержит количество байт передаваемых в поле данных. 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						01	1
          Счетчик байт						03	2
          Данные(Входы 10204-10197)				AC	3
          Данные(Входы 10212-10205)				DB	4
          Данные(Входы 10218-10213)				35	5
          Контрольная сумма (CRC или LRC)			--  
    */
#if ModBusUseFunc2!=0     
    if(PaketRX[1]==0x02)
      {
      //вычисление адреса запрашиваемых бит
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление количества запрашиваемых бит
      unsigned short KolvoBit=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5]));
      
      //если неправильный адрес и количество
      if(KolvoBit>ModBusMaxInBitTX || KolvoBit==0)
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }
      if((AdresBit+KolvoBit)>ModBusMaxInBit)
        {//неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      Prg2ModBusInBit();//Заполнение регистров Модбас (GlobalDate->ModBus)
      
      //формирование пакета ответа
      //адрес
      ModBusPUT(PaketRX[0]);
      {
      unsigned short TmpCRC=CRCfunc(0xFFFF,PaketRX[0]);
      //код команды    
      ModBusPUT(2);
      TmpCRC=CRCfunc(TmpCRC,2);
      //количества полных байт
      ModBusPUT((KolvoBit+7)>>3);
      TmpCRC=CRCfunc(TmpCRC,((KolvoBit+7)>>3));
      //копирование битов в пакет ответа
      {
      unsigned char TxByte=0;//текущий байт
      unsigned char Bit=AdresBit&7;//указатель бит 
      AdresBit=AdresBit>>3;//указатель байт 
      //копирование из регистра ModBusInBit[] в пакет
      {
      unsigned char i=0;
      while(!0)
        {
        if((ModBusInBit[AdresBit].byte)&(1<<Bit))
          {//устанавливаем бит в пакете
          TxByte=TxByte|(1<<(i&7));
          }
        //инкрементруем указатели 
        Bit++;
        if(Bit==8){Bit=0;AdresBit++;}
        i++;
        if((i&7)==0)
          {//скопирован байт
          ModBusPUT(TxByte);
          TmpCRC=CRCfunc(TmpCRC,TxByte);
          TxByte=0;
          if(i==KolvoBit) break; else continue;
          }
        if(i==KolvoBit)
          {//скопированы все биты
          ModBusPUT(TxByte);
          TmpCRC=CRCfunc(TmpCRC,TxByte);
          break;
          }
        }
      }
      }
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      }
      //конец
      return;
      }
#endif    
    
    //Код функции 03 - чтение значения выходных/внутренних регистров. 
    /*Сообщение-запрос содержит адрес начального исходного/внутреннего регистра (двухбайтовое слово), 
    и количество регистров для чтения. Регистры нумеруются начиная с 0.
    03 Read Holding Registers 
          ОПИСАНИЕ 
          Чтение двоичного содержания регистров (ссылка 4Х) в подчиненном. 
          ЗАПРОС 
          Сообщение запроса специфицирует начальный регистр и количество регистров для чтения. 
          Регистры адресуются начина с 0: регистры 1-16 адресуются как 0-15.
          Ниже приведен пример чтения регистров 40108-40110 с подчиненного устройства 17. 
          Запрос 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						03	1
          Начальный адрес ст.					00	2
          Начальный адрес мл.					6B	3
          Кол-во регистров ст.					00	4
          Кол-во регистров мл.					03	5
          Контрольная сумма					--

          ОТВЕТ 
          Данные регистров в ответе передаются как два байта на регистр. 
          Для каждого регистра, первый байт содержит старшие биты второй байт содержит младшие биты.
          За одно обращение может считываться 125 регистров для контроллеров 984-Х8Х (984-685 и т.д.), 
          и 32 регистра для других контроллеров. Ответ дается, когда все данные укомплектованы.
          Это пример ответа на запрос представленный выше: 
          Ответ 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						03	1
          Счетчик байт						06	2
          Данные (регистр 40108) ст.				02	3
          Данные (регистр 40108) мл.				2B	4
          Данные (регистр 40109) ст.				00	5
          Данные (регистр 40109) мл.				00	6
          Данные (регистр 40110) ст.				00	7
          Данные (регистр 40110) мл.				64	8
          Контрольная сумма					--
    */
#if ModBusUseFunc3!=0      
    if(PaketRX[1]==0x03)
      {
      //вычисление адреса запрашиваемых слов
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление адреса количества запрашиваемых слов
      unsigned short KolvoWord=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])); 
      
      //если неправильный адрес и количество
      if(KolvoWord>ModBusMaxOutRegTX || KolvoWord==0)
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxOutReg)
        {//неправильный адрес 
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      Prg2ModBusOutReg();//Заполнение регистров Модбас (GlobalDate->ModBus)
      
      //формирование пакета ответа
      //адрес
      ModBusPUT(PaketRX[0]);
      {
      unsigned short TmpCRC=CRCfunc(0xFFFF,PaketRX[0]);
      //код команды    
      ModBusPUT(3);
      TmpCRC=CRCfunc(TmpCRC,3);
      //количества полных байт
      ModBusPUT(KolvoWord<<1);
      TmpCRC=CRCfunc(TmpCRC,(KolvoWord<<1));
      //Копирование из регистра ModBusOutReg[] в пакет ответа
      {
      unsigned char i;
      for(i=0;i<KolvoWord;i++)
        {
        ModBusPUT(ModBusOutReg[AdresWord+i]>>8);
        TmpCRC=CRCfunc(TmpCRC,(ModBusOutReg[AdresWord+i]>>8));
        ModBusPUT(ModBusOutReg[AdresWord+i]>>0);
        TmpCRC=CRCfunc(TmpCRC,(ModBusOutReg[AdresWord+i]>>0));
        }
      }
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      }
      //конец
      return;
      }
#endif     
    
    //Код функции 04 - чтение значения входных регистров
    /*04 Read Input Registers 
          СОДЕРЖАНИЕ 
          Чтение двоичного содержания входных регистров (ссылка 3Х) в подчиненном. 
          ЗАПРОС 
          Запрос содержит номер начального регистра и количество регистров для чтения.
          Ниже приведен пример запроса для чтения регистра 30009 с подчиненного устройства 17. 
          Запрос 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						03	1
          Начальный адрес ст.					00	2
          Начальный адрес мл.					6B	3
          Кол-во регистров ст.					00	4
          Кол-во регистров мл.					03	5
          Контрольная сумма					--
   
          ОТВЕТ 
          Данные регистров в ответе передаются как два байта на регистр. 
          Для каждого регистра, первый байт содержит старшие биты, второй байт содержит младшие биты.
          За одно обращение может считываться 125 регистров для контроллеров 984-Х8Х (984-685 и т.д.), 
          и 32 регистра для других контроллеров. Ответ дается когда все данные укомплектованы.
          Это пример ответа на запрос представленный выше: 
          Ответ 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						03	1
          Счетчик байт						02	2
          Данные (регистр 30009) ст.				00	3
          Данные (регистр 30009) мл.				2A	4
          Контрольная сумма					--  
    */
#if ModBusUseFunc4!=0     
    if(PaketRX[1]==0x04)
      {
      //вычисление адреса запрашиваемых слов
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление адреса количества запрашиваемых слов
      unsigned short KolvoWord=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])); 
      
      //если неправильный адрес и количество
      if(KolvoWord>ModBusMaxInRegTX || KolvoWord==0)
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxInReg)
        {//неправильный адрес 
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      Prg2ModBusInReg();//Заполнение регистров Модбас (GlobalDate->ModBus)
      
      //формирование пакета ответа
      //адрес
      ModBusPUT(PaketRX[0]);
      {
      unsigned short TmpCRC=CRCfunc(0xFFFF,(PaketRX[0]));
      //код команды    
      ModBusPUT(4);
      TmpCRC=CRCfunc(TmpCRC,4);
      //количества полных байт
      ModBusPUT(KolvoWord<<1);
      TmpCRC=CRCfunc(TmpCRC,(KolvoWord<<1));
      //Копирование из регистра ModBusInReg[] в пакет ответа
      {
      unsigned char i;
      for(i=0;i<KolvoWord;i++)
        {
        ModBusPUT(ModBusInReg[AdresWord+i]>>8);
        TmpCRC=CRCfunc(TmpCRC,(ModBusInReg[AdresWord+i]>>8));
        ModBusPUT(ModBusInReg[AdresWord+i]>>0);
        TmpCRC=CRCfunc(TmpCRC,(ModBusInReg[AdresWord+i]>>0));
        }
      }
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      }
      //конец
      return;//повторный запрос не требуется
      }
#endif      
    
    //Код функции 05 - запись выходного/внутреннего бита
    /*05 Force Single Coil 
          ОПИСАНИЕ 
          Установка единичного выхода (ссылка 0Х) в ON или OFF. 
          При широковещательной передаче функция устанавливает все выходы с данным адресом во всех подчиненных контроллерах. 
          ЗАМЕЧАНИЕ Функция может пересекаться с установкой защиты
                          памяти и установкой недоступности выходов. 
          ЗАПРОС 
          Запрос содержит номер выхода для установки. Выходы адресуются начиная с 0. Выход 1 адресуется как 0.
          Состояние, в которое необходимо установить выход (ON/OFF) описывается в поле данных. 
          Величина FF00 Hex - ON. Величина 0000 - OFF. Любое другое число неверно и не влияет на выход.
          В приведенном ниже примере устанавливается выход 173 в состояние ON в подчиненном устройстве 17. 
          Запрос 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						05	1
          Адрес выхода мл.					00	2
          Адрес выхода ст.					AC	3
          Данные ст.						FF	4
          Данные мл.						00	5
          Контрольная сумма					--
   
          ОТВЕТ 
          Нормальный ответ повторяет запрос. 
          Ответ 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						05	1
          Адрес выхода мл.					00	2
          Адрес выхода ст.					AC	3
          Данные ст.						FF	4
          Данные мл.						00	5
          Контрольная сумма					--  
    */
#if ModBusUseFunc5!=0     
    if(PaketRX[1]==0x05)
      {
      //вычисление адреса записываемого выхода
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      
      //проверка на допустимый адрес  
      if(AdresBit>=ModBusMaxOutBit)
        {//неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      //установка/сброс бита
      switch (((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])))
        {
        case 0xFF00:
        //установка бита
        ModBusOutBit[(AdresBit>>3)].byte|=(1<<(AdresBit&7));
        break;
        case 0x0000:
        //сброс бита
        ModBusOutBit[(AdresBit>>3)].byte&=(~(1<<(AdresBit&7)));
        break;
        default:
          {//ошибка
#if ModBusUseErrMes!=0         
          MesErrRTU(ILLEGAL_DATA_VALUE);
#endif          
          return;
          } 
        }
      //формирование пакета ответа
      {
      unsigned char i;
      for(i=0;i<8;i++) ModBusPUT(PaketRX[i]);
      }
      ModBus2PrgOutBit();//Считывание регистров Модбас (ModBus->GlobalDate)
      
      //конец
      return;
      }
#endif     
    
    //Код функции 06 - запись выходного/внутреннего регистра. 
    /*Функция аналогична 05, но оперирует с регистрами (словами). 
    В запросе указывается номер выходного/внутреннего регистра и его значение. 
    06 Preset Single Register 
          ОПИСАНИЕ. Записывает величину в единичный регистр (ссылка 4Х).
          При щироковещательной передаче на всех подчиненных устройствах устанавливается один и тот же регистр. 
          ЗАМЕЧАНИЕ 
          Функция может пересекаться с установленной защитой памяти. 
          ЗАПРОС 
          Запрос содержит ссылку на регистр, который необходимо установить. Регистры адресуются с 0.
          Величина, в которую необходимо установить регистр передается в поле данных. 
          Контроллеры M84 и 484 используют 10-ти битную величину, старшие шесть бит заполняются 0. 
          Все другие контроллеры используют 16 бит.
          В приведенном ниже примере в регистр 40002 записывается величина 0003 Hex в подчиненном устройстве 17. 
          Запрос 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						06	1
          Адрес регистра мл.					00	2
          Адрес регистра ст.					01	3
          Данные ст.						00	4
          Данные мл.						03	5
          Контрольная сумма					--
   
          ОТВЕТ 
          Нормальный ответ повторяет запрос. 
          Ответ 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						06	1
          Адрес регистра мл.					00	2
          Адрес регистра ст.					01	3
          Данные ст.						00	4
          Данные мл.						03	5
          Контрольная сумма					--  
    */
#if ModBusUseFunc6!=0    
    if(PaketRX[1]==0x06)
      {
      //вычисление адреса записываемого регистра
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      
      //проверка на допустимый адрес  
      if(AdresWord>=(ModBusMaxOutReg))
        {//если неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      //запись слова
      ModBusOutReg[AdresWord]=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //формирование пакета ответа
      {
      unsigned char i;
      for(i=0;i<8;i++) ModBusPUT(PaketRX[i]);
      }
      ModBus2PrgOutReg();//Считывание регистров Модбас (ModBus->GlobalDate)
      
      //конец
      return;
      }
#endif     
    
    //Код функции 0x0F - запись нескольких выходных/внутренних битов. 
    /*В запросе указывается начальный адрес бита, количество бит для записи, счетчик байтов и непосредственно значения. 
    15 (0F Hex) Force Multiple Coils 
          ОПИСАНИЕ 
          Устанавливает каждый выход (ссылка 0Х) последовательности выходов в одно из состояний ON или OFF. 
          При широковещательной передаче функция устанавливает подобные выходы на всех подчиненных. 
          ЗАМЕЧАНИЕ Функция может пересекаться с установкой защиты памяти и установкой недоступности выходов. 
          ЗАПРОС 
          Запрос специфицирует выходы для установки. Выходы адресуются начиная с 0.
          Ниже показан пример запроса на установку последовательности выходов начиная с 20 (адресуется как 19) 
          в подчиненном устройстве 17.
          Поле данных запроса содержит 2 байта: CD 01 Hex (1100 1101 0000 0001 двоичное). 
          Соответствие битов и выходов представлено ниже: 
          Бит:    1  1  0  0  1  1  0  1		0  0  0  0  0  0   0  1 
          Выход: 27 26 25 24 23 22 21 20		-  -  -  -  -  -  29 28 
          Запрос 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						0F	1
          Адрес выхода ст.					00	2
          Адрес выхода мл.					13	3
          Кол-во выходов ст.					00	4
          Кол-во выходов мл.					0A	5
          Счетчик байт						02	6
          Данные для установки (Выходы 27-20)			CD	7
          Данные для установки (Выходы 29-28) 			01	8
          Контрольная сумма					--	9
   
          ОТВЕТ 
          Нормальный ответ возвращает адрес подчиненного, код функции, начальный адрес, и количество установленных выходов.
          Это пример ответа на представленный выше запрос. 
          Ответ 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						0F	1
          Адрес выхода ст.					00	2
          Адрес выхода мл.					13	3
          Кол-во выходов ст.					00	4
          Кол-во выходов мл.					0A	5
          Контрольная сумма					--
    */
#if ModBusUseFunc15!=0    
    if(PaketRX[1]==0x0F)
      {
      //вычисление адреса записываемых бит
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление количества записываемых бит
      unsigned short KolvoBit=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //если неправильный адрес и количество
      if(KolvoBit>ModBusMaxOutBitRX || KolvoBit==0 || ((KolvoBit+7)>>3)!=PaketRX[6])
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }     
      if((AdresBit+KolvoBit)>ModBusMaxOutBit)
        {//неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      //установка битов
      {
      unsigned char Bit=(AdresBit&7);//указатель бит в ModBusOutBit[]
      AdresBit=AdresBit>>3;//указатель байт ModBusOutBit[]
      //цикл по битам
      {
      unsigned char i;
      for(i=0;i<KolvoBit;i++)
        {
        if(PaketRX[7+(i>>3)]&(1<<(i&7)))//если текущий бит PaketRX равен 1
          {//устанавливаем бит в ModBusOutBit[]
          ModBusOutBit[AdresBit].byte=(ModBusOutBit[AdresBit].byte)|((unsigned char)(1<<Bit));
          }
        else
          {//сбрасываем бит ModBusOutBit[]
          ModBusOutBit[AdresBit].byte=(ModBusOutBit[AdresBit].byte)&((unsigned char)(~(1<<Bit)));
          }
        //инкрементруем указатели 
        Bit++; if(Bit==8){Bit=0;AdresBit++;}
        }           
      }
      }
      //формирование пакета ответа
      {
      unsigned short TmpCRC=0xFFFF;
      {
      unsigned char i;
      for(i=0;i<6;i++)
        {
        ModBusPUT(PaketRX[i]);
        TmpCRC=CRCfunc(TmpCRC,(PaketRX[i]));
        }
      }
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      }
      ModBus2PrgOutBit();//Считывание регистров Модбас (ModBus->GlobalDate)
      
      //конец
      return;
      }
#endif     
    //Код функции 0x10 запись нескольких выходных/внутренних регистров.
    /*16 (10 Hex) Preset Multiple Regs 
          ОПИСАНИЕ 
          Запись данных в последовательность регистров (ссылка 4Х). 
          При широковещательной передаче, функция устанавливает подобные регистры во всех подчиненных устройствах. 
          ЗАМЕЧАНИЕ 
          Функция может пересекаться с установленной защитой памяти. 
          ЗАПРОС 
          Запрос специфицирует регистры для записи. Регистры адресуются начиная с 0.
          Данные для записи в регистры содержатся в поле данных запроса. 
          Контроллеры M84 и 484 используют 10-битовую величину, со старшими шестью битами установленными в 0. 
          Все остальные контроллеры используют 16 бит.
          Ниже приведен пример запроса на установку двух регистров начиная с 40002 в 00 0A и 01 02 Hex, 
          в подчиненном устройстве 17: 
          Запрос 
          Имя поля						Пример
                                                                  (Hex) 
          Адрес подчиненного					11	0
          Функция						10	1
          Начальный адрес					00	2
          Начальный адрес					01	3
          Кол-во регистров ст.					00	4
          Кол-во регистров мл.					02	5
          Счетчик байт						04	6
          Данные ст.						00	7
          Данные мл.						0A	8
          Данные ст.						01	9
          Данные мл.						02	10
          Контрольная сумма					--
   
          ОТВЕТ 
          Нормальный ответ содержит адрес подчиненного, код функции, начальный адрес, и количество регистров. 
    */
#if ModBusUseFunc16!=0     
    if(PaketRX[1]==0x10)
      {
      //вычисление адреса записываемых слов
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление количества записываемых слов
      unsigned short KolvoWord=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //если неправильный адрес и количество
      if(KolvoWord>ModBusMaxOutRegRX || KolvoWord==0 || (KolvoWord<<1)!=PaketRX[6])
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxOutReg)
        {//неправильный адрес 
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      //Копирование из пакета в регистр ModBusOutReg[]
      {
      unsigned char i;
      for(i=0;i<KolvoWord;i++)
        {
        ModBusOutReg[AdresWord+i]=(((unsigned short)PaketRX[7+(i<<1)])<<8)|(PaketRX[8+(i<<1)]);
        }
      }
      //формирование пакета ответа
      {
      unsigned short TmpCRC=0xFFFF;
      {
      unsigned char i;
      for(i=0;i<6;i++)
        {
        ModBusPUT(PaketRX[i]);
        TmpCRC=CRCfunc(TmpCRC,(PaketRX[i]));
        }
      }
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      }
      ModBus2PrgOutReg();//Считывание регистров Модбас (ModBus->GlobalDate)
           
      //конец
      return;
      }
#endif         
     
    //Код функции 0x16 запись выходного регистра по маске
    /*22 (16 Hex) Mask Write 4X Register 
	Описание 
	Модифицирует содержание регистров 4ХХХХ используя комбинацию OR-маску, AND-маску и текущего содержимого регистра.
        Функция может использоваться для установки или сброса отдельного бита в регистре.
	Алгоритм функции следующий: 
	Результат=(Текущ. знач. AND Маска_И) OR (Маска_ИЛИ AND ~Маска_И) 
	Например:			Hex			Двоичное 
	Текущее значение			12			0001 0010
	Маска_И				F2			1111 0010
	Маска_ИЛИ			25			0010 0101 
	~Маска_И			0D			0000 1101 
	Результат			17			0001 0111 
	Пример записи с маской в регистр 5 в подчиненное устройство 17, с приведенной выше маской описан ниже. 
	Запрос 
	Имя поля						Пример (Hex) 
	Адрес подчиненного					17      0
	Функция							16      1
	Адрес ссылки ст.					00      2
	Адрес ссылки мл.					04      3
	Маска_И ст.						00      4
	Маска_И мл.						F2      5
	Маска_ИЛИ ст.						00      6
	Маска_ИЛИ мл.						25      7
	Контрольная сумма					--

	ОТВЕТ 
	Нормальный ответ повторяет запрос. Ответ возвращается после записи в регистр. 
	Ответ 
	Имя поля						Пример (Hex) 
	Адрес подчиненного					17      0
	Функция							16      1
	Адрес ссылки ст.					00      2
	Адрес ссылки мл.					04      3
	Маска_И ст.						00      4
	Маска_И мл.						F2      5
	Маска_ИЛИ ст.						00      6
	Маска_ИЛИ мл.						25      7
	Контрольная сумма					--
    */
#if ModBusUseFunc22!=0    
     if(PaketRX[1]==0x16)
      {
      //вычисление адреса записываемого регистра
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление масок
      unsigned short MaskAND=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      unsigned short MaskOR=(((((unsigned short)PaketRX[6])<<8)|(PaketRX[7])));
      
      //проверка на допустимый адрес  
      if(AdresWord>=(ModBusMaxOutReg))
        {//если неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      //модификация слова
      ModBusOutReg[AdresWord]=((ModBusOutReg[AdresWord])&MaskAND)|(MaskOR&(~MaskAND));
      
      //формирование пакета ответа
      {
      unsigned char i;
      for(i=0;i<10;i++) ModBusPUT(PaketRX[i]);
      }
      ModBus2PrgOutReg();//Считывание регистров Модбас (ModBus->GlobalDate)
      
      //конец
      return;
#endif     
      }
    
    //полный конец
#if ModBusUseErrMes!=0    
    MesErrRTU(ILLEGAL_FUNCTION);//Ошибка, нераспознаная команда
#endif    
    return;
    }
  return;
  }

//Функция конвертация шеснадцатиричных символов в число
static unsigned char Hex2Dig(unsigned char h)
  {
  if((h>='0')&&(h<='9')) return (h -'0');
  if((h>='A')&&(h<='F')) return (h -'A'+10);
  return 0;
  }
static unsigned char LRCmodbus;//текущий LRC
static unsigned char Simvol0;//предыдущий принятый символ
#define ASCII_CR (0x0D)//возврат каретки 
#define ASCII_LF (0x0A)//перевод строки
static __flash unsigned char BCD[]="0123456789ABCDEF";//строка для конвертации числа в символ
#if ModBusUseErrMes!=0 
static void MesErrASCII(unsigned char Err)
  {
  ModBusPUT(':');
  //адрес
  ModBusPUT(BCD[PaketRX[0]>>4]);//Передаем старший 
  ModBusPUT(BCD[PaketRX[0]&0x0F]);//передаем младший
  LRCmodbus=0-PaketRX[0];//считаем LRC
  //код команды    
  ModBusPUT(BCD[(0x80|PaketRX[1])>>4]);//Передаем старший 
  ModBusPUT(BCD[(0x80|PaketRX[1])&0x0F]);//передаем младший
  LRCmodbus=LRCmodbus-(0x80|PaketRX[1]);//считаем LRC
  //код ошибки
  ModBusPUT(BCD[Err>>4]);//Передаем старший 
  ModBusPUT(BCD[Err&0x0F]);//передаем младший
  LRCmodbus=LRCmodbus-Err;//считаем LRC
  //Конец    
  ModBusPUT(BCD[LRCmodbus>>4]);
  ModBusPUT(BCD[LRCmodbus&0x0F]);
  ModBusPUT(ASCII_CR);
  ModBusPUT(ASCII_LF);
  }
#endif  

//Функция обработки сообщений модбас ASCII
void ModBusASCII(void)
  {
  if(Sost==0)
    {//Состояние прием
    while(!0)
      {//Цикл приема символов
      unsigned short Tmp=ModBusGET(); //читаем символ из входного потока
      if(Tmp==0) return; //если нет данных повторный запрос не требуется 
      //Символ принят
      Tmp=Tmp&0xFF;//отбрасываем признак приема байта
      //проверка на начало пакета
      if(Tmp==':')
        {//начало пакета
        LRCmodbus=0;//обнуляем LRC
        UkPaket=0;//указатель в массиве, текущий принятый символ
        continue;//запускаем повторный запрос символа
        }
       
      //проверка на алфавит сообщения
      if(!(
           ((Tmp>='0')&&(Tmp<='9'))||
           ((Tmp>='A')&&(Tmp<='F'))||
           (Tmp==ASCII_CR)||
           (Tmp==ASCII_LF)
           )) 
        {
        return;//Ошибка, повторный запрос не требуется
        }
        
      //сохраняем принятый символ
      if((UkPaket&1)==0)
        {//указатель принятых данных четный 0,2,4,6...
        Simvol0=Tmp; //сохраняем первый символ пакета
        UkPaket++; //инкременируем указатель пакета
        continue;//запускаем повторный запрос 
        }
      else 
        {//указатель принятых данных нечетный 1,3,5,7...
        if(Tmp!=ASCII_LF)
          {//не достигнут конец
          PaketRX[UkPaket>>1]=(Hex2Dig(Simvol0)<<4)|(Hex2Dig(Tmp));//сохраняем байт пакета 
          LRCmodbus=LRCmodbus-PaketRX[UkPaket>>1];//считаем LRC
          UkPaket++;//икреметируем указатель пакета
          if(UkPaket>(ModBusMaxPaketRX<<1))//проверка на переполнение
            {//Буфер приема переполнился
            UkPaket=0;//сбросить указатель пакета
            return;//ошибка, повторный запрос не требуется
            }
          }
        else break;//Пакет принят!
        }
      }      
    
    //Проверка LCR
    if(LRCmodbus!=0) return;//Ошибка, повторный запрос не требуется
    
    //Проверка адреса
    if((PaketRX[0]!=ModBusID)&&(PaketRX[0]!=ModBusID_FF))
      {//Не наш адрес
      return;
      }
      
    //преходим в состояние передача
    Sost=!0;
    TimModbus=ModBusSysTimer;//запомнить таймер
#if ModBusMaxPauseResp!=0  
    return;//повторный запрос не требуется
#endif  
    }  
  
  ///////////////////////////////////////////////////////////////////////////// 
  if(Sost!=0 
#if ModBusMaxPauseResp!=0     
     && (ModBusSysTimer-TimModbus)>=ModBusMaxPauseResp
#endif     
     )
    {//Состояние передача ответа
    Sost=0;
    
#if ModBusUseFunc1!=0     
    //01 Чтение статуса выходов 
    if(PaketRX[1]==0x01)
      {
      //вычисление адреса запрашиваемых бит
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление количества запрашиваемых бит
      unsigned short KolvoBit=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5]));
      
      //если неправильный адрес и количество
      if(KolvoBit>ModBusMaxOutBitTX || KolvoBit==0)
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }     
      if((AdresBit+KolvoBit)>ModBusMaxOutBit)
        {//неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      Prg2ModBusOutBit();//Заполнение регистров Модбас (GlobalDate->ModBus)
      
      //формирование пакета ответа
      ModBusPUT(':');
      //адрес
      ModBusPUT(BCD[PaketRX[0]>>4]);//Передаем старший 
      ModBusPUT(BCD[PaketRX[0]&0x0F]);//передаем младший
      LRCmodbus=0-PaketRX[0];//считаем LRC
      //код команды    
      ModBusPUT(BCD[1>>4]);//Передаем старший 
      ModBusPUT(BCD[1&0x0F]);//передаем младший
      LRCmodbus=LRCmodbus-1;//считаем LRC
      //количества полных байт
      ModBusPUT(BCD[((KolvoBit+7)>>3)>>4]);//Передаем старший 
      ModBusPUT(BCD[((KolvoBit+7)>>3)&0x0F]);//передаем младший
      LRCmodbus=LRCmodbus-((KolvoBit+7)>>3);//считаем LRC
      //копирование битов в пакет ответа
      {
      unsigned char TxByte=0;//текущий байт
      unsigned char Bit=AdresBit&7;//указатель бит в ModBusOutBit[]
      AdresBit=AdresBit>>3;//указатель байт ModBusOutBit[]
      
      //копирование из регистра ModBusOutBit[] в пакет
      {
      unsigned char i=0;
      while(!0)
        {
        if((ModBusOutBit[AdresBit].byte)&(1<<Bit))//если текущий бит ModBusOutBit[] равен 1
          {//устанавливаем бит в пакете
          TxByte=TxByte|(1<<(i&7));
          }
        //инкрементруем указатели 
        Bit++;
        if(Bit==8){Bit=0;AdresBit++;}
        i++;
        if((i&7)==0)
          {
          ModBusPUT(BCD[TxByte>>4]);//Передаем старший 
          ModBusPUT(BCD[TxByte&0x0F]);//передаем младший
          LRCmodbus=LRCmodbus-TxByte;//считаем LRC
          TxByte=0;
          if(i==KolvoBit) break; else continue;
          }
        if(i==KolvoBit) 
          {
          ModBusPUT(BCD[TxByte>>4]);//Передаем старший 
          ModBusPUT(BCD[TxByte&0x0F]);//передаем младший
          LRCmodbus=LRCmodbus-TxByte;//считаем LRC
          break;
          }
        }
      }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      //конец
      return;
      }
#endif
#if ModBusUseFunc2!=0     
    //02 Read Input Status 
    if(PaketRX[1]==0x02)
      {
      //вычисление адреса запрашиваемых бит
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление количества запрашиваемых бит
      unsigned short KolvoBit=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5]));
      
      //если неправильный адрес и количество
      if(KolvoBit>ModBusMaxInBitTX || KolvoBit==0)
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }
      if((AdresBit+KolvoBit)>ModBusMaxInBit)
        {//неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
           
      Prg2ModBusInBit();//Заполнение регистров Модбас (GlobalDate->ModBus)
      
      //формирование пакета ответа
      ModBusPUT(':');
      //адрес
      ModBusPUT(BCD[PaketRX[0]>>4]);//Передаем старший 
      ModBusPUT(BCD[PaketRX[0]&0x0F]);//передаем младший
      LRCmodbus=0-PaketRX[0];//считаем LRC
      //код команды    
      ModBusPUT(BCD[2>>4]);//Передаем старший 
      ModBusPUT(BCD[2&0x0F]);//передаем младший
      LRCmodbus=LRCmodbus-2;//считаем LRC
      //количества полных байт
      ModBusPUT(BCD[((KolvoBit+7)>>3)>>4]);//Передаем старший 
      ModBusPUT(BCD[((KolvoBit+7)>>3)&0x0F]);//передаем младший
      LRCmodbus=LRCmodbus-((KolvoBit+7)>>3);//считаем LRC
      //копирование битов в пакет ответа
      {
      unsigned char TxByte=0;//текущий байт
      unsigned char Bit=AdresBit&7;//указатель бит в ModBusOutBit[]
      AdresBit=AdresBit>>3;//указатель байт ModBusOutBit[]
      
      //копирование из регистра ModBusOutBit[] в пакет
      {
      unsigned char i=0;
      while(!0)
        {
        if((ModBusInBit[AdresBit].byte)&(1<<Bit))//если текущий бит ModBusOutBit[] равен 1
          {//устанавливаем бит в пакете
          TxByte=TxByte|(1<<(i&7));
          }
        //инкрементруем указатели 
        Bit++;
        if(Bit==8){Bit=0;AdresBit++;}
        i++;
        if((i&7)==0)
          {
          ModBusPUT(BCD[TxByte>>4]);//Передаем старший 
          ModBusPUT(BCD[TxByte&0x0F]);//передаем младший
          LRCmodbus=LRCmodbus-TxByte;//считаем LRC
          TxByte=0;
          if(i==KolvoBit) break; else continue;
          }
        if(i==KolvoBit) 
          {
          ModBusPUT(BCD[TxByte>>4]);//Передаем старший 
          ModBusPUT(BCD[TxByte&0x0F]);//передаем младший
          LRCmodbus=LRCmodbus-TxByte;//считаем LRC
          break;
          }
        }
      }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      //конец
      return;
      }
#endif
#if ModBusUseFunc3!=0     
    //03 Read Holding Registers 
    if(PaketRX[1]==0x03)
      {
      //вычисление адреса запрашиваемых слов
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление адреса количества запрашиваемых слов
      unsigned short KolvoWord=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])); 
      
      //если неправильный адрес и количество
      if(KolvoWord>ModBusMaxOutRegTX || KolvoWord==0)
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxOutReg)
        {//неправильный адрес 
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      Prg2ModBusOutReg();//Заполнение регистров Модбас (GlobalDate->ModBus)
      
      //формирование пакета ответа
      ModBusPUT(':');
      //адрес
      ModBusPUT(BCD[PaketRX[0]>>4]);//Передаем старший 
      ModBusPUT(BCD[PaketRX[0]&0x0F]);//передаем младший
      LRCmodbus=0-PaketRX[0];//считаем LRC
      //код команды
      ModBusPUT(BCD[3>>4]);//Передаем старший 
      ModBusPUT(BCD[3&0x0F]);//передаем младший
      LRCmodbus=LRCmodbus-3;//считаем LRC
      //количества полных байт
      ModBusPUT(BCD[(KolvoWord<<1)>>4]);//Передаем старший 
      ModBusPUT(BCD[(KolvoWord<<1)&0x0F]);//передаем младший
      LRCmodbus=LRCmodbus-(KolvoWord<<1);//считаем LRC
      //Копирование из регистра ModBusOutReg[] в пакет ответа
      {
      unsigned char i;
      for(i=0;i<KolvoWord;i++)
        {
        ModBusPUT(BCD[((ModBusOutReg[AdresWord+i])>>8)>>4]);//Передаем старший 
        ModBusPUT(BCD[((ModBusOutReg[AdresWord+i])>>8)&0x0F]);//передаем младший
        LRCmodbus=LRCmodbus-((ModBusOutReg[AdresWord+i])>>8);//считаем LRC
        ModBusPUT(BCD[(((ModBusOutReg[AdresWord+i])>>0)>>4)&0x0F]);//Передаем старший 
        ModBusPUT(BCD[(((ModBusOutReg[AdresWord+i])>>0)>>0)&0x0F]);//передаем младший
        LRCmodbus=LRCmodbus-((ModBusOutReg[AdresWord+i])>>0);//считаем LRC
        }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      //конец
      return;
      }
#endif
#if ModBusUseFunc4!=0     
    //04 Read Input Registers 
    if(PaketRX[1]==0x04)
      {
      //вычисление адреса запрашиваемых слов
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление адреса количества запрашиваемых слов
      unsigned short KolvoWord=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])); 
      
      //если неправильный адрес и количество
      if(KolvoWord>ModBusMaxInRegTX || KolvoWord==0)
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxInReg)
        {//неправильный адрес 
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
           
      Prg2ModBusInReg();//Заполнение регистров Модбас (GlobalDate->ModBus)
      
      //формирование пакета ответа
      ModBusPUT(':');
      //адрес
      ModBusPUT(BCD[PaketRX[0]>>4]);//Передаем старший 
      ModBusPUT(BCD[PaketRX[0]&0x0F]);//передаем младший
      LRCmodbus=0-PaketRX[0];//считаем LRC
      //код команды
      ModBusPUT(BCD[4>>4]);//Передаем старший 
      ModBusPUT(BCD[4&0x0F]);//передаем младший
      LRCmodbus=LRCmodbus-4;//считаем LRC
      //количества полных байт
      ModBusPUT(BCD[(KolvoWord<<1)>>4]);//Передаем старший 
      ModBusPUT(BCD[(KolvoWord<<1)&0x0F]);//передаем младший
      LRCmodbus=LRCmodbus-(KolvoWord<<1);//считаем LRC
      //Копирование из регистра ModBusOutReg[] в пакет ответа
      {
      unsigned char i;
      for(i=0;i<KolvoWord;i++)
        {
        ModBusPUT(BCD[((ModBusInReg[AdresWord+i])>>8)>>4]);//Передаем старший 
        ModBusPUT(BCD[((ModBusInReg[AdresWord+i])>>8)&0x0F]);//передаем младший
        LRCmodbus=LRCmodbus-((ModBusInReg[AdresWord+i])>>8);//считаем LRC
        ModBusPUT(BCD[(((ModBusInReg[AdresWord+i])>>0)>>4)&0x0F]);//Передаем старший 
        ModBusPUT(BCD[(((ModBusInReg[AdresWord+i])>>0)>>0)&0x0F]);//передаем младший
        LRCmodbus=LRCmodbus-((ModBusInReg[AdresWord+i])>>0);//считаем LRC
        }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      //конец
      return;
      }
#endif
#if ModBusUseFunc5!=0     
    //05 Force Single Coil 
    if(PaketRX[1]==0x05)
      {
      //вычисление адреса записываемого выхода
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      
      //проверка на допустимый адрес  
      if(AdresBit>=ModBusMaxOutBit)
        {//если неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      //установка сброс бита
      switch (((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])))
        {
        case 0xFF00:
        //установка бита
        ModBusOutBit[(AdresBit>>3)].byte|=(1<<(AdresBit&7));
        break;
        case 0x0000:
        //сброс бита
        ModBusOutBit[(AdresBit>>3)].byte&=(~(1<<(AdresBit&7)));
        break;
        default:
          {//ошибка
#if ModBusUseErrMes!=0         
          MesErrASCII(ILLEGAL_DATA_VALUE);
#endif         
          return;
          } 
        }
      
      //Ответ
      ModBusPUT(':');
      {
      unsigned char i;
      for(i=0;i<7;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//Передаем старший 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//передаем младший
        }
      }
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
         
      ModBus2PrgOutBit();//Считывание регистров Модбас (ModBus->GlobalDate)
      //конец
      return; 
      }
#endif
#if ModBusUseFunc6!=0     
    //06 Preset Single Register 
    if(PaketRX[1]==0x06)
      {
      //вычисление адреса записываемого регистра
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      
      //проверка на допустимый адрес  
      if(AdresWord>=(ModBusMaxOutReg))
        {//если неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
          
      //запись слова
      ModBusOutReg[AdresWord]=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //Ответ
      ModBusPUT(':');
      {
      unsigned char i;
      for(i=0;i<7;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//Передаем старший 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//передаем младший
        }
      }
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      
      ModBus2PrgOutReg();//Считывание регистров Модбас (ModBus->GlobalDate)
        
      //конец
      return;
      }
#endif
#if ModBusUseFunc15!=0      
    //15 (0F Hex) Force Multiple Coils 
    if(PaketRX[1]==0x0F)
      {
      //вычисление адреса записываемых бит
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление количества записываемых бит
      unsigned short KolvoBit=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //если неправильный адрес и количество
      if(KolvoBit>ModBusMaxOutBitRX || KolvoBit==0 || ((KolvoBit+7)>>3)!=PaketRX[6])
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }     
      if((AdresBit+KolvoBit)>ModBusMaxOutBit)
        {//неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
            
      //установка битов
      {
      unsigned char Bit=(AdresBit&7);//указатель бит в ModBusOutBit[]
      AdresBit=AdresBit>>3;//указатель байт ModBusOutBit[]
      {
      unsigned char i;
      for(i=0;i<KolvoBit;i++)
        {
        if(PaketRX[7+(i>>3)]&(1<<(i&7)))//если текущий бит PaketRX равен 1
          {//устанавливаем бит в ModBusOutBit[]
          ModBusOutBit[AdresBit].byte=(ModBusOutBit[AdresBit].byte)|((unsigned char)(1<<Bit));
          }
        else
          {//сбрасываем бит ModBusOutBit[]
          ModBusOutBit[AdresBit].byte=(ModBusOutBit[AdresBit].byte)&((unsigned char)(~(1<<Bit)));
          }
        //инкрементруем указатели 
        Bit++;if(Bit==8){Bit=0;AdresBit++;}
        }           
      }
      }
      //вычисляем LRC пакета передачи и передаем
      LRCmodbus=0;
      ModBusPUT(':');
      {
      unsigned char i;
      for(i=0;i<6;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//Передаем старший 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//передаем младший
        LRCmodbus=LRCmodbus-PaketRX[i];//считаем LRC
        }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      
      ModBus2PrgOutBit();//Считывание регистров Модбас (ModBus->GlobalDate)
      //конец
      return;
      }
#endif
#if ModBusUseFunc16!=0        
    //16 (10 Hex) Preset Multiple Regs 
    if(PaketRX[1]==0x10)
      {
      //вычисление адреса записываемых слов
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление количества записываемых слов
      unsigned short KolvoWord=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //если неправильный адрес и количество
      if(KolvoWord>ModBusMaxOutRegRX || KolvoWord==0 || (KolvoWord<<1)!=PaketRX[6])
        {//неправильное количество
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxOutReg)
        {//неправильный адрес 
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
            
      //Копирование из пакета в регистр ModBusOutReg[]
      {
      unsigned char i;
      for(i=0;i<KolvoWord;i++)
        {
        ModBusOutReg[AdresWord+i]=(((unsigned short)PaketRX[7+(i<<1)])<<8)|(PaketRX[8+(i<<1)]);
        }
      }
      //вычисляем LRC пакета передачи и передаем
      LRCmodbus=0;
      ModBusPUT(':');
      {
      unsigned char i;
      for(i=0;i<6;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//Передаем старший 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//передаем младший
        LRCmodbus=LRCmodbus-PaketRX[i];//считаем LRC
        }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      
      ModBus2PrgOutReg();//Считывание регистров Модбас (ModBus->GlobalDate)
      //конец
      return; 
      }

#endif 
#if ModBusUseFunc22!=0     
    //22 (0x16) Mask Write 4X Register
    if(PaketRX[1]==0x16)
      {
      //вычисление адреса записываемого регистра
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //вычисление масок
      unsigned short MaskAND=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      unsigned short MaskOR=(((((unsigned short)PaketRX[6])<<8)|(PaketRX[7])));
      
      //проверка на допустимый адрес  
      if(AdresWord>=(ModBusMaxOutReg))
        {//если неправильный адрес
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
          
      //модификация слова
      ModBusOutReg[AdresWord]=((ModBusOutReg[AdresWord])&MaskAND)|(MaskOR&(~MaskAND));
      
      //Ответ
      ModBusPUT(':');
      {
      unsigned char i;
      for(i=0;i<9;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//Передаем старший 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//передаем младший
        }
      }
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      ModBus2PrgOutReg();//Считывание регистров Модбас (ModBus->GlobalDate)
      //конец
      return;
      }
#endif    
    
    //полный конец
#if ModBusUseErrMes!=0    
    MesErrASCII(ILLEGAL_FUNCTION);//Ошибка, нераспознаная команда
#endif    
    return;
    } 
  return;
  }









