#pragma once

/*
	
*/

#include <ESP8266WiFi.h>

using namespace DragonNETns::PacketConst;

class Driver
{
	const char *SERVER = "10.0.1.20";
	const uint16_t PORT = 35000;
	
	public:
		typedef std::function<void(byte type, byte *data, uint8_t length)> rx_function_t;
	
	
	
		/*
			Полный пакет данных:
			
			[0], 0x5B - Стартовый байт;
			[1], 0xXX - Тип пакета;
			[2], 0xXX - Длина данных пакета;
			[3+], 0xXX - Данные;
			[], 0x5D - Стоповый байт;
			
			
			[0], 0x5B - Стартовый байт;
			[1], 0xXX - Тип пакета;
			[2], 0xXX - Длина данных пакета;
				[3], 0xXX - ID устройства;
				[4], 0xXX - Тип устройства;
				[5], 0xXX - Тип данных;
				[6], 0xXX - Длина данных;
					[7+], 0xXX - Данные;
			[], 0x5D - Стоповый байт;
			
			
			
			
			Тип пакета:
			  Входящие ( Сервер -> Устройство )
				0x01 - Приглашение сервера на авторизацию клиента.
				0x02 - Ответ сервера на авторизацию клиента. Данные: 0x00 - успешно, иначе - код ошибки.
				0x10 - Данные от сервера (Для конкретного устройства).
				0x20 - Данные от сервера для конкретного клиента (ретрансляция пакета между клиентами).
			
			  Исходящие ( Устройство -> Сервер )
				0x81 - Передача токена клиента серверу.
				0x91 - Данные от клиента (От конкретного устройства)..
				0xA1 - Данные от клиента для конкретного клиента (ретрансляция пакета между клиентами).
			
			
		*/
		
		
		
		/*
			5B 02 01 00 5D - авторизация
			5B 10 04 31 32 33 34 5D
			
			5B 20 15 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 42 41 45 46 47 5D
			5B 20 15 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 42 41 45 46 47 5D
			

		
		*/
	
	
	
	
	
	
	
		void Connect(const char *ssid, const char *pass, const byte *token)
		{
			_ssid = ssid;
			_pass = pass;
			_token = token;
			
			_Connect2WiFi();
			_Connect2Server();
			

			
			//_isAuth = _Auth();
			
			if(_isAuth)
				Serial.println("Auth OK!");
			else
				Serial.println("Auth ERROR!");
			

		
		}



		bool Send(byte type, byte *packet, uint8_t size) // byte
		{
			
			
			if(_tcp_client.connected() == false)
			{
				_isAuth = false;
				_Connect2Server();
			}
			// Если не получилось, то не пытаться что-то отправлять.
			
			if(_isAuth == false)
			{
				Serial.println("No Auth !!!");
				
				return false;
			}
			
			
			byte full_packet[size + 4];
			full_packet[0] = PACKET_START;
			full_packet[1] = type;
			full_packet[2] = size;
			for(uint8_t i = 0; i < size; ++i)
			{
				full_packet[i+3] = packet[i];
			}
			full_packet[size+3] = PACKET_END;
			
			_tcp_client.write(full_packet, sizeof(full_packet));
			//_tcp_client.flush(1000);
			
			
			
			//for(uint8_t i = 0; i < size; ++i)
			{
				//Serial.write(packet, size);
				
				uint8_t offset = 32;
				for(uint8_t i = 0; i < size; ++i)
				{
					--offset;
					//if(offset % 8 == 0) Serial.print(" ");
					if(packet[i] < 0x10) Serial.print("0");
					Serial.print(packet[i], HEX);
					Serial.print(" ");
				}
				
				for(; offset > 0; --offset) Serial.print("__ ");
				
				Serial.print("| ");
				
				for(uint8_t i = 0; i < size; ++i)
				{
					if(packet[i] < 0x20 || packet[i] > 0x7F) Serial.print(".");
					else Serial.print(char(packet[i]));
				}
				
				Serial.println();
			}
			
			return true;
		}
		










		void Processing()
		{
			if(_ReadTCP() == true)
			{
				switch( _rx_buffer[1] )
				{
					// Запрос на авторизацию.
					case PACKET_TYPE_AUTH_REQ:
					{
						_AuthSend();
						
						break;
					}
					// Результат авторизации.
					case PACKET_TYPE_AUTH_RES:
					{
						_isAuth = _AuthChech();
						if(_isAuth)
							Serial.println("Auth OK!");
						else
							Serial.println("Auth ERROR!");
						
						break;
					}
					default:
					{
						_rx_function( _rx_buffer[1], _rx_buffer+3, _rx_buffer[2] );
						
						break;
					}
				}
			}
			
			return;
		}







		void RegRXData(rx_function_t func)
		{
			_rx_function = func;
			
			return;
		}



#define LoggingSerial(str)		(Serial.print(str))
#define LoggingSerialln(str)	(Serial.println(str))
		
		
	private:
		bool _Connect2WiFi()
		{
			bool result = false;
			
			LoggingSerial("\r\nConnect to WiFI ");
			
			WiFi.mode(WIFI_STA);
			WiFi.begin(_ssid, _pass);
			
			uint32_t time_start = millis();
			while( WiFi.status() != WL_CONNECTED )
			{
				if(millis() - time_start > 10000)
				{
					LoggingSerialln(" timeout!");
					
					break;
				}
				
				LoggingSerial(".");
				delay(250);
			}
			
			if( WiFi.status() == WL_CONNECTED )
			{
				result = true;
				
				LoggingSerialln(" ok");
				
				LoggingSerial("|- MAC: ");
				LoggingSerialln( WiFi.macAddress() );
				
				LoggingSerial("|- IP: ");
				LoggingSerial( WiFi.localIP() );
				LoggingSerial("/");
				LoggingSerialln( WiFi.subnetMask() );
				
				LoggingSerial("|- Gateway: ");
				LoggingSerialln( WiFi.gatewayIP() );
				
				LoggingSerial("|- Gateway: ");
				LoggingSerialln( WiFi.dnsIP() );
			}
			
			return result;
		}
		
		bool _Connect2Server()
		{
			bool result = false;
			
			LoggingSerial("\r\nConnect to Server ...");
			
			_tcp_client.stop(1000);
			_tcp_client.setDefaultNoDelay(true);
			_tcp_client.setDefaultSync(true);
			result = _tcp_client.connect(SERVER, PORT);
			
			if(result == true)
			{
				LoggingSerialln(" ok");
				
				LoggingSerial("|- IP: ");
				LoggingSerial( _tcp_client.localIP() );
				LoggingSerial(":");
				LoggingSerial( _tcp_client.localPort() );
				LoggingSerial(" <=> ");
				LoggingSerial( _tcp_client.remoteIP() );
				LoggingSerial(":");
				LoggingSerialln( _tcp_client.remotePort() );
			}
			else
			{
				LoggingSerialln(" error");
			}
			
			return result;
		}
		
		/*
		bool _Auth()
		{
			bool result = false;
			
			byte packet_tx[20];
			packet_tx[0] = PACKET_START;
			packet_tx[1] = 0x81;
			packet_tx[2] = 0x10;
			memcpy(packet_tx+3, _token, 0x10);
			packet_tx[19] = PACKET_END;
			_tcp_client.write(packet_tx, sizeof(packet_tx));
			
			if( _ReadTCP(5000, true) == true )
			{
				if(_rx_buffer[1] == 0x02 && _rx_buffer[3] == 0x00)
				{
					result = true;
				}
			}
			
			return result;
		}
		*/
		
		
		void _AuthSend()
		{
			byte packet_tx[20];
			packet_tx[0] = PACKET_START;
			packet_tx[1] = PACKET_TYPE_AUTH_TOKEN;
			packet_tx[2] = TOKEN_LENGTH;
			memcpy(packet_tx+3, _token, TOKEN_LENGTH);
			packet_tx[19] = PACKET_END;
			_tcp_client.write(packet_tx, sizeof(packet_tx));
			
			return;
		}
		
		bool _AuthChech()
		{
			bool result = false;
			
			if(_rx_buffer[3] == 0x00)
			{
				result = true;
			}
			
			return result;
		}
		
		/*
			Читает принимаемые данные и возвращает true если пакет принят и готов к обработке.
				uint16_t timeout - Время ожидания конца пакета, в мс.
				bool force - Ожидание пакета принудительно, даже если в буфере нету ещё ничего.
				return - true если пакет принят и целый, иначе false;
		*/
		bool _ReadTCP(uint16_t timeout = 100, bool force = false)
		{
			bool result = false;
			
			if(force == true || _tcp_client.available() > 0)
			{
				uint32_t time_start = millis();
				uint8_t bytes_left = 4;
				
				_CrearRXBuffer();
				
				do
				{
					if(millis() - time_start > timeout) break;
					
					if(_tcp_client.available() > 0)
					{
						_rx_buffer[_rx_buffer_index] = _tcp_client.read();
						
						if(_rx_buffer_index == 2)
						{
							bytes_left = _rx_buffer[_rx_buffer_index] + 2;
						}
						
						++_rx_buffer_index;
						--bytes_left;
					}
				} while(bytes_left > 0);
				
				// Пакет собран целиком (по длине).
				if(bytes_left == 0)
				{
					// Проверяем что у нас есть начало и конец пакета, а не 'обрывок' данных.
					if(_rx_buffer[0] == PACKET_START && _rx_buffer[_rx_buffer_index-1] == PACKET_END)
					{
						result = true;
					}
					// Структура пакета нарушена по причине потери синхронизации.
					else
					{
						// Вычитываем всё что есть в сокете в надежде что следующий пакет получится синхронизировать.
						// Да, минимум два пакета при таком раскладе потеряются, но я не придумал другого способа.
						while(_tcp_client.available() > 0) _tcp_client.read();
						
						//_CrearRXBuffer();
					}
				}
				// Пакет не собран по причине timeout.
				else
				{
					//_CrearRXBuffer();
				}
			}
			
			return result;
		}
		
		void _CrearRXBuffer()
		{
			// Зачем чистить буфер, если достаточно очищать индекс.
			memset(_rx_buffer, 0x00, sizeof(_rx_buffer));
			_rx_buffer_index = 0;
			
			return;
		}
	
	
	
	
	
	
	
	
	
		WiFiClient _tcp_client;
		
		
		const char *_ssid;
		const char *_pass;
		const byte *_token;
		
		rx_function_t _rx_function = nullptr;
		
		byte _rx_buffer[256+4];
		uint8_t _rx_buffer_index;
		
		// Пересчитать размеры буфера, т.к. размер полезной нагрузки и размер пакета максимум равно 256 и если данных будет 256 байт, то в пакете будет переполнение длинны пакета.
		
		
		bool _isAuth = false;
		
};
