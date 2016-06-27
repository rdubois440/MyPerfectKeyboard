#define LED_PIN 13

/*
device = 0	keyboard
device = 1 	mouse key
device = 2 	mouse move
*/

int device;
int code;
int value;

int ardCode;
int index;

char msg[64];


void setup() {
  
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(LED_PIN, LOW);
  Keyboard.begin();
  Mouse.begin();

  Serial.begin(9600);  
  Serial1.begin(115200);

  index = 0;
  ///action = 0;
}

void loop() 
{

  char c;

  char* chDevice;
  char* chCode;
  char* chValue;

  unsigned char code;
  int value;
  int device;
 

  while (Serial1.available() > 0) 
  {
    c=Serial1.read();
    //Serial.print("Index -");
    //Serial.print(index);
    //Serial.print("Just read -");
    //Serial.println(c, HEX);


    if (c == 0x0a)
    {
     // Keyboard.press('a');
     // Keyboard.release('a');

      index = 0;
      c = 0;
      Serial.println(msg);

      chDevice = strtok(msg, " ");
      chCode = strtok(NULL, " ");
      chValue = strtok(NULL, " ");

//      code = (chCode[2] - 0x30) * 16 + (chCode[3] - 0x30);
      sscanf(chCode, "%x", &code);


      value = atoi(chValue);
      device = atoi(chDevice);






      Serial.println(chCode);
      Serial.println(chCode[0], HEX);
      Serial.println(chCode[1], HEX);

      Serial.println(chCode[2], HEX);
      Serial.println(chCode[3], HEX);

      Serial.println(code, HEX);
      Serial.println(value, HEX);
      Serial.println("");
      
      if (device == 0)
      {
        if (value >0)
        {
          Keyboard.press(code);
        }
  
        else
        {
          Keyboard.release(code);
        }
      }

      if ((device == 1) && (code == 0) && (value == 1))
      {
        Serial.println("Mouse left button press");
        Mouse.press();
      }

      else if ((device == 1) && (code == 0) && (value == 0))
      {
        Serial.println("Mouse right button press");
        Mouse.release();
      }

      else if (device == 2)
      {
        if (code == 0)
        {
          Mouse.move(value, 0);
        }
        else if (code == 1)
        {
          Mouse.move(0, value);
        }
      }
  
   }
    else
   { 
    msg[index] = c;
    msg[index + 1] = 0;
    
//    Serial.print("index is - ");
//    Serial.print(index);
//    Serial.print(" - msg is now - ");
//    Serial.println(msg);
     index++;
     
   }
  }
   return;
}















