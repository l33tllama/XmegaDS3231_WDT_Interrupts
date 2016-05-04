/* Very basic WDT timer library
 *
 * Used mainly for the purposes of the GPS tracker
 */
#include "WDT.h"

void WDT_enable_with_timeout(WDT_PER_t p){
	// WDT settings - enable it, write 1 to CEN to say the register needs to be changed, and write the period
	uint8_t wdt_config = WDT_ENABLE_bm | WDT_CEN_bm | p;
	// disable config change protection (IO registers)
	CCP = CCP_IOREG_gc;
	// actually write to the WDT Control register
	WDT.CTRL = wdt_config;
	// wait until WDT register has been written
	while(WDT_IS_SYNC_BUSY) {}
}

void WDT_disable(){
	// WDT settings - same as before but NOT WDT_ENABLE (disable wdt) or'd with change enable bit
	uint8_t wdt_config = (WDT.CTRL & ~WDT_ENABLE_bm) | WDT_CEN_bm;

	// disable config change protection
	CCP = CCP_IOREG_gc;

	// set WDT values
	WDT.CTRL = wdt_config;

	// wait for WDT.CTRL to finish writing
	while(WDT_IS_SYNC_BUSY) {}
}
