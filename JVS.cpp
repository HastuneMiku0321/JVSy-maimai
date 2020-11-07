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
  Joystick.button(9, 0);


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
      Serial.print(" ");
      Serial.print(incomingByte, DEC);
    }
      Serial.print(" ");
      Serial.print(incomingByte, HEX);
      Serial.print(" ");
      Serial.print(incomingByte, DEC); 
      if ((int) incomingByte == 0xFFFFFFD0) {
       int escapeByte =  _Uart.read();
        if (escapeByte == 0xDF)
            incomingByte = 0xE0;
    }
    switch (counter) {
      // fourth byte (first three bytes are sync and
      case 2:
      //TEST
      if bitRead(incomingByte, 7)
        Keyboard.pressKey(KEY_F2);
      else
        Keyboard.releaseKey(KEY_F2);
      break;
      case 3:
        // p1 b1
        shift_mode = bitRead(incomingByte, 7);

        if (shift_mode != old_shift) {
          if (shift_mode == 0 && !pressed_smth) {
            //never pressed anything, wants start
              if (full_joy) {
                Joystick.button(9, 1);
              } else {
                Keyboard.pressKey(KEY_1);
              }
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
              else
                Keyboard.releaseModifier(KEY_LEFT_CTRL);
              if bitRead(incomingByte, 0)
                Keyboard.pressModifier(KEY_LEFT_ALT);
              else
                Keyboard.releaseModifier(KEY_LEFT_ALT);
              if bitRead(incomingByte, 2)
                Keyboard.pressKey(KEY_RIGHT);
              else
                Keyboard.releaseKey(KEY_RIGHT);
              if bitRead(incomingByte, 3)
                Keyboard.pressKey(KEY_LEFT);
              else
                Keyboard.releaseKey(KEY_LEFT);
              if bitRead(incomingByte, 4)
                Keyboard.pressKey(KEY_DOWN);
              else
                Keyboard.releaseKey(KEY_DOWN);
              if bitRead(incomingByte, 5)
                Keyboard.pressKey(KEY_UP);
              else
                Keyboard.releaseKey(KEY_UP);
              if bitRead(incomingByte, 6)
                Keyboard.pressKey(KEY_9);
              else
                Keyboard.releaseKey(KEY_9);
              //Joystick.button(7,bitRead(incomingByte, 6)); // Service à voir
            }
          }
        }
        break;
      case 4:
        // p1 b2
        if (shift_mode) {
          if (bitRead(incomingByte, 7)) {
            Keyboard.pressKey(KEY_ESC);
            pressed_smth = true;
          }
        } else {
          if (analog) {
              Joystick.button(8, bitRead(incomingByte, 2));
              Joystick.button(7, bitRead(incomingByte, 3));
              Joystick.button(6, bitRead(incomingByte, 4));
              Joystick.button(5, bitRead(incomingByte, 5));
              Joystick.button(4, bitRead(incomingByte, 6));
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
                Keyboard.pressKey(KEY_Z);
              else
                Keyboard.releaseKey(KEY_Z);
              if bitRead(incomingByte, 4)
                Keyboard.pressKey(KEY_X);
              else
                Keyboard.releaseKey(KEY_X);
              if bitRead(incomingByte, 3)
                Keyboard.pressKey(KEY_C);
              else
                Keyboard.releaseKey(KEY_C);
              if bitRead(incomingByte, 2)
                Keyboard.pressKey(KEY_C);
              else
                Keyboard.releaseKey(KEY_F);
              if bitRead(incomingByte, 7)
                Keyboard.pressKey(KEY_SPACE);
              else
                Keyboard.releaseKey(KEY_SPACE);
              if bitRead(incomingByte, 6)
                Keyboard.pressModifier(KEY_LEFT_SHIFT);
              else
                Keyboard.releaseModifier(KEY_LEFT_SHIFT);
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
           if (full_joy) {
                Joystick2.button(9, bitRead(incomingByte, 7));
              } else {
                if bitRead(incomingByte, 7)
                  Keyboard.pressKey(KEY_2);
                else
                  Keyboard.releaseKey(KEY_2);
                }
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
              Keyboard.pressKey(KEY_A);
            else
              Keyboard.releaseKey(KEY_A);
            if bitRead(incomingByte, 0)
              Keyboard.pressKey(KEY_S);
            else
              Keyboard.releaseKey(KEY_S);
            if bitRead(incomingByte, 2)
              Keyboard.pressKey(KEY_G);
            else
              Keyboard.releaseKey(KEY_G);
            if bitRead(incomingByte, 3)
              Keyboard.pressKey(KEY_D);
            else
              Keyboard.releaseKey(KEY_D);
            if bitRead(incomingByte, 4)
              Keyboard.pressKey(KEY_F);
            else
              Keyboard.releaseKey(KEY_F);
            if bitRead(incomingByte, 5)
              Keyboard.pressKey(KEY_R);
            else
              Keyboard.releaseKey(KEY_R);
            if bitRead(incomingByte, 6)
              Keyboard.pressKey(KEY_0);
            else
              Keyboard.releaseKey(KEY_0);
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
          Joystick2.button(8, bitRead(incomingByte, 2));
          Joystick2.button(7, bitRead(incomingByte, 3));
          Joystick2.button(6, bitRead(incomingByte, 4));
          Joystick2.button(5, bitRead(incomingByte, 5));
          Joystick2.button(4, bitRead(incomingByte, 6));
          Joystick2.button(3, bitRead(incomingByte, 7));
          } else {
            if bitRead(incomingByte, 7)
              Keyboard.pressKey(KEY_Q);
            else
              Keyboard.releaseKey(KEY_Q);
            if bitRead(incomingByte, 6)
              Keyboard.pressKey(KEY_W);
            else
              Keyboard.releaseKey(KEY_W);
            if bitRead(incomingByte, 5)
              Keyboard.pressKey(KEY_I);
            else
              Keyboard.releaseKey(KEY_I);
            if bitRead(incomingByte, 4)
              Keyboard.pressKey(KEY_K);
            else
              Keyboard.releaseKey(KEY_K);
            if bitRead(incomingByte, 3)
              Keyboard.pressKey(KEY_J);
            else
              Keyboard.releaseKey(KEY_J);
            if bitRead(incomingByte, 2)
              Keyboard.pressKey(KEY_L);
            else
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
