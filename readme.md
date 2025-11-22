Reprogramming the AT89S51 in the heart lamp


https://www.aliexpress.com/item/1005008809128215.html?spm=a2g0o.order_list.order_list_main.97.3c7d1802EkMpCm

Steps 

1. Purchase
    - the lamp kit
    - addition AT89S51 microcontroller, as this model can be programmed in ciruit
    - two USBasp programmer devices (two because you need one of these to program the other)
    - a 10 pin IDC female connector
    - additional wires at least seven
2. Assemble the kit
3. Solder an IDC female connector to the wires
4. Solder the wire ends to the AT89S51 pins for in circuit serial programming (ISP):
    - MOSI  (master out slave in) is pin 6 on the AT89S51
    - MISO  (master in slave out) is pin 7
    - SCK   (slave clock) is pin 8
    - RST   (reset) is pin 9. Held high to enter ISP mode.
    - VCC   (voltage at the common collector) is pin 40
    - GND   (ground) is pin 20
5. Update the firmware on a USBasp, by using AVRDUDESS or avrdude and the firmware in tools/USBasp
5. Connect the updated USBasp programmer to the IDC connector and a PC USB port
6. Run AVRDUDESS or use avedude on the commandline to update the AT89S51 with the built code in bin/heartleds.hex

