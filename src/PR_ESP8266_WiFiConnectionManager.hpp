#pragma once

#include <arduino.h>
//#include <string.h>
#include <Esp.h>                    // core
#include <memory>

#include <EEPROM.h>

extern "C" {
  #include "user_interface.h"
}

#include <ESP8266WiFi.h>            //https://github.com/esp8266/Arduino

#include "PR_GetTau.hpp"                         

#define		WIFI_TIME_TO_CONNECT_FAILED		20	//[s]

	#define	STA_SSID_PWD_MAX_LEN				31
	#define	EEPROM_STA_CREDITALS_ADDR			0
	#define	EEPROM_STA_CREDITALS_SALT			12321

typedef enum WiFiconnectionStatus 
{
	WIFI_INIT 				= 3,
	WIFI_RECONNECT			= 2,
	WIFI_CONNECTING 		= 1, 
	WIFI_CONNECTED 			= 0, 
	WIFI_CONNECT_FAILED 	= -1
} WiFiconnectionStatus_t;

class STAcredentials {
	public:
		bool		save();
		bool		restore();
		void		restoreDefaults();
		void		erase();				//for debuging
		
		char		ssid[STA_SSID_PWD_MAX_LEN];
		char		password[STA_SSID_PWD_MAX_LEN];
		int		salt;
};


class PR_ESP8266_WiFiConnectionManagerClass {
	public:
		PR_ESP8266_WiFiConnectionManagerClass ();

		void	loop();	
		bool	setSTAcredentials(const String sta_ssid, const String sta_password);	
		
		WiFiconnectionStatus	getStaus();
		
		bool	beginAP();
		bool	beginAP(String& apName);
		
		bool	endAP();
		
		bool	setOnConnectedCallback( void (*func)(void) );
		bool	setOnConnectFailledCallback( void (*func)(void) );
		
	
	protected:

		STAcredentials	_sta;
		
		void 	_onConnected();
		void 	_onConnectionFailed();
		
		void 	(*_onConnectedCallback)(void) 		= NULL;
		void 	(*_onConnectFailledCallback)(void) 	= NULL;
		
	
		String			_apName			= "";    		//for access point
		String			_apPassword;
				
		//String        _sta_ssid       = "";			//WiFi credentials
		//String        _sta_password	= "";

		WiFiconnectionStatus_t smWIFIstatus;// = WIFI_INIT;
		
		
	template <typename Generic>
	void	DEBUG_WM(Generic text);
};

PR_ESP8266_WiFiConnectionManagerClass	WIFIconnectionManager;


////////////////////////////////////////////////////////////////////
////	PR_ESP8266_WiFiConnectionManagerClass members
////////////////////////////////////////////////////////////////////

PR_ESP8266_WiFiConnectionManagerClass::PR_ESP8266_WiFiConnectionManagerClass() {
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
	smWIFIstatus = WIFI_INIT;
}


bool	PR_ESP8266_WiFiConnectionManagerClass::setSTAcredentials(const String sta_ssid, const String sta_password) {
	
	WiFi.begin(sta_ssid.c_str(), sta_password.c_str(), 0, NULL, false); //just to save the parameters without actually establishing connection to the access point
	smWIFIstatus = WIFI_INIT;
}

WiFiconnectionStatus	PR_ESP8266_WiFiConnectionManagerClass::getStaus() {
	
	switch (smWIFIstatus) {
		case  WIFI_CONNECTED:
			return WIFI_CONNECTED;
			break;
		
		case  WIFI_CONNECT_FAILED:
			return WIFI_CONNECT_FAILED;
			break;
		
		case  WIFI_INIT: 
		case  WIFI_RECONNECT: 
		case  WIFI_CONNECTING:
			return WIFI_CONNECTING;
			break;		
	} //switch
}


void	PR_ESP8266_WiFiConnectionManagerClass::loop() {
	
	static uint32_t	lastConnTime = 0;
//Serial.print(smWIFIstatus);
    	
	switch (smWIFIstatus) {
		case  WIFI_INIT: {
		
			}		
			{	// change state conditions
				smWIFIstatus = WIFI_RECONNECT;
				WiFi.begin();
			}
			break;
		case  WIFI_RECONNECT: {
			//
				//WiFi.begin();
				lastConnTime = millis()/1000;
			}		
			{	// change state conditions
				smWIFIstatus = WIFI_CONNECTING;
			}
			break;	
		case  WIFI_CONNECTING: {
			// awaiting connection
			}
			{	// change state conditions
				if ( WiFi.status() == WL_CONNECTED ) { 
					smWIFIstatus = WIFI_CONNECTED;
					
					DEBUG_WM( F("got IP=") );
					DEBUG_WM( WiFi.localIP() );
					
					_onConnected();
				}
				else if (PR_getTauS(lastConnTime) > WIFI_TIME_TO_CONNECT_FAILED ) {
					smWIFIstatus = WIFI_CONNECT_FAILED;
					_onConnectionFailed();
				}
			}	
			break;		
		case  WIFI_CONNECTED: {
			//
			}
			{	// change state conditions
				if ( WiFi.status() != WL_CONNECTED ) smWIFIstatus = WIFI_RECONNECT;
			}	
			break;
		case  WIFI_CONNECT_FAILED: {

				
			}
			{	// change state conditions
				if ( WiFi.status() == WL_CONNECTED ) {
					smWIFIstatus = WIFI_CONNECTED;
					_onConnected();
				}
			}	
			break;
	} //switch

//Serial.printf("       Connection status: %d\n", WiFi.status());	
	
} //loop

bool	PR_ESP8266_WiFiConnectionManagerClass::beginAP() {
	//return beginAP( String( ESP.getChipId() ) );
}


bool	PR_ESP8266_WiFiConnectionManagerClass::beginAP(String& apName) {
	
	WiFi.mode(WIFI_AP_STA);
	DEBUG_WM(F("SET WIFI mode AP+STA"));

	WiFi.softAP(apName.c_str(), _apPassword.c_str());
	
	DEBUG_WM(F("start Acess Point:"));
	DEBUG_WM(apName);
	DEBUG_WM(_apPassword);
	
	delay(500); // Without delay I've seen the IP address blank
	DEBUG_WM(F("AP IP address: "));
	DEBUG_WM(WiFi.softAPIP());
	return true;
}

bool	PR_ESP8266_WiFiConnectionManagerClass::endAP() {
	//bool ESP8266WiFiAPClass::softAPdisconnect(bool wifioff) ????
	WiFi.mode(WIFI_STA);
	
	DEBUG_WM(F("AP closed"));	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
//		Callback functions

bool	PR_ESP8266_WiFiConnectionManagerClass::setOnConnectedCallback( void (*func)(void) ) {
	_onConnectedCallback = func;
	return true;
}

bool	PR_ESP8266_WiFiConnectionManagerClass::setOnConnectFailledCallback( void (*func)(void) ) {
	_onConnectFailledCallback = func;
	return true;
}

void 	PR_ESP8266_WiFiConnectionManagerClass::_onConnected() {
	
	if ( _onConnectedCallback != NULL) {
		_onConnectedCallback();
	}
}
	
void 	PR_ESP8266_WiFiConnectionManagerClass::_onConnectionFailed() {
	
	if ( _onConnectFailledCallback != NULL) {
		_onConnectFailledCallback();
	}
}










////////////////////////////////////////////////////////////////////////////////
////	STAcredentials members

bool    STAcredentials::save() {
	salt = EEPROM_STA_CREDITALS_SALT;
	EEPROM.begin(512);
	EEPROM.put(EEPROM_STA_CREDITALS_ADDR, *this);
	EEPROM.end();
	Serial.println("STA credentials saved to EEPROM");
		
}

bool	STAcredentials::restore() {
	EEPROM.begin(512);
	EEPROM.get(EEPROM_STA_CREDITALS_ADDR, *this);
	EEPROM.end();
	
	if (salt != EEPROM_STA_CREDITALS_SALT) {
		Serial.println("Invalid STA credentials in EEPROM, set defaults");
		restoreDefaults();
		return false;
	}       
	return true;
}  

void	STAcredentials::restoreDefaults() {

	//ssid;
	//password;

} 

void    STAcredentials::erase() {
	
	char	err[300];
	for (int i = 0 ; i <299; i++) {
		err[i] = 88;
	}
	EEPROM.begin(512);
	EEPROM.put(EEPROM_STA_CREDITALS_ADDR, err);
	EEPROM.end();
	Serial.println("MQTT credentials errased");
}



template <typename Generic>
void PR_ESP8266_WiFiConnectionManagerClass::DEBUG_WM(Generic text) {
  if (true) {
    Serial.print("*WM: ");
    Serial.println(text);
  }
}




/*
typedef enum {
    WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
    WL_IDLE_STATUS      = 0,
    WL_NO_SSID_AVAIL    = 1,
    WL_SCAN_COMPLETED   = 2,
    WL_CONNECTED        = 3,
    WL_CONNECT_FAILED   = 4,
    WL_CONNECTION_LOST  = 5,
    WL_DISCONNECTED     = 6
} wl_status_t;


typedef enum WiFiMode 
{
    WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3
} WiFiMode_t;

*/

