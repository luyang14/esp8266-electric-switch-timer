/*
 * Copyright (c) 2018, Rosen Silva
 * All rights reserved.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Create a WiFi access point and provide a web server on it to control power switch. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <Ticker.h>

Ticker flipper;

//Pin to used as the power switch controller
int RELAY_CONTOL = D3;
int IS_ON = true;

/* Set these to your desired credentials. */
const char *ssid = "FanController";
const char *password = "fan123";

ESP8266WebServer server(80);

/* 
 *  Go to http://192.168.4.1 in a web browser to set the timer for the electric switch
*/

char htmlResponse[3000];

void handleRoot() {

  snprintf ( htmlResponse, 3000,
    "<!DOCTYPE html>\
    <html lang=\"en\">\
    <head>\
    <meta charset=\"utf-8\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
    </head>\
    <body>\
         <h1>Time</h1>\
         <input type='text' name='date_hh' id='date_hh' size=2 autofocus> hh \
         <input type='text' name='date_mm' id='date_mm' size=2 autofocus> mm \
         <input type='text' name='date_ss' id='date_ss' size=2 autofocus> ss \
         <div>\
         <br><button id=\"save_button\">Save</button>\
         </div>\
    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js\"></script>\    
    <script>\
      var hh;\
      var mm;\
      var ss;\
      $('#save_button').click(function(e){\
        e.preventDefault();\
        hh = $('#date_hh').val();\
        mm = $('#date_mm').val();\
        ss = $('#date_ss').val();\        
        $.get('/save?hh=' + hh + '&mm=' + mm + '&ss=' + ss, function(data){\
          console.log(data);\
        });\
      });\      
    </script>\
    </body>\
    </html>"); 

   server.send ( 200, "text/html", htmlResponse );  
}


void handleSave() {
  String hour = "0", minute = "0", second = "0";
  if (server.arg("hh")!= ""){
    // Serial.println("Hours: " + server.arg("hh"));
    hour = server.arg("hh");
  }

  if (server.arg("mm")!= ""){
    // Serial.println("Minutes: " + server.arg("mm"));
    minute = server.arg("mm");
  }

  if (server.arg("ss")!= ""){
    // Serial.println("Seconds: " + server.arg("ss"));
    second = server.arg("ss");
  }
  IS_ON = true;
  
  int totalTime = hour.toInt()*3600 + minute.toInt()*60 + second.toInt(); 
  // Serial.println( "totalTime");
  // Serial.println( totalTime);

  digitalWrite(RELAY_CONTOL, HIGH);
  flipper.attach(totalTime, flip);
}

void flip() {
  if(IS_ON == true){
    Serial.println("CAME FLIP");
    digitalWrite(RELAY_CONTOL, LOW);
    IS_ON = false;
  }
}

void setup() {
   pinMode(RELAY_CONTOL, OUTPUT);
   digitalWrite(RELAY_CONTOL, HIGH);

	delay(1000);
	Serial.begin(115200);
	Serial.println();
	Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", handleRoot);
	server.begin();
	Serial.println("HTTP server started");
}
void loop() {
	server.handleClient();
}
