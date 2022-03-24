#pragma once

/*
	
*/

#include "const_packet.h"
#include "const_device.h"
#include "DragonProxy.h"
#include "devices/Device.h"
#include "drivers/Driver.h"

/*
	https://forum.arduino.cc/t/are-anonymous-function-or-lambda-possible-with-uno-yes-solved/420186
	https://github.com/esp8266/Arduino/blob/master/libraries/SPISlave/src/SPISlave.h
	https://github.com/esp8266/Arduino
	https://arduino-esp8266.readthedocs.io/en/3.0.2/esp8266wifi/client-class.html?highlight=WiFiClient
	
	https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h
	https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.cpp
	https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClient.h
	https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClient.cpp
*/

using namespace DragonNETns;

template <uint8_t _max_device = 32, uint8_t _max_proxy = 4>
class DragonNETCore
{
	public:
		DragonNETCore()
		{
			_InitDriver();
			
			return;
		}
		
		void Connect(const char *ssid, const char *pass, const byte *token)
		{
			_driver.Connect(ssid, pass, token);
			
			return;
		}
		
		void RegDevice(Device &object, uint8_t id)
		{
			if(_devices_idx == _max_device) return;
			
			(&object)->_SetListener(id, [&](uint8_t id, DeviceConst::device_type_t type, DeviceConst::device_cmd_t cmd, byte *data, uint8_t length)
			{
				byte packet[ 4 + length ] = {0x00};
				packet[0] = id;
				packet[1] = type;
				packet[2] = cmd;
				packet[3] = length;
				memcpy(packet + 4, data, length);
				
				return _driver.Send(PacketConst::PACKET_TYPE_FROM_DEVICE, packet, length + 4);
			});
			
			_devices[ _devices_idx ].object = &object;
			++_devices_idx;
			
			return;
		}
		
		void RegProxy(DragonProxy &object)
		{
			if(_proxies_idx == _max_proxy) return;
			
			(&object)->_SetListener([&](byte *data, uint8_t length)
			{
				_driver.Send(PacketConst::PACKET_TYPE_FROM_PROXY, data, length);
			});
			
			_proxies[ _proxies_idx ].object = &object;
			++_proxies_idx;
			
			return;
		}
		
		void Processing()
		{
			uint32_t current_time;
			for(uint8_t i = 0; i < _devices_idx; ++i)
			{
				// Каждую итерацию:
				//  Определяем текущее время;
				//  Запускаем метод обработки устройства;
				//  Запускаем метод приёма данные по TCP;
				
				current_time = millis();
				
				_devices[i].object->Processing(current_time);
				
				_driver.Processing();
			}
			
			return;
		}
		
	private:
		void _InitDriver()
		{
			_driver.RegRXData([&](byte type, byte *data, uint8_t length)
			{
				switch(type)
				{
					case PACKET_TYPE_FOR_DEVICE:
					{
						for(uint8_t i = 0; i < _devices_idx; ++i)
						{
							if( _devices[i].object->_IsHere(data) == true )
							{
								_devices[i].object->_FromServer((DeviceConst::device_cmd_t)data[2], data + 4, length - 4);
								
								break;
							}
						}
						
						break;
					}
					case PACKET_TYPE_FOR_PROXY:
					{
						for(uint8_t i = 0; i < _proxies_idx; ++i)
						{
							if( _proxies[i].object->_IsHere(data) == true )
							{
								_proxies[i].object->_FromServer(data, length);
								
								break;
							}
						}
						
						break;
					}
				}
				
				Serial.print("\r\nRX: ");
				for(uint8_t i = 0; i < length; ++i)
				{
					if(data[i] < 0x10) Serial.print("0");
					Serial.print(data[i], HEX);
					Serial.print(" ");
				}
				Serial.println();
				
			});
			
			return;
		}
		
		Driver _driver;
		
		struct device_t
		{
			Device *object;
		} _devices[_max_device];
		uint8_t _devices_idx = 0;
		
		struct proxy_t
		{
			DragonProxy *object;
		} _proxies[_max_proxy];
		uint8_t _proxies_idx = 0;
		
};
