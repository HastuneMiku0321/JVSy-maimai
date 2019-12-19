/*
 * JVS.cpp
 *
 *  Created on: 14/apr/2014
 *      Author: k4roshi
 */

#include "Arduino.h"
#include "JVS.h"
#include "constants.h"

bool analog;
int keys_Player1[16];   // Touches Joueur 1 
int keys_Player2[16];   // Touches Joueur 2 
int keys_Specials[4];   // Touches Speciales
const unsigned int keymap1[] = {        // Player 1
  KEY_LEFT_CTRL,      // Button 1
  KEY_LEFT_ALT,       // Button 2
  KEY_5,              // Coin
  KEY_1,              // Start
  KEY_UP,             // Up
  KEY_DOWN,           // Down
  KEY_LEFT,           // Left
  KEY_RIGHT,          // Right
  KEY_SPACE,          // Button 3
  KEY_LEFT_SHIFT,     // Button 4
  KEY_Z,              // Button 5
  KEY_X               // Button 6
};

const unsigned int keymap2[] = {       // Player 2
  KEY_A,              // Button 1
  KEY_S,              // Button 2
  KEY_6,              // Coin
  KEY_2,              // Start
  KEY_R,              // Up
  KEY_F,              // Down
  KEY_D,              // Left
  KEY_G,              // Right
  KEY_Q,              // Button 3
  KEY_W,              // Button 4
  KEY_I,              // Button 5
  KEY_K               // Button 6
};

const unsigned int keymap3[] = {       // Specials
  KEY_ESC,            // EXIT
  KEY_TAB,            // Menu Mame
  KEY_P,              // Pause
  KEY_ENTER,          // Enter
};

JVS::JVS(HardwareSerial& serial) :
_Uart(serial) // Need to initialize references before body
{
	coins1 = 0;
	coins2 = 0;
	coin_pressed_at = 0;
	initialized = false;
	shift_mode = false;
	pressed_smth = false;
	old_key = 0;
  analog = false;
}

void JVS::reset() {
	char str[] = { (char) CMD_RESET, (char) CMD_RESET_ARG };
	this->write_packet(BROADCAST, str, 2);
	delay(ASSIGN_DELAY);
	Serial.println("RESET");
}

void JVS::assign(int attempt) {
	char str[] = { (char) CMD_ASSIGN_ADDR, (char) attempt };
	this->cmd(BROADCAST, str, 2);
	Serial.println("ADDR");
}

void JVS::init(int board) {
	Serial.println("ADDR");
	char str[] = { (char) CMD_ASSIGN_ADDR, board };
	this->cmd(BROADCAST, str, 2);
	Serial.println("REQ");
	char str1[] = { (char) CMD_REQUEST_ID };
	this->cmd(board, str1, 1);
	Serial.println("CMD");
	char str2[] = { (char) CMD_COMMAND_VERSION };
	this->cmd(board, str2, 1);
	Serial.println("JVS");
	char str3[] = { (char) CMD_JVS_VERSION };
	this->cmd(board, str3, 1);
	Serial.println("CMS");
	char str4[] = { (char) CMD_COMMS_VERSION };
	this->cmd(board, str4, 1);
	Serial.println("CAP");
	char str5[] = { (char) CMD_CAPABILITIES };
	this->cmd(board, str5, 1);
	initialized = true;
}

void JVS::switches(int board) {
	char str[] = { CMD_READ_DIGITAL, 0x02, 0x02, CMD_READ_COINS, 0x02, CMD_READ_ANALOG, 0x04};
	//char str[ ] = { 0x20, 0x02, 0x02, 0x21, 0x02, 0x22, 0x08};
	this->write_packet(board, str, sizeof str);
	char incomingByte;
  int nbKeys;
	while (!_Uart.available()) {
	}
	while (_Uart.read() != 0xE0) {
	} // wait for sync
	while (_Uart.read() != 0) {
	} // only if it's for me
	while (!_Uart.available()) {
	} // wait for length
	int length = _Uart.read();
	int counter = 0;
  int X_player1 = 512;
  int Y_player1 = 512;
  int X_player2 = 512;
  int Y_player2 = 512; 
	int coin1 = 0;
  int coin2 = 0;
	bool old_shift = shift_mode;
  keys_Player1[3]=0;
  keys_Player2[3]=0;
  keys_Player1[2]=0;
  keys_Specials[0]=0;
  keys_Specials[1]=0;
  keys_Specials[2]=0;
  keys_Specials[3]=0;
  

	if	(DEBUG_MODE) {
		Serial.print("swthc: E0 0 ");
		Serial.print(length, HEX);
	}
	while (counter < length) {
		while (!_Uart.available()) {
		}
		incomingByte = _Uart.read();
		if (DEBUG_MODE){
			Serial.print(" ");
			Serial.print(incomingByte, HEX);
		}
		switch (counter) {
		// fourth byte (first three bytes are sync and
		case 3:
			// p1 b1
			shift_mode = bitRead(incomingByte, 7);

			if (shift_mode != old_shift) {
				if (shift_mode == 0 && !pressed_smth) {
					//never pressed anything, wants start
					keys_Player1[3]=1;
				}
				else
					pressed_smth = false;
			}
			if (shift_mode) {
//				Serial.println("shiftmode on");
				if(bitRead(incomingByte, 1)) 
					keys_Player1[2]=1;
				if(bitRead(incomingByte, 2))
					keys_Specials[1]=1;
				if(bitRead(incomingByte, 3))
					keys_Specials[3]=1;
				if(bitRead(incomingByte, 4))
					keys_Specials[2]=1;
      
      if (keys_Player1[3] || keys_Player1[2] || keys_Specials[1] || keys_Specials[3] || keys_Specials[2])
          pressed_smth = true;

			} else {
        if bitRead(incomingByte, 1)
                    Keyboard.pressModifier(KEY_LEFT_CTRL);
        else
                    Keyboard.releaseModifier(KEY_LEFT_CTRL);
        if bitRead(incomingByte, 0)
                    Keyboard.pressModifier(KEY_LEFT_ALT);
        else
                    Keyboard.releaseModifier(KEY_LEFT_ALT);
				if bitRead(incomingByte, 2)
										keys_Player1[7]=1;
        else
                    keys_Player1[7]=0;
				if bitRead(incomingByte, 3)
										keys_Player1[6]=1;
        else
                    keys_Player1[6]=0;
				if bitRead(incomingByte, 4)
										keys_Player1[5]=1;
        else
                    keys_Player1[5]=0;
				if bitRead(incomingByte, 5)
										keys_Player1[4]=1;
         else
                    keys_Player1[4]=0;
				//Joystick.button(7,bitRead(incomingByte, 6)); // Service à voir
			}
			break;
		case 4:
			// p1 b2
			if (shift_mode) {

			} else {
				if bitRead(incomingByte, 7)
                    keys_Player1[8]=1;
        else
                    keys_Player1[8]=0;
        if bitRead(incomingByte, 6)
                    Keyboard.pressModifier(KEY_LEFT_SHIFT);
        else
                    Keyboard.releaseModifier(KEY_LEFT_SHIFT);
        if bitRead(incomingByte, 5)
                    keys_Player1[10]=1;
        else
                    keys_Player1[10]=0; 
        if bitRead(incomingByte, 4)
                    keys_Player1[11]=1;
        else
                    keys_Player1[11]=0; 
			}
			break;
		case 5:
			// p2 b1
			if (shift_mode) {
				if(bitRead(incomingByte, 7))
					keys_Specials[0]=1;
				if (keys_Specials[0])
					pressed_smth = true;
			} else {
				if bitRead(incomingByte, 1)
                    keys_Player2[0]=1;
        else
                    keys_Player2[0]=0;
        if bitRead(incomingByte, 0)
                    keys_Player2[1]=1;
        else
                    keys_Player2[1]=0;
        if bitRead(incomingByte, 2)
                    keys_Player2[7]=1;
        else
                    keys_Player2[7]=0;
        if bitRead(incomingByte, 3)
                    keys_Player2[6]=1;
        else
                    keys_Player2[6]=0;
        if bitRead(incomingByte, 4)
                    keys_Player2[5]=1;
        else
                    keys_Player2[5]=0;
        if bitRead(incomingByte, 5)
                    keys_Player2[4]=1;
         else
                    keys_Player2[4]=0;
				//Joystick2.button(7,bitRead(incomingByte, 6)); //Service
				if bitRead(incomingByte, 7)
					keys_Player2[3]=1;
				else
					keys_Player2[3]=0;
			}
			break;
		case 6:
			// p2 b2
			if (shift_mode) {

			} else {
        if bitRead(incomingByte, 7)
                    keys_Player2[8]=1;
        else
                    keys_Player2[8]=0;
        if bitRead(incomingByte, 6)
                    keys_Player2[9]=1;
        else
                    keys_Player2[9]=0;
        if bitRead(incomingByte, 5)
                    keys_Player2[10]=1;
        else
                    keys_Player2[10]=0; 
        if bitRead(incomingByte, 4)
                    keys_Player2[11]=1;
        else
                    keys_Player2[11]=0; 
			}
			break;
		case 8:
			// coins 1 status
			break;
		case 9:
			// coins1
			if (incomingByte > coins1) {
				// added coin
				coin1 = 1;
				coins1 = incomingByte;
			}
			if (coin1){
				coin_pressed_at = millis();
				keys_Player1[2]=1;
			} else if (coin_pressed_at > 0){
				if (millis() - coin_pressed_at > 50){
					coin_pressed_at = 0;
					keys_Player1[2]=0;
				}
			}
			break;
		case 10:
			// coins2 status
			break;
		case 11:
			// coins2
      if (incomingByte > coins2) {
        // added coin
        coin2 = 1;
        coins2 = incomingByte;
      }
      if (coin2){
        coin_pressed_at = millis();
        keys_Player2[2]=1;
      } else if (coin_pressed_at > 0){
        if (millis() - coin_pressed_at > 50){
          coin_pressed_at = 0;
          keys_Player2[2]=0;
        }
      }
      break;
    case 13:
      // Analog X P1 
      //Serial.print(incomingByte,DEC);
      if(incomingByte < 0) {
        X_player1 = 1024 + (incomingByte * 4);
      } else {
        X_player1 = 4 * incomingByte;
      }
      Joystick.X(X_player1);
      break;
    case 15:
    // Analog Y P1 
      if(incomingByte < 0) {
        Y_player1 = 1024 + (incomingByte * 4);
      } else {
        Y_player1 = 4 * incomingByte;
      }
      Joystick.Y(Y_player1);
      break;
    case 17:
      // Analog X P2
      //Serial.print(incomingByte,DEC);
      if(incomingByte < 0) {
        X_player2 = 1024 + (incomingByte * 4);
      } else {
        X_player2 = 4 * incomingByte;
      }
      Joystick2.X(X_player2);
      break;
    case 19:
      // Analog Y P2
      if(incomingByte < 0) {
        Y_player2 = 1024 + (incomingByte * 4);
      } else {
        Y_player2 = 4 * incomingByte;
      }
      Joystick2.Y(Y_player2);
      break;
    }  
		counter++;
    if (analog) {
      Joystick.send_now(); 
      Joystick2.send_now();
    }
	}

  for (nbKeys = 0; nbKeys < 12; nbKeys++) {
    // Player 1
    if (keys_Player1[nbKeys])
      Keyboard.pressKey(keymap1[nbKeys]);
    else
      Keyboard.releaseKey(keymap1[nbKeys]);
    // Player 2
    if (keys_Player2[nbKeys])
      Keyboard.pressKey(keymap2[nbKeys]);
    else
      Keyboard.releaseKey(keymap2[nbKeys]);
  }
  for (nbKeys = 0; nbKeys < 4; nbKeys++) {
      // Special Keys
    if (keys_Specials[nbKeys])
      Keyboard.pressKey(keymap3[nbKeys]);
    else
      Keyboard.releaseKey(keymap3[nbKeys]);
  }
	Keyboard.send();
	delay(SWCH_DELAY);

	//	if (coins1 > 0){
	//		char str1[ ] = {CMD_DECREASE_COIN};
	//		this->cmd(board, str1, 1);
	//	}
	if (DEBUG_MODE)
		Serial.println();
}

int* JVS::cmd(char destination, char data[], int size) {
	this->write_packet(destination, data, size);
	char incomingByte;
	while (!_Uart.available()) {
	}
	while (_Uart.read() != 0xE0) {
	} // wait for sync
	while (_Uart.read() != 0) {
	} // only if it's for me
	while (!_Uart.available()) {
	} // wait for length
	int length = _Uart.read();
	Serial.print("Received: E0 0 ");
	Serial.print(length, HEX);
	int counter = 0;
	int * res = (int *) malloc(length * sizeof(int));
	while (counter < length) {
		while (!_Uart.available()) {
		}
		incomingByte = _Uart.read();
		res[counter] = incomingByte;
		// actually do something with incomingByte
		Serial.print(" ");
		Serial.print(res[counter], HEX);
		counter++;
	}
	Serial.println();
	delay(CMD_DELAY);
	return res;
}

void JVS::write_packet(char destination, char data[], int size) {
	_Uart.write(SYNC);
	_Uart.write(destination);
	_Uart.write(size + 1);
	char sum = destination + size + 1;
	for (int i = 0; i < size; i++) {
		if (data[i] == SYNC || data[i] == ESCAPE) {
			_Uart.write(ESCAPE);
			_Uart.write(data[i] - 1);
		} else {
			_Uart.write(data[i]);
		}
		sum = (sum + data[i]) % 256;
	}
	_Uart.write(sum);
	_Uart.flush();
}
//Button debugs
//Serial.print("1P1: ");
//for (int i = 0; i<=7; i++)
//	Serial.print(bitRead(incomingByte, i));
//Serial.println();
