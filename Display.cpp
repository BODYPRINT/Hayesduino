//Comiler directive won't compile this section if display is disabled in Display.h

#if USE_DISPLAY

#include "Display.h"

#define PORTRAIT  0
#define LANDSCAPE 1
#define TOUCH_ORIENTATION LANDSCAPE

#if (TOUCH_ORIENTATION == LANDSCAPE)
  #define DPWIDTH   320
  #define DPHEIGHT  240
  #define TXMIN     77
  #define TXMAX     901
  #define TYMIN     99
  #define TYMAX     920
#else
  #define DPWIDTH   240
  #define DPHEIGHT  320
  #define TXMIN     901
  #define TXMAX     77
  #define TYMIN     920
  #define TYMAX     99
#endif

#define IP_ADDRESS    0
#define GATEWAY_ADDRESS   1
#define DNS_ADDRESS  2

#define KEY_SIZE          39
#define BUTTON_TEXT_SIZE  2
#define NORMAL_TEXT_SIZE  1

const char *keyLetters[] =    {"A","B","C","D","E","1","2","3"
                              ,"F","G","H","I","J","4","5","6"
                              ,"K","L","M","N","O","7","8","9"
                              ,"P","Q","R","S","T",".","0",":"
                              ,"U","V","W","X","Y","Z"," ","<"};
const char *keyLettersLC[] =  {"a","b","c","d","e","1","2","3"
                              ,"f","g","h","i","j","4","5","6"
                              ,"k","l","m","n","o","7","8","9"
                              ,"p","q","r","s","t",".","0",":"
                              ,"u","v","w","x","y","z"," ","<"};

MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP,YP,XM,YM,300);
TSPoint tp;
byte touchStage=0;
byte keyboardMode=0, addressMode, addIPStart;
int touchX=-1;
int touchY=-1;
byte buttonCount = 0;
uint32_t buttonBounds[42];
byte btnIndex=0;
byte menuID = MENU_MAIN;
bool needTouch = false;
char strInput[40];
char oldInput[40];
byte dispCursorIndex=0;
bool keyShift=false;

void DisplaySetup()
{
  tft.reset();
  tft.begin(DEV_ID);
  tft.setRotation(TOUCH_ORIENTATION);
  tft.fillScreen(BLACK);
  pinMode(13, OUTPUT);

  #if FASTADC
   // set prescale to 2
   // https://forum.arduino.cc/index.php?topic=6549.msg51573#msg51573
   cbi(ADCSRA,ADPS2) ;
   cbi(ADCSRA,ADPS1) ;
   cbi(ADCSRA,ADPS0) ;

  // set ADC to free run mode Mega2560 Datasheet page 287\
  //http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega640-1280-1281-2560-2561-Datasheet-DS40002211A.pdf
  cbi(ADCSRB,ADTS2);
  cbi(ADCSRB,ADTS1);
  cbi(ADCSRB,ADTS0);

  //set higher gain (MUX) for lower bit depth
//  cbi(ADCSRB,MUX5);
//  sbi(ADMUX,MUX4);
//  sbi(ADMUX,MUX3);
//  sbi(ADMUX,MUX2);
//  sbi(ADMUX,MUX1);
//  sbi(ADMUX,MUX0);
//  sbi(ADMUX,REFS1);
//  sbi(ADMUX,REFS0);
  #endif
  
  //Turn off digital inputs
  //DIDR0 = 0x1f;   
    
}

Message(const char *mtext)
{
  tft.fillRect(5,219,309,16,RED);
  tft.drawRect(5,219,309,16,YELLOW);
  tft.setTextSize(NORMAL_TEXT_SIZE);
  tft.setTextColor(YELLOW);
  uint16_t w, h;
  tft.getTextBounds(mtext,5,219,&w,&h,&w,&h);
  tft.setCursor (5 + ((309-w)/2),219 +((16-h)/2));                                            
  tft.setTextColor(WHITE);
  tft.print(mtext);
}

//Biggest buttton size = 255 x 127
//Button Bounds to Long conversion (320Lsh23)+(240Lsh15)+(255Lsh7)+127
void Button(const int px, const int py, const int wd, const int ht, const char *btext)
{
  tft.setTextSize(BUTTON_TEXT_SIZE);

  //Get the text size for center math
  uint16_t w, h;
  tft.getTextBounds(btext,px,py,&w,&h,&w,&h);
  
  tft.fillRect(px,py,wd,ht,BLUE);
  tft.drawRect(px,py,wd,ht,WHITE);
  
  //Center Text
  tft.setCursor (px + ((wd-w)/2),py +((ht-h)/2));                                            
  tft.setTextColor(WHITE);
  tft.print(btext);
  
  buttonBounds[buttonCount]=uint32_t(((uint32_t)px<<23)+((uint32_t)py<<15)+((uint32_t)wd<<7)+ (uint32_t)ht);
  Serial.println(buttonBounds[buttonCount]);
  buttonCount++;
}

void Button(const int px, const int py, const char *btext)
{
  Button(px,py,80,40,btext);
}

DrawEnterShiftButtons()
{
  tft.fillRect(3+KEY_SIZE*6,238-(6*KEY_SIZE),KEY_SIZE,KEY_SIZE,BLUE);
  tft.drawRect(3+KEY_SIZE*6,238-(6*KEY_SIZE),KEY_SIZE,KEY_SIZE,WHITE);
  tft.setTextSize(NORMAL_TEXT_SIZE);
  tft.setTextColor(WHITE);
  if(keyShift)
  {
    //Draw inverted Shift Key
    tft.drawRect(3+KEY_SIZE*7,238-(6*KEY_SIZE),KEY_SIZE,KEY_SIZE,WHITE);
    tft.drawRect(4+KEY_SIZE*7,239-(6*KEY_SIZE),KEY_SIZE-2,KEY_SIZE-2,BLUE);
    tft.fillRect(5+KEY_SIZE*7,240-(6*KEY_SIZE),KEY_SIZE-4,KEY_SIZE-4,WHITE);
    tft.setTextColor(BLUE);
    tft.setCursor(8+KEY_SIZE*7,253-(6*KEY_SIZE));
    tft.print("SHIFT");
    tft.setTextColor(WHITE);
    //Change Enter to "Cancel"
    tft.setCursor(5+KEY_SIZE*6,253-(6*KEY_SIZE));
    tft.print("CANCEL");
  }
  else
  {
    //Draw nomal Shift Key
    tft.fillRect(3+KEY_SIZE*7,238-(6*KEY_SIZE),KEY_SIZE,KEY_SIZE,BLUE);
    tft.drawRect(3+KEY_SIZE*7,238-(6*KEY_SIZE),KEY_SIZE,KEY_SIZE,WHITE);
    tft.setTextColor(WHITE);
    tft.setCursor(8+KEY_SIZE*7,253-(6*KEY_SIZE));
    tft.print("SHIFT");
    //Restore Enter to "Enter"
    tft.setCursor(8+KEY_SIZE*6,253-(6*KEY_SIZE));
    tft.print("ENTER");
  }
}

DrawFullKeyboard()
{  
  for (byte yy=238-(5*KEY_SIZE); yy<239-KEY_SIZE; yy+=KEY_SIZE)
  { 
    for (int xx=3; xx<319-KEY_SIZE ; xx+=KEY_SIZE)
    {
      Button(xx,yy,KEY_SIZE,KEY_SIZE,keyLetters[buttonCount]);
    }
  }
  Button(3+KEY_SIZE*6,238-(6*KEY_SIZE),KEY_SIZE,KEY_SIZE,"");
  Button(3+KEY_SIZE*7,238-(6*KEY_SIZE),KEY_SIZE,KEY_SIZE,"");
  tft.setTextSize(NORMAL_TEXT_SIZE);
  tft.setTextColor(WHITE);
  tft.setCursor(8+KEY_SIZE*6,253-(6*KEY_SIZE));
  tft.print("ENTER");
  tft.setCursor(8+KEY_SIZE*7,253-(6*KEY_SIZE));
  tft.print("SHIFT");
  tft.setTextSize(BUTTON_TEXT_SIZE);
  keyShift=false;
}

void ip2CharArray(IPAddress ip, char* buf) {
  sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

void DisplayMenu()
{
  buttonCount = 0;
  keyboardMode = 0; //Default to no keyboard

  if (menuID == MENU_MAIN)
  {
    tft.fillScreen(BLACK);
    Button(5,5,"Setup");
    Button(5,45,"Dial");
    touchStage=1;
  }
  if (menuID == MENU_SETUP)
  {
    tft.fillScreen(BLACK);
    Button(5,5,200,40,"IP Address");
    Button(5,45,200,40,"Gateway Address");
    Button(5,85,200,40,"DNS Address");
    touchStage=1;
  }
  if (menuID == MENU_IPADDRESS)
  {
    if (addressMode = IP_ADDRESS)addIPStart = LOCAL_IP_1;
    if (addressMode = GATEWAY_ADDRESS)addIPStart = GATEWAY_IP_1;
    if (addressMode = DNS_ADDRESS)addIPStart = DNS_IP_1;
    //Get current IP Address
    ip2CharArray(
    IPAddress(EEPROM.read(addIPStart), EEPROM.read(addIPStart+1),
    EEPROM.read(addIPStart+2), EEPROM.read(addIPStart+3)) , strInput);
    strcpy(oldInput, strInput);
    
    tft.fillScreen(BLACK);
    tft.setTextSize(NORMAL_TEXT_SIZE);
    tft.setTextColor(WHITE);
    tft.setCursor(5,3);
    tft.print("IP ADDRESS");
    DrawFullKeyboard();
    touchStage=2;
    keyboardMode=1; //IP Number Input
    tft.setCursor(5,20);
    tft.print(strInput);
    dispCursorIndex = strlen(strInput);
  }
}

int bx, by, bw, bh;

//void DisplayPrint(const char *btext)
//{
//  tft.println(btext);
//}

int readTouch(void) 
{
//  digitalWrite(13, HIGH);
  tp = ts.getPoint();
//  digitalWrite(13, LOW);
 
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);
  digitalWrite(XM, HIGH);
}

int getPressure(){return tp.z;}
int getTouchStage(){ return touchStage;}
bool NeedTouch(){return needTouch;}

bool checkButtonHit(uint32_t bounds)
{
    bx = bounds>>23;
//    Serial.println(bx);
    by = (bounds>>15)&255;
//    Serial.println(by);
    bw = (bounds>>7)&255;
//    Serial.println(bw);
    bh = bounds&127;
//    Serial.println(bh);

    if(touchX>=bx && touchX<= bx+bw && touchY>=by && touchY<= by+bh)
      return true;
    else
      return false;
}

const uint16_t colorArray[] = {0xDEFF, 0xBDFF,0x9CFF, 0x7BFF, 0x5AFF, 0x39FF, 0x18FF};

byte ProcessKeyboardIP()
{
    if(btnIndex==39)  //BackSpace
    {
    if(dispCursorIndex>0) //Cant delete more than nothing
    {
      dispCursorIndex--;
      strInput[dispCursorIndex]='\0';
      tft.fillRect(5,15,232,20,BLACK);
      tft.setCursor(5,20);
      tft.print(strInput);
    }
    return 0;
  }
  else if(btnIndex==41)  //Shift Key was pressed
  {
    keyShift = !keyShift;
    DrawEnterShiftButtons();
    return 0;
  }
  else if(btnIndex==40)  //Save the value (Hit Enter Key)
  {
    if(keyShift)
      return 2; //Cancel
    else
      return 1; //Enter
  }
  else if(btnIndex == buttonCount) //Not a valid touch
  {
    return 0;
  }
  else  //Everything passes enter the value
  {
    if(dispCursorIndex < 15) //IP Address size limit
    {
      //Only allow IP Address characters
      if((String)keyLetters[btnIndex] >= "." and (String)keyLetters[btnIndex]<="9") 
      {          
        strcat(strInput, keyLetters[btnIndex]);
        dispCursorIndex++;
        tft.setCursor(5,20);
        tft.print(strInput);
      }
    }
    return 0;
  }
}

void processTouch()
{
//  Serial.println(touchStage);
  if(touchStage==0)
  {
    //First hit is a startup dummy value
      needTouch = true;
      touchStage=1;
  }
  else if(touchStage==1)
  {
    // Get the x,y coordinates
    needTouch = true;
    if(tp.z>10)
    {
      touchX = map(tp.y,79,899,0,320);
      touchY = map(tp.x,105,920,0,240);
  
//      btnIndex = 0;
      touchStage=2;
    }    
  }
  else if(touchStage==2)     //Check if we hit a button
  {
    needTouch = false;
    for(btnIndex = 0; btnIndex<buttonCount; btnIndex++)
    {
      bool buttonResult = checkButtonHit(buttonBounds[btnIndex]);
      if(buttonResult)break;
    }
    Serial.println(btnIndex);
    if(btnIndex != buttonCount) //We hit a button before the end    
    {
      Serial.println((String)"Button Index# = " + btnIndex);

      for(byte selectGrow=1;selectGrow<8; selectGrow++)
      {
        tft.drawRect(bx+selectGrow,by+selectGrow,bw-selectGrow*2,bh-selectGrow*2,colorArray[selectGrow-1]);
      }
      touchStage=3;
    }
    else //We missed all the button, go back
    {
      touchStage = 1;
    }
  }
  else if(touchStage==3)    //Wait for release touch
  {
    needTouch = true;
    //Debug(true);
    if(tp.z == 0)
    {
      for(byte selectGrow=1;selectGrow<8; selectGrow++)
      {
        tft.drawRect(bx+selectGrow,by+selectGrow,bw-selectGrow*2,bh-selectGrow*2,BLUE);
      }
      touchStage = 4;
    }
  }
  else if(touchStage==4)    //Process button press
  {    
    needTouch = false;
    if(keyboardMode == 1)
    {
      touchStage=10;
    }
    else if(menuID == MENU_MAIN)
    {
      if(btnIndex == BTN_SETUP)
      {
        menuID=MENU_SETUP;
        DisplayMenu();
      }
    }
    else if(menuID == MENU_SETUP)
    {
      if(btnIndex == BTN_IPADDRESS)
      {
        menuID=MENU_IPADDRESS;
        addressMode = IP_ADDRESS;
        DisplayMenu();
      }
    }
  }
  
  if(touchStage==10)  //Enter IP Address
  {
    byte retResult = ProcessKeyboardIP();
    Serial.println(retResult);
    if(retResult==2) //Cancelled
    {
      menuID = MENU_SETUP;
      DisplayMenu();
      Message("Cancelled! Nothing Saved.");
    }    
    if(retResult==1) //Save
    {
      menuID = MENU_SETUP;
      DisplayMenu();
      if(strcmp(strInput, oldInput))
      {
        Message("Do SAVE routine. Nothing Saved");
      }
      else
      {
        Message("Values were not changed. Nothing Saved");
      }
    }
    touchStage = 1;
  }
}

bool swap=false;
void DisplaySwap(bool LED)
{
  swap=!swap;
  if(LED)
  {
    digitalWrite(LED_BUILTIN,swap);
  }
  else
  {
    if (swap==0)
      tft.fillRect(200,150,50,50,RED);
    else
      tft.fillRect(200,150,50,50,WHITE);
  }
}

byte oldTS;
int oldPress; //, oldX, oldY;
bool oldNeed;
void Debug(bool force)
{
  if(touchStage!=oldTS or force)
  {
    oldTS = touchStage; 
    Serial.println((String)"TouchStage = " + touchStage + "   : Forced = " + force);
  }
  if(needTouch!=oldNeed or force)
  {
    oldNeed = needTouch; 
    Serial.println((String)"Need Touch = " + needTouch + "   : Forced = " + force);
  }
  if(tp.z != oldPress or force)
  {
    oldPress = tp.z;
    Serial.println((String)"Pressure = " + tp.z);// + "   : Forced = " + force);
//    oldX = tp.x;
//    oldY = tp.y;
    Serial.println((String)"Touch(X,Y) = " + tp.x + " , " + tp.y + "   : Forced = " + force);
  }
}

#endif
