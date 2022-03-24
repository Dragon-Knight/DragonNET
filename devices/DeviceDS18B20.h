#pragma once

/*
	Устройство измерения температуры типа 'DS18B20'.
	
	Зависимости:
		- Библиотека OneWire: https://github.com/PaulStoffregen/OneWire
*/

#include <OneWire.h>

template <uint8_t _sensor_count>
class DeviceDS18B20 : public Device
{
	using respondent_t = void (*)(uint8_t id, byte *address, float temperature);
	
	public:
		DeviceDS18B20(uint8_t pin, uint16_t interval) : _pin(pin), _interval(interval)//, _type(DeviceConst::TYPE_DS18B20)
		{
			_type = DeviceConst::TYPE_DS18B20;
			
			return;
		}
		
		void SetRespondent(respondent_t callback)
		{
			_respond_callback = callback;
			
			return;
		}
		
		void Init()
		{
			_onewire = OneWire(_pin);
			_SearchSensors();
			
			return;
		}
		
		
		
		
		void Processing(uint32_t current_time) override
		{
			if(current_time - _last_time > _interval)
			{
				if(_converted == false)
				{
					_last_time = current_time - _interval + 800;
					
					for(uint8_t i = 0; i < _sensors_length; ++i)
					{
						_RequestTemp(_sensors[i].address);
					}
					
					_converted = true;
				}
				else
				{
					_last_time = current_time;
					
					for(uint8_t i = 0; i < _sensors_length; ++i)
					{
						float temp = NAN;
						if( _ReadTemp(_sensors[i].address, temp) == true )
						{
							_sensors[i].temperature = temp;
						}
						
						_ToServer(i);
						_respond_callback(i, _sensors[i].address, _sensors[i].temperature);
					}
					
					_converted = false;
				}
			}
			
			return;
		}
		
		static inline float ToFahrenheit(float celsius)
		{
			return celsius * 1.8 + 32.0;
		}
		
		static inline float ToKelvin(float celsius)
		{
			return celsius + 273.15;
		}
		
		
		void _ToServer(uint8_t id)
		{
			const byte *ptr = (const byte*) &_sensors[id].temperature;
			
			byte data[5] = {0x00};
			data[0] = id;
			data[1] = *ptr++;
			data[2] = *ptr++;
			data[3] = *ptr++;
			data[4] = *ptr++;
			
			_TXFunction( _device_id, _type, DeviceConst::CMD_EVENT, data, sizeof(data) );
			
			return;
		}
		
		void _FromServer(DeviceConst::device_cmd_t cmd, byte *data, uint8_t length) override
		{
			return;
		}
		
	//protected:

		
	private:
		
		// Поиск датчиков.
		uint8_t _SearchSensors()
		{
			byte address[8] = {0x00};
			while(_onewire.search(address) == 1)
			{
				if(address[0] != 0x10 && address[0] != 0x28 && address[0] != 0x22) continue;
				
				if(OneWire::crc8(address, 7) == address[7])
				{
					memcpy( _sensors[_sensors_length++].address, address, 8 );
				}
			}
			_onewire.reset_search();
			
			return _sensors_length;
		}
		
		// Запуск конвертации температуры в датчике.
		bool _RequestTemp(const byte address[8])
		{
			bool result = false;
			
			if(_onewire.reset() == 1)
			{
				_onewire.select(address);
				_onewire.write(0x44, 1);
				
				result = true;
			}
			
			return result;
		}
		
		// Чтение температуры из датчика.
		bool _ReadTemp(const byte address[8], float &temperature)
		{
			bool result = false;
			
			if(_onewire.reset() == 1)
			{
				byte data[9] = {0x00};
				
				_onewire.select(address);
				_onewire.write(0xBE);
				
				for(uint8_t i = 0; i < 9; ++i)
				{
					data[i] = _onewire.read();
				}
				
				if(OneWire::crc8(data, 8) == data[8])
				{
					int16_t raw = (data[1] << 8) | data[0];
					
					if(address[0] == 0x10)
					{
						raw = raw << 3;
						if(data[7] == 0x10)
						{
							raw = (raw & 0xFFF0) + 12 - data[6];
						}
					}
					else
					{
						byte cfg = (data[4] & 0x60);
						if(cfg == 0x00) raw = raw & ~7;
						else if(cfg == 0x20) raw = raw & ~3;
						else if(cfg == 0x40) raw = raw & ~1;
					}
					temperature = (float)raw / 16.0;
					
					result = true;
				}
			}
			
			return result;
		}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
		
		OneWire _onewire;
		struct sensor_t
		{
			byte address[8];
			float temperature;
			bool lost;
		} _sensors[_sensor_count];
		uint8_t _sensors_length;
		
		bool _converted;
		
		uint8_t _pin;
		uint16_t _interval;
		
		
		uint32_t _last_time;
		
		respondent_t _respond_callback;
		

		
};
