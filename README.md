# Self-Balancing-Bike

Arduino nano, Nidec 24H motors, MPU6050, 3S 1000 mAh LiPo battery.

Balancing controller can be tuned remotely over bluetooth.

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
 
More about this:

https://youtu.be/Je9Y2WaRB6g

