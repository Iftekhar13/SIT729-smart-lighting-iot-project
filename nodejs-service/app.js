const dns = require("dns");
dns.setServers(["8.8.8.8", "1.1.1.1"]);

require("dotenv").config();

const express = require("express");
const mqtt = require("mqtt");
const { MongoClient } = require("mongodb");

// --------------------
// Express web service
// --------------------
const app = express();
const port = process.env.PORT || 3000;

app.get("/", (req, res) => {
  res.send("Smart Lighting Event Service is running");
});

app.get("/health", (req, res) => {
  res.json({
    status: "ok",
    service: "smart-lighting-event-service"
  });
});

app.listen(port, () => {
  console.log(`HTTP service running on port ${port}`);
});

// --------------------
// MongoDB connection
// --------------------
const mongoClient = new MongoClient(process.env.MONGODB_URI);

let eventsCollection;

async function connectMongoDB() {
  try {
    await mongoClient.connect();

    const database = mongoClient.db("smartLightingDB");
    eventsCollection = database.collection("events");

    console.log("Connected to MongoDB");
  } catch (error) {
    console.log("MongoDB connection error:", error.message);
  }
}

// --------------------
// MQTT connection
// --------------------
const broker = "mqtt://broker.hivemq.com";

const mqttClient = mqtt.connect(broker, {
  clientId: "smart-lighting-node-" + Math.random().toString(16).slice(2, 10),
  clean: true
});

mqttClient.on("connect", () => {
  console.log("Connected to MQTT broker");

  mqttClient.subscribe("smartlighting/#", (err) => {
    if (err) {
      console.log("Subscription error:", err.message);
    } else {
      console.log("Subscribed to smartlighting/#");
    }
  });
});

mqttClient.on("message", async (topic, message) => {
  const value = message.toString();

  console.log(`${topic} -> ${value}`);

  if (eventsCollection) {
    const event = {
      topic: topic,
      value: value,
      timestamp: new Date()
    };

    try {
      await eventsCollection.insertOne(event);
      console.log("Event stored in MongoDB");
    } catch (error) {
      console.log("Database insert error:", error.message);
    }
  }
});

mqttClient.on("error", (error) => {
  console.log("MQTT error:", error.message);
});

// Start MongoDB connection
connectMongoDB();