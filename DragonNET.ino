#include "DragonNET.h"



const char *wf_name = "Global Zone";
const char *wf_pass = "1qwerty8";
const byte token[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};


// Токен другого устройства куда нужно отправить данные.
const byte ex_token1[] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
const byte ex_token2[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F};



DragonNETCore DragonNET;

DeviceRelay relay1(D7, true);
DeviceVoltage voltage1( A0, 1250, DeviceVoltage::GetCoefficient(5000000) );
DeviceDS18B20<4> sensors1(D2, 2000);
DeviceLedRGB rgb1(D3, D5, D6, false);



// Добавить возможность создавать объект без токена и указывать его отдельно. SetToken(), но проверять и не давать работать если указан конструктор без токена и не указан этот метод.
void onProxyRX(const byte *token, byte *data, uint8_t length);
DragonProxy proxy1(ex_token1, onProxyRX);
DragonProxy proxy2(ex_token2, onProxyRX);


void setup()
{
	Serial.begin(115200);
	
	DragonNET.Connect(wf_name, wf_pass, token);
	
	DragonNET.RegDevice(relay1, 12);
	DragonNET.RegDevice(voltage1, 15);
	DragonNET.RegDevice(sensors1, 74);
	DragonNET.RegDevice(rgb1, 125);
	
	DragonNET.RegProxy(proxy1);
	DragonNET.RegProxy(proxy2);
	
	voltage1.SetRequester([](uint8_t pin)
	{
		int32_t raw = analogRead(pin);
		
		return raw;
	});
	voltage1.SetRespondent([](uint8_t pin, int32_t voltage)
	{
		//Serial.print(pin);
		//Serial.print(" - ");
		//Serial.println(voltage);
	});
	
	sensors1.SetRespondent([](uint8_t id, byte *address, float temperature)
	{
		//Serial.print("sensors: ");
		//Serial.print(id);
		//Serial.print(", ");
		//Serial.print(temperature);
		//Serial.println(".");
	});
	
	relay1.Init();
	voltage1.Init();
	sensors1.Init();
	rgb1.Init();
	
	return;
}



bool relayState = false;



uint32_t lasttime1 = 0;
uint32_t lasttime2 = 0;
uint32_t lasttime3 = 0;


void loop()
{
	uint32_t time = millis();
	
	DragonNET.Processing();
	
	
	// Симуляция активной деятельности.
	if(time - lasttime1 > 1000)
	{
		lasttime1 = time;
		
		(relayState == false) ? relay1.On() : relay1.Off();
		relayState = !relayState;
	}
	
	if(time - lasttime2 > 1730)
	{
		lasttime2 = time;
		
		byte data1[] = {0x12, 0x01, 0x55, 0x56, 0x57};
		proxy1.Send(data1, sizeof(data1));
	}
	
	if(time - lasttime3 > 7519)
	{
		lasttime3 = time;
		
		byte data2[] = {0x22, 0x21, 0x25, 0x26, 0x27};
		proxy2.Send(data2, sizeof(data2));
	}
	
	
	return;
}

void onProxyRX(const byte *token, byte *data, uint8_t length)
{
	Serial.print("onProxyRX({");
	for(uint8_t i = 0; i < TOKEN_LENGTH; ++i)
	{
		if(token[i] < 0x10) Serial.print("0");
		Serial.print(token[i], HEX);
		if(i < 15) Serial.print(" ");
	}
	Serial.print("}, {");
	for(uint8_t i = 0; i < length; ++i)
	{
		if(data[i] < 0x10) Serial.print("0");
		Serial.print(data[i], HEX);
		if(i < length-1) Serial.print(" ");
	}
	Serial.print("}, ");
	Serial.print(length);
	Serial.println(");");
	
	return;
}
