/*

This code replies as if it is a car with OBD2 

Any OBD2 standard PID requested gets a reply with a fake value
 
*/

#include "esp32_can.h"     // https://github.com/collin80/esp32_can AND https://github.com/collin80/can_common

#define YELLOW_LED        12  // CAN PACKET RECEIVED
#define BLUE_LED          13  // CAN INIT FAILED
#define CAN_RX_PIN        16  // 13 for RejsaCAN v3.x
#define CAN_TX_PIN        17  // 14 for RejsaCAN v3.x
#define ANALOG_INPUT1     33  // 1, 2, 3, 6 or 7 for RejsaCAN v3.x  
#define ANALOGMAX         4095


#define PIDS_SUPPORT_01_20   	0x00
#define PIDS_SUPPORT_21_40   	0x20
#define PIDS_SUPPORT_41_60   	0x40
#define ENGINE_RPM           	0x0c
#define VEHICLE_SPEED        	0x0d
#define TIMING_ADVANCE       	0x0e
#define AIR_INTAKE_TEMPERATURE  0x0f
#define MAF_AIR_FLOW_RATE       0x10
#define THROTTLE_POSITION    	0x11
#define ENGINE_OIL_TEMPERATURE  0x5c

// add more from https://en.wikipedia.org/wiki/OBD-II_PIDs


void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Booting fake car...");
  
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  
  CAN0.setCANPins((gpio_num_t)CAN_RX_PIN, (gpio_num_t)CAN_TX_PIN);
  if(CAN0.begin()) { 
    Serial.println("CAN0 init OK"); 
  } 
  else { 
    digitalWrite(BLUE_LED, HIGH);
    Serial.println("CAN0 failed"); 
  }
  CAN0.watchFor();
}


void loop() {
  CAN_FRAME incoming;       
  CAN_FRAME outgoing;
  static uint8_t lobyte = 0;
  static uint8_t hibyte = 0;
  
  if (CAN0.read(incoming)) {

    digitalWrite(YELLOW_LED, HIGH);
    Serial.print("REQST ");
    Serial.print(incoming.id, HEX);
    for(uint8_t i=0;i<8;i++) {
      printhex(incoming.data.uint8[i]);
    }
    Serial.println();

    // DEFAULTS AND ONE BYTE DUMMY REPLY    
    outgoing.id = 0x7E8;
    outgoing.length = 8;
    outgoing.extended = 0;
    outgoing.rtr = 0;
    outgoing.data.uint8[0] = 0x03;  // number of bytes
    outgoing.data.uint8[1] = 0x41;
    outgoing.data.uint8[2] = incoming.data.uint8[2];
    outgoing.data.uint8[3] = lobyte;
    outgoing.data.uint8[4] = 0x00;
    outgoing.data.uint8[5] = 0x00;
    outgoing.data.uint8[6] = 0x00;
    outgoing.data.uint8[7] = 0x00;

    // ANALOG INPUT ONE BYTE REPLY, ADD MORE PIDS HERE
    if( incoming.data.uint8[2] == VEHICLE_SPEED ) { 
      outgoing.data.uint8[3] = (uint8_t)((255*analogRead(ANALOG_INPUT1))/ANALOGMAX);
    }

    // TWO BYTE DUMMY REPLY, ADD MORE PIDS HERE
    if( incoming.data.uint8[2] == ENGINE_RPM ) { 
      outgoing.data.uint8[0] = 0x04;  // number of bytes
      outgoing.data.uint8[3] = hibyte;
      outgoing.data.uint8[4] = lobyte;
    }

    // SUPPORTED PIDS REPLY
    if( incoming.data.uint8[2] == PIDS_SUPPORT_01_20 || incoming.data.uint8[2] == PIDS_SUPPORT_21_40 || incoming.data.uint8[2] == PIDS_SUPPORT_41_60 ) { 
      outgoing.data.uint8[0] = 0x06;  // number of bytes
      outgoing.data.uint8[3] = 0xFF;
      outgoing.data.uint8[4] = 0xFF;
      outgoing.data.uint8[5] = 0xFF;
      outgoing.data.uint8[6] = 0xFF;
    }

    Serial.print("REPLY ");
    Serial.print(outgoing.id, HEX);
    for(uint8_t i=0;i<8;i++) {
      printhex(outgoing.data.uint8[i]);
    }
    Serial.println();

    CAN0.sendFrame(outgoing);
    lobyte++;
    if(lobyte==0) {
      hibyte++;
    }
  }
  delay(5);
  digitalWrite(YELLOW_LED, LOW);
}


void printhex(uint8_t val) {
  Serial.print("\t");
  if (val<0x10) {
      Serial.print("0");
  }
  Serial.print(val, HEX);
}
