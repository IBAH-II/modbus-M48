#define __MODBUS_C
#include "modbus.h"

static unsigned long TimModbus; //����� ������ ������ �� ���������� �������
static unsigned short CRCmodbus;//������� CRC
static unsigned char UkPaket;//��������� � �������, ������� �������� ������ 
static unsigned char Sost;//��������� 0/1 �����/��������
static unsigned char PaketRX[ModBusMaxPaketRX];//������ ��� ���������� ������

//������������� ModBus
void ModBusIni(void)
  {
  TimModbus=ModBusSysTimer;//��������� ������
  UkPaket=0;//�������� ��������� ������
  CRCmodbus=0xFFFF; //���������� ��������� �������� CRC
  Sost=0;//��������� �����
  //������������� ��������� ModBus
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

//������� ���������� CRC
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

//������� ��������� ������
#if ModBusUseErrMes!=0
#define ILLEGAL_FUNCTION (1) //�������� ��� ������� �� ����� ���� ��������� �� �����������. 
#define ILLEGAL_DATA_ADDRESS (2) //����� ������ ��������� � ������� �� �������� ������� ������������.
#define ILLEGAL_DATA_VALUE (3) //��������, ������������ � ���� ������ ������� �������� �� ���������� ��������� ��� ������������.
static void MesErrRTU(unsigned char Err)
  {
  //�����
  ModBusPUT(PaketRX[0]);
  {
  unsigned short TmpCRC=CRCfunc(0xFFFF,PaketRX[0]);
  //��� �������    
  ModBusPUT(0x80|PaketRX[1]);
  TmpCRC=CRCfunc(TmpCRC,(0x80|PaketRX[1]));
  //��� ������
  ModBusPUT(Err);
  TmpCRC=CRCfunc(TmpCRC,Err);
  ModBusPUT(TmpCRC);
  ModBusPUT(TmpCRC>>8);
  }
  }
#endif

//������� ��������� ��������� ModBus
void ModBusRTU(void)
  {
  if(Sost==0)
    {//��������� �����
    while(!0)
      {//���� ������ ��������
      unsigned short Tmp=ModBusGET(); //������ ������ �� �������� ������
      if(Tmp==0) return; //���� ��� ������ - ������� 
      //������ ������
      Tmp=Tmp&0xFF;//����������� ������� ������ �����
      //�������� ���������� ��������� ����� ���������
      if((ModBusSysTimer-TimModbus)>ModBusMaxPause)
        {//�������� �������, �������� ����� ������ ������
        PaketRX[0]=Tmp;//��������� �������� ������ � ����� ������
        UkPaket=1;//���������� ��������� ������
        TimModbus=ModBusSysTimer;//�������� ������
        //���������� CRC
        CRCmodbus=CRCfunc(0xFFFF,Tmp);
        continue;//��������� ������ �������
        }
      else
        {//������� �� ��������, ��������� ��� ������� �����
        TimModbus=ModBusSysTimer;//�������� ������
        PaketRX[UkPaket]=Tmp;//��������� �������� ������
        UkPaket++;//��������� ��������� ������
        if(UkPaket==ModBusMaxPaketRX)//��������� ���������� �������� ����
          {//����� ������ ������������
          UkPaket=0;//�������� ��������� ������
          CRCmodbus=0xFFFF; //���������� ��������� �������� CRC
          return;//������, ��������� ������ ������� ���������
          }
        CRCmodbus=CRCfunc(CRCmodbus,Tmp);//���������� CRC
        }
      //���� ������� ���� ������
      if(UkPaket<8) continue; //��������� ������ �������
      //�������� �� �������� ������
      if(CRCmodbus==0) 
        {//�������� �� ������� ������
        if(PaketRX[1]==15 || PaketRX[1]==16)
          {//���� ������� ������� (15,16) , ��������� "������� ����"
          if((PaketRX[6]+9)!=UkPaket) continue;
          }
        break; //���! ����� ������!!!
        }
      }
    //���! ����� ������!!!
    CRCmodbus=0xFFFF; //���������� ��������� �������� CRC
    UkPaket=0;//�������� ��������� ������    
    //�������� ������
    if((PaketRX[0]!=ModBusID)&&(PaketRX[0]!=ModBusID_FF))
      {//�� ��� �����
      return;
      }    
    //��������� � ��������� �������� ������
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
    {//��������� �������� ������
    Sost=0;
    //��� ������� 01 - ������ ������� Coils (���������� �������� �����). 
    /*���������-������ �������� ����� ���������� ���� � ���������� ����� ��� ������. 
    ���� ���������� ������� � 0. 
    � ���������-������ ������ �������� ���������� ���������� ����� �����,
    �� ���� � ����� ����� �������� ������ 8 ����� ����������. 
    ���� ���������� �� �� ������ ������, ��������� ���� � ����� ����������� ������. 
    ������� ������� ���������� ���� � ���� ������.
    01 ������ ������� ������� 
          �������� 
          ������ ������� ON/OFF ���������� ������� � �����������. 
          ������ 
          ������ �������� ����� ���������� ������ � ���������� ������� ��� ������. 
          ������ ���������� ������� � ����: ������ 1-16 ���������� ��� 0-15.
          ���� �������� ������ ������� �� ������ ������� 20-56 � ������������ ���������� 17. 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						01	1
          ��������� ����� Hi					00	2
          ��������� ����� Lo					13	3
          ���������� Hi						00	4
          ���������� Lo						25	5
          ����������� ����� (CRC ��� LRC)			--

          ����� 
          ������ ������� � �������� ��������� ���������� ��� ���� ����� �� ���.
          ���� ������������ ���������� ������� �� ������ ������, �� ���������� ���� � ��������� ����� ��������� ����� ����������� � 0. 
          ������� ���� �������� ���������� ���� ������������ � ���� ������. 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						01	1
          ������� ����						05	2
          ������(������ 27-20)					CD	3
          ������(������ 35-28)					6B	4
          ������(������ 43-36)					B2	5
          ������(������ 51-44)					0E	6
          ������(������ 56-52)					1B	7
          ����������� ����� (CRC ��� LRC)			--
    */
#if ModBusUseFunc1!=0       
    if(PaketRX[1]==0x01)
      {
      //���������� ������ ������������� ���
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ���������� ������������� ���
      unsigned short KolvoBit=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5]));
      
      //���� ������������ ����� � ����������
      if(KolvoBit>ModBusMaxOutBitTX || KolvoBit==0)
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }     
      if((AdresBit+KolvoBit)>ModBusMaxOutBit)
        {//������������ �����
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      Prg2ModBusOutBit();//���������� ��������� ������ (GlobalDate->ModBus)
      
      //������������ ������ ������
      //�����
      ModBusPUT(PaketRX[0]);
      {
      unsigned short TmpCRC=CRCfunc(0xFFFF,PaketRX[0]);
      //��� �������    
      ModBusPUT(1);
      TmpCRC=CRCfunc(TmpCRC,1);
      //���������� ������ ����
      ModBusPUT((KolvoBit+7)>>3);
      TmpCRC=CRCfunc(TmpCRC,((KolvoBit+7)>>3));
      
      //����������� ����� � ����� ������
      {
      unsigned char TxByte=0;//������� ����
      unsigned char Bit=AdresBit&7;//��������� ��� � ModBusOutBit[]
      AdresBit=AdresBit>>3;//��������� ���� ModBusOutBit[]
      //����������� �� �������� ModBusOutBit[] � �����
      {
      unsigned char i=0;
      while(!0)
        {
        if((ModBusOutBit[AdresBit].byte)&(1<<Bit))
          {
          TxByte=TxByte|(1<<(i&7));
          }
        //������������� ��������� 
        Bit++;
        if(Bit==8){Bit=0;AdresBit++;}
        i++;
        if((i&7)==0)
          {//���������� ����
          ModBusPUT(TxByte);
          TmpCRC=CRCfunc(TmpCRC,TxByte);
          TxByte=0;
          if(i==KolvoBit) break; else continue;
          }
        if(i==KolvoBit) 
          {//����������� ��� ����
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
      //�����
      return;
      }
#endif    
    
    //��� ������� 2 - ������ ������� ���������� ������
    /*02 Read Input Status 
          �������� 
          ������ ON/OFF ��������� ���������� ������ (������ 1�) � �����������. 
          ������ 
          ������ �������� ����� ���������� ����� � ���������� ������ ��� ������. ����� ���������� ������� � 0.
          ���� �������� ������ ������� �� ������ ������ 10197-10218 � ������������ ���������� 17. 
                  ������ 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						02	1
          ��������� ����� ��.					00	2
          ��������� ����� ��.					C4	3
          ���-�� ������ ��.					00	4
          ���-�� ������ ��.					16	5
          ����������� �����					--

          ����� 
          ������ ������ � �������� ��������� ���������� ��� ���� ����� �� ���.
          ���� ������������ ���������� ������ �� ������ ������, �� ���������� ���� � ��������� ����� ��������� ����� ����������� � 0. 
          ������� ���� �������� ���������� ���� ������������ � ���� ������. 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						01	1
          ������� ����						03	2
          ������(����� 10204-10197)				AC	3
          ������(����� 10212-10205)				DB	4
          ������(����� 10218-10213)				35	5
          ����������� ����� (CRC ��� LRC)			--  
    */
#if ModBusUseFunc2!=0     
    if(PaketRX[1]==0x02)
      {
      //���������� ������ ������������� ���
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ���������� ������������� ���
      unsigned short KolvoBit=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5]));
      
      //���� ������������ ����� � ����������
      if(KolvoBit>ModBusMaxInBitTX || KolvoBit==0)
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }
      if((AdresBit+KolvoBit)>ModBusMaxInBit)
        {//������������ �����
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      Prg2ModBusInBit();//���������� ��������� ������ (GlobalDate->ModBus)
      
      //������������ ������ ������
      //�����
      ModBusPUT(PaketRX[0]);
      {
      unsigned short TmpCRC=CRCfunc(0xFFFF,PaketRX[0]);
      //��� �������    
      ModBusPUT(2);
      TmpCRC=CRCfunc(TmpCRC,2);
      //���������� ������ ����
      ModBusPUT((KolvoBit+7)>>3);
      TmpCRC=CRCfunc(TmpCRC,((KolvoBit+7)>>3));
      //����������� ����� � ����� ������
      {
      unsigned char TxByte=0;//������� ����
      unsigned char Bit=AdresBit&7;//��������� ��� 
      AdresBit=AdresBit>>3;//��������� ���� 
      //����������� �� �������� ModBusInBit[] � �����
      {
      unsigned char i=0;
      while(!0)
        {
        if((ModBusInBit[AdresBit].byte)&(1<<Bit))
          {//������������� ��� � ������
          TxByte=TxByte|(1<<(i&7));
          }
        //������������� ��������� 
        Bit++;
        if(Bit==8){Bit=0;AdresBit++;}
        i++;
        if((i&7)==0)
          {//���������� ����
          ModBusPUT(TxByte);
          TmpCRC=CRCfunc(TmpCRC,TxByte);
          TxByte=0;
          if(i==KolvoBit) break; else continue;
          }
        if(i==KolvoBit)
          {//����������� ��� ����
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
      //�����
      return;
      }
#endif    
    
    //��� ������� 03 - ������ �������� ��������/���������� ���������. 
    /*���������-������ �������� ����� ���������� ���������/����������� �������� (������������ �����), 
    � ���������� ��������� ��� ������. �������� ���������� ������� � 0.
    03 Read Holding Registers 
          �������� 
          ������ ��������� ���������� ��������� (������ 4�) � �����������. 
          ������ 
          ��������� ������� ������������� ��������� ������� � ���������� ��������� ��� ������. 
          �������� ���������� ������ � 0: �������� 1-16 ���������� ��� 0-15.
          ���� �������� ������ ������ ��������� 40108-40110 � ������������ ���������� 17. 
          ������ 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						03	1
          ��������� ����� ��.					00	2
          ��������� ����� ��.					6B	3
          ���-�� ��������� ��.					00	4
          ���-�� ��������� ��.					03	5
          ����������� �����					--

          ����� 
          ������ ��������� � ������ ���������� ��� ��� ����� �� �������. 
          ��� ������� ��������, ������ ���� �������� ������� ���� ������ ���� �������� ������� ����.
          �� ���� ��������� ����� ����������� 125 ��������� ��� ������������ 984-�8� (984-685 � �.�.), 
          � 32 �������� ��� ������ ������������. ����� ������, ����� ��� ������ ��������������.
          ��� ������ ������ �� ������ �������������� ����: 
          ����� 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						03	1
          ������� ����						06	2
          ������ (������� 40108) ��.				02	3
          ������ (������� 40108) ��.				2B	4
          ������ (������� 40109) ��.				00	5
          ������ (������� 40109) ��.				00	6
          ������ (������� 40110) ��.				00	7
          ������ (������� 40110) ��.				64	8
          ����������� �����					--
    */
#if ModBusUseFunc3!=0      
    if(PaketRX[1]==0x03)
      {
      //���������� ������ ������������� ����
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ������ ���������� ������������� ����
      unsigned short KolvoWord=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])); 
      
      //���� ������������ ����� � ����������
      if(KolvoWord>ModBusMaxOutRegTX || KolvoWord==0)
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxOutReg)
        {//������������ ����� 
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      Prg2ModBusOutReg();//���������� ��������� ������ (GlobalDate->ModBus)
      
      //������������ ������ ������
      //�����
      ModBusPUT(PaketRX[0]);
      {
      unsigned short TmpCRC=CRCfunc(0xFFFF,PaketRX[0]);
      //��� �������    
      ModBusPUT(3);
      TmpCRC=CRCfunc(TmpCRC,3);
      //���������� ������ ����
      ModBusPUT(KolvoWord<<1);
      TmpCRC=CRCfunc(TmpCRC,(KolvoWord<<1));
      //����������� �� �������� ModBusOutReg[] � ����� ������
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
      //�����
      return;
      }
#endif     
    
    //��� ������� 04 - ������ �������� ������� ���������
    /*04 Read Input Registers 
          ���������� 
          ������ ��������� ���������� ������� ��������� (������ 3�) � �����������. 
          ������ 
          ������ �������� ����� ���������� �������� � ���������� ��������� ��� ������.
          ���� �������� ������ ������� ��� ������ �������� 30009 � ������������ ���������� 17. 
          ������ 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						03	1
          ��������� ����� ��.					00	2
          ��������� ����� ��.					6B	3
          ���-�� ��������� ��.					00	4
          ���-�� ��������� ��.					03	5
          ����������� �����					--
   
          ����� 
          ������ ��������� � ������ ���������� ��� ��� ����� �� �������. 
          ��� ������� ��������, ������ ���� �������� ������� ����, ������ ���� �������� ������� ����.
          �� ���� ��������� ����� ����������� 125 ��������� ��� ������������ 984-�8� (984-685 � �.�.), 
          � 32 �������� ��� ������ ������������. ����� ������ ����� ��� ������ ��������������.
          ��� ������ ������ �� ������ �������������� ����: 
          ����� 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						03	1
          ������� ����						02	2
          ������ (������� 30009) ��.				00	3
          ������ (������� 30009) ��.				2A	4
          ����������� �����					--  
    */
#if ModBusUseFunc4!=0     
    if(PaketRX[1]==0x04)
      {
      //���������� ������ ������������� ����
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ������ ���������� ������������� ����
      unsigned short KolvoWord=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])); 
      
      //���� ������������ ����� � ����������
      if(KolvoWord>ModBusMaxInRegTX || KolvoWord==0)
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxInReg)
        {//������������ ����� 
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      Prg2ModBusInReg();//���������� ��������� ������ (GlobalDate->ModBus)
      
      //������������ ������ ������
      //�����
      ModBusPUT(PaketRX[0]);
      {
      unsigned short TmpCRC=CRCfunc(0xFFFF,(PaketRX[0]));
      //��� �������    
      ModBusPUT(4);
      TmpCRC=CRCfunc(TmpCRC,4);
      //���������� ������ ����
      ModBusPUT(KolvoWord<<1);
      TmpCRC=CRCfunc(TmpCRC,(KolvoWord<<1));
      //����������� �� �������� ModBusInReg[] � ����� ������
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
      //�����
      return;//��������� ������ �� ���������
      }
#endif      
    
    //��� ������� 05 - ������ ���������/����������� ����
    /*05 Force Single Coil 
          �������� 
          ��������� ���������� ������ (������ 0�) � ON ��� OFF. 
          ��� ����������������� �������� ������� ������������� ��� ������ � ������ ������� �� ���� ����������� ������������. 
          ��������� ������� ����� ������������ � ���������� ������
                          ������ � ���������� ������������� �������. 
          ������ 
          ������ �������� ����� ������ ��� ���������. ������ ���������� ������� � 0. ����� 1 ���������� ��� 0.
          ���������, � ������� ���������� ���������� ����� (ON/OFF) ����������� � ���� ������. 
          �������� FF00 Hex - ON. �������� 0000 - OFF. ����� ������ ����� ������� � �� ������ �� �����.
          � ����������� ���� ������� ��������������� ����� 173 � ��������� ON � ����������� ���������� 17. 
          ������ 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						05	1
          ����� ������ ��.					00	2
          ����� ������ ��.					AC	3
          ������ ��.						FF	4
          ������ ��.						00	5
          ����������� �����					--
   
          ����� 
          ���������� ����� ��������� ������. 
          ����� 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						05	1
          ����� ������ ��.					00	2
          ����� ������ ��.					AC	3
          ������ ��.						FF	4
          ������ ��.						00	5
          ����������� �����					--  
    */
#if ModBusUseFunc5!=0     
    if(PaketRX[1]==0x05)
      {
      //���������� ������ ������������� ������
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      
      //�������� �� ���������� �����  
      if(AdresBit>=ModBusMaxOutBit)
        {//������������ �����
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      //���������/����� ����
      switch (((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])))
        {
        case 0xFF00:
        //��������� ����
        ModBusOutBit[(AdresBit>>3)].byte|=(1<<(AdresBit&7));
        break;
        case 0x0000:
        //����� ����
        ModBusOutBit[(AdresBit>>3)].byte&=(~(1<<(AdresBit&7)));
        break;
        default:
          {//������
#if ModBusUseErrMes!=0         
          MesErrRTU(ILLEGAL_DATA_VALUE);
#endif          
          return;
          } 
        }
      //������������ ������ ������
      {
      unsigned char i;
      for(i=0;i<8;i++) ModBusPUT(PaketRX[i]);
      }
      ModBus2PrgOutBit();//���������� ��������� ������ (ModBus->GlobalDate)
      
      //�����
      return;
      }
#endif     
    
    //��� ������� 06 - ������ ���������/����������� ��������. 
    /*������� ���������� 05, �� ��������� � ���������� (�������). 
    � ������� ����������� ����� ���������/����������� �������� � ��� ��������. 
    06 Preset Single Register 
          ��������. ���������� �������� � ��������� ������� (������ 4�).
          ��� ����������������� �������� �� ���� ����������� ����������� ��������������� ���� � ��� �� �������. 
          ��������� 
          ������� ����� ������������ � ������������� ������� ������. 
          ������ 
          ������ �������� ������ �� �������, ������� ���������� ����������. �������� ���������� � 0.
          ��������, � ������� ���������� ���������� ������� ���������� � ���� ������. 
          ����������� M84 � 484 ���������� 10-�� ������ ��������, ������� ����� ��� ����������� 0. 
          ��� ������ ����������� ���������� 16 ���.
          � ����������� ���� ������� � ������� 40002 ������������ �������� 0003 Hex � ����������� ���������� 17. 
          ������ 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						06	1
          ����� �������� ��.					00	2
          ����� �������� ��.					01	3
          ������ ��.						00	4
          ������ ��.						03	5
          ����������� �����					--
   
          ����� 
          ���������� ����� ��������� ������. 
          ����� 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						06	1
          ����� �������� ��.					00	2
          ����� �������� ��.					01	3
          ������ ��.						00	4
          ������ ��.						03	5
          ����������� �����					--  
    */
#if ModBusUseFunc6!=0    
    if(PaketRX[1]==0x06)
      {
      //���������� ������ ������������� ��������
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      
      //�������� �� ���������� �����  
      if(AdresWord>=(ModBusMaxOutReg))
        {//���� ������������ �����
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      //������ �����
      ModBusOutReg[AdresWord]=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //������������ ������ ������
      {
      unsigned char i;
      for(i=0;i<8;i++) ModBusPUT(PaketRX[i]);
      }
      ModBus2PrgOutReg();//���������� ��������� ������ (ModBus->GlobalDate)
      
      //�����
      return;
      }
#endif     
    
    //��� ������� 0x0F - ������ ���������� ��������/���������� �����. 
    /*� ������� ����������� ��������� ����� ����, ���������� ��� ��� ������, ������� ������ � ��������������� ��������. 
    15 (0F Hex) Force Multiple Coils 
          �������� 
          ������������� ������ ����� (������ 0�) ������������������ ������� � ���� �� ��������� ON ��� OFF. 
          ��� ����������������� �������� ������� ������������� �������� ������ �� ���� �����������. 
          ��������� ������� ����� ������������ � ���������� ������ ������ � ���������� ������������� �������. 
          ������ 
          ������ ������������� ������ ��� ���������. ������ ���������� ������� � 0.
          ���� ������� ������ ������� �� ��������� ������������������ ������� ������� � 20 (���������� ��� 19) 
          � ����������� ���������� 17.
          ���� ������ ������� �������� 2 �����: CD 01 Hex (1100 1101 0000 0001 ��������). 
          ������������ ����� � ������� ������������ ����: 
          ���:    1  1  0  0  1  1  0  1		0  0  0  0  0  0   0  1 
          �����: 27 26 25 24 23 22 21 20		-  -  -  -  -  -  29 28 
          ������ 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						0F	1
          ����� ������ ��.					00	2
          ����� ������ ��.					13	3
          ���-�� ������� ��.					00	4
          ���-�� ������� ��.					0A	5
          ������� ����						02	6
          ������ ��� ��������� (������ 27-20)			CD	7
          ������ ��� ��������� (������ 29-28) 			01	8
          ����������� �����					--	9
   
          ����� 
          ���������� ����� ���������� ����� ������������, ��� �������, ��������� �����, � ���������� ������������� �������.
          ��� ������ ������ �� �������������� ���� ������. 
          ����� 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						0F	1
          ����� ������ ��.					00	2
          ����� ������ ��.					13	3
          ���-�� ������� ��.					00	4
          ���-�� ������� ��.					0A	5
          ����������� �����					--
    */
#if ModBusUseFunc15!=0    
    if(PaketRX[1]==0x0F)
      {
      //���������� ������ ������������ ���
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ���������� ������������ ���
      unsigned short KolvoBit=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //���� ������������ ����� � ����������
      if(KolvoBit>ModBusMaxOutBitRX || KolvoBit==0 || ((KolvoBit+7)>>3)!=PaketRX[6])
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }     
      if((AdresBit+KolvoBit)>ModBusMaxOutBit)
        {//������������ �����
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      //��������� �����
      {
      unsigned char Bit=(AdresBit&7);//��������� ��� � ModBusOutBit[]
      AdresBit=AdresBit>>3;//��������� ���� ModBusOutBit[]
      //���� �� �����
      {
      unsigned char i;
      for(i=0;i<KolvoBit;i++)
        {
        if(PaketRX[7+(i>>3)]&(1<<(i&7)))//���� ������� ��� PaketRX ����� 1
          {//������������� ��� � ModBusOutBit[]
          ModBusOutBit[AdresBit].byte=(ModBusOutBit[AdresBit].byte)|((unsigned char)(1<<Bit));
          }
        else
          {//���������� ��� ModBusOutBit[]
          ModBusOutBit[AdresBit].byte=(ModBusOutBit[AdresBit].byte)&((unsigned char)(~(1<<Bit)));
          }
        //������������� ��������� 
        Bit++; if(Bit==8){Bit=0;AdresBit++;}
        }           
      }
      }
      //������������ ������ ������
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
      ModBus2PrgOutBit();//���������� ��������� ������ (ModBus->GlobalDate)
      
      //�����
      return;
      }
#endif     
    //��� ������� 0x10 ������ ���������� ��������/���������� ���������.
    /*16 (10 Hex) Preset Multiple Regs 
          �������� 
          ������ ������ � ������������������ ��������� (������ 4�). 
          ��� ����������������� ��������, ������� ������������� �������� �������� �� ���� ����������� �����������. 
          ��������� 
          ������� ����� ������������ � ������������� ������� ������. 
          ������ 
          ������ ������������� �������� ��� ������. �������� ���������� ������� � 0.
          ������ ��� ������ � �������� ���������� � ���� ������ �������. 
          ����������� M84 � 484 ���������� 10-������� ��������, �� �������� ������ ������ �������������� � 0. 
          ��� ��������� ����������� ���������� 16 ���.
          ���� �������� ������ ������� �� ��������� ���� ��������� ������� � 40002 � 00 0A � 01 02 Hex, 
          � ����������� ���������� 17: 
          ������ 
          ��� ����						������
                                                                  (Hex) 
          ����� ������������					11	0
          �������						10	1
          ��������� �����					00	2
          ��������� �����					01	3
          ���-�� ��������� ��.					00	4
          ���-�� ��������� ��.					02	5
          ������� ����						04	6
          ������ ��.						00	7
          ������ ��.						0A	8
          ������ ��.						01	9
          ������ ��.						02	10
          ����������� �����					--
   
          ����� 
          ���������� ����� �������� ����� ������������, ��� �������, ��������� �����, � ���������� ���������. 
    */
#if ModBusUseFunc16!=0     
    if(PaketRX[1]==0x10)
      {
      //���������� ������ ������������ ����
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ���������� ������������ ����
      unsigned short KolvoWord=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //���� ������������ ����� � ����������
      if(KolvoWord>ModBusMaxOutRegRX || KolvoWord==0 || (KolvoWord<<1)!=PaketRX[6])
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxOutReg)
        {//������������ ����� 
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      //����������� �� ������ � ������� ModBusOutReg[]
      {
      unsigned char i;
      for(i=0;i<KolvoWord;i++)
        {
        ModBusOutReg[AdresWord+i]=(((unsigned short)PaketRX[7+(i<<1)])<<8)|(PaketRX[8+(i<<1)]);
        }
      }
      //������������ ������ ������
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
      ModBus2PrgOutReg();//���������� ��������� ������ (ModBus->GlobalDate)
           
      //�����
      return;
      }
#endif         
     
    //��� ������� 0x16 ������ ��������� �������� �� �����
    /*22 (16 Hex) Mask Write 4X Register 
	�������� 
	������������ ���������� ��������� 4���� ��������� ���������� OR-�����, AND-����� � �������� ����������� ��������.
        ������� ����� �������������� ��� ��������� ��� ������ ���������� ���� � ��������.
	�������� ������� ���������: 
	���������=(�����. ����. AND �����_�) OR (�����_��� AND ~�����_�) 
	��������:			Hex			�������� 
	������� ��������			12			0001 0010
	�����_�				F2			1111 0010
	�����_���			25			0010 0101 
	~�����_�			0D			0000 1101 
	���������			17			0001 0111 
	������ ������ � ������ � ������� 5 � ����������� ���������� 17, � ����������� ���� ������ ������ ����. 
	������ 
	��� ����						������ (Hex) 
	����� ������������					17      0
	�������							16      1
	����� ������ ��.					00      2
	����� ������ ��.					04      3
	�����_� ��.						00      4
	�����_� ��.						F2      5
	�����_��� ��.						00      6
	�����_��� ��.						25      7
	����������� �����					--

	����� 
	���������� ����� ��������� ������. ����� ������������ ����� ������ � �������. 
	����� 
	��� ����						������ (Hex) 
	����� ������������					17      0
	�������							16      1
	����� ������ ��.					00      2
	����� ������ ��.					04      3
	�����_� ��.						00      4
	�����_� ��.						F2      5
	�����_��� ��.						00      6
	�����_��� ��.						25      7
	����������� �����					--
    */
#if ModBusUseFunc22!=0    
     if(PaketRX[1]==0x16)
      {
      //���������� ������ ������������� ��������
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� �����
      unsigned short MaskAND=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      unsigned short MaskOR=(((((unsigned short)PaketRX[6])<<8)|(PaketRX[7])));
      
      //�������� �� ���������� �����  
      if(AdresWord>=(ModBusMaxOutReg))
        {//���� ������������ �����
#if ModBusUseErrMes!=0         
        MesErrRTU(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      //����������� �����
      ModBusOutReg[AdresWord]=((ModBusOutReg[AdresWord])&MaskAND)|(MaskOR&(~MaskAND));
      
      //������������ ������ ������
      {
      unsigned char i;
      for(i=0;i<10;i++) ModBusPUT(PaketRX[i]);
      }
      ModBus2PrgOutReg();//���������� ��������� ������ (ModBus->GlobalDate)
      
      //�����
      return;
#endif     
      }
    
    //������ �����
#if ModBusUseErrMes!=0    
    MesErrRTU(ILLEGAL_FUNCTION);//������, ������������� �������
#endif    
    return;
    }
  return;
  }

//������� ����������� ���������������� �������� � �����
static unsigned char Hex2Dig(unsigned char h)
  {
  if((h>='0')&&(h<='9')) return (h -'0');
  if((h>='A')&&(h<='F')) return (h -'A'+10);
  return 0;
  }
static unsigned char LRCmodbus;//������� LRC
static unsigned char Simvol0;//���������� �������� ������
#define ASCII_CR (0x0D)//������� ������� 
#define ASCII_LF (0x0A)//������� ������
static __flash unsigned char BCD[]="0123456789ABCDEF";//������ ��� ����������� ����� � ������
#if ModBusUseErrMes!=0 
static void MesErrASCII(unsigned char Err)
  {
  ModBusPUT(':');
  //�����
  ModBusPUT(BCD[PaketRX[0]>>4]);//�������� ������� 
  ModBusPUT(BCD[PaketRX[0]&0x0F]);//�������� �������
  LRCmodbus=0-PaketRX[0];//������� LRC
  //��� �������    
  ModBusPUT(BCD[(0x80|PaketRX[1])>>4]);//�������� ������� 
  ModBusPUT(BCD[(0x80|PaketRX[1])&0x0F]);//�������� �������
  LRCmodbus=LRCmodbus-(0x80|PaketRX[1]);//������� LRC
  //��� ������
  ModBusPUT(BCD[Err>>4]);//�������� ������� 
  ModBusPUT(BCD[Err&0x0F]);//�������� �������
  LRCmodbus=LRCmodbus-Err;//������� LRC
  //�����    
  ModBusPUT(BCD[LRCmodbus>>4]);
  ModBusPUT(BCD[LRCmodbus&0x0F]);
  ModBusPUT(ASCII_CR);
  ModBusPUT(ASCII_LF);
  }
#endif  

//������� ��������� ��������� ������ ASCII
void ModBusASCII(void)
  {
  if(Sost==0)
    {//��������� �����
    while(!0)
      {//���� ������ ��������
      unsigned short Tmp=ModBusGET(); //������ ������ �� �������� ������
      if(Tmp==0) return; //���� ��� ������ ��������� ������ �� ��������� 
      //������ ������
      Tmp=Tmp&0xFF;//����������� ������� ������ �����
      //�������� �� ������ ������
      if(Tmp==':')
        {//������ ������
        LRCmodbus=0;//�������� LRC
        UkPaket=0;//��������� � �������, ������� �������� ������
        continue;//��������� ��������� ������ �������
        }
       
      //�������� �� ������� ���������
      if(!(
           ((Tmp>='0')&&(Tmp<='9'))||
           ((Tmp>='A')&&(Tmp<='F'))||
           (Tmp==ASCII_CR)||
           (Tmp==ASCII_LF)
           )) 
        {
        return;//������, ��������� ������ �� ���������
        }
        
      //��������� �������� ������
      if((UkPaket&1)==0)
        {//��������� �������� ������ ������ 0,2,4,6...
        Simvol0=Tmp; //��������� ������ ������ ������
        UkPaket++; //������������� ��������� ������
        continue;//��������� ��������� ������ 
        }
      else 
        {//��������� �������� ������ �������� 1,3,5,7...
        if(Tmp!=ASCII_LF)
          {//�� ��������� �����
          PaketRX[UkPaket>>1]=(Hex2Dig(Simvol0)<<4)|(Hex2Dig(Tmp));//��������� ���� ������ 
          LRCmodbus=LRCmodbus-PaketRX[UkPaket>>1];//������� LRC
          UkPaket++;//������������ ��������� ������
          if(UkPaket>(ModBusMaxPaketRX<<1))//�������� �� ������������
            {//����� ������ ������������
            UkPaket=0;//�������� ��������� ������
            return;//������, ��������� ������ �� ���������
            }
          }
        else break;//����� ������!
        }
      }      
    
    //�������� LCR
    if(LRCmodbus!=0) return;//������, ��������� ������ �� ���������
    
    //�������� ������
    if((PaketRX[0]!=ModBusID)&&(PaketRX[0]!=ModBusID_FF))
      {//�� ��� �����
      return;
      }
      
    //�������� � ��������� ��������
    Sost=!0;
    TimModbus=ModBusSysTimer;//��������� ������
#if ModBusMaxPauseResp!=0  
    return;//��������� ������ �� ���������
#endif  
    }  
  
  ///////////////////////////////////////////////////////////////////////////// 
  if(Sost!=0 
#if ModBusMaxPauseResp!=0     
     && (ModBusSysTimer-TimModbus)>=ModBusMaxPauseResp
#endif     
     )
    {//��������� �������� ������
    Sost=0;
    
#if ModBusUseFunc1!=0     
    //01 ������ ������� ������� 
    if(PaketRX[1]==0x01)
      {
      //���������� ������ ������������� ���
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ���������� ������������� ���
      unsigned short KolvoBit=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5]));
      
      //���� ������������ ����� � ����������
      if(KolvoBit>ModBusMaxOutBitTX || KolvoBit==0)
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }     
      if((AdresBit+KolvoBit)>ModBusMaxOutBit)
        {//������������ �����
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      
      Prg2ModBusOutBit();//���������� ��������� ������ (GlobalDate->ModBus)
      
      //������������ ������ ������
      ModBusPUT(':');
      //�����
      ModBusPUT(BCD[PaketRX[0]>>4]);//�������� ������� 
      ModBusPUT(BCD[PaketRX[0]&0x0F]);//�������� �������
      LRCmodbus=0-PaketRX[0];//������� LRC
      //��� �������    
      ModBusPUT(BCD[1>>4]);//�������� ������� 
      ModBusPUT(BCD[1&0x0F]);//�������� �������
      LRCmodbus=LRCmodbus-1;//������� LRC
      //���������� ������ ����
      ModBusPUT(BCD[((KolvoBit+7)>>3)>>4]);//�������� ������� 
      ModBusPUT(BCD[((KolvoBit+7)>>3)&0x0F]);//�������� �������
      LRCmodbus=LRCmodbus-((KolvoBit+7)>>3);//������� LRC
      //����������� ����� � ����� ������
      {
      unsigned char TxByte=0;//������� ����
      unsigned char Bit=AdresBit&7;//��������� ��� � ModBusOutBit[]
      AdresBit=AdresBit>>3;//��������� ���� ModBusOutBit[]
      
      //����������� �� �������� ModBusOutBit[] � �����
      {
      unsigned char i=0;
      while(!0)
        {
        if((ModBusOutBit[AdresBit].byte)&(1<<Bit))//���� ������� ��� ModBusOutBit[] ����� 1
          {//������������� ��� � ������
          TxByte=TxByte|(1<<(i&7));
          }
        //������������� ��������� 
        Bit++;
        if(Bit==8){Bit=0;AdresBit++;}
        i++;
        if((i&7)==0)
          {
          ModBusPUT(BCD[TxByte>>4]);//�������� ������� 
          ModBusPUT(BCD[TxByte&0x0F]);//�������� �������
          LRCmodbus=LRCmodbus-TxByte;//������� LRC
          TxByte=0;
          if(i==KolvoBit) break; else continue;
          }
        if(i==KolvoBit) 
          {
          ModBusPUT(BCD[TxByte>>4]);//�������� ������� 
          ModBusPUT(BCD[TxByte&0x0F]);//�������� �������
          LRCmodbus=LRCmodbus-TxByte;//������� LRC
          break;
          }
        }
      }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      //�����
      return;
      }
#endif
#if ModBusUseFunc2!=0     
    //02 Read Input Status 
    if(PaketRX[1]==0x02)
      {
      //���������� ������ ������������� ���
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ���������� ������������� ���
      unsigned short KolvoBit=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5]));
      
      //���� ������������ ����� � ����������
      if(KolvoBit>ModBusMaxInBitTX || KolvoBit==0)
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }
      if((AdresBit+KolvoBit)>ModBusMaxInBit)
        {//������������ �����
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
           
      Prg2ModBusInBit();//���������� ��������� ������ (GlobalDate->ModBus)
      
      //������������ ������ ������
      ModBusPUT(':');
      //�����
      ModBusPUT(BCD[PaketRX[0]>>4]);//�������� ������� 
      ModBusPUT(BCD[PaketRX[0]&0x0F]);//�������� �������
      LRCmodbus=0-PaketRX[0];//������� LRC
      //��� �������    
      ModBusPUT(BCD[2>>4]);//�������� ������� 
      ModBusPUT(BCD[2&0x0F]);//�������� �������
      LRCmodbus=LRCmodbus-2;//������� LRC
      //���������� ������ ����
      ModBusPUT(BCD[((KolvoBit+7)>>3)>>4]);//�������� ������� 
      ModBusPUT(BCD[((KolvoBit+7)>>3)&0x0F]);//�������� �������
      LRCmodbus=LRCmodbus-((KolvoBit+7)>>3);//������� LRC
      //����������� ����� � ����� ������
      {
      unsigned char TxByte=0;//������� ����
      unsigned char Bit=AdresBit&7;//��������� ��� � ModBusOutBit[]
      AdresBit=AdresBit>>3;//��������� ���� ModBusOutBit[]
      
      //����������� �� �������� ModBusOutBit[] � �����
      {
      unsigned char i=0;
      while(!0)
        {
        if((ModBusInBit[AdresBit].byte)&(1<<Bit))//���� ������� ��� ModBusOutBit[] ����� 1
          {//������������� ��� � ������
          TxByte=TxByte|(1<<(i&7));
          }
        //������������� ��������� 
        Bit++;
        if(Bit==8){Bit=0;AdresBit++;}
        i++;
        if((i&7)==0)
          {
          ModBusPUT(BCD[TxByte>>4]);//�������� ������� 
          ModBusPUT(BCD[TxByte&0x0F]);//�������� �������
          LRCmodbus=LRCmodbus-TxByte;//������� LRC
          TxByte=0;
          if(i==KolvoBit) break; else continue;
          }
        if(i==KolvoBit) 
          {
          ModBusPUT(BCD[TxByte>>4]);//�������� ������� 
          ModBusPUT(BCD[TxByte&0x0F]);//�������� �������
          LRCmodbus=LRCmodbus-TxByte;//������� LRC
          break;
          }
        }
      }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      //�����
      return;
      }
#endif
#if ModBusUseFunc3!=0     
    //03 Read Holding Registers 
    if(PaketRX[1]==0x03)
      {
      //���������� ������ ������������� ����
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ������ ���������� ������������� ����
      unsigned short KolvoWord=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])); 
      
      //���� ������������ ����� � ����������
      if(KolvoWord>ModBusMaxOutRegTX || KolvoWord==0)
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxOutReg)
        {//������������ ����� 
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      Prg2ModBusOutReg();//���������� ��������� ������ (GlobalDate->ModBus)
      
      //������������ ������ ������
      ModBusPUT(':');
      //�����
      ModBusPUT(BCD[PaketRX[0]>>4]);//�������� ������� 
      ModBusPUT(BCD[PaketRX[0]&0x0F]);//�������� �������
      LRCmodbus=0-PaketRX[0];//������� LRC
      //��� �������
      ModBusPUT(BCD[3>>4]);//�������� ������� 
      ModBusPUT(BCD[3&0x0F]);//�������� �������
      LRCmodbus=LRCmodbus-3;//������� LRC
      //���������� ������ ����
      ModBusPUT(BCD[(KolvoWord<<1)>>4]);//�������� ������� 
      ModBusPUT(BCD[(KolvoWord<<1)&0x0F]);//�������� �������
      LRCmodbus=LRCmodbus-(KolvoWord<<1);//������� LRC
      //����������� �� �������� ModBusOutReg[] � ����� ������
      {
      unsigned char i;
      for(i=0;i<KolvoWord;i++)
        {
        ModBusPUT(BCD[((ModBusOutReg[AdresWord+i])>>8)>>4]);//�������� ������� 
        ModBusPUT(BCD[((ModBusOutReg[AdresWord+i])>>8)&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-((ModBusOutReg[AdresWord+i])>>8);//������� LRC
        ModBusPUT(BCD[(((ModBusOutReg[AdresWord+i])>>0)>>4)&0x0F]);//�������� ������� 
        ModBusPUT(BCD[(((ModBusOutReg[AdresWord+i])>>0)>>0)&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-((ModBusOutReg[AdresWord+i])>>0);//������� LRC
        }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      //�����
      return;
      }
#endif
#if ModBusUseFunc4!=0     
    //04 Read Input Registers 
    if(PaketRX[1]==0x04)
      {
      //���������� ������ ������������� ����
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ������ ���������� ������������� ����
      unsigned short KolvoWord=((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])); 
      
      //���� ������������ ����� � ����������
      if(KolvoWord>ModBusMaxInRegTX || KolvoWord==0)
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxInReg)
        {//������������ ����� 
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
           
      Prg2ModBusInReg();//���������� ��������� ������ (GlobalDate->ModBus)
      
      //������������ ������ ������
      ModBusPUT(':');
      //�����
      ModBusPUT(BCD[PaketRX[0]>>4]);//�������� ������� 
      ModBusPUT(BCD[PaketRX[0]&0x0F]);//�������� �������
      LRCmodbus=0-PaketRX[0];//������� LRC
      //��� �������
      ModBusPUT(BCD[4>>4]);//�������� ������� 
      ModBusPUT(BCD[4&0x0F]);//�������� �������
      LRCmodbus=LRCmodbus-4;//������� LRC
      //���������� ������ ����
      ModBusPUT(BCD[(KolvoWord<<1)>>4]);//�������� ������� 
      ModBusPUT(BCD[(KolvoWord<<1)&0x0F]);//�������� �������
      LRCmodbus=LRCmodbus-(KolvoWord<<1);//������� LRC
      //����������� �� �������� ModBusOutReg[] � ����� ������
      {
      unsigned char i;
      for(i=0;i<KolvoWord;i++)
        {
        ModBusPUT(BCD[((ModBusInReg[AdresWord+i])>>8)>>4]);//�������� ������� 
        ModBusPUT(BCD[((ModBusInReg[AdresWord+i])>>8)&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-((ModBusInReg[AdresWord+i])>>8);//������� LRC
        ModBusPUT(BCD[(((ModBusInReg[AdresWord+i])>>0)>>4)&0x0F]);//�������� ������� 
        ModBusPUT(BCD[(((ModBusInReg[AdresWord+i])>>0)>>0)&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-((ModBusInReg[AdresWord+i])>>0);//������� LRC
        }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      //�����
      return;
      }
#endif
#if ModBusUseFunc5!=0     
    //05 Force Single Coil 
    if(PaketRX[1]==0x05)
      {
      //���������� ������ ������������� ������
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      
      //�������� �� ���������� �����  
      if(AdresBit>=ModBusMaxOutBit)
        {//���� ������������ �����
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
      //��������� ����� ����
      switch (((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])))
        {
        case 0xFF00:
        //��������� ����
        ModBusOutBit[(AdresBit>>3)].byte|=(1<<(AdresBit&7));
        break;
        case 0x0000:
        //����� ����
        ModBusOutBit[(AdresBit>>3)].byte&=(~(1<<(AdresBit&7)));
        break;
        default:
          {//������
#if ModBusUseErrMes!=0         
          MesErrASCII(ILLEGAL_DATA_VALUE);
#endif         
          return;
          } 
        }
      
      //�����
      ModBusPUT(':');
      {
      unsigned char i;
      for(i=0;i<7;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//�������� ������� 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//�������� �������
        }
      }
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
         
      ModBus2PrgOutBit();//���������� ��������� ������ (ModBus->GlobalDate)
      //�����
      return; 
      }
#endif
#if ModBusUseFunc6!=0     
    //06 Preset Single Register 
    if(PaketRX[1]==0x06)
      {
      //���������� ������ ������������� ��������
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      
      //�������� �� ���������� �����  
      if(AdresWord>=(ModBusMaxOutReg))
        {//���� ������������ �����
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
          
      //������ �����
      ModBusOutReg[AdresWord]=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //�����
      ModBusPUT(':');
      {
      unsigned char i;
      for(i=0;i<7;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//�������� ������� 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//�������� �������
        }
      }
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      
      ModBus2PrgOutReg();//���������� ��������� ������ (ModBus->GlobalDate)
        
      //�����
      return;
      }
#endif
#if ModBusUseFunc15!=0      
    //15 (0F Hex) Force Multiple Coils 
    if(PaketRX[1]==0x0F)
      {
      //���������� ������ ������������ ���
      unsigned short AdresBit=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ���������� ������������ ���
      unsigned short KolvoBit=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //���� ������������ ����� � ����������
      if(KolvoBit>ModBusMaxOutBitRX || KolvoBit==0 || ((KolvoBit+7)>>3)!=PaketRX[6])
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif        
        return;
        }     
      if((AdresBit+KolvoBit)>ModBusMaxOutBit)
        {//������������ �����
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
            
      //��������� �����
      {
      unsigned char Bit=(AdresBit&7);//��������� ��� � ModBusOutBit[]
      AdresBit=AdresBit>>3;//��������� ���� ModBusOutBit[]
      {
      unsigned char i;
      for(i=0;i<KolvoBit;i++)
        {
        if(PaketRX[7+(i>>3)]&(1<<(i&7)))//���� ������� ��� PaketRX ����� 1
          {//������������� ��� � ModBusOutBit[]
          ModBusOutBit[AdresBit].byte=(ModBusOutBit[AdresBit].byte)|((unsigned char)(1<<Bit));
          }
        else
          {//���������� ��� ModBusOutBit[]
          ModBusOutBit[AdresBit].byte=(ModBusOutBit[AdresBit].byte)&((unsigned char)(~(1<<Bit)));
          }
        //������������� ��������� 
        Bit++;if(Bit==8){Bit=0;AdresBit++;}
        }           
      }
      }
      //��������� LRC ������ �������� � ��������
      LRCmodbus=0;
      ModBusPUT(':');
      {
      unsigned char i;
      for(i=0;i<6;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//�������� ������� 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-PaketRX[i];//������� LRC
        }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      
      ModBus2PrgOutBit();//���������� ��������� ������ (ModBus->GlobalDate)
      //�����
      return;
      }
#endif
#if ModBusUseFunc16!=0        
    //16 (10 Hex) Preset Multiple Regs 
    if(PaketRX[1]==0x10)
      {
      //���������� ������ ������������ ����
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� ���������� ������������ ����
      unsigned short KolvoWord=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      
      //���� ������������ ����� � ����������
      if(KolvoWord>ModBusMaxOutRegRX || KolvoWord==0 || (KolvoWord<<1)!=PaketRX[6])
        {//������������ ����������
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_VALUE);
#endif 
        return;
        }
      if((AdresWord+KolvoWord)>ModBusMaxOutReg)
        {//������������ ����� 
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
            
      //����������� �� ������ � ������� ModBusOutReg[]
      {
      unsigned char i;
      for(i=0;i<KolvoWord;i++)
        {
        ModBusOutReg[AdresWord+i]=(((unsigned short)PaketRX[7+(i<<1)])<<8)|(PaketRX[8+(i<<1)]);
        }
      }
      //��������� LRC ������ �������� � ��������
      LRCmodbus=0;
      ModBusPUT(':');
      {
      unsigned char i;
      for(i=0;i<6;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//�������� ������� 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-PaketRX[i];//������� LRC
        }
      }
      ModBusPUT(BCD[LRCmodbus>>4]);
      ModBusPUT(BCD[LRCmodbus&0x0F]);
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      
      ModBus2PrgOutReg();//���������� ��������� ������ (ModBus->GlobalDate)
      //�����
      return; 
      }

#endif 
#if ModBusUseFunc22!=0     
    //22 (0x16) Mask Write 4X Register
    if(PaketRX[1]==0x16)
      {
      //���������� ������ ������������� ��������
      unsigned short AdresWord=(((((unsigned short)PaketRX[2])<<8)|(PaketRX[3])));
      //���������� �����
      unsigned short MaskAND=(((((unsigned short)PaketRX[4])<<8)|(PaketRX[5])));
      unsigned short MaskOR=(((((unsigned short)PaketRX[6])<<8)|(PaketRX[7])));
      
      //�������� �� ���������� �����  
      if(AdresWord>=(ModBusMaxOutReg))
        {//���� ������������ �����
#if ModBusUseErrMes!=0         
        MesErrASCII(ILLEGAL_DATA_ADDRESS);
#endif        
        return;
        }
          
      //����������� �����
      ModBusOutReg[AdresWord]=((ModBusOutReg[AdresWord])&MaskAND)|(MaskOR&(~MaskAND));
      
      //�����
      ModBusPUT(':');
      {
      unsigned char i;
      for(i=0;i<9;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//�������� ������� 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//�������� �������
        }
      }
      ModBusPUT(ASCII_CR);
      ModBusPUT(ASCII_LF);
      ModBus2PrgOutReg();//���������� ��������� ������ (ModBus->GlobalDate)
      //�����
      return;
      }
#endif    
    
    //������ �����
#if ModBusUseErrMes!=0    
    MesErrASCII(ILLEGAL_FUNCTION);//������, ������������� �������
#endif    
    return;
    } 
  return;
  }









