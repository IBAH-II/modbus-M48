# modbus-M48
Портирование "ModBus Slave RTU/ASCII без смс и регистрации" на IAR AVR v3, для микроконтроллера ATMega48
Демка подключения к панели Weintek
Для запуска демки:
        1) Прошейте Mega48 прошивкой Release/Exe/modbus.a90
        2) Подключите UART Mega48 через преобразователь к COM2 компьютера
        3) Установите EasyBuilder Pro v5 ftp://weintek:weintek@avtomatika.nnz.ru/Weintek/Software/EBpro_V50702105/EBproV50702105.zip
        4) Запустите в EasyBuilder Pro режим "Инструменты/Онлайн симуляция"