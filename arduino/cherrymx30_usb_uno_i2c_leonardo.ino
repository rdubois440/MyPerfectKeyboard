#include <Wire.h>


#define LED_PIN 13

int type;
int code;
int value;

int buf[8];
char usb2ascii[256];
int isKeyPressed[256];
int wasKeyPressed[256];



void setup() 
{
  Wire.begin(9);                // Start I2C Bus as a Slave (Device Number 9)
  Wire.onReceive(receiveEvent); // register event
  
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("Program started");
  
  digitalWrite(LED_PIN, LOW);
  Keyboard.begin();
  //Mouse.begin();
  
  ///action = 0;

  for (int i=0; i<256; i++)
  {
    usb2ascii[i] = 0;
    isKeyPressed[i] = 1;
    wasKeyPressed[i] = 1;
  }




  usb2ascii[0x14] = 'q';
  usb2ascii[0x1a] = 'w';
  usb2ascii[0x08] = 'e';
  usb2ascii[0x15] = 'r';
  usb2ascii[0x17] = 't';
  usb2ascii[0x1c] = 'y';
  
  usb2ascii[0x29] = 0xb1;  //escape
  usb2ascii[0x3a] = 0xc2;  //f1
                usb2ascii[0x3b] = 0xc3;  //f2
                usb2ascii[0x3c] = 0xc4;   //f3
                usb2ascii[0x3d] = 0xc5;  //f4
                usb2ascii[0x3e] = 0xc6;  //f5
                usb2ascii[0x3f] = 0xc7;   //f6
                usb2ascii[0x40] = 0xc8;  //f7
                usb2ascii[0x41] = 0xc9;  //f8
                usb2ascii[0x42] = 0xca;  //f9
                usb2ascii[0x43] = 0xcb;  //f10
                usb2ascii[0x44] = 0xce;   //f11 - print screen - 0x46 + 0x88 = 0xce
                usb2ascii[0x45] = KEY_DELETE;  //f12 - delete
                usb2ascii[0x4c] = 0xd4;  //Del



                usb2ascii[0x35] = '`';        //tilde
                usb2ascii[0x1e] = '1';      
                usb2ascii[0x1f] = '2';
                usb2ascii[0x20] = '3';
                usb2ascii[0x21] = '4';
                usb2ascii[0x22] = '5';
                usb2ascii[0x23] = '6';
                usb2ascii[0x24] = '7';
                usb2ascii[0x25] = '8';
                usb2ascii[0x26] = '9';
                usb2ascii[0x27] = '0';
                usb2ascii[0x2d] = '-'; // -
                usb2ascii[0x2e] = '='; // =
                usb2ascii[0x2a] = 0xb2; // backspace

 

                usb2ascii[0x2b] = 0xb3; //tab
                usb2ascii[0x14] = 'q';      
                usb2ascii[0x1a] = 'w';     
                usb2ascii[0x08] = 'e';      
                usb2ascii[0x15] = 'r';       
                usb2ascii[0x17] = 't';       
                usb2ascii[0x1c] = 'y';      
                usb2ascii[0x18] = 'u';      
                usb2ascii[0x0c] = 'i';       
                usb2ascii[0x12] = 'o';      
                usb2ascii[0x13] = 'p';      
                usb2ascii[0x2f] = '[';       
                usb2ascii[0x30] = ']';       
                usb2ascii[0x28] = 0xb0;  //Enter

                //usb2ascii[0x2b] = 0x39;              //Caps Lock - very special
                usb2ascii[0x04] = 'a';      
                usb2ascii[0x16] = 's';      
                usb2ascii[0x07] = 'd';      
                usb2ascii[0x09] = 'f';       
                usb2ascii[0x0a] = 'g';      
                usb2ascii[0x0b] = 'h';     
                usb2ascii[0x0d] = 'j';       
                usb2ascii[0x0e] = 'k';      
                usb2ascii[0x0f] = 'l';        
                usb2ascii[0x33] = ';';       
                usb2ascii[0x34] = '\'';     
                usb2ascii[0x31] = '\\';    
               usb2ascii[0x32] = '\\';    


     //          usb2ascii[0x04] = 0x81;  // left shift
     //           usb2ascii[0x04] = '\\';    
                usb2ascii[0x1D] = 'z';      
                usb2ascii[0x1B] = 'x';      
                usb2ascii[0x06] = 'c';      
                usb2ascii[0x19] = 'v';      
                usb2ascii[0x05] = 'b';      
                usb2ascii[0x11] = 'n';      
                usb2ascii[0x10] = 'm';    
                usb2ascii[0x36] = ',';       
                usb2ascii[0x37] = '.';       
                usb2ascii[0x38] = '/';      
                //usb2ascii[0x38] = 0x85;  // right shift

                usb2ascii[0x65] = 0xed; // App key 


                usb2ascii[0x2c] = ' '; // space      
                 

                usb2ascii[0x52] = 0xda;  // up
                usb2ascii[0x51] = 0xd9;  // down              
                usb2ascii[0x50] = 0xd8;  // left   
                usb2ascii[0x4f] = 0xd7;  // right 


}

void loop() 
{
//  delay(2);
  return;
}

void receiveEvent(int howMany) 
{

  for (int i=0; i<8; i++)
  {
    buf[i] = Wire.read();  
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }

  Serial.println("");


  for (int i=0; i<256; i++)
  {
    wasKeyPressed[i] = isKeyPressed[i];
    isKeyPressed[i] = 0;
  }



    if (buf[0] != 0)
    {
    
      if (buf[0] & 0x1)
        {
        Keyboard.press(KEY_LEFT_CTRL);
        isKeyPressed[KEY_LEFT_CTRL] = 0;
        }

      if (buf[0] & 0x2)
        {
        Keyboard.press(KEY_LEFT_SHIFT);
        isKeyPressed[KEY_LEFT_SHIFT] = 0;
    }
        
      if (buf[0] & 0x4)
        {
          Keyboard.press(KEY_LEFT_ALT);
          isKeyPressed[KEY_LEFT_ALT] = 0;
          }
        
      if (buf[0] & 0x8)
        {
        Keyboard.press(KEY_LEFT_GUI);
        isKeyPressed[KEY_LEFT_GUI] = 0;
        }
        
      if (buf[0] & 0x10)
        {
        Keyboard.press(KEY_RIGHT_CTRL);
        isKeyPressed[KEY_RIGHT_CTRL] = 0;
        }

      if (buf[0] & 0x20)
        {
          Keyboard.press(KEY_RIGHT_SHIFT);
          isKeyPressed[KEY_RIGHT_SHIFT] = 0;
          }
        
      if (buf[0] & 0x40)
        {
          Keyboard.press(KEY_RIGHT_ALT);
          isKeyPressed[KEY_RIGHT_ALT] = 0;
        }
        
      if (buf[0] & 0x80)
        {
        Keyboard.press(KEY_RIGHT_GUI);
        isKeyPressed[KEY_RIGHT_GUI] = 0;
        }
    }
    else
    {
        Keyboard.release(KEY_LEFT_CTRL);
        Keyboard.release(KEY_LEFT_SHIFT);
        Keyboard.release(KEY_LEFT_ALT);
        Keyboard.release(KEY_LEFT_GUI);

        Keyboard.release(KEY_RIGHT_CTRL);
        Keyboard.release(KEY_RIGHT_SHIFT);
        Keyboard.release  (KEY_RIGHT_ALT);
        Keyboard.release(KEY_RIGHT_GUI);
    }


    // u pointer to usb buffer
  for (int u=2; u<8; u++)
  {
    int key = buf[u];

    

    // always check buf[2], but do not check after the first null after buf[2]
/*    if ((key == 0) && (u>2))
    {
      break;
    } */
    if (key != 0)
    {
      Serial.print("Handling index ");
      Serial.print(u, HEX);
      Serial.print(" - Handling key ");
      Serial.println(key, HEX);
    }
    
    if (key != 0)
    {
      isKeyPressed[key] = 1;
    }
  }
    
  for (int i=0; i<256; i++)
  {
    if (isKeyPressed[i] &&  !wasKeyPressed[i])
    {
      Serial.print("New key pressed ");
      Serial.println(i, HEX);
      Keyboard.press(usb2ascii[i]);
      if (i == 0x39)
      {
        Mouse.press();
      }
      if (i == 0x64)
      {
        Mouse.press(MOUSE_RIGHT);
      }
    }

    if (isKeyPressed[i] &&  wasKeyPressed[i])
    {
      Serial.print("New key was pressed and remains pressed" );
      Serial.println(i, HEX);
    }

    if (!isKeyPressed[i] &&  wasKeyPressed[i])
    {
      Serial.print("Key was released " );
      Serial.println(i, HEX);
      Keyboard.release(usb2ascii[i]);
      if (i == 0x39)
      {
        Mouse.release();
      }
      if (i == 0x64)
      {
        Mouse.release(MOUSE_RIGHT);
      }
    }
    
  }
}
    
/*      Serial.print("Pressed keys After : ");
      for (int i=0; i<256; i++)
      {
        if (isKeyPressed[i])
        {
          Serial.print(i, HEX);
          Serial.print(" ");
        }
      }
    Serial.println();
  
    Serial.print("Remain pressed After : ");
    for (int i=0; i<256; i++)
    {
      if (wasKeyPressed[i])
      {
        Serial.print(i, HEX);
        Serial.print(" ");
        //Keyboard.release(usb2ascii[i]);
        Serial.println("Releasing ");
        Serial.println(i, HEX);
//        Keyboard.release(usb2ascii[i]);
        }
      }
      Serial.println();

*/





