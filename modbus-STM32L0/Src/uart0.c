#define _UART0_C
#include "uart0.h"

//����������� ���������� 
//��� ������ ������
static unsigned char RxFifo[FifoRxBuf];//����� ��� ������ ������ 
#define RxHead (FifoRxBuf-((unsigned short)(DMA1_Channel6->CNDTR)))//������, �� ������ ������� DMA ����� �� ������
static unsigned short RxTail=0;//�����, ������ �������� ������ � ������

//����������� ���������� 
//��� �������� ������
static unsigned char TxFifo[FifoTxBuf];//����� ��� �������� ������ 
static unsigned short TxHead=0;//������, ���� ��� �������� ������ �� �������
static volatile unsigned short TxTail=0;//�����, � ������ � ����

//*****************************************************************************
//��������������� �����
void FifoDeInit0(void)
  {
  //��������� ���������� 
  NVIC_DisableIRQ(AES_RNG_LPUART1_IRQn);
  LL_LPUART_DisableIT_TC(LPUART1); 
  
  //���������� ����
  LL_LPUART_Disable(LPUART1);
  LL_LPUART_DeInit(LPUART1);
  
  //��������� ���� PB1_LPUART1_DE PB10_LPUART1_TX PB11_LPUART1_RX 
  LL_GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = (LL_GPIO_PIN_1)|(LL_GPIO_PIN_10)|(LL_GPIO_PIN_11);
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  //��������� DMA
  LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_6);
  LL_DMA_DeInit(DMA1, LL_DMA_CHANNEL_6);
  
  return;
  }

//*****************************************************************************
//������������� UART FIFO
//�������� �������� ������
void FifoInit0(unsigned long Speed)
  {
  //������������ 
  LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);
  //***************************************************************************
  //LPUART1 GPIO Configuration  
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  //PB1   ------> LPUART1_DE
  LL_GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  //PB10   ------> LPUART1_TX
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  //PB11   ------> LPUART1_RX 
  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  //***************************************************************************
  //DMA controller clock enable 
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  //LPUART1 DMA LPUART1_RX Init
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_6, LL_DMA_REQUEST_5);
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_6, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  
  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_6, LL_DMA_PRIORITY_VERYHIGH);
  
  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_6, LL_DMA_MODE_CIRCULAR);
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_6, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_6, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_6, LL_DMA_PDATAALIGN_BYTE);
  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_6, LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_6, FifoRxBuf);
  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_6, 
                         (unsigned long)(&(LPUART1->RDR)), 
                         (unsigned long)RxFifo, 
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_6);
  
  //***************************************************************************
  //UART Peripheral clock enable 
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPUART1);
  //UART init
  LL_LPUART_InitTypeDef LPUART_InitStruct;
  LPUART_InitStruct.BaudRate = Speed;
  LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
  LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
  LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
  LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
  LL_LPUART_EnableDEMode(LPUART1);
  LL_LPUART_SetDESignalPolarity(LPUART1, LL_LPUART_DE_POLARITY_HIGH);
  LL_LPUART_SetDEAssertionTime(LPUART1, 0);
  LL_LPUART_SetDEDeassertionTime(LPUART1, 0);
  LL_LPUART_DisableOverrunDetect(LPUART1);
  LL_LPUART_DisableDMADeactOnRxErr(LPUART1);
  
  //��������� UART_RX  � DMA
  LL_LPUART_EnableDMAReq_RX(LPUART1);
  
  //��������� ���������� "�������� ���������"
  //Enable the UART Transmission Complete Interrupt Enable 
  LL_LPUART_ClearFlag_TC(LPUART1);
  LL_LPUART_EnableIT_TC(LPUART1);  
  
  //�������� USART
  LL_LPUART_Enable(LPUART1);
  
  //�������� ���������
  RxTail=0;//�����, ������ �������� ������ � ������
  TxHead=0;//������, ���� ��� �������� ������ �� �������
  TxTail=0;//�����, � ������ � ����
    
  //USART interrupt Init
  NVIC_SetPriority(AES_RNG_LPUART1_IRQn, 0);
  NVIC_ClearPendingIRQ(AES_RNG_LPUART1_IRQn);
  NVIC_EnableIRQ(AES_RNG_LPUART1_IRQn);
  
  return;
  }

//*****************************************************************************
//���������� ���������� UART1
void AES_RNG_LPUART1_IRQHandler(void);
void AES_RNG_LPUART1_IRQHandler(void)
  {
  //���������� ���������� "�������� ���������" 
  if(LPUART1->ISR & USART_ISR_TC)  
    {
    //��������� ������� ���� � ������
    if(TxHead==TxTail)
      {//���������� ���� ����������
      LPUART1->ICR=USART_ICR_TCCF;
      }
    else
      {//������ ����, ��������, FIFO->UART
      LPUART1->TDR=TxFifo[TxTail];//� ������ � ����
      TxTail=(TxTail+1)&(FifoTxBuf-1);
      }
    }
  return;
  }

//*****************************************************************************
//Function:  - ����� ����� �� ����  
//Result:   �������� ���� 
unsigned char GetFifo0(void)
  {
  while(RxTail==RxHead);
  unsigned char RetData=RxFifo[RxTail];//������ � ������
  RxTail=(RxTail+1)&(FifoRxBuf-1);
  return RetData;  
  }

//������� ��������� ������
void FifoClear0(void)
  {
  RxTail=RxHead;
  }

//Function: ����� ����� ��� ��������                                
//Result:   �������� ����, ���� ���� �� ������ �� ������� 
unsigned char InkeyFifo0(void)
  {
  if(RxTail==RxHead)return 0;//��� ������
  LPUART1->ICR=USART_ICR_NCF|USART_ICR_FECF;//�������� ����� ������
  unsigned char RetData=RxFifo[RxTail];//������ � ������
  RxTail=(RxTail+1)&(FifoRxBuf-1);
  return RetData;
  }

//Function: ����� ����� ��� ��������                                
//Result:���� ���� �������� ������  
//������� ���� �� ����
//������� ���� �������� ����
//���� ������ �� �������, �� ���� 
unsigned short Inkey16Fifo0(void)
  {
  if(RxTail==RxHead)return 0;//��� ������
  LPUART1->ICR=USART_ICR_NCF|USART_ICR_FECF;//�������� ����� ������
  unsigned char RetData=RxFifo[RxTail];//������ � ������
  RxTail=(RxTail+1)&(FifoRxBuf-1);
  return 0x0100|RetData;
  }

//Function: �������� ���� � ���� 
//Argument: ������������ ����   
void PutFifo0(unsigned char data)
  {
  //���� � TX������ ����� � ������� �������� ���� 
  if(
     (TxHead==TxTail)
     &&
     (((LPUART1->ISR)&USART_ISR_TXE)!=0)
    )
    {//����� ��������� ��������
    LPUART1->TDR=data;
    return;
    }
  //���� � ������ �� ����� ������ � Tx �����
  TxFifo[TxHead]=data;//������ �� ������
  TxHead=(TxHead+1)&(FifoTxBuf-1);
  return;
  }

//Function: ����� ������ � Fifo �� FLASH                                
//Result: 
void PutStrFifoF0(unsigned char const* a)
  {
  unsigned short i=0;
  while(a[i]!=0)
    {
    PutFifo0(a[i]);
    i++;
    if(i==FifoTxBuf)return;
    }  
  }

//Function: USART - ����� ������ �� RAM                                
void PutStrFifoM0(unsigned char* a)
  {
  char i=0;
  while(a[i]!=0)
    {
    PutFifo0(a[i]);
    i++;
    if(i==255)return;
    }
  }

//����� � ���������� ���� �� ������
void PutDec8sFifo0(signed char a)
  {
  long b;
  if(a<0) {PutFifo0('-');a=-a;}
  else PutFifo0('+'); 
  for(b=100;b>0;b=b/10)
    {
    PutFifo0((a/b)+'0');
    a=a%b;
    }
  }
//����� � ���������� ���� �� ������
void PutDec16sFifo0(signed short a)
  {
  long b;
  if(a<0) {PutFifo0('-');a=-a;}
  else PutFifo0('+'); 
for(b=10000;b>0;b=b/10)
  {
  PutFifo0((a/b)+'0');
  a=a%b;
  }
  }
//����� � ���������� ���� �� ������
void PutDec32sFifo0(signed long a)
  {
  long b;
  if(a<0) {PutFifo0('-');a=-a;}
  else PutFifo0('+'); 
  for(b=1000000000;b>0;b=b/10)
    {
    PutFifo0((a/b)+'0');
    a=a%b;
    }
  }
//����� � ���������� ���� �� ������ �� �������, N ���������� ������
void PutDecNsFifo0(char N,signed long a)
  {
  long b,c=9;
  if(N==0) return;
  if(N>10)N=10;
  if(a<0) {PutFifo0('-');a=-a;}else PutFifo0('+'); 
  //    4294967296
  for(b=1000000000;b>0;b=b/10)
    {
    if(c<N)PutFifo0((a/b)+'0');
    a=a%b;
    c--;
    }
  }
//����� � ���������� ���� ��� �����
void PutDec8uFifo0(unsigned char a)
  {
  long b;
  for(b=100;b>0;b=b/10)
    {
    PutFifo0((a/b)+'0');
    a=a%b;
    }
  }
//����� � ���������� ���� ��� �����
void PutDec16uFifo0(unsigned short a)
  {
  long b;
  for(b=10000;b>0;b=b/10)
    {
    PutFifo0((a/b)+'0');
    a=a%b;
    }
  }
//����� � ���������� ���� ��� �����
void PutDec32uFifo0(unsigned long a)
  {
  long b;
  //    4294967296
  for(b=1000000000;b>0;b=b/10)
    {
    PutFifo0((a/b)+'0');
    a=a%b;
    }
  }
//����� � ���������� ���� ��� ����� �� �������, N ���������� ������
void PutDecNuFifo0(char N,unsigned long a)
  {
  long b,c=9;
  if(N==0) return;
  if(N>10)N=10;
  //    4294967296
  for(b=1000000000;b>0;b=b/10)
    {
    if(c<N)PutFifo0((a/b)+'0');
    a=a%b;
    c--;
    }
  }
//����� � ���������� ���� ��� ����� ��� �������� �����
void PutDecuFifo0(unsigned long a)
  {
  if(a==0){PutFifo0('0');return;}
  if(a<10){PutDecNuFifo0(1,a);return;}
  if(a<100){PutDecNuFifo0(2,a);return;}
  if(a<1000){PutDecNuFifo0(3,a);return;}
  if(a<10000){PutDecNuFifo0(4,a);return;}
  if(a<100000){PutDecNuFifo0(5,a);return;}
  if(a<1000000){PutDecNuFifo0(6,a);return;}
  if(a<10000000){PutDecNuFifo0(7,a);return;}
  if(a<100000000){PutDecNuFifo0(8,a);return;}
  if(a<1000000000){PutDecNuFifo0(9,a);return;}
  PutDec32uFifo0(a);     
  }
//����� � ���������� ���� �� ������ ��� �������� �����
void PutDecsFifo0(signed long a)
  {
  if(a<0){a=-a;PutFifo0('-');}
  PutDecuFifo0(a);
  }

//����� � ���������������� ����
void PutHex8Fifo0(unsigned char a)
  {
  static const char hex[]="0123456789ABCDEF";
  PutFifo0(hex[a>>4]);
  PutFifo0(hex[a&0x0F]);
  }
//����� � ���������������� ����
void PutHex16Fifo0(unsigned short a)
  {
  PutHex8Fifo0(a>>8);
  PutHex8Fifo0(a);
  }
//����� � ���������������� ����
void PutHex32Fifo0(unsigned long a)
  {
  PutHex8Fifo0(a>>24);
  PutHex8Fifo0(a>>16);
  PutHex8Fifo0(a>>8);
  PutHex8Fifo0(a);
  }
//����� � ���������������� ����
void PutHex64Fifo0(unsigned long long a)
  {
  PutHex8Fifo0(a>>56);
  PutHex8Fifo0(a>>48);
  PutHex8Fifo0(a>>40);
  PutHex8Fifo0(a>>32);
  PutHex8Fifo0(a>>24);
  PutHex8Fifo0(a>>16);
  PutHex8Fifo0(a>>8);
  PutHex8Fifo0(a);
  }

//����� � �������� ����
void PutBin8Fifo0(unsigned char a)
  {
  long i;
  for(i=0;i<8;i++)
    {
    if((a&0x80)!=0)PutFifo0('1');else PutFifo0('0');
    a=a<<1;
    }
  }

//����� � �������� ����
void PutBin16Fifo0(unsigned short a)
  {
  PutBin8Fifo0(a>>8);
  PutBin8Fifo0(a);
  }

//����� � �������� ����
void PutBin32Fifo0(unsigned long a)
  {
  PutBin8Fifo0(a>>24);
  PutBin8Fifo0(a>>16);
  PutBin8Fifo0(a>>8);
  PutBin8Fifo0(a);
  }

//����� � �������� ����
void PutBin64Fifo0(unsigned long long a)
  {
  PutBin8Fifo0(a>>56);
  PutBin8Fifo0(a>>48);
  PutBin8Fifo0(a>>40);
  PutBin8Fifo0(a>>32);
  PutBin8Fifo0(a>>24);
  PutBin8Fifo0(a>>16);
  PutBin8Fifo0(a>>8);
  PutBin8Fifo0(a);
  }

//����� � �������� ���� ������� ������
void PutBinRev8Fifo0(unsigned char a)
  {
  long i;
  for(i=0;i<8;i++)
    {
    if((a&0x01)!=0)PutFifo0('1');else PutFifo0('0');
    a=a>>1;
    }
  }
//����� � �������� ���� ������� ������
void PutBinRev16Fifo0(unsigned short a)
  {
  PutBinRev8Fifo0(a);
  PutBinRev8Fifo0(a>>8);
  }
//����� � �������� ���� ������� ������
void PutBinRev32Fifo0(unsigned long a)
  {
  PutBinRev8Fifo0(a);
  PutBinRev8Fifo0(a>>8);
  PutBinRev8Fifo0(a>>16);
  PutBinRev8Fifo0(a>>24);
  }

//����� Float �� �������
//���������
//unsigned char pered ����� ������ ����� �������
//unsigned char posle ����� ������ ����� �������
static inline unsigned long st10(unsigned char x)
  {unsigned long a=1;for(unsigned char n=0;n<x;n++){a=a*10;}return a;}
void PutFloatFifo0(unsigned char pered,unsigned char posle,unsigned char znak,float a)
  {
  //fix
  if(pered>9)pered=9;
  if(posle>9)posle=9;
  //��������� ���� 
  if(a<0)
    {
    a=-a;
    PutFifo0('-');
    }
  else 
    {
    PutFifo0('+');
    }
  //�������� ����� �����
  unsigned long a1=(unsigned long)a;
  //�������� ������� �����
  unsigned long a2=((unsigned long)((a-a1)*st10(posle)));
  //������ ����� �����
  long b,c=9;
  if(pered!=0)
    {
    //    4294967296
    for(b=1000000000;b>0;b=b/10)
      {
      if(c<pered)PutFifo0((a1/b)+'0');
      a1=a1%b;
      c--;
      }
    } 
  //�����
  PutFifo0(znak);
  //������ ������� �����
  c=9;
  if(posle!=0)
    {
    //    4294967296
    for(b=1000000000;b>0;b=b/10)
      {
      if(c<posle)PutFifo0((a2/b)+'0');
      a2=a2%b;
      c--;
      }
    }
  }

//����� double �� �������
//���������
//unsigned char pered ����� ������ ����� �������
//unsigned char posle ����� ������ ����� �������
static inline unsigned long long st10double(unsigned char x)
  {unsigned long long a=1;for(unsigned char n=0;n<x;n++){a=a*10;}return a;}
void PutDoubleFifo0(unsigned char pered,unsigned char posle,unsigned char znak,double a)
  {
  //fix
  if(pered>16)pered=16;
  if(posle>16)posle=16;
  //��������� ���� 
  if(a<0)
    {
    a=-a;
    PutFifo0('-');
    }
  else 
    {
    PutFifo0('+');
    }
  //�������� ����� �����
  unsigned long long a1=(unsigned long long)a;
  //�������� ������� �����
  unsigned long long a2=((unsigned long long)((a-a1)*st10double(posle)));
  //������ ����� �����
  unsigned long long b,c=19;
  if(pered!=0)
    {
    //    18446744073709551616
    for(b=10000000000000000000U;b>0;b=b/10)
      {
      if(c<pered)PutFifo0((a1/b)+'0');
      a1=a1%b;
      c--;
      }
    } 
  //�����
  PutFifo0(znak);
  //������ ������� �����
  c=19;
  if(posle!=0)
    {
    //    18446744073709551616
    for(b=10000000000000000000U;b>0;b=b/10)
      {
      if(c<posle)PutFifo0((a2/b)+'0');
      a2=a2%b;
      c--;
      }
    }
  }


