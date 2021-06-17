#include <math.h>
#include <WiFi.h>
const char* ssid     = "enter your SSID";
const char* password = "enter your Password";
WiFiServer server(80);
String header;
String valueString = String(5);
int pos1 = 0;
int pos2 = 0;

//define pins
#define EN34 33
#define O4A 25
#define O3A 26
#define EN12 27
#define O2A 14
#define O1A 12

#define pwmFreq 100
#define pwmLeftChannel 2
#define pwmRightChannel 1
#define pwmRes 8


int rx, ry = 0; //recieved coordinates
int leftspeed = 0;
int rightspeed = 0;

unsigned long lastMillis = 0;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(O4A, OUTPUT);
  pinMode(O3A, OUTPUT);
  pinMode(O2A, OUTPUT);
  pinMode(O1A, OUTPUT);

  //PWM Setup
  ledcSetup(pwmRightChannel, pwmFreq, pwmRes);
  ledcSetup(pwmLeftChannel, pwmFreq, pwmRes);
  ledcAttachPin(EN34, pwmLeftChannel);
  ledcAttachPin(EN12, pwmRightChannel);


  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  WiFiClient client = server.available();   // Listen for incoming clients
}

void loop() {
  client = server.available();
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            printWebsite();

            if (header.indexOf("GET /?M") >= 0) {
              valueString = header.substring(header.indexOf("/?X") + 3, header.indexOf("/X"));
              rx = valueString.toInt();
              //              Serial.print("X ");
              //              Serial.print(valueString);

              valueString = header.substring(header.indexOf("/?Y") + 3, header.indexOf("/Y"));
              ry = valueString.toInt() * -1 ; //the joystick considers down to be positive so we adjust that here
              //              Serial.print("   Y ");
              //              Serial.print(valueString);
              //
              //              Serial.print("            ");
              //              Serial.println(millis() - lastMillis);
              //              lastMillis = millis();

            }
            client.println();
            setMotorsXY(rx, ry);

            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }
}

void setMotorsXY(int x, int y) {
  //lets move things to polar, it will make the motor setting much easier since we really only have to look at theta
  double theta = PI / 2; //default to forward
  double rightSpeed = 0;
  double leftSpeed = 0;
  if (x != 0) {  //just some exception handling here
    if (y > 0) {
      theta =  PI - atan2(y, x * (-1));
    } else if (y < 0) {
      theta = 2 * PI + atan2(y, x);
    }
  } else {
    if (y > 0) {
      theta = PI / 2;
    } else {
      theta = 3 * PI / 2;
    }
  }
  double r = sqrt(x * x + y * y) / 100; //we want this value to be less than 1 for later oprations

  if (theta < PI / 2) {
    leftSpeed = 100 * r;
    rightSpeed = (200 * (2 * theta) / PI - 100) * r;
  } else if (theta < PI) {
    leftSpeed = (100 - 400 * ((theta / PI) - .5)) * r;
    rightSpeed = 100 * r;
  } else if (theta < 3 * PI / 2) {
    leftSpeed = -100 * r;
    rightSpeed = (100 - 400 * ((theta / PI) - 1)) * r;
  } else if (theta < 2 * PI) {
    rightSpeed = -100 * r;
    leftSpeed = ( 400 * ((theta / PI) - 1.5) - 100) * r;
  }

  setMotors((int)rightSpeed, (int) leftSpeed);

}


/*dumb switching function, intended to be used with other timing code
  takes values from -100 to 100, with 0 being off
*/
void setMotors(int _right, int _left) {
  int rightVal = abs(_right) ;
  int leftVal = abs(_left);
  //
  Serial.print("Right ");
  Serial.print(_right);
  Serial.print(" Left ");
  Serial.println(_left);

  if (_right > 0) {
    ledcWrite(pwmRightChannel, map(rightVal, 0, 100, 0, 245));
    digitalWrite(O2A, HIGH);
    digitalWrite(O1A, LOW);
  } else if (_right < 0) {
    ledcWrite(pwmRightChannel, map(rightVal, 0, 100, 0, 245));
    digitalWrite(O2A, LOW);
    digitalWrite(O1A, HIGH);
  } else {
    ledcWrite(pwmRightChannel, map(rightVal, 0, 100, 0, 245)); //it'll only get here if its zero anyway
    digitalWrite(O2A, LOW);
    digitalWrite(O1A, LOW);
  }

  if (_left > 0) {
    ledcWrite(pwmLeftChannel, map(leftVal, 0, 100, 0, 245));
    digitalWrite(O4A, LOW);
    digitalWrite(O3A, HIGH);
  } else if (_left < 0) {
    ledcWrite(pwmLeftChannel, map(leftVal, 0, 100, 0, 245));
    digitalWrite(O4A, HIGH);
    digitalWrite(O3A, LOW);
  }
  else {
    ledcWrite(pwmLeftChannel, map(leftVal, 0, 100, 0, 245));
    digitalWrite(O3A, LOW);
    digitalWrite(O4A, LOW);
  }
}

