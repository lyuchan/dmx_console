#include <RotaryEncoder.h>
#include <EEPROM.h>

#define PIN_IN1 23
#define PIN_IN2 22

#define PIN_AIN1 32
#define PIN_AIN2 33

#define ROTARYMIN 0
int ROTARYMAX = 100;
int Amax = 255;
int dmx_console[512] = { 0 };
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
RotaryEncoder encoderA(PIN_AIN1, PIN_AIN2, RotaryEncoder::LatchMode::TWO03);
int page = 0, pos = 0, oldpos = -1, pos2 = 0;
int ch = 0;
int oldpos2 = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(1024);
  pinMode(18, INPUT_PULLUP);
  recallsd();
  end();
  delay(1000);
  topage(0);
  delay(2000);
  topage(1);
  setqpicc(0, 2);
  setqpicc(1, 1);
}

void loop() {
  encoder.tick();
  encoderA.tick();
  pos = encoder_loop();
  pos2 = encoderA_loop();
  if (page == 0) {
    ROTARYMAX = 1;
    if (oldpos != pos) {
      oldpos = pos;
      if (pos == 0) {
        setqpicc(0, 2);
        setqpicc(1, 1);
      } else {
        setqpicc(0, 1);
        setqpicc(1, 2);
      }
    }
    if (digitalRead(18) == LOW) {  //sw
      while (digitalRead(18) == LOW) {
        delay(1);
      }
      if (pos == 0) {
        topage(2);
        page = 1;
        setqpicc(0, 4);
        setqpicc(1, 3);
        setqpicc(2, 3);
        encoder.setPosition(0);
      } else {
        topage(3);
        page = 2;
        setqpicc(0, 7);
        setqpicc(1, 6);
        setqpicc(2, 6);
        encoder.setPosition(0);
      }
    }
  } else if (page == 1) {  //setmenu
    ROTARYMAX = 2;
    if (oldpos != pos) {
      oldpos = pos;
      if (pos == 0) {
        setqpicc(0, 4);
        setqpicc(1, 3);
        setqpicc(2, 3);
      } else if (pos == 1) {
        setqpicc(0, 3);
        setqpicc(1, 4);
        setqpicc(2, 3);
      } else {
        setqpicc(0, 3);
        setqpicc(1, 3);
        setqpicc(2, 4);
      }
    }
    if (digitalRead(18) == LOW) {  //sw
      while (digitalRead(18) == LOW) {
        delay(1);
      }
      if (pos == 0) {
        //console
        topage(5);
        page = 4;
        encoder.setPosition(0);
        encoderA.setPosition(ch);
        setnum(0, ch + 1);
        setnum(1, dmx_console[ch]);
        setqpicc(0, 11);
        setqpicc(1, 10);
        setqpicc(2, 10);
      } else if (pos == 1) {
        //artnet
      } else {
        topage(1);
        page = 0;
        encoder.setPosition(0);
        setqpicc(0, 2);
        setqpicc(1, 1);
      }
    }
  } else if (page == 2) {  //sysset
    ROTARYMAX = 2;
    if (oldpos != pos) {
      oldpos = pos;
      if (pos == 0) {
        setqpicc(0, 7);
        setqpicc(1, 6);
        setqpicc(2, 6);
      } else if (pos == 1) {
        setqpicc(0, 6);
        setqpicc(1, 7);
        setqpicc(2, 6);
      } else {
        setqpicc(0, 6);
        setqpicc(1, 6);
        setqpicc(2, 7);
      }
    }
    if (digitalRead(18) == LOW) {  //sw
      while (digitalRead(18) == LOW) {
        delay(1);
      }
      if (pos == 0) {
        //wifiset
        wifiset();
      } else if (pos == 1) {
        //filesys
        topage(4);
        page = 3;
        encoder.setPosition(0);
        setqpicc(0, 9);
        setqpicc(1, 8);
        setqpicc(2, 8);
      } else {
        topage(1);
        page = 0;
        encoder.setPosition(0);
        setqpicc(0, 2);
        setqpicc(1, 1);
      }
    }
  } else if (page == 3) {  //filesys
    ROTARYMAX = 2;
    if (oldpos != pos) {
      oldpos = pos;
      if (pos == 0) {
        setqpicc(0, 9);
        setqpicc(1, 8);
        setqpicc(2, 8);
      } else if (pos == 1) {
        setqpicc(0, 8);
        setqpicc(1, 9);
        setqpicc(2, 8);
      } else {
        setqpicc(0, 8);
        setqpicc(1, 8);
        setqpicc(2, 9);
      }
    }
    if (digitalRead(18) == LOW) {  //sw
      while (digitalRead(18) == LOW) {
        delay(1);
      }
      if (pos == 0) {
        //save
        savesd();
      } else if (pos == 1) {
        recallsd();
        //recall
      } else {
        topage(3);
        page = 2;
        encoder.setPosition(2);
        setqpicc(0, 6);
        setqpicc(1, 6);
        setqpicc(2, 7);
      }
    }
  } else if (page == 4) {  //console
    ROTARYMAX = 3;

    if (oldpos != pos) {
      oldpos = pos;
      if (pos == 0) {

        encoderA.setPosition(ch);
        setqpicc(0, 11);
        setqpicc(1, 10);
        setqpicc(2, 10);
        setqpicc(3, 10);
      } else if (pos == 1) {
        encoderA.setPosition(dmx_console[ch]);
        setqpicc(0, 10);
        setqpicc(1, 11);
        setqpicc(2, 10);
        setqpicc(3, 10);
      } else if (pos == 2) {
        setqpicc(0, 10);
        setqpicc(1, 10);
        setqpicc(2, 11);
        setqpicc(3, 10);
      } else {
        setqpicc(0, 10);
        setqpicc(1, 10);
        setqpicc(2, 10);
        setqpicc(3, 11);
      }
    }
    if (pos == 0) {
      Amax = 511;
    } else {
      Amax = 255;
    }
    if (oldpos2 != pos2) {
      oldpos2 = pos2;
      if (pos == 0) {
        ch = pos2;
        setnum(0, ch + 1);
        setnum(1, dmx_console[ch]);
      } else if (pos == 1) {
        dmx_console[ch] = pos2;
        setnum(1, dmx_console[ch]);
      }
    }
    if (digitalRead(18) == LOW) {  //sw
      while (digitalRead(18) == LOW) {
        delay(1);
      }
      if (pos == 2) {
        topage(2);
        page = 1;
        setqpicc(0, 4);
        setqpicc(1, 3);
        setqpicc(2, 3);
        encoder.setPosition(0);
      }
      if (pos == 3) {
       // encoder.setPosition(0);
       // encoderA.setPosition(0);
        for (int j = 0; j < 512; j++) {
          dmx_console[j] = 0;
        }
      }
    }
  }


  // put your main code here, to run repeatedly:
}
void wifiset() {
}
void savesd() {
  saveIntArrayToEEPROM(0, dmx_console, 512);
}
void recallsd() {
  loadIntArrayFromEEPROM(0, dmx_console, 512);
}

int encoder_loop() {
  int newPos = encoder.getPosition();

  if (newPos < ROTARYMIN) {
    encoder.setPosition(ROTARYMIN);
    newPos = ROTARYMIN;

  } else if (newPos > ROTARYMAX) {
    encoder.setPosition(ROTARYMAX);
    newPos = ROTARYMAX;
  }  // if
  return newPos;
}
int encoderA_loop() {
  int newPos = encoderA.getPosition();

  if (newPos < 0) {
    encoderA.setPosition(0);
    newPos = 0;

  } else if (newPos > Amax) {
    encoderA.setPosition(Amax);
    newPos = Amax;
  }  // if
  return newPos;
}
void end() {
  Serial.print("\xff\xff\xff");
}
void topage(int p) {
  Serial.print("page " + String(p));
  end();
}
void setqpicc(int q, int pic) {
  Serial.print("q" + String(q) + ".picc=" + String(pic));
  end();
}
void setnum(int n, int val) {
  Serial.print("n" + String(n) + ".val=" + String(val));
  end();
}
//eeprom
void writeIntToEEPROM(int address, int value) {
  EEPROM.write(address, (value >> 8) & 0xFF);
  EEPROM.write(address + 1, value & 0xFF);
}

int readIntFromEEPROM(int address) {
  int highByte = EEPROM.read(address);
  int lowByte = EEPROM.read(address + 1);
  return (highByte << 8) | lowByte;
}

void saveIntArrayToEEPROM(int startingAddress, int* intArray, int arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
    writeIntToEEPROM(startingAddress + i * 2, intArray[i]);
  }
   EEPROM.commit();
}

void loadIntArrayFromEEPROM(int startingAddress, int* intArray, int arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
    intArray[i] = readIntFromEEPROM(startingAddress + i * 2);
  }
}