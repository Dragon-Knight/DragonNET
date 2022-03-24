#pragma once

/*

*/

namespace DragonNETns
{
	namespace DeviceConst
	{
		// Тип устройства.
		enum device_type_t : byte
		{
			TYPE_NONE =		0x00,
			TYPE_RELAY =	0x01,
			TYPE_LED =		0x02,
			TYPE_BUTTON =	0x03,
			TYPE_VOLTAGE =	0x04,
			TYPE_DS18B20 =	0x05,
			TYPE_LEDRGB =	0x06,
			
			TYPE_RAW =		0xFF
		};
		
		// Тип данных.
		enum device_cmd_t : byte
		{
			CMD_NONE =		0x00,	// ...
			CMD_EVENT =		0x01,	// Событие. Уведомляем сервер что устройство изменило состояние. Например реле включилось (как удалённо, так и локально).
			CMD_GET = 		0x02,	// Команда состояния. Например реле включено?
			CMD_SET = 		0x03	// Команда установки. Например включить реле.
		};
	}
}
