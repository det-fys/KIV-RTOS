#pragma once

#include <drivers/bcm_aux.h>
#include <drivers/bridges/uart_defs.h>

#include "fs/filesystem.h"
#include "process/spinlock.h"

constexpr int CUART_BUF_SIZE = 8192;

using TUART_Rx_Callback = void (*)(void* userdata);

class CUART {
    private:
        // odkaz na AUX driver
        CAUX& mAUX;

        // byl UART kanal otevreny?
        bool mOpened;
        spinlock_t mOpenLock;

        // nastavena baud rate, ukladame ji proto, ze do registru se uklada (potencialne ztratovy) prepocet
        NUART_Baud_Rate mBaud_Rate;

        NUART_Blocking_State mBlocking_State;

        // cyklicky buffer pro cteni
        char mRx_Buf[CUART_BUF_SIZE];
        uint32_t mRx_Head = 0;
        uint32_t mRx_Tail = 0;
        spinlock_t mRx_Lock;

        // callback pro notifikaci pri prijeti dat
        TUART_Rx_Callback mRx_Callback = nullptr;
        void* mRx_Callback_Userdata = nullptr;

    public:
        CUART(CAUX& aux);

        // otevre UART kanal, exkluzivne
        bool Open();
        // uzavre UART kanal, uvolni ho pro ostatni
        void Close();
        // je UART kanal momentalne otevreny?
        bool Is_Opened() const;

        NUART_Char_Length Get_Char_Length();
        void Set_Char_Length(NUART_Char_Length len);

        NUART_Baud_Rate Get_Baud_Rate();
        void Set_Baud_Rate(NUART_Baud_Rate rate);

        // IRQ handler vola tuto rutinu po signalizaci IRQ
        void IRQ_Callback();

        // miniUART na RPi0 nepodporuje nic moc jineho uzitecneho, napr. paritni bity, vice stop-bitu nez 1, atd.

        void Write(char c);
        void Write(const char* str);
        void Write(const char* str, unsigned int len);
        void Write(unsigned int num);
        void Write_Hex(unsigned int num);

        // precist nejvyse len znaku z bufferu, pokud je prazdny, vrati 0
        int Read(char* str, unsigned int len);

        // nastavi callback pro notifikaci pri IRQ
        void Set_Rx_Callback(TUART_Rx_Callback callback, void* userdata = nullptr);
};

extern CUART sUART0;
