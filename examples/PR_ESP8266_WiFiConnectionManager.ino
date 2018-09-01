#include    "PR_ESP8266_WiFiConnectionManager.hpp"






void    setup() {
    Serial.begin(115200);
    Serial.flush();
    Serial.println("start");

    //WIFIconnectionManager.setSTAcredentials("Guest---", "123456789");
    WIFIconnectionManager.setSTAcredentials("AirS", "Ya6lok0va$");
	//WIFIconnectionManager.setSTAcredentials("TP-LINK_B1DF34", "EDB1DF34");
    WiFi.mode(WIFI_STA);
    Serial.println("SET STA");
	
	
	
	
	
}

void    loop() {
    WIFIconnectionManager.loop();
    
	

  }
	
	
	
