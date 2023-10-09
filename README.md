# Derpy-AltiVario-diy-esp8266

![Alt text](/images/1.webp)
![Alt text](/images/2.webp)
![Alt text](/images/2bis.webp)
![Alt text](/images/3.webp)
![Alt text](/images/4.webp)
![Alt text](/images/5.webp)
![Alt text](/images/6.webp)
![Alt text](/images/7.webp)

## first recomendation

after doing this i highly recoment to use a multi core micro controller with a support to real time system like free rtos.

I d'ont recomend to fully copy paste this project

the actual esp8266 cannot have at the same time rtos and support for arduino framowork. Maybe the esp32 is a better choice to do the job

this is why this project is build with the library "cath" (next later)

because of the pinout , buttons cannot be manage by an proper cpu interruption so the buttons are manage with pulling. This is why button can looks laggy

## Generals

this project is mainly based on the cath, for psedo parallel programing on arduino like systems.
https://www.youtube.com/watch?v=aGwHYCcQ3Io
https://github.com/Phildem/Cath

this is NOT made on arduino ide but on platformio with vscode


features : 
- blutooth for xctrack (unsing protocol nmea lk8ex1 with HC-06 Board (8266 cannot handle bluetooth))
- handle date and houre (using DS3231 board)
- handle temp and humidity (Dht11)
- "precise" pressure mesurment (thanks to bmp180)
- display datas with an monochromatic oled screen (sh1106)
- realistic sound (passive buzzer wired up with a transistor , 1k resistor and a diode )
like so https://electronics.stackexchange.com/questions/148335/switching-piezo-buzzer-with-a-transistor
- tree main screen , the main, a short graph / history of altitude, a summary sreen with total value like the total fly time using this vario
- settings and menu
- battery monitor (but not the cleanest, i d'ont take care about battery discharge curve)
- works with 4 AAA RECHARGABLE BATTERY ONLY

note : HC-06 and buzzer are powered directly on the battery + to have close to 4.8v the others are powered with th 3v output pin of esp8266

to prevent Hc-06 from fail start i have added a capacitor near to it

battery monitor use the pin A0 of esp8266 with a 4k / 1k resistor bridge to not going over 1v

for some reason the system can only be programed when the bluettoth switch is turned on.

take care to use the same pinouts as mine because the esp8266 use some of them to know what type os foot to do (programing mode)

the button are not progamed with physical interrupt to they are laggy, use 3 1k pull up resistors

other device are i2c so they ar wired on the same sda scl pins

i include my realthunder freecad link files and stl for 3d printing.
 


