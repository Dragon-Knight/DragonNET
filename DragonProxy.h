#pragma once

/*
	Класс для общения устройств бинарно-безопасными данными через облачный сервер.
	На вход и на выход поступают только данные.
	Сам пакет представляет собой 16 байт токена устройства назначения + пользовательские данные.
*/

using namespace DragonNETns::PacketConst;

class DragonProxy
{
	typedef std::function<void(const byte *token, byte *data, uint8_t length)> rx_callback_t;
	typedef std::function<void(byte *data, uint8_t length)> tx_callback_t;
	
	public:
		DragonProxy(rx_callback_t rx_callback) : _RXFunction(rx_callback)
		{
			return;
		}
		
		DragonProxy(const byte *token, rx_callback_t rx_callback) : _token(token), _RXFunction(rx_callback)
		{
			return;
		}
		
		void SetToken(byte *token)
		{
			// Реализовать.
		}
		
		void Send(byte *data, uint8_t length)
		{
			byte packet[TOKEN_LENGTH + length];
			memcpy(packet, _token, TOKEN_LENGTH);
			memcpy(packet + TOKEN_LENGTH, data, length);
			_TXFunction(packet, sizeof(packet));
			
			return;
		}
		
		
		void _FromServer(byte *data, uint8_t length)
		{
			_RXFunction(data, data + TOKEN_LENGTH, length - TOKEN_LENGTH);
			
			return;
		}
		
		void _SetListener(tx_callback_t func)
		{
			_TXFunction = func;
			
			return;
		}
		
		bool _IsHere(byte *data)
		{
			return (memcmp(_token, data, TOKEN_LENGTH) == 0);
		}
		
	private:
		rx_callback_t _RXFunction = nullptr;
		tx_callback_t _TXFunction = nullptr;
		const byte *_token;
};
