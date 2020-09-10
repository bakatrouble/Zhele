// Define target cpu frequence.
#define F_CPU 8000000

// !!!! For USART its important to define correct frequence values.
// For example, I use STM32F401cc for tests and it has 16MHz HSI value.
// So, I define HSI_VALUE 16000000 in main.cpp or (it's better) as build option in platformio.ini
//#define HSI_VALUE 16000000 // For my f401cc
//#define F_CPU 16000000 // For my f401cc

#include <iopins.h>
#include <usart.h>

using namespace Zhele;
using namespace Zhele::IO;

using UsartConnection = Usart1;
using Led = Pa7;

// This program enables LED and USART. After initial setings sends "Hello\r\n" to USART.
// Then program waits input data. If "on" has been received LED will be turn on and controller transmits "LED is turn on\r\n".
// If "off" has been received LED will be turn off and controller transmits "LED is turn off\r\n".
int main()
{
    Led::Port::Enable();
    Led::SetConfiguration(Led::Configuration::Out);
    Led::SetDriverType(Led::DriverType::PushPull);
    Led::Set();

    // Init usart1, baud = 9600
    UsartConnection::Init(9600);
    // Select pins
    UsartConnection::SelectTxRxPins<Pb6, Pb7>();
    // Or
    //UsartConnection::SelectTxRxPins<1, 1>();
    // Or
    //UsartConnection::SelectTxRxPins(1, 1);
    // Enable interrupt for receive
    UsartConnection::EnableInterrupt(UsartConnection::InterruptFlags::RxNotEmptyInt);
    // Write sync "Hello\r\n"
    UsartConnection::Write("Hello\r\n", 7);

    for (;;)
    {
    }
}

char Buffer[8];
int Size = 0;


extern "C"
{
    void USART1_IRQHandler()
    {
        // Check that usart is ready to read
        if(UsartConnection::ReadReady())
        {
            // Read next byte
            Buffer[Size++] = UsartConnection::Read();
        }

        // Clear interrupt flag
        UsartConnection::ClearInterruptFlag(UsartConnection::InterruptFlags::RxNotEmptyInt);

        // Check that we receive 2 bytes and it is "on"
        if(Size == 2 && Buffer[0] == 'o' && Buffer[1] == 'n')
        {
            // Turn on led
            Led::Set();
            // Send info to USART
            UsartConnection::Write("LED is turn on\r\n", 16);
            // Reset buffer
            Size = 0;
        }
        if(Size == 3 && Buffer[0] == 'o' && Buffer[1] == 'f' && Buffer[2] == 'f')
        {
            // Turn on led
            Led::Clear();
            // Send info to USART
            UsartConnection::Write("LED is turn off\r\n", 17);
            // Reset buffer
            Size = 0;
        }
    }
}