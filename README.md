 
<a href="https://github.com/adricpjw/Wearable-TTGO-Watch"><img src="wearable.gif"></a>

# TTGO Watch PickByLight Wearable

> To provide haptic and visual feedback if incorrect object is picked

[![Build Status](http://img.shields.io/travis/badges/badgerbadgerbadger.svg?style=flat-square)](https://travis-ci.org/badges/badgerbadgerbadger) 

- Compiled and Uploaded via PlatformIO

---

## Table of Contents 

- [Installation](#installation)
- [Usage](#usage)
- [Documentation](#documentation)
- [Tests](#tests)

---

## Installation

- Clone this repo to your local machine using `https://github.com/adricpjw/https://github.com/adricpjw/Wearable-TTGO-Watch`
- Make sure this is in your catkin workspace

### Setup

You can edit and build this project on Arduino or PlatformIO.

#### PlatformIO

1. Install [PlatformIO](Extension) on Visual Studio Code via the Extension Marketplace 

2. After opening PlatformIO Home Page, Choose `Import Arduino Project` and select the folder that contains `Motor.ino`

![](https://i.imgur.com/pO30aVE.png)

3. Alternatively, open this project if it contains `.pio`

4. Lastly, Build and Upload this project to the TTGO T-Watch via MicroUSB connection and by clicking this icon

![](https://i.imgur.com/QRq4rON.png)

---
## Usage

The Wearable works by connecting to a network and listening to an open port on that network via MQTT model

### Setting the WiFi Network
In `Motor.ino`, find the following lines and change it accordingly to adjust the network connection and IP address to listen to:

```C
/* ------------------ WIFI LOGIN ------------------- */
const char* ssid = "BTIA_JEJ1SGP";
const char* password = "4OoMHjCVIjnU";

/* ------------------ MQTT IP ADDRESS ------------------- */
const char* mqtt_server = "192.168.88.221";

```

### Current Usage

Currently, to send a command to the wearable, I am using [mosquitto](https://www.arubacloud.com/tutorial/how-to-install-and-secure-mosquitto-on-ubuntu-20-04.aspx) as a MQTT broker.

After Mosquitto is installed, run the following in the terminal to send a message 
```shell
$ mosquitto_pub -t {topic} -m {message}
```

Alternatively, you can develop a code to publish messages with mosquitto as the broker depending on conditions you give it.

---
## Documentation 

This program requires the following libraries to be installed:
1. [PubSubClient](https://www.arduino.cc/reference/en/libraries/pubsubclient/)
2. [WiFiManager](https://www.arduino.cc/reference/en/libraries/wifimanager/)
3. [TTGO-TWATCH-Library](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library)

Currently, the wearable is configured to display a big red X and 'HDPE' if a '1' is received via MQTT
![](https://i.imgur.com/xr58fpi.jpg)

and Display a Big Green Tick with 'PET' if '0' is received
![](https://i.imgur.com/uXK5zTf.jpg)

---
## Common Issues





---