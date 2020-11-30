Портирование "ModBus Slave RTU/ASCII без смс и регистрации" на IAR AVR v3, 
для микроконтроллера ATMega48.

Портирование "ModBus Slave RTU/ASCII без смс и регистрации" на IAR ARM, 
для микроконтроллера STM32L052.

Тесты производительности.

Демка подключения к панели Weintek.
Для запуска демки:
  1) Прошейте Mega48(STM32L052) прошивкой modbus-ATMega48/Release/Exe/modbus.a90 
     (modbus-STM32L0/EWARM/Stend/Exe/Stend.hex)
  2) Подключите UART Mega48(STM32L052) через преобразователь к COM2 компьютера
  3) Установите EasyBuilder Pro v5      ftp://weintek:weintek@avtomatika.nnz.ru/Weintek/Software/EBpro_V50702105/EBproV50702105.zip
  4) Запустите в EasyBuilder Pro режим "Инструменты/Онлайн симуляция"