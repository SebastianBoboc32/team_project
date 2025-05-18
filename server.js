const SERIAL_PORT = "COM6";
const BAUD_RATE = 9600;
const HTTP_PORT = 3000;

import { SerialPort, ReadlineParser } from "serialport";
import express from "express";
import { WebSocketServer } from "ws";

const port = new SerialPort({ path: SERIAL_PORT, baudRate: BAUD_RATE });
const parser = port.pipe(new ReadlineParser({ delimiter: "\n" }));

const app = express();
app.use(express.static("public"));
const server = app.listen(HTTP_PORT, () =>
  console.log(`HTTP → http://localhost:${HTTP_PORT}`)
);

const wss = new WebSocketServer({ server });
wss.on("connection", () => console.log("WebSocket: client connected"));

parser.on("data", (line) => {
  if (!line.startsWith("{")) return;
  wss.clients.forEach((c) => c.readyState === 1 && c.send(line));
  console.log(line);
});
