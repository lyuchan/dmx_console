#include <RotaryEncoder.h>
#include <EEPROM.h>
#include <Arduino.h>

#include <esp_dmx.h>
int transmitPin = 17;
int receivePin = 16;
int enablePin = 21;
dmx_port_t dmxPort = 1;
byte dmx_send_data[DMX_PACKET_SIZE];
uint8_t dmx_console[DMX_PACKET_SIZE] = { 0 };

#define PIN_IN1 23
#define PIN_IN2 22

#define PIN_AIN1 32
#define PIN_AIN2 33

#define ROTARYMIN 0
int ROTARYMAX = 100;

RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
RotaryEncoder encoderA(PIN_AIN1, PIN_AIN2, RotaryEncoder::LatchMode::TWO03);
TaskHandle_t Task1;
int page = 0, pos = 0, oldpos = -1, pos2 = 0;
int ch = 0;
int oldpos2 = 0, Amax = 0;
;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(1024);
  //dmx.initWrite(TOTAL_CHANNELS);
  //dmx.init(512);
  dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);
  dmx_driver_install(dmxPort, DMX_DEFAULT_INTR_FLAGS);

  pinMode(18, INPUT_PULLUP);
  recallsd();
  end_screen();
  delay(1000);
  topage(0);
  delay(2000);
  topage(1);
  setqpicc(0, 2);
  setqpicc(1, 1);

  xTaskCreatePinnedToCore(
    loop2,          // name of the task function
    "buttonCheck",  // name of the task
    10000,          // memory assigned for the task
    NULL,           // parameter to pass if any
    1,              // priority of task, starting from 0(Highestpriority) *IMPORTANT*( if set to 1 and there is no activity in your 2nd loop, it will reset the esp32)
    &Task1,         // Reference name of taskHandle variable
    1);             // choose core (0 or 1)
}

void loop() {
  //loop2();
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
        setnum(1, dmx_console[ch+1]);
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
        encoderA.setPosition(dmx_console[ch+1]);
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
        setnum(1, dmx_console[ch+1]);
      } else if (pos == 1) {
        dmx_console[ch+1] = pos2;
        setnum(1, dmx_console[ch+1]);
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
          dmx_console[j+1] = 0;
        }
      }
    }
  }


  // put your main code here, to run repeatedly:
}

void loop2(void* parameter) {  //void* parameter
  for (;;) {
    dmx_write(dmxPort, dmx_console, DMX_PACKET_SIZE);
    dmx_send(dmxPort, DMX_PACKET_SIZE);
    dmx_wait_sent(dmxPort, DMX_TIMEOUT_TICK);
    //dmx.update();
    //delay(1);
  }
}
void wifiset() {
}
void savesd() {
  saveIntArrayToEEPROM(0, dmx_console, 513);
}
void recallsd() {
  loadIntArrayFromEEPROM(0, dmx_console, 513);
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
void end_screen() {
  Serial.print("\xff\xff\xff");
}
void topage(int p) {
  Serial.print("page " + String(p));
  end_screen();
}
void setqpicc(int q, int pic) {
  Serial.print("q" + String(q) + ".picc=" + String(pic));
  end_screen();
}
void setnum(int n, int val) {
  Serial.print("n" + String(n) + ".val=" + String(val));
  end_screen();
}
//eeprom
void writeIntToEEPROM(int address, uint8_t value) {
  // EEPROM.write(address, (value >> 8) & 0xFF);
  EEPROM.write(address, value);
}

uint8_t readIntFromEEPROM(int address) {
  uint8_t highByte = EEPROM.read(address);
  // int lowByte = EEPROM.read(address + 1);
  return highByte;  //(highByte << 8) | lowByte;
}

void saveIntArrayToEEPROM(int startingAddress, uint8_t* intArray, int arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
    writeIntToEEPROM(startingAddress + i, intArray[i]);
  }
  EEPROM.commit();
}

void loadIntArrayFromEEPROM(int startingAddress, uint8_t* intArray, int arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
    intArray[i] = readIntFromEEPROM(startingAddress + i);
  }
}