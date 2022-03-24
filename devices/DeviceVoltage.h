#pragma once

/*
	Устройство измерения напряжения.
*/

class DeviceVoltage : public Device
{
	using requester_t = int32_t (*)(uint8_t pin);
	using respondent_t = void (*)(uint8_t pin, int32_t voltage);
	
	public:
		DeviceVoltage(uint8_t pin) : _pin(pin)//, _type(DeviceConst::TYPE_VOLTAGE)
		{
			_type = DeviceConst::TYPE_VOLTAGE;
			
			return;
		}
		
		DeviceVoltage(uint8_t pin, uint16_t interval, uint32_t coefficient) : _pin(pin), _interval(interval), _coefficient(coefficient)//, _type(DeviceConst::TYPE_VOLTAGE)
		{
			_type = DeviceConst::TYPE_VOLTAGE;
			
			return;
		}
		
		void SetRequester(requester_t callback)
		{
			_request_callback = callback;
			
			return;
		}
		
		void SetRespondent(respondent_t callback)
		{
			_respond_callback = callback;
			
			return;
		}
		
		void SetInterval(uint16_t interval)
		{
			_interval = interval;
			
			return;
		}
		
		void SetCoefficient(uint32_t coefficient)
		{
			_coefficient = coefficient;
			
			return;
		};
		
		void Init()
		{
			pinMode(_pin, INPUT);
			
			return;
		}
		
		int32_t GetVoltage()
		{
			int32_t raw = _request_callback(_pin);
			
			return (raw * _coefficient) / 1000;
		};
		
		void Processing(uint32_t current_time) override
		{
			if(current_time - _last_time > _interval)
			{
				_last_time = current_time;
				
				int32_t voltage = GetVoltage();
				
				_respond_callback(_pin, voltage);
				_ToServer(voltage);
			}
			
			return;
		}
		
		static inline uint32_t GetCoefficient(float vref, float r1 = 0, float r2 = 1, float quantization = 1024)
		{
			return ((vref / quantization) * ((r1 + r2) / r2)) + 0.5;
		}
		
		
		void _ToServer(int32_t voltage)
		{
			const byte *ptr = (const byte*) &voltage;
			
			byte data[4] = {0x00};
			data[0] = *ptr++;
			data[1] = *ptr++;
			data[2] = *ptr++;
			data[3] = *ptr++;
			
			_TXFunction( _device_id, _type, DeviceConst::CMD_EVENT, data, sizeof(data) );
			
			return;
		}
		
		void _FromServer(DeviceConst::device_cmd_t cmd, byte *data, uint8_t length) override
		{
			return;
		}
		

		
	private:
		
		requester_t _request_callback;
		respondent_t _respond_callback;
		uint16_t _interval;
		uint32_t _coefficient;
		uint32_t _last_time;
		uint8_t _pin;
		
		
};
