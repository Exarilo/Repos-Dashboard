#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Base64.h>

//-----------------------------------Global------------------------------------
const char* client_id = "myclientid";
const char* client_secret = "myclientsecret";

//-----------------------------------OTHER------------------------------------
BearSSL::WiFiClientSecure client;
HTTPClient http; 
WiFiServer server(80);
int LoopDelay = 1000;
//-----------------------------------BUTTON-----------------------------------
const int buttonPrevious = D5;
const int buttonNext = D1;
const int buttonPlayPause = D2;

int buttonPreviousState = 0;
int buttonNextState = 0;
int buttonPlayPauseState = 0;
bool isStart = true;

//-----------------------------------SPOTIFY----------------------------------
String bearerToken = "";
double expireInSec = 0;
String redirect_uri= "http://" + WiFi.localIP().toString() + "/callback";

void getToken(String code, WiFiClient client) {
  String token_url = "https://accounts.spotify.com/api/token";
  String grant_type = "authorization_code";
  String post_data = "grant_type=" + grant_type +
                     "&code=" + code +
                     "&redirect_uri=" + redirect_uri +
                     "&client_id=" + client_id +
                     "&client_secret=" + client_secret;
                     
  http.begin(client, token_url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int http_code = http.POST(post_data);
  
  String response = http.getString();
  http.end();
  
  Serial.print("HTTP response code: ");
  Serial.println(http_code);
  Serial.print("HTTP response body: ");
  Serial.println(response);
}

void DoPlayPause() {
  String baseUrl = "https://api.spotify.com/v1/me/player/";
  String action = isStart ? "pause" : "play";
  String url = baseUrl + action;
  isStart = !isStart;

  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " + bearerToken);
  http.addHeader("Content-Type", "application/json");
  String put_data = "{}";
  int http_code = http.PUT(put_data);
  String response = http.getString();
  
  Serial.println("Code de réponse HTTP : " + String(http_code));
  Serial.println("Réponse : " + response);
}
void DoNext() {
  String url = "https://api.spotify.com/v1/me/player/next";

  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " + bearerToken);
  http.addHeader("Content-Type", "application/json");
  String post_data = "{}";
  int http_code = http.POST(post_data);
  String response = http.getString();
  
  Serial.println("Code de réponse HTTP : " + String(http_code));
  Serial.println("Réponse : " + response);
}
void DoPrevious() {
  String url = "https://api.spotify.com/v1/me/player/previous";

  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " + bearerToken);
  http.addHeader("Content-Type", "application/json");
  String post_data = "{}";
  int http_code = http.POST(post_data);
  String response = http.getString();
  
  Serial.println("Code de réponse HTTP : " + String(http_code));
  Serial.println("Réponse : " + response);
}

double getExpireInFromJson(const String json) {
  int expires_in_start = json.indexOf("expires_in\":") + 12;
  int expires_in_end = json.indexOf("}", expires_in_start);
  String expires_in_str = json.substring(expires_in_start, expires_in_end);
  int expires_in = expires_in_str.toInt();
  return expires_in;
}

String getTokenFromJson(const String json) {
  int token_start = json.indexOf(":\"") + 2;
  int token_end = json.indexOf("\",", token_start);
  return json.substring(token_start, token_end);
}


String getJsonAuth(String code) {
  String auth = String(client_id) + ":" + String(client_secret);
  String encodedAuth = base64::encode(auth);

  client.setInsecure();// Set the client to verify the SSL certificate
  if (!client.connect("accounts.spotify.com", 443)) {
    Serial.println("Error connecting to Spotify API.");
    Serial.println(client.getLastSSLError());
  }

  Serial.println("--------------------------------------------------------");
  String encodedCode =code;
  String requestBody = "grant_type=authorization_code&code=" + encodedCode + "&redirect_uri=http://"+WiFi.localIP().toString()+"/callback";
  client.print(String("POST ") + "/api/token" + " HTTP/1.1\r\n" +
               "Host: " + "accounts.spotify.com" + "\r\n" +
               "Authorization: Basic " + encodedAuth + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Content-Length: " + String(requestBody.length()) + "\r\n" +
               "\r\n" +
               requestBody);
  Serial.println(""+requestBody);
  
  Serial.println("--------------------------------------------------------");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if(line.indexOf("access_token") > 0)
    {
      return line;
    }
  }

  return "";
}

//-----------------------------------WIFI-------------------------------------
const char* ssid = "Wifi";
const char* password = "Password";


void connectToWiFi() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


//-----------------------------------SERVER-------------------------------------

void startServer() {
  server.begin();
  Serial.println("Server started");
}


String handleClient() {
  WiFiClient client = server.available();
  
  if (client) {
    String request = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        
        if (request.endsWith("\r\n\r\n")) {
          break;
        }
      }
    }
    
    int codeStart = request.indexOf("code=") + 5;
    int codeEnd = request.indexOf(" HTTP");
    String code = request.substring(codeStart, codeEnd);
    
    if (code != "") {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<html><body><h1>Connexion reussie!</h1></body></html>");
    }
    
    Serial.print("Authorization code: ");
    Serial.println(code);
    return code;
  }
  return "";
}

void setup() {
  pinMode(buttonPrevious, INPUT);
  pinMode(buttonNext, INPUT);
  pinMode(buttonPlayPause, INPUT);
  connectToWiFi();
  startServer();
}


void loop() {
  while(bearerToken==""){
    String code = handleClient();
    if(code!=""){
      String jsonAuth = getJsonAuth(code);
      bearerToken = getTokenFromJson(jsonAuth);
      expireInSec = getExpireInFromJson(jsonAuth);
            Serial.println(jsonAuth);
      Serial.println("Authorization token: " + bearerToken);
      client.stop();
    }
  }

  //Serial.println("PlayPause" + String(digitalRead(buttonPlayPause)));
  //Serial.println("Next" + String(digitalRead(buttonNext)));
  //Serial.println("Previous" + String(digitalRead(buttonPrevious)));
  buttonPlayPauseState = digitalRead(buttonPlayPause);
  if (buttonPlayPauseState == HIGH) {
    DoPlayPause(); 
  }

  buttonPreviousState = digitalRead(buttonPrevious);
  if (buttonPreviousState == HIGH) {
    DoPrevious(); 
  }
  buttonNextState = digitalRead(buttonNext);
  if (buttonNextState == HIGH) {
    DoNext(); 
  }
  expireInSec = expireInSec-(LoopDelay/1000);
  
  //Serial.println("Session expire in : " + String(expireInSec));
  delay(LoopDelay);
}