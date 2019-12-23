/*
   JVS.cpp

    Created on: 14/apr/2014
        Author: k4roshi
*/

#include "Arduino.h"
#include "JVS.h"
#include "constants.h"

bool analog;
bool mirroring;
bool full_joy;

/*int keys_Player1[16];   // Touches Joueur 1
  /int keys_Player2[16];   // Touches Joueur 2
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
  KEY_X,              // Button 6
  KEY_C,              // Button 7
  KEY_F               // Button 8
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
  KEY_K,              // Button 6
  KEY_J,              // Button 7
  KEY_L               // Button 8
  };

  const unsigned int keymap3[] = {       // Specials
  KEY_ESC,            // EXIT
  KEY_TAB,            // Menu Mame
  KEY_P,              // Pause
  KEY_ENTER,          // Enter
  };*/

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
  full_joy = false;
  mirroring = false;
  pinMode(6, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  analog = !digitalRead(6);
  mirroring= !digitalRead(5);
  full_joy = !digitalRead(4);

}


void JVS::reset() {
  char str[] = { (char) CMD_RESET, (char) CMD_RESET_ARG };
  this->write_packet(BROADCAST, str, 2);
  delay(ASSIGN_DELAY);
    if (analog)
    Serial.println("Analogique active");
  if (mirroring)
    Serial.println("Mirroring active");
  if (full_joy)
    Serial.println("Full Joystick active");
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
  digitalWrite(11, LOW);
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
  Keyboard.releaseKey(KEY_5);
  Keyboard.releaseKey(KEY_ESC);
  Keyboard.releaseKey(KEY_TAB);
  Keyboard.releaseKey(KEY_P);
  Keyboard.releaseKey(KEY_ENTER);
  Keyboard.releaseKey(KEY_6);
  Keyboard.releaseKey(KEY_1);


  if	(DEBUG_MODE) {
    Serial.print("swthc: E0 0 ");
    Serial.print(length, HEX);
  }
  while (counter < length) {
    while (!_Uart.available()) {
    }
    incomingByte = _Uart.read();
    if (DEBUG_MODE) {
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
            Keyboard.pressKey(KEY_1);
          }
          else
            pressed_smth = false;
        }
        if (shift_mode) {
          //				Serial.println("shiftmode on");
          if (bitRead(incomingByte, 1)) {
            Keyboard.pressKey(KEY_5);
            pressed_smth = true;
          }

          if (bitRead(incomingByte, 2)) {
            Keyboard.pressKey(KEY_TAB);
            pressed_smth = true;
          }

          if (bitRead(incomingByte, 3)) {
            Keyboard.pressKey(KEY_ENTER);
            pressed_smth = true;
          }

          if (bitRead(incomingByte, 4)) {
            Keyboard.pressKey(KEY_P);
            pressed_smth = true;
          }

        } else {
          if (full_joy) {
            Serial.println("On passe bien dans le fulljoy");
            Joystick.button(1, bitRead(incomingByte, 1));
            Joystick.button(2, bitRead(incomingByte, 0));
            if bitRead(incomingByte, 2)
              X_player1 += 511;
            if bitRead(incomingByte, 3)
              X_player1 -= 512;
            Joystick.X(X_player1);
            if bitRead(incomingByte, 4)
              Y_player1 += 511;
            if bitRead(incomingByte, 5)
              Y_player1 -= 512;
            Joystick.Y(Y_player1);
          } else {
            if (analog) {
              Joystick.button(1, bitRead(incomingByte, 1));
              Joystick.button(2, bitRead(incomingByte, 0));
            } else {
              if bitRead(incomingByte, 1)
                Keyboard.pressModifier(KEY_LEFT_CTRL);
              //keys_Player1[0]=1;
              else
                Keyboard.releaseModifier(KEY_LEFT_CTRL);
              //keys_Player1[0]=0;
              if bitRead(incomingByte, 0)
                Keyboard.pressModifier(KEY_LEFT_ALT);
              //keys_Player1[1]=1;
              else
                Keyboard.releaseModifier(KEY_LEFT_ALT);
              //keys_Player1[1]=0;
              if bitRead(incomingByte, 2)
                //keys_Player1[7]=1;
                Keyboard.pressKey(KEY_RIGHT);
              else
                //keys_Player1[7]=0;
                Keyboard.releaseKey(KEY_RIGHT);
              if bitRead(incomingByte, 3)
                //keys_Player1[6]=1;
                Keyboard.pressKey(KEY_LEFT);
              else
                //keys_Player1[6]=0;
                Keyboard.releaseKey(KEY_LEFT);
              if bitRead(incomingByte, 4)
                //keys_Player1[5]=1;
                Keyboard.pressKey(KEY_DOWN);
              else
                //keys_Player1[5]=0;
                Keyboard.releaseKey(KEY_DOWN);
              if bitRead(incomingByte, 5)
                //keys_Player1[4]=1;
                Keyboard.pressKey(KEY_UP);
              else
                //keys_Player1[4]=0;
                Keyboard.releaseKey(KEY_UP);
              //Joystick.button(7,bitRead(incomingByte, 6)); // Service à voir
            }
          }
        }
        break;
      case 4:
        // p1 b2
        if (shift_mode) {

        } else {
          if (analog) {
            Joystick.button(3, bitRead(incomingByte, 7));
          } else {
            if (full_joy) {
              Joystick.button(8, bitRead(incomingByte, 2));
              Joystick.button(7, bitRead(incomingByte, 3));
              Joystick.button(6, bitRead(incomingByte, 4));
              Joystick.button(5, bitRead(incomingByte, 5));
              Joystick.button(4, bitRead(incomingByte, 6));
              Joystick.button(3, bitRead(incomingByte, 7));
            } else {
              if bitRead(incomingByte, 5)
                //keys_Player1[10]=1;
                Keyboard.pressKey(KEY_Z);
              else
                //keys_Player1[10]=0;
                Keyboard.releaseKey(KEY_Z);
              if bitRead(incomingByte, 4)
                //keys_Player1[11]=1;
                Keyboard.pressKey(KEY_X);

              else
                //keys_Player1[11]=0;
                Keyboard.releaseKey(KEY_X);
              if bitRead(incomingByte, 3)
                //keys_Player1[12]=1;`
                Keyboard.pressKey(KEY_C);
              else
                //keys_Player1[12]=0;
                Keyboard.releaseKey(KEY_C);
              if bitRead(incomingByte, 2)
                //keys_Player1[13]=1;
                Keyboard.pressKey(KEY_C);
              else
                //keys_Player1[13]=0;
                Keyboard.releaseKey(KEY_F);
              if bitRead(incomingByte, 7)
                //keys_Player1[8]=1;
                Keyboard.pressKey(KEY_SPACE);
              else
                //keys_Player1[8]=0;
                Keyboard.releaseKey(KEY_SPACE);
              if bitRead(incomingByte, 6)
                Keyboard.pressModifier(KEY_LEFT_SHIFT);
              //keys_Player1[9]=1;
              else
                Keyboard.releaseModifier(KEY_LEFT_SHIFT);
              //keys_Player1[9]=0;
            }
          }
        }
        break;
      case 5:
        // p2 b1
        if (shift_mode) {
          if (bitRead(incomingByte, 7)) {
            Keyboard.pressKey(KEY_ESC);
            pressed_smth = true;
          }
        } else {
          if bitRead(incomingByte, 7)
            //keys_Player2[3]=1;
            Keyboard.pressKey(KEY_2);
          else
            //keys_Player2[3]=0;
            Keyboard.releaseKey(KEY_2);
        }
        if (full_joy) {
          Joystick2.button(1, bitRead(incomingByte, 1));
          Joystick2.button(2, bitRead(incomingByte, 0));
          if bitRead(incomingByte, 2)
            X_player2 += 511;
          if bitRead(incomingByte, 3)
            X_player2 -= 512;
          Joystick2.X(X_player2);
          if bitRead(incomingByte, 4)
            Y_player2 += 511;
          if bitRead(incomingByte, 5)
            Y_player2 -= 512;
          Joystick2.Y(Y_player2);
        } else {
          if (analog) {
            Joystick2.button(1, bitRead(incomingByte, 1));
            Joystick2.button(2, bitRead(incomingByte, 0));
          } else {
            if bitRead(incomingByte, 1)
              //keys_Player2[0]=1;
              Keyboard.pressKey(KEY_A);
            else
              //keys_Player2[0]=0;
              Keyboard.releaseKey(KEY_A);
            if bitRead(incomingByte, 0)
              //keys_Player2[1]=1;
              Keyboard.pressKey(KEY_S);
            else
              //keys_Player2[1]=0;
              Keyboard.releaseKey(KEY_S);
            if bitRead(incomingByte, 2)
              //keys_Player2[7]=1;
              Keyboard.pressKey(KEY_G);
            else
              //keys_Player2[7]=0;
              Keyboard.releaseKey(KEY_G);
            if bitRead(incomingByte, 3)
              //keys_Player2[6]=1;
              Keyboard.pressKey(KEY_D);
            else
              //keys_Player2[6]=0;
              Keyboard.releaseKey(KEY_D);
            if bitRead(incomingByte, 4)
              //keys_Player2[5]=1;
              Keyboard.pressKey(KEY_F);
            else
              //keys_Player2[5]=0;
              Keyboard.releaseKey(KEY_F);
            if bitRead(incomingByte, 5)
              //keys_Player2[4]=1;
              Keyboard.pressKey(KEY_R);
            else
              //keys_Player2[4]=0;
              Keyboard.releaseKey(KEY_R);
            //Joystick2.button(7,bitRead(incomingByte, 6)); //Service
          }
        }
        break;
      case 6:
        // p2 b2
        //if (shift_mode) {

        //} else {
        if (full_joy) {
          Joystick2.button(8, bitRead(incomingByte, 2));
          Joystick2.button(7, bitRead(incomingByte, 3));
          Joystick2.button(6, bitRead(incomingByte, 4));
          Joystick2.button(5, bitRead(incomingByte, 5));
          Joystick2.button(4, bitRead(incomingByte, 6));
          Joystick2.button(3, bitRead(incomingByte, 7));
        } else {
          if (analog) {
            Joystick2.button(3, bitRead(incomingByte, 7));
          } else {
            if bitRead(incomingByte, 7)
              //keys_Player2[8]=1;
              Keyboard.pressKey(KEY_Q);
            else
              //keys_Player2[8]=0;
              Keyboard.releaseKey(KEY_Q);
            if bitRead(incomingByte, 6)
              //keys_Player2[9]=1;
              Keyboard.pressKey(KEY_W);
            else
              //keys_Player2[9]=0;
              Keyboard.releaseKey(KEY_W);
            if bitRead(incomingByte, 5)
              //keys_Player2[10]=1;
              Keyboard.pressKey(KEY_I);
            else
              //keys_Player2[10]=0;
              Keyboard.releaseKey(KEY_I);
            if bitRead(incomingByte, 4)
              //keys_Player2[11]=1;
              Keyboard.pressKey(KEY_K);
            else
              //keys_Player2[11]=0;
              Keyboard.releaseKey(KEY_K);
            if bitRead(incomingByte, 3)
              //keys_Player2[12]=1;
              Keyboard.pressKey(KEY_J);
            else
              //keys_Player2[12]=0;
              Keyboard.releaseKey(KEY_J);
            if bitRead(incomingByte, 2)
              //keys_Player2[13]=1;
              Keyboard.pressKey(KEY_L);
            else
              //keys_Player2[13]=0;
              Keyboard.releaseKey(KEY_L);
          }
        }
        //	}
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
        if (coin1) {
          coin_pressed_at = millis();
          Keyboard.pressKey(KEY_5);
        } else if (coin_pressed_at > 0) {
          if (millis() - coin_pressed_at > 50) {
            coin_pressed_at = 0;
            Keyboard.pressKey(KEY_5);
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
        if (coin2) {
          coin_pressed_at = millis();
          Keyboard.pressKey(KEY_6);
        } else if (coin_pressed_at > 0) {
          if (millis() - coin_pressed_at > 50) {
            coin_pressed_at = 0;
            Keyboard.pressKey(KEY_6);
          }
        }
        break;
      case 13:
        if (analog) { // Analog X P1
            //Serial.print(incomingByte,DEC);
            if (incomingByte < 0) {
              X_player1 = 1024 + (incomingByte * 4);
            } else {
              X_player1 = 4 * incomingByte;
            }
            Joystick.X(X_player1);
        }
        break;
      case 15:
        if (analog) { // Analog Y P1
            if (incomingByte < 0) {
              Y_player1 = 1024 + (incomingByte * 4);
            } else {
              Y_player1 = 4 * incomingByte;
            }
            Joystick.Y(Y_player1);
        }
        break;
      case 17:
        if (analog) { // Analog X P2
            //Serial.print(incomingByte,DEC);
            if (incomingByte < 0) {
              X_player2 = 1024 + (incomingByte * 4);
            } else {
              X_player2 = 4 * incomingByte;
            }
            Joystick2.X(X_player2);
        }
        break;
      case 19:
        if (analog) { // Analog Y P2
            if (incomingByte < 0) {
              Y_player2 = 1024 + (incomingByte * 4);
            } else {
              Y_player2 = 4 * incomingByte;
            }
            Joystick2.Y(Y_player2);
        }
        break;
    }
    counter++;
    if (analog || full_joy) {
      Joystick.send_now();
      Joystick2.send_now();
    }
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
