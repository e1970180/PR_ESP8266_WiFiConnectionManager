#pragma once

#include <stdint.h>             //used for Limits of integer types
#include <arduino.h>
	
uint32_t	PR_getTauMS(uint32_t lastCallMS); 		//period [ms] since lastCall 
uint32_t	PR_getTauS(uint32_t lastCallS); 		//period [s] since lastCall 


uint32_t	PR_getTauMS(uint32_t lastCallMS) { 		//period [ms] since lastCall 
	uint32_t tms;
	uint32_t t = millis();
	if (t < lastCallMS)  tms = (UINT32_MAX - lastCallMS + t); 
		else 		   tms = (t - lastCallMS);
	return tms;
}

uint32_t	PR_getTauS(uint32_t lastCallS) { 		//period [s] since lastCall 
	uint32_t ts;
	uint32_t t = millis()/1000;
	if (t < lastCallS)  ts = (UINT32_MAX/1000 - lastCallS + t); 
		else 		    ts = (t - lastCallS);
	return ts;
}

