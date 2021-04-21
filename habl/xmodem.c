/*
xmodem.c

For specification see:
http://www.atmel.com/dyn/resources/prod_documents/doc1472.pdf
http://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_id=200&item_type=project


Created using code found on internet written by Lukas Karrer and others.



Change history:


2007-03-15
Adapted for ATMEGA88.
/Henrik Bjorkman

2007-06-27
Made a correction suggested by Jerry Mulchin at
JWM Engineering Group, Inc.
Shall be able to load larger files now.


2012-05-14
Some comments added.
Henrik Bjorkman

*/


#include <stdio.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/boot.h>
#include <setjmp.h>
#include "avr_cfg.h"
#include "avr_uart.h"


#define XMODEM_BUFFER_SIZE 128

#define XMODEM_SOH 0x01 // Start of header
#define XMODEM_EOT 0x04 // End of transmission
#define XMODEM_ACK 0x06 // Acknowledge
#define XMODEM_NACK 0x15 // Not acknowledge
#define XMODEM_RRC 'C' // Ready to receive with CRC


#if XMODEM_BUFFER_SIZE < SPM_PAGESIZE
#error
#endif


unsigned char xPacketNumber=1;  // alias blk
unsigned char xDataBuffer[XMODEM_BUFFER_SIZE];
unsigned int xDataCounter=0; // How much data there is in data buffer

unsigned int programMemoryPtr=0; // How much data written to program flash memory
unsigned char programBuffer[SPM_PAGESIZE];
unsigned int programBufferCounter=0; // How much data there is in programBuffer

int ch;
int i;

#ifndef IGNORE_CSUM
#ifdef USE_XMODEM_CRC16
// This calculates a crc16 checksum
inline int xmodem_csum(unsigned char *ptr, int count)
{
  int crc=0;
  while (--count >= 0)
  {
    crc = crc ^ (int) *ptr++ << 8;
    char i = 8;
    do
    {
      if (crc & 0x8000)
      {
        crc = crc << 1 ^ 0x1021;
      }
      else
      {
        crc = crc << 1;
      }
    } while(--i);
  }
  return (crc);
}
#else
// This calculates a simple 8 bit checksum
inline int xmodem_csum(unsigned char *ptr, int count)
{
  unsigned char csum=0;
  while (--count >= 0)
  {
    csum+=*ptr++;
  }
  return (csum);
}
#endif
#endif


// Wait for input, wait roughly a second. Return -1 if timeout
int waitGetChar(void)
{
  long int w=0;

  while(w<1000000)
  {
    const int ch = uart_getchar();
    if (ch>=0)
    {
      // something was received
      return ch;
    }
    ++w;
  }


  // Nothing was received
  return -1;
}


// Wait for sender to be quiet and then send NACK
/*
inline void sendNACK(void)
{
  // wait for sender to be quiet for roughly a second.
  while(waitGetChar()>=0)
  {
      // something was received
  }

  // send the NACK
  uart_putchar(XMODEM_NACK);
}
*/

// Returns: 0 if OK,  -1 if not OK
inline char programFlash(unsigned char *ptr, int count)
{

  // Copy data to programBuffer 
  while(count>0)
  {
    programBuffer[programBufferCounter]=*ptr;
    ++programBufferCounter;
    ++ptr;
    --count;

    // When programBuffer is full write to flash memory
    if(programBufferCounter>=SPM_PAGESIZE)
    {
      // We have a full programBuffer

      // Is there room in flash?
      if (programMemoryPtr<=(PROGRAM_MEMORY_SIZE-SPM_PAGESIZE))
      {
        // yes, erase next page in flash memory
        boot_page_erase(programMemoryPtr); 
        while(boot_rww_busy())
        {
          boot_rww_enable();
        }

        // write next page to flash memory
        for(i=0;i<SPM_PAGESIZE;i+=2)
        {
          boot_page_fill(programMemoryPtr%SPM_PAGESIZE,programBuffer[i]+(programBuffer[i+1]<<8));
          programMemoryPtr+=2;
        }  
        boot_page_write(programMemoryPtr-1);
        while(boot_rww_busy())
        {
          boot_rww_enable();
        }
      }
      else
      {
        // Memory is full! Program to big.
	return -1;
      }
      programBufferCounter=0;
    }
  }
  return 0;  
}






int xmodem(void)
{

  // Wait for start of header, send 'C' roughly once per second.
  while(waitGetChar()!=XMODEM_SOH)
  {
      uart_putchar(XMODEM_RRC);
  }


  // Loop while receiving packets
  for(;;)
  {
    if(xPacketNumber==(unsigned char)waitGetChar())
    {
      //if(((unsigned char)(255-xPacketNumber))==(unsigned char)(waitGetChar()))
      if(xPacketNumber==(unsigned char)(~waitGetChar()))
      {
        xDataCounter=0;
        for(;;)
        {
          ch = waitGetChar();
          if (ch>=0)
          {
            xDataBuffer[xDataCounter]=(unsigned char)ch;  
            ++xDataCounter;

            // Is all packet data received?
            if (xDataCounter==128)
            {
              // It is, now we expect checksum, but is it 1 or 2 bytes?
              // Recive checksum (1 or 2 bytes)
              #ifndef  USE_XMODEM_CRC16
                unsigned int csum=waitGetChar();
              #else
                unsigned int csum=(waitGetChar()<<8);
                csum+=waitGetChar();
              #endif

              // is checksum or CRC OK
              #ifndef IGNORE_CSUM
              if(xmodem_csum(xDataBuffer, 128) == csum)
              #endif
              {
                // Checksum is OK (or it was not checked)

                if (programFlash(xDataBuffer, 128)!=0)
		{
	          uart_putchar(XMODEM_NACK);
		}
		else
		{
                  uart_putchar(XMODEM_ACK); // now ready for next packet, send ack.
		}
                ++xPacketNumber;
              }
              break; // this packet is done (or failed), go and wait for next packet
            }
          }
          else
          {
            // timeout waiting for data. Give up this packet, sender will hopefully retry sending it.
            break;
          }
        }
      }
    }

    // Loop waiting for XMODEM_SOH or XMODEM_EOT, ignoring other data.
    for(;;)
    {
      ch=waitGetChar();
      if (ch==XMODEM_SOH)
      {
        // More packets shall be received
        break;
      }
      else if (ch==XMODEM_EOT)
      {
        // sender sent EOT, we are done

        #if ((SPM_PAGESIZE!=16) && (SPM_PAGESIZE!=32) && (SPM_PAGESIZE!=64) && (SPM_PAGESIZE!=128))
          // This code is only needed if SPM_PAGESIZE is not 16, 32, 64 or 128. Only then can we have data left in xDataBuffer. 
          // If SPM_PAGESIZE>256 this might not work very well.
          if (programBufferCounter>0)
          {    
            for(xDataCounter=0;xDataCounter<128;xDataCounter++)
            {
              xDataBuffer[xDataCounter]=0;
            }
            if (programFlash(xDataBuffer, SPM_PAGESIZE-programBufferCounter)!=0)
            {
              uart_putchar(XMODEM_NACK);
            }
          }
        #endif

        uart_putchar(XMODEM_ACK);

        // test code, to be removed later. Send 'x' until user press 'x'.
        /*while(waitGetChar()!='x')
        {
          uart_putchar('x');
        }*/

        return 0;
      }
      else
      {
        // wait for sender to be quiet for roughly a second.
        while(waitGetChar()>=0)
        {
          // something was received
        }

        // send the NACK
        uart_putchar(XMODEM_NACK);
      }
    }
  }
}



