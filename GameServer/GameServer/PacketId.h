#pragma once

enum PacketId : INT32
{
	// Generic 0 ~ 99
	CLOSE_CONN = 1,
	ERROR_OCCUR = 11,


	GAME_ENTER_REQ = 101, 
	GAME_ENTER_NTF = 102,
	GAME_ENTER_RES = 103,
};