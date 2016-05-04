#ifndef WDT_H_
#define WDT_H_
#include "avr/io.h"
#include "avr/wdt.h"

// wait until WDT register has been written
#define WDT_IS_SYNC_BUSY WDT.STATUS & WDT_SYNCBUSY_bm

#define WDT_RESET() __asm__ __volatile__("wdr") //( watchdog_reset( ) )

void WDT_enable_with_timeout(WDT_PER_t p);
void WDT_disable();


#endif
