# ESP32_OBD2_Emulator
Replies to all OBD2 requests with dummy data

Very simple code that replies to any request it gets with fake data. It's pretty easy to add proper support for specific OBD2 pids in the code, currently all requests apart from ENGINE_RPM and VEHICLE_SPEED just gets a single data byte reply with a slowly increasing dummy value. OBD2 pids that should get more complex replies like multiple bytes or multiple data packets are not handled properly. They all get a single byte reply. It's easy to add proper support though for more pids in the code if you want.  

The value returned for the OBD2 pid VEHICLE_SPEED is taken from an analog input, just connect a potentiometer to the GPIO, VCC and GND and vary the voltage to adjust the value returned for vehicle speed. The idea is that you can add more potentiometers, each one representing other pids like THROTTLE_POSITION, TIMING_ADVANCE and so on.  
  
![reversed OBD2](https://github.com/MagnusThome/ESP32_OBD2_Emulator/assets/32169384/7a6e37de-12e2-4fc4-9d18-ddffeafebf19)
