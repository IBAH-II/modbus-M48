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
static const unsigned short Tabl[256]=
  {
  0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
  0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
  0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
  0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
  0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
  0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
  0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
  0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
  0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
  0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
  0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
  0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
  0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
  0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
  0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
  0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
  0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
  0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
  0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
  0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
  0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
  0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
  0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
  0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
  0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
  0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
  0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
  0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
  0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
  0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
  0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
  0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
  };
static inline unsigned short CRCfunc(unsigned short inCRC, unsigned char in)
  {
  return (inCRC>>8)^Tabl[(inCRC^in)&0xFF];
  }
#else
static unsigned short CRCfunc(unsigned short inCRC, unsigned char in)
  {
  inCRC=inCRC^in;
  for(int i=0;i<8;i++){if(inCRC&1){inCRC=(inCRC>>1)^0xA001U;}else {inCRC=inCRC>>1;}}
  return inCRC;
  }
#endif

//������� ��������� ������
#if ModBusUseErrMes!=0
#define ILLEGAL_FUNCTION (1) //�������� ��� ������� �� ����� ���� ��������� �� �����������. 
#define ILLEGAL_DATA_ADDRESS (2) //����� ������ ��������� � ������� �� �������� ������� ������������.
#define ILLEGAL_DATA_VALUE (3) //��������, ������������ � ���� ������ ������� �������� �� ���������� ��������� ��� ������������.
static inline void MesErrRTU(unsigned char Err)
  {
  //�����
  ModBusPUT(PaketRX[0]);
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
      unsigned short TmpCRC=CRCfunc(0xFFFF,PaketRX[0]);
      //��� �������    
      ModBusPUT(1);
      TmpCRC=CRCfunc(TmpCRC,1);
      //���������� ������ ����
      ModBusPUT((KolvoBit+7)>>3);
      TmpCRC=CRCfunc(TmpCRC,((KolvoBit+7)>>3));
      
      //����������� ����� � ����� ������
      unsigned char TxByte=0;//������� ����
      unsigned char Bit=AdresBit&7;//��������� ��� � ModBusOutBit[]
      AdresBit=AdresBit>>3;//��������� ���� ModBusOutBit[]
      //����������� �� �������� ModBusOutBit[] � �����
      int i=0;
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
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      
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
      unsigned short TmpCRC=CRCfunc(0xFFFF,PaketRX[0]);
      //��� �������    
      ModBusPUT(2);
      TmpCRC=CRCfunc(TmpCRC,2);
      //���������� ������ ����
      ModBusPUT((KolvoBit+7)>>3);
      TmpCRC=CRCfunc(TmpCRC,((KolvoBit+7)>>3));
      //����������� ����� � ����� ������
      unsigned char TxByte=0;//������� ����
      unsigned char Bit=AdresBit&7;//��������� ��� 
      AdresBit=AdresBit>>3;//��������� ���� 
      //����������� �� �������� ModBusInBit[] � �����
      int i=0;
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
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      
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
      unsigned short TmpCRC=CRCfunc(0xFFFF,PaketRX[0]);
      //��� �������    
      ModBusPUT(3);
      TmpCRC=CRCfunc(TmpCRC,3);
      //���������� ������ ����
      ModBusPUT(KolvoWord<<1);
      TmpCRC=CRCfunc(TmpCRC,(KolvoWord<<1));
      //����������� �� �������� ModBusOutReg[] � ����� ������
      for(int i=0;i<KolvoWord;i++)
        {
        ModBusPUT(ModBusOutReg[AdresWord+i]>>8);
        TmpCRC=CRCfunc(TmpCRC,(ModBusOutReg[AdresWord+i]>>8));
        ModBusPUT(ModBusOutReg[AdresWord+i]>>0);
        TmpCRC=CRCfunc(TmpCRC,(ModBusOutReg[AdresWord+i]>>0));
        }
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      
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
      unsigned short TmpCRC=CRCfunc(0xFFFF,(PaketRX[0]));
      //��� �������    
      ModBusPUT(4);
      TmpCRC=CRCfunc(TmpCRC,4);
      //���������� ������ ����
      ModBusPUT(KolvoWord<<1);
      TmpCRC=CRCfunc(TmpCRC,(KolvoWord<<1));
      //����������� �� �������� ModBusInReg[] � ����� ������
      for(int i=0;i<KolvoWord;i++)
        {
        ModBusPUT(ModBusInReg[AdresWord+i]>>8);
        TmpCRC=CRCfunc(TmpCRC,(ModBusInReg[AdresWord+i]>>8));
        ModBusPUT(ModBusInReg[AdresWord+i]>>0);
        TmpCRC=CRCfunc(TmpCRC,(ModBusInReg[AdresWord+i]>>0));
        }
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      
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
      for(int i=0;i<8;i++) ModBusPUT(PaketRX[i]);
      
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
      for(int i=0;i<8;i++) ModBusPUT(PaketRX[i]);
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
      unsigned char Bit=(AdresBit&7);//��������� ��� � ModBusOutBit[]
      AdresBit=AdresBit>>3;//��������� ���� ModBusOutBit[]
      //���� �� �����
      for(int i=0;i<KolvoBit;i++)
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
      
      //������������ ������ ������
      unsigned short TmpCRC=0xFFFF;
      for(int i=0;i<6;i++)
        {
        ModBusPUT(PaketRX[i]);
        TmpCRC=CRCfunc(TmpCRC,(PaketRX[i]));
        }
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      
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
      for(int i=0;i<KolvoWord;i++)
        {
        ModBusOutReg[AdresWord+i]=(((unsigned short)PaketRX[7+(i<<1)])<<8)|(PaketRX[8+(i<<1)]);
        }
      
      //������������ ������ ������
      unsigned short TmpCRC=0xFFFF;
      for(int i=0;i<6;i++)
        {
        ModBusPUT(PaketRX[i]);
        TmpCRC=CRCfunc(TmpCRC,(PaketRX[i]));
        }
      ModBusPUT(TmpCRC);
      ModBusPUT(TmpCRC>>8);
      
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
      for(int i=0;i<10;i++) ModBusPUT(PaketRX[i]);
      
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
static inline unsigned char Hex2Dig(unsigned char h)
  {
  if((h>='0')&&(h<='9')) return (h -'0');
  if((h>='A')&&(h<='F')) return (h -'A'+10);
  return 0;
  }
static unsigned char LRCmodbus;//������� LRC
static unsigned char Simvol0;//���������� �������� ������
#define ASCII_CR (0x0D)//������� ������� 
#define ASCII_LF (0x0A)//������� ������
static const unsigned char BCD[]="0123456789ABCDEF";//������ ��� ����������� ����� � ������
#if ModBusUseErrMes!=0 
static inline void MesErrASCII(unsigned char Err)
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
      unsigned char TxByte=0;//������� ����
      unsigned char Bit=AdresBit&7;//��������� ��� � ModBusOutBit[]
      AdresBit=AdresBit>>3;//��������� ���� ModBusOutBit[]
      
      //����������� �� �������� ModBusOutBit[] � �����
      int i=0;
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
      unsigned char TxByte=0;//������� ����
      unsigned char Bit=AdresBit&7;//��������� ��� � ModBusOutBit[]
      AdresBit=AdresBit>>3;//��������� ���� ModBusOutBit[]
      
      //����������� �� �������� ModBusOutBit[] � �����
      int i=0;
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
      for(int i=0;i<KolvoWord;i++)
        {
        ModBusPUT(BCD[((ModBusOutReg[AdresWord+i])>>8)>>4]);//�������� ������� 
        ModBusPUT(BCD[((ModBusOutReg[AdresWord+i])>>8)&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-((ModBusOutReg[AdresWord+i])>>8);//������� LRC
        ModBusPUT(BCD[(((ModBusOutReg[AdresWord+i])>>0)>>4)&0x0F]);//�������� ������� 
        ModBusPUT(BCD[(((ModBusOutReg[AdresWord+i])>>0)>>0)&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-((ModBusOutReg[AdresWord+i])>>0);//������� LRC
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
      for(int i=0;i<KolvoWord;i++)
        {
        ModBusPUT(BCD[((ModBusInReg[AdresWord+i])>>8)>>4]);//�������� ������� 
        ModBusPUT(BCD[((ModBusInReg[AdresWord+i])>>8)&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-((ModBusInReg[AdresWord+i])>>8);//������� LRC
        ModBusPUT(BCD[(((ModBusInReg[AdresWord+i])>>0)>>4)&0x0F]);//�������� ������� 
        ModBusPUT(BCD[(((ModBusInReg[AdresWord+i])>>0)>>0)&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-((ModBusInReg[AdresWord+i])>>0);//������� LRC
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
      for(int i=0;i<7;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//�������� ������� 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//�������� �������
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
      for(int i=0;i<7;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//�������� ������� 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//�������� �������
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
      unsigned char Bit=(AdresBit&7);//��������� ��� � ModBusOutBit[]
      AdresBit=AdresBit>>3;//��������� ���� ModBusOutBit[]
      for(int i=0;i<KolvoBit;i++)
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
      
      //��������� LRC ������ �������� � ��������
      LRCmodbus=0;
      ModBusPUT(':');
      for(int i=0;i<6;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//�������� ������� 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-PaketRX[i];//������� LRC
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
      for(int i=0;i<KolvoWord;i++)
        {
        ModBusOutReg[AdresWord+i]=(((unsigned short)PaketRX[7+(i<<1)])<<8)|(PaketRX[8+(i<<1)]);
        }
      //��������� LRC ������ �������� � ��������
      LRCmodbus=0;
      ModBusPUT(':');
      for(int i=0;i<6;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//�������� ������� 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//�������� �������
        LRCmodbus=LRCmodbus-PaketRX[i];//������� LRC
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
      for(int i=0;i<9;i++)
        {
        ModBusPUT(BCD[PaketRX[i]>>4]);//�������� ������� 
        ModBusPUT(BCD[PaketRX[i]&0x0F]);//�������� �������
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









