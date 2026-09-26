# Scalable IoT Smart Lighting System

This project implements a scalable IoT-based smart lighting system for residential use. The system uses a simulated ESP32, PIR motion sensor, light sensor and LED to control lighting automatically based on movement and surrounding light conditions.

The project was developed using Wokwi, MQTT, Node-RED, Node.js, MongoDB Atlas and AWS Elastic Beanstalk.

## System Behaviour

The smart lighting logic follows a simple rule:

- If motion is detected and the light sensor value is above 700, the LED turns ON.
- Otherwise, the LED remains OFF.

In this Wokwi simulation, a higher light sensor reading represents a darker condition.

## System Architecture

The main data flow is:

```text
PIR Motion Sensor + Light Sensor
              ↓
           ESP32
              ↓
       Lighting Decision
              ↓
             LED

ESP32
  ↓
HiveMQ MQTT Broker
  ├──→ Node-RED → Dashboard
  │
  └──→ Node.js Event Service → MongoDB Atlas
                           ↓
                AWS Elastic Beanstalk


The ESP32 publishes sensor and LED information through MQTT. Node-RED receives the messages and displays live values on the dashboard. The Node.js event service also receives the MQTT messages and stores the events in MongoDB.
The Node.js service is deployed on AWS Elastic Beanstalk using a load-balanced environment with Auto Scaling configured between one and two instances.
MQTT Topics

The ESP32 publishes data using the following MQTT topics:

smartlighting/motion
smartlighting/light
smartlighting/led

Node-RED and the Node.js service subscribe to:

Wokwi Prototype

The Wokwi simulation contains:
- ESP32
- PIR motion sensor
- light sensor
- LED
- resistor

The ESP32 reads the motion and light values and applies the lighting rule before publishing the values through MQTT.
Node-RED

Node-RED subscribes to the MQTT messages using: smartlighting/#

The incoming messages are separated according to their topics and displayed through a dashboard.
The dashboard shows:

- motion status
- light level
- LED status

The dashboard is used for monitoring only. The lighting decision is performed automatically by the ESP32.
Node.js Event Service

The Node.js event service connects to the MQTT broker and subscribes to the smart-lighting topics.
For each message received, the service:

1. reads the MQTT topic and value;
2. creates an event;
3. adds a timestamp;
4. stores the event in MongoDB.

The service also provides a simple HTTP endpoint so that the deployed application can be checked through AWS Elastic Beanstalk.
MongoDB

MongoDB Atlas is used for persistent event storage.

Each stored event contains information such as:

topic
value
timestamp

The database used by the project is: smartLightingDB

The event collection is: events

Running the Node.js Service

Open a terminal inside the nodejs-service folder.

Install the required packages:

npm install

Create a local .env file inside the same folder.

Add: MONGODB_URI=your_mongodb_connection_string

Start the service using:npm start

When the service is running correctly, the terminal should confirm the HTTP service, MQTT connection and MongoDB connection.
AWS Deployment

The Node.js event service was deployed using AWS Elastic Beanstalk.

The AWS environment was configured as:

- Web server environment
- Load balanced
- Minimum instances: 1
- Maximum instances: 2
- Auto Scaling enabled
- Service role: LabRole
- EC2 instance profile: LabInstanceProfile

A simple HTTP endpoint was used to confirm that the deployed application was running.
Scalability Testing

A burst of HTTP requests was sent to the deployed Elastic Beanstalk endpoint.
AWS monitoring was used to observe:

- request activity
- network traffic
- CPU activity
- response measurements

Auto Scaling was configured so that the environment could increase from one instance to a maximum of two instances when the configured scaling conditions are reached.
Security

The MongoDB connection string is not hard-coded inside app.js.

For local development, the connection string is stored in a .env file.

For AWS deployment, the connection string is provided through the:MONGODB_URI environment property.

The .env file is excluded from GitHub using .gitignore.

The current public AWS endpoint uses HTTP. HTTPS and stronger secret-management methods would be suitable improvements for a production deployment.
Technologies Used

- Wokwi
- ESP32
- PIR Motion Sensor
- Light Sensor
- MQTT
- HiveMQ
- Node-RED
- Node.js
- Express
- MongoDB Atlas
- AWS Elastic Beanstalk
- AWS Auto Scaling
Project Purpose

The project demonstrates how a simple IoT lighting prototype can be extended into a complete event-driven system that includes sensing, communication, live monitoring, event processing, database storage, cloud deployment and scalability.
