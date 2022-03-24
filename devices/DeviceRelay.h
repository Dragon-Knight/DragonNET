#pragma once

/*
	Устройство типа 'Реле'.
*/

class DeviceRelay : public Device
{
	public:
		DeviceRelay(uint8_t pin, bool inverted) : _pin(pin), _inverted(inverted)//, _type{DeviceConst::TYPE_RELAY}
		{
			_type = DeviceConst::TYPE_RELAY;
			
			return;
		}
		
		
		
		void Init(bool state = false)
		{
			pinMode(_pin, OUTPUT);
			state ? On() : Off();
			
			return;
		}
		
		void On()
		{
			digitalWrite(_pin, (_inverted ? LOW : HIGH));
			_state = true;
			
			_ToServer();
			
			return;
		}
		
		void Off()
		{
			digitalWrite(_pin, (_inverted ? HIGH : LOW));
			_state = false;
			
			_ToServer();
			
			return;
		}
		
		void Write(bool state)
		{
			return state ? On() : Off();
		}
		
		void Toggle()
		{
			(_state == false) ? On() : Off();
			
			return;
		}
		
		bool State()
		{
			return _state;
		}
		
		void OnDelay(uint16_t time_on)
		{
			
		}
		
		void OffDelay(uint16_t time_off)
		{
			
		}
		
		void Blink(uint16_t time_on, uint16_t time_off)
		{
			
		}
		
		
		void _ToServer()
		{
			byte data[1];
			data[0] = _state;
			
			_TXFunction( _device_id, DeviceConst::TYPE_RELAY, DeviceConst::CMD_EVENT, data, sizeof(data) );
			
			return;
		}
		
		void _FromServer(DeviceConst::device_cmd_t cmd, byte *data, uint8_t length) override
		{
			switch(cmd)
			{
				case DeviceConst::CMD_GET:
				{
					_ToServer();
					
					break;
				}
				case DeviceConst::CMD_SET:
				{
					data[1] ? On() : Off();
					
					break;
				}
			}
			
			return;
		}
		
		
		

		

		
	protected:
		
		
	private:

		
		uint8_t _pin;
		bool _inverted;
		
		
		bool _state;
		
};
