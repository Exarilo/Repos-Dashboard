#include <ESP8266WiFi.h>


const char* ssid = "Wifi";
const char* password = "Password";

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Envoyer une réponse HTML de base
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>Ma page Web Arduino</title></head>");
  client.println("<body>");
  client.println("<h1>Bienvenue sur ma page Web Arduino!</h1>");
  client.println("<p>Cette page est servie par votre Arduino.</p>");
  client.println("</body>");
  client.println("</html>");

  delay(1);
  Serial.println("Client déconnecté");
}
