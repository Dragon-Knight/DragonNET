#pragma once

/*
	Базовый класс устройства.
*/

using namespace DragonNETns;

class Device
{
	typedef std::function<bool(uint8_t id, DeviceConst::device_type_t type, DeviceConst::device_cmd_t cmd, byte *data, uint8_t length)> tx_callback_t;
	
	public:
		Device()
		{
			return;
		}
		
		/*
			Выполняет внутреннюю обработку устройства, если требуется.
		*/
		virtual void Processing(uint32_t current_time){ return; };
		
		/*
			Указывает лямбду отправки данных на сервер. Файл DragonNET.h.
				uint8_t id - Пользовательский ID устройства.
				tx_callback_t func - Функция отправки.
		*/
		void _SetListener(uint8_t id, tx_callback_t func)
		{
			_device_id = id;
			_TXFunction = func;
			
			return;
		}
		
		/*
			Вызывается при получении данных для конкретного устройства.
				device_cmd_t cmd - Тип данных.
				byte *data - Данные.
				uint8_t length - Длина данных.
		*/
		virtual void _FromServer(DeviceConst::device_cmd_t cmd, byte *data, uint8_t length){ return; };
		
		/*
			Проверяет входящий пакет на принадлежность конкретному устройству.
				byte *data - Пакет целиком (L3).
		*/
		bool _IsHere(byte *data)
		{
			return (data[0] == _device_id && data[1] == _type);
		}
		
	protected:
		tx_callback_t _TXFunction;
		uint8_t _device_id;
		DeviceConst::device_type_t _type;
		
};

#include "DeviceRelay.h"
#include "DeviceVoltage.h"
#include "DeviceDS18B20.h"
#include "DeviceLedRGB.h"
