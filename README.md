# Data Plant EKO

This project aims to have a plant pot fitted with electronics capable of monitoring a plant and alerting it’s owned if some parameter they see fit are out of certainty bounds. The captured data from the ambient and the plant is streamed to an MQTT Broker for later treatment if the MQTT option is enabled. This means the plant works as a stand alone unit monitoring and alerting the used via Neopixel lights or it can work in tandem with other systems streaming it’s data and listening for configuration changes

## Variables meassured

The following variables are measured and have a default lower and upper bound indicating the acceptable contritions that the plant need for it’s life to continue:

- Ambient Temperature [ºC]
    - Hardware: DHT11
    - Operating range: 0 - 50
    - Alerting range: <10 or >35


- Ambient Humidity [%]
    - Hardware: DHT11
    - Operating range: 0 - 100
    - Alerting range: <30 or >80


- Plant soil Temperature [ºC]
    - Hardware: NTC 3950
    - Operating range: :-20 - 105
    - Alerting range: <10 or >35


- Plant soil Humidity [%]
    - Hardware: HL-69
    - Operating range: 0 - 100
    - Alerting range: <30 or >80


- Ambient light [Lux]
    - Hardware: TEMP6000
    - Operating range: 0 -1000
    - Alerting range: 500

## Hardware used

![https://descubrearduino.com/wp-content/uploads/2020/06/ESP32-pinout.jpg](https://descubrearduino.com/wp-content/uploads/2020/06/ESP32-pinout.jpg)

The main CPU of the project is the ESP32 a microcontroller with integrated WiFi that is widely used in IoT projects. For fast prototyping and ease of repair the package used is the the Node MCU version of the ESP chip, which has breakout pins that can be easily soldered. 

The hardware used for the sensors is listed above with the corresponding variable measured.

No internal power supply is used in this project but the concept contemplates future improvements such as that. The electronics are powered by a micro usb connected directly to the Node MCU using a usb power plug as power source.

The soil sensors are fitted from under the pot and are isolated using hot glue to prevent water getting into the electronics. The ambient and light sensors are fitted on the side and top of the pot respectively. The Node MCU and Neopixel ring are located a the bottom of the pot. The pot on the other hand is made out of 3D printed transparent PLA which allows the Neopixel ring to light it out from the inside. This STLs files are provided in the Hardware folder of the project.
