#pragma once

/*
	Устройство типа 'RGB светодиод / лента'.
	
	https://github.com/ratkins/RGBConverter
*/

class DeviceLedRGB : public Device
{
	using respondent_t = void (*)(uint8_t r, uint8_t g, uint8_t b);
	
	public:
		DeviceLedRGB(uint8_t pinR, uint8_t pinG, uint8_t pinB, bool inverted) : _pins{pinR, pinG, pinB}, _inverted(inverted), _mode_pins(true)//, _type(DeviceConst::TYPE_LEDRGB)
		{
			_type = DeviceConst::TYPE_LEDRGB;
			
			return;
		}
		
		DeviceLedRGB(bool inverted) : _inverted(inverted), _mode_pins(false)//, _type(DeviceConst::TYPE_LEDRGB)
		{
			_type = DeviceConst::TYPE_LEDRGB;
			
			return;
		}
		
		void SetRespondent(respondent_t callback)
		{
			_respond_callback = callback;
			
			return;
		}
		
		void Init()
		{
			if(_mode_pins == true)
			{
				pinMode(_pins.r, OUTPUT);
				pinMode(_pins.g, OUTPUT);
				pinMode(_pins.b, OUTPUT);
				
				if(_inverted == true)
				{
					digitalWrite(_pins.r, HIGH);
					digitalWrite(_pins.g, HIGH);
					digitalWrite(_pins.b, HIGH);
				}
			}
			
			return;
		}
		
		
		
		
		void SetOn()
		{
			
			
			return;
		}
		
		void SetOff()
		{
			
			
			return;
		}
		
		void SetColorRGB(uint8_t r, uint8_t g, uint8_t b)
		{
			if(_inverted == true)
			{
				r = 255 - r;
				g = 255 - g;
				b = 255 - b;
			}
			
			if(_mode_pins == true)
			{
				analogWrite(_pins.r, r);
				analogWrite(_pins.g, g);
				analogWrite(_pins.b, b);
			}
			else
			{
				_respond_callback(r, g, b);
			}
			
			return;
		}
		
		void SetColorHSV(float h, float s, float v)
		{
			uint8_t rgb[3];
			_hsvToRgb(h, s, v, rgb);
			SetColorRGB(rgb[0], rgb[1], rgb[2]);
			
			return;
		}
		
		void FadeToRGB(uint8_t r, uint8_t g, uint8_t b, uint16_t duration, uint8_t step)
		{
			_fadeTo.r = r;
			_fadeTo.g = g;
			_fadeTo.b = b;
			_fadeTo.r_delay = duration / (abs(_color.r - _fadeTo.r) / step);
			_fadeTo.g_delay = duration / (abs(_color.g - _fadeTo.g) / step);
			_fadeTo.b_delay = duration / (abs(_color.b - _fadeTo.b) / step);
			_fadeTo.step = step;
			
			return;
		}
		
		void FadeToHSV(float h, float s, float v, uint16_t duration)
		{
			
			
			return;
		}
		
		void Processing(uint32_t current_time) override
		{
			
			
			return;
		}
		

		
		void _ToServer()
		{
			byte data[1] = {0x00};
			//data[0] = _state ? 0xFF : 0x00;
			
			_TXFunction( _device_id, _type, DeviceConst::CMD_EVENT, data, sizeof(data) );
			
			return;
		}
		
		void _FromServer(DeviceConst::device_cmd_t cmd, byte *data, uint8_t length) override
		{
			//data[0] ? On() : Off();
			
			return;
		}
		
	//protected:

		
	private:
	
	
	
		void _hsvToRgb(double h, double s, double v, byte rgb[])
		{
			double r, g, b;

			int i = int(h * 6);
			double f = h * 6 - i;
			double p = v * (1 - s);
			double q = v * (1 - f * s);
			double t = v * (1 - (1 - f) * s);

			switch(i % 6){
				case 0: r = v, g = t, b = p; break;
				case 1: r = q, g = v, b = p; break;
				case 2: r = p, g = v, b = t; break;
				case 3: r = p, g = q, b = v; break;
				case 4: r = t, g = p, b = v; break;
				case 5: r = v, g = p, b = q; break;
			}

			rgb[0] = r * 255;
			rgb[1] = g * 255;
			rgb[2] = b * 255;
		}
	
	
	
	
	
	
	
	
	
	
	
	
	
		struct pins_t { uint8_t r; uint8_t g; uint8_t b; } _pins;
		struct color_t
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			float h;
			float s;
			float v;
		} _color;
		

		struct fade_t
		{
			uint8_t r;		// Итоговая яркость красного компонента.
			uint8_t g;		// Итоговая яркость зелёного компонента.
			uint8_t b;		// Итоговая яркость синего компонента.
			uint16_t r_delay;	// Интервал одного такта изменения красного компонента.
			uint16_t g_delay;	// Интервал одного такта изменения зелёного компонента.
			uint16_t b_delay;	// Интервал одного такта изменения синего компонента.
			uint8_t step;		// Шаг изменения любого компонента за один такт.
		} _fadeTo;

		
		//uint8_t _pin;
		bool _inverted;
		
		bool _mode_pins;
		
		respondent_t _respond_callback = nullptr;
		
		
		//bool _state;

		
};
