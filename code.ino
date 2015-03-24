// _____        __  __          _____                 _             
///  __ \      / _|/ _|        |_   _|               | |            
//| /  \/ ___ | |_| |_ ___  ___  | |_      _____  ___| |_ ___ _ __  
//| |    / _ \|  _|  _/ _ \/ _ \ | \ \ /\ / / _ \/ _ \ __/ _ \ '__| 
//| \__/\ (_) | | | ||  __/  __/ | |\ V  V /  __/  __/ ||  __/ |    
// \____/\___/|_| |_| \___|\___| \_/ \_/\_/ \___|\___|\__\___|_|    
//                                                            V.2.2                                 
//                           
// http://www.twitter.com/coffee_tweeter 

// Pins: 7 & GND
// Don't forget to set IP adress before uploading!
 
//vars: 
int checkpin = 7; 
int state = 0; 
int state_checkpin = LOW; 
 
// twitter / ethernet stuff 
#include <SPI.h> 
#include <Ethernet.h> 
#include <Twitter.h> 

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x17, 0x5A };
byte ip[] = { 192, 168, 1, 177 };
Twitter twitter("<<OAUTH KEY HERE>>"); 
char *berichten[9] = {"The coffee is ready! #coffeeisready", "Fresh coffee! ready to drink! #coffeeisready", "I just made some coffee! #coffeeisready", "Coffee! Come get it while it's hot! #coffeeisready", "Error #418 - I'm a teapot! #coffeeisready", "My java runtime? about 6 minutes... #coffeeisready", "How about a nice cup of joe? #coffeeisready", "fo' shizzle! I'm done with my drizzle! #coffeeisready", "Come get me f*ckers! I'm hot!! #coffeeisready"}; 
long rndnr = 0; 
 
 
void setup() { 
  // init program: 
  Serial.begin(9600); 
  Serial.println("Koffiepot 2.0 init..."); 
 
  //init pin: 
  pinMode(checkpin, INPUT_PULLUP); 
   
  //check status to determine if there is water in the pot or not.   
  state_checkpin = digitalRead(checkpin);   
  if (state_checkpin == LOW) 
  { 
    Serial.println ("There is water in the pot, state = 2");     
    state = 2; 
  }else{ 
    Serial.println ("There is NO water in the pot, state = 1");     
    state = 1; 
  } 
  
//  // attempt a DHCP connection:
//  Serial.println("Attempting to get an IP address using DHCP:");
//  if (!Ethernet.begin(mac)) {
//    // if DHCP fails, start with a hard-coded address:
//    Serial.println("failed to get an IP address using DHCP");
//  }else{
//    Serial.print("My address:");
//    Serial.println(Ethernet.localIP());
//  }
Serial.println("connecting...");
Ethernet.begin(mac,ip);
Serial.println("connected.");
} 
 
void loop() { 
  // make nice for debug 
  Serial.println ("----------------------------------------------------------------------------------------------------"); 
  // read pin   
  state_checkpin = digitalRead(checkpin); 
   
  //check for state   
  switch (state) 
  { 
    case 0: 
      // this should be impossible 
      Serial.println("Something has gone horribly wrong! state = 0 ??!"); 
      break;    
      case 1: 
      // no water in the pot... check if this is still the case       
      if (state_checkpin == LOW) 
      { 
        Serial.println ("Water has been added to the pot, state is now 2"); 
        state = 2; 
      }else{ 
        Serial.println ("Waiting for master to put water in the pot..."); 
      }       
      break;    
      case 2: 
      // there is/was water in the pot... is it still there? 
      if (state_checkpin == LOW) 
      { 
        Serial.println ("There is still water in the pot, waiting for it to go away...");       
      }else{ 
        Serial.println ("Oh noes! the water is gone! I'd better do stuff..."); 
        // #DO STUFF
        delay(50);
        Serial.println("connecting to twitter ...");
        delay(50);        
        randomSeed(analogRead(0)); 
        rndnr = random(0,10);
        char *msg = berichten[rndnr];
        Serial.print("nr: ");
        Serial.println(rndnr);
        Serial.print("Message to say: ");
        Serial.println (msg);
        if (twitter.post(msg)) {           
          int status = twitter.wait();
          if (status == 200) { 
            Serial.println("Tweet sent!.");         
            delay(50);        
            state = 3; 
          } else { 
            Serial.print("failed to send tweet: code "); 
            delay(50);             
            Serial.println(status); 
            delay(50);             
            if (status == 403) // error 403 = 'forbidden', there is an empty message or it maches your last tweet. 
            { 
              Serial.print ("Error 403, trying with a different message"); 
            }else{                
              state = 4; 
            } 
          } 
        } else { 
          Serial.println("connection failed."); 
          delay(50);       
          state = 4; 
        }         
      }       
      break;      
      case 3: 
      // actions done, reset 
      Serial.println("I'm done with my action! time to reset and wait for a new run."); 
      state = 1;     
      break;   
      case 4: 
      // error 
      Serial.println("There was a huge explosion! loads of fire and many dead people!... Nah I'm kiddin', but there WAS an error tho', please check log"); 
      break; 
 } 
  Serial.print("<heartbeat>... state = "); 
  Serial.println (state); 
    // make nice for debug 
  Serial.println ("----------------------------------------------------------------------------------------------------");   
  delay(5000);
} 
