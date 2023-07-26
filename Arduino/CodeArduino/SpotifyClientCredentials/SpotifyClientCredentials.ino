#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <Base64.h>

int LoopDelay = 1000;
// Spotify client credentials
const char* client_id = "myclientid";
const char* client_secret = "myclientsecret";

// Spotify API endpoint and authorization token URL
const char* spotify_api_url = "accounts.spotify.com";
const char* token_url = "/api/token";

// WiFi network
const char* ssid = "Wifi";
const char* password = "Password";

String bearerToken = "";
int expireInSec = 0;
BearSSL::WiFiClientSecure client;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi.");
  client.setInsecure();// Set the client to verify the SSL certificate
}

void loop() {
  if(expireInSec<200){
    String json = getJsonAuth(client_id, client_secret);
    bearerToken = getTokenFromJson(json);
    expireInSec = getExpireInFromJson(json);
    Serial.println("Authorization token: " + bearerToken);
  }
  expireInSec = expireInSec-(LoopDelay/1000);
  //Serial.println("Session expire in : " + String(expireInSec));
  delay(LoopDelay);
}


String getJsonAuth(const char* client_id, const char* client_secret) {
  String auth = String(client_id) + ":" + String(client_secret);
  String encodedAuth = base64::encode(auth);

  if (!client.connect(spotify_api_url, 443)) {
    Serial.println("Error connecting to Spotify API.");
    Serial.println(client.getLastSSLError());
  }

  String requestBody = "grant_type=client_credentials";
  client.print(String("POST ") + token_url + " HTTP/1.1\r\n" +
               "Host: " + spotify_api_url + "\r\n" +
               "Authorization: Basic " + encodedAuth + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Content-Length: " + String(requestBody.length()) + "\r\n" +
               "\r\n" +
               requestBody);

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if(line.indexOf("access_token") > 0)
    {
      return line;
    }
  }

  return "";
}

String getTokenFromJson(const String json) {
  int token_start = json.indexOf(":\"") + 2;
  int token_end = json.indexOf("\",", token_start);
  return json.substring(token_start, token_end);
}

int getExpireInFromJson(const String json) {
  int expires_in_start = json.indexOf("expires_in\":") + 12;
  int expires_in_end = json.indexOf("}", expires_in_start);
  String expires_in_str = json.substring(expires_in_start, expires_in_end);
  int expires_in = expires_in_str.toInt();
  return expires_in;
}