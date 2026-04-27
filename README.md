# Landslide Sensor

This landslide sensor has been developed as part of Project Kurinda, a landslide monitoring network based on simple, affordable open-source (Arduino) sensors and a minimal server back-end. The architecture of the sensor network is described below, followed by a tutorial for setting up a simple version of the network for demonstration purposes.

## System Architecture

The landslide sensor part of Project Kurinda is split up into 4 main categories.

1. Low-cost, distributed landslide sensors based on the Arduino framework.
2. MQTT broker to relay the data from the sensors.
3. Minimal MQTT subscriber to receive the data and store it in a database.
3. Light-weight database server back-end to store sensor readings.
4. Grafana dashboard to plot the data.

### Arduino landslide sensors

Each sensor is based on an Arduino microcontroller with several sensors attached (currently temperature, acceleration, and soil moisture). These are connected using a mesh network (TBD), allowing the sensors to be deployed in remote areas with minimal infrastructure. The mesh network is explored in further detail elsewhere, but for the purposes of this demonstration a simple WiFi network is used instead because it simplifies development.

The Arduino microcontroller monitors the sensor readings and stores the data. Currently, the sensors are measured every 10 seconds and a rolling average over the last minute is calculated. This helps to reduce the impact of random noise on the sensor readings and reduce false positives.

Once a minute, the Arduino connects to the WiFi network and publishes the averaged data from the last minute. After transmitting the data, the Arduino switches off the WiFi module to save power until the next time it needs to transmit.

#### Arduino code

## Data packet format

Each data packet (containing the time-averaged data from the last minute) is a JSON string containing the following data:

- ```sid``` "site ID": _a unique sensor identification number_
- ```ts``` "timestamp": _the timestamp of the data in ISO format_
- ```ac``` "acceleration"
- ```tmp``` "temperature"
- ```sm``` "soil moisture"

The names are heavily abbreviated to minimise the number of characters transmitted by the sensor and hence reduce the transmit energy required.

The format of the JSON string is thus:

```
{
    "sid":<XXX>,
    "ts":<XXX>,
    "ac":<XXX>,
    "tmp":<XXX>,
    "sm":<XXX>
}
```

## Installation instructions

The software for the Arduino sensor was written using VS Code using the PlatformIO plug-in (https://platformio.org/), and can be used by cloning this repo.

## A note on cybersecurity and system resilience

This project is intended to be a baseline, minimal working example of a system which can monitor conditions and detect landslides using sensors on a mesh network. For simplicity of development and public understanding, the system does not currently include any form of user authentication or encryption, and only minimal error checking. Consequenctly, bad actors could easily inject false data to trigger warnings or pollute the database. Nor does it consider system availability or redundancy, meaning that currently a single server issue (even as simple as losing internet connection) could prevent the whole system working at all.

If this demonstration system is to be scaled up and used for real monitoring, suitable precautions should be taken to minimise the likelihood of this occurring. For example, server redundancy and database backup might be a simple first step, followed by user authentication and encryption on the data transmissions to prevent bad actors spoofing the data. However, all these considerations add complexity, cost and effort to the implementation of this system.