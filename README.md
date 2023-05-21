# Self-Balancing-Bike

Arduino nano, Nidec 24H motors, MPU6050, 3S 1000 mAh LiPo battery.

Balancing controller can be tuned remotely over bluetooth (but do this only if you know what you are doing).

Example (change K1):

Send p+ (or p+p+p+p+p+p+p+) for increase K1.

Send p- (or p-p-p-p-p-p-p-) for decrease K1.

Remote control over Joy BT Commander app.

<img src="/pictures/bike.jpg" alt="Self balancing bike"/>
<img src="/pictures/schematic.png" alt="Schematic"/>

About schematic:

Battery: 3S1P LiPo (11.1V 500-1200mAh). 

Buzzer: any 5V active buzzer.

Transistor: 2N2222 or similar.

Servo: TowerPro MG995 or similar size.

The voltage regulator 7805 is not the best choice. Use any 5V switch type small regulator.

First connect to controller over bluetooth. You will see a message that you need to calibrate the balancing point. Send c+ from serial monitor. This activate calibrating procedure. Set the bike to balancing point. Hold still when the bike does not fall to either side. Send c- from serial monitor. This will write the offsets to the EEPROM. After calibrating, the bike will begin to balance.

More about this:

https://youtu.be/Je9Y2WaRB6g

If something doesn't work, you can try motors_test. This sketch tests the motors and steering servo. Follow the messages in the serial terminal.

