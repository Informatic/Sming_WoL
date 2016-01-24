#include <user_config.h>
#include <SmingCore/SmingCore.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "PleaseEnterSSID" // Put you SSID and Password here
	#define WIFI_PWD "PleaseEnterPass"
#endif

// Target WoL MAC
uint8_t target_mac[6] = {
	0x12, 0x34, 0x56, 0x78, 0x21, 0x37
};

// Magic packet transmission interval
#define PACKET_INTERVAL 100

// GPIO settings
#define LED_PIN 2 // GPIO number
#define BTN_PIN 0 // Flash button on NodeMCU

UdpConnection udp;
Timer timer;

#define MAGIC_PACKET_SIZE 17*6

void sendWOL(uint8_t mac[6]) {
	uint8_t magic_packet[MAGIC_PACKET_SIZE] = {};
	for(int i = 0; i < 17; i++) {
		for(int j = 0; j < 6; j++) {
			// Set 0xff in first 6 bytes
			magic_packet[i*6 + j] = (i == 0) ? 0xff : mac[j];
		}
	}

	udp.send((char*) magic_packet, MAGIC_PACKET_SIZE);
}

// Will be called when WiFi station was connected to AP
void connectOk()
{
	Serial.println("I'm CONNECTED");
	timer.initializeMs(PACKET_INTERVAL, *[] {
			digitalWrite(LED_PIN, digitalRead(BTN_PIN));
			if(!digitalRead(BTN_PIN))
			{
				Serial.println("Sending WOL");
				sendWOL(target_mac);
			}
			}).start();
}

void init()
{
	pinMode(LED_PIN, OUTPUT);
	pinMode(BTN_PIN, INPUT);

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiAccessPoint.enable(false);

	// Run our method when station was connected to AP
	WifiStation.waitConnection(connectOk);
}
