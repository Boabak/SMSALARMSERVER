//-- Libraries Included --------------------------------------------------------------
  #include <ESP8266WiFi.h>
  #include <EEPROM.h>
//------------------------------------------------------------------------------------
  // Define I/O Pins
  #define     intpin        2         // WIFI Module LED

//------------------------------------------------------------------------------------
  // Authentication Variables
  char*       ssid;              // SERVER WIFI NAME
  char*       password;          // SERVER PASSWORD

//------------------------------------------------------------------------------------
  // WiFi settings
  #define     MAXSC     6           // MAXIMUM NUMBER OF CLIENTS
  #define     rstpin    04
  IPAddress APlocal_IP(192, 168, 0, 100);
  IPAddress APgateway(192, 168, 0, 100);
  IPAddress APsubnet(255, 255, 255, 0);
  IPAddress local_IP(192, 168, 0, 10);

  unsigned int TCPPort = 8888;
 
  WiFiServer  TCP_SERVER(TCPPort);      // THE SERVER AND THE PORT NUMBER
  WiFiClient  TCP_Client;        // THE SERVER CLIENTS Maximum number

//------------------------------------------------------------------------------------
  // Some Variables
  char result[10];
  bool savenvr=false;
  int addr=50;
  String Message ;
  char passwd[64]={};
  int i;
  int len;
  bool firsttime=true,rstbtnvlu=true;
  char x;
  char buf(64);
  unsigned int now;
  //------------------------------------------------Function declare---------------------------------
void IRAM_ATTR fatoryreset(){


 }
void HandleClients();
void SetWifi(char* , char*);
void readnvr();
  //---------------------------------------------------------------------------------

void setup(){
  
  // Setting the serial port
  Serial.begin(115200);           // Computer Communication
  EEPROM.begin(512);
  Serial.println(" ");              //trace
  
  EEPROM.write(0,firsttime);            //برای خواندن ار اپ فعال و برای خواندن از رم غیر فعال   
   Serial.print("0 cell value is: ");         //trace
   Serial.print(EEPROM.read(0));
   Serial.println(" ");
	firsttime=EEPROM.read(0);
  Serial.print("firsttime value is: ");
  Serial.println(firsttime);
  if(!firsttime){                                 //تشخیص دفعه اول یا خیر
    readnvr();

  }
  // Setting the mode of the pins
  //pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light
   
    
  SetWifi("YASAP",passwd);
  Serial.println("password is : ");
  Serial.println(passwd);
                                      //Waite for user to enter DATA
  while(1){
    //while(!savenvr){   
    HandleClients();//}     
    if(savenvr){
        len=Message.length();
        Serial.print("len is :");
        Serial.println(len);
        Serial.println("password is");
      for(i=0;i<(len-58);i++){
          Serial.print(Message[i+58]);
      }
      for(i=0;i<(len-58);i++){
          EEPROM.write(addr+i,Message[i+58]);
         
          }
          Serial.println("password saved");
         readnvr();
		 firsttime = false;

          EEPROM.write(0,firsttime);
          EEPROM.commit();
          Serial.println("All done, password changed, going to restart in 5 sec");
          delay(5000);
		  ESP.restart();

    }
    if(savenvr){
      
      now=millis();
      if((millis()-now)>=1500){
    Serial.println("not save yet. ");                          
    now=millis();}
  }

  }
}

//====================================================================================
 
void loop(){}

//====================================================================================
 
void SetWifi(char* Name, char* Password){
  // Stop any previous WIFI
  WiFi.disconnect();

  // Setting The Wifi Mode
  WiFi.mode(WIFI_AP_STA);
  Serial.println("WIFI Mode : AccessPoint");
  
  // Setting the AccessPoint name & password
  ssid      = Name;
  password  = Password;
   Serial.println("Password is : ");
  Serial.println(Password);
  // Starting the access point
  WiFi.softAPConfig(APlocal_IP, APgateway, APsubnet);                 // softAPConfig (local_ip, gateway, subnet)
  WiFi.softAP(ssid, Password, 1, 0, MAXSC);                           // WiFi.softAP(ssid, password, channel, hidden, max_connection)     
  Serial.println("WIFI < " + String(ssid) + " > ... Started");
   WiFi.config(local_IP, APgateway, APsubnet);
   Serial.print("local IP is :");
   Serial.println(WiFi.localIP());
  // wait a bit
  delay(50);
   
  // getting server IP
  IPAddress IP = WiFi.softAPIP();
  
  // printing the server IP address
  Serial.print("AccessPoint IP : ");
  Serial.println(IP);

  // starting server
  TCP_SERVER.begin();                                                 // which means basically WiFiServer(TCPPort);
  
  Serial.println("Server Started");
  
}

//====================================================================================

void HandleClients(){
unsigned long tNow;
       
  if(TCP_SERVER.hasClient()){
    WiFiClient TCP_Client = TCP_SERVER.available();
    TCP_Client.setNoDelay(1);                                          // enable fast communication
    Serial.println("App is connected ");
    while(1){
      //---------------------------------------------------------------
      // If clients are connected
      //---------------------------------------------------------------
      if(TCP_Client.available()){
        // read the message
        
		 Serial.print("Received packet of size before  ");					//trace code
        						//trace code
		Message = TCP_Client.readStringUntil('\n');
          Serial.println(Message.length());	
        // print the message on the screen
        Serial.print("Received packet of size ");
        Serial.println(Message.length());
          
        // print who sent it
        Serial.print("From ");
        Serial.print(TCP_Client.remoteIP());
        Serial.print(", port ");
        Serial.println(TCP_Client.remotePort());

        // content
        Serial.print("Content: ");
        Serial.println(Message);
			  savenvr=true;
         Serial.print("before returtn ");              //trace code
         return;
        
        // generate a response - current run-time -> to identify the speed of the response
       // tNow=millis();
       // dtostrf(tNow, 8, 0, result);
          
       /* // reply to the client with a message     
        TCP_Client.println(result);                             // important to use println instead of print, as we are looking for a '\r' at the client
        TCP_Client.flush();*/
      }
       
      //---------------------------------------------------------------
      // If clients are disconnected                                            // does not realy work....
      //---------------------------------------------------------------     
      /*if(!TCP_Client || !TCP_Client.connected()){
        // Here We Turn Off The LED To Indicated The Its Disconnectted
        digitalWrite(LED0, LOW);
        break;
      }
      */
    }   
  }
 /* else{
    // the LED blinks if no clients are available
    digitalWrite(LED0, HIGH);
    delay(250);
    digitalWrite(LED0, LOW);
    delay(250);
  }*/
}
void readnvr(){
   int raddr=50;
   Serial.println("READ PASS: ");
   for(int j=0; j<=9;j++){
    passwd[j]=(EEPROM.read(raddr+j));
    
    Serial.write(EEPROM.read(raddr+j));
   }
   Serial.println("readnvr pass 10 = ");
   Serial.write(passwd[10]);
  passwd[10]='\0';

}

