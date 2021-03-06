// "/arduino/servo/130"  -> myservo.write(130) on pin 3
// "/arduino/onoff/13" -> digitalRead(13)
// "/arduino/dimmer/130"  -> analogWrite(ledPin, brightness); on pin5

#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

// Listen to the default port 5555, the YUN webserver
// will forward there all the HTTP requests you send
BridgeServer server;

#include <Servo.h>
Servo myservo;
int pos = 0;
void setup() {
  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
  myservo.attach(3);
}

void loop() {
  // Get clients coming from server
  BridgeClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void process(BridgeClient client) {
  // read the command
  String command = client.readStringUntil('/');

  // el comando es onoff, servo o dimmer
  if (command == "onoff") {
    onoff(client);
  }

  if (command == "servo") {
    servoWrite(client);
  }

  if (command == "dimmer") {
    dimmer(client);
  }
}

void onoff(BridgeClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  } else {
    value = digitalRead(pin);
  }

  client.println("Status: 200");
  client.println("Access-Control-Allow-Origin: *");   
  client.println("Access-Control-Allow-Methods: GET");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  // Send feedback to client  
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);

  // Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));
}

void servoWrite(BridgeClient client) {
  int pos;
  pos = client.parseInt();
  myservo.write(pos);

  client.println("Status: 200");
  client.println("Access-Control-Allow-Origin: *");   
  client.println("Access-Control-Allow-Methods: GET");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
}

void dimmer(BridgeClient client) {
  int pin = 5, brightness;
  brightness = client.parseInt();
  analogWrite(pin, brightness);

  client.println("Status: 200");
  client.println("Access-Control-Allow-Origin: *");   
  client.println("Access-Control-Allow-Methods: GET");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
}
