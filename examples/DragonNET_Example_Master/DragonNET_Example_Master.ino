/*
 *	DragonNET_Example_Master.ino
 *
 *	@author		Nikolai Tikhonov aka Dragon_Knight <dubki4132@mail.ru>, https://vk.com/globalzone_edev
 *	@coauthor	Valeriy V Dmitriev aka valmat <ufabiz@gmail.com>
 *	@licenses	MIT https://opensource.org/licenses/MIT
 *	@repo		https://github.com/Dragon-Knight/DragonNET
 */


#define DRAGONNET_USE_SOFTWARESERIAL
#include "DragonNET.h"

#if !defined(DRAGONNET_USE_SOFTWARESERIAL)
	DragonNETMaster bus(Serial, 13);
#else
	SoftwareSerial mySerial(2, 3);
	DragonNETMaster bus(mySerial, 13);
#endif

DragonNETPacket_t Packet;

void setup()
{
	Serial.begin(115200);
	
	bus.Begin(115200, 0x00, false, OnReceive, OnError);
	
	byte data[] = {'5', '5', '5'};
	Packet.PutData2(data, sizeof(data));
	Packet.PutToAddress(0x02);
	bus.Send(Packet);
	
	return;
}

void loop()
{
	bus.Processing();
	
	return;
}

bool OnReceive(DragonNETPacket_t &request, DragonNETPacket_t &response)
{
	/*
	// Выбор нужен ответ или нет, через ретурн.
	response.PutToAddress(request.TakeFromAddress());
	response.PutFromAddress(request.TakeToAddress());
	response.PutData1(0x50);
	response.PutData1(0x50);
	response.PutData1(0x50);
	response.PutData1(0x50);
	response.PutData1(0x50);
	*/
	
	Serial.write(request.TakeData2(), request.TakeDataLength());
	
	return false;
}

void OnError(uint8_t errorType)
{
	Serial.write(">");
	Serial.write(errorType);
	Serial.write("<");
	
	
	return;
}
