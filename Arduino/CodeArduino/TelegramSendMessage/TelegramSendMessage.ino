#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "Wifi";
const char* password = "Password";

//URL de l'API Telegram pour l'envoi de messages
String serverName = "https://api.telegram.org";
String apiKey = "my-key";
String chatId = "5022071202";
String messageText = "text";

void setup() {
  Serial.begin(9600);
  delay(10);

  //connexion WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    //initialisation du client WiFi sécurisé
    WiFiClientSecure client;
    client.setInsecure();

    //initialisation de la requête HTTPS
    HTTPClient http;
    http.begin(client, serverName + "/bot" + apiKey + "/sendMessage");

    //ajout des paramètres de la requête POST
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String requestBody = "chat_id=" + chatId + "&text=" + messageText;

    //envoi de la requête HTTPS
    int httpResponseCode = http.POST(requestBody);

    //vérification de la réponse du serveur
    if (httpResponseCode == 200) {
      String response = http.getString();
      Serial.println("Message envoye avec succes !");
    } else {
      Serial.print("Erreur lors de l'envoi du message. Code de réponse HTTP : ");
      Serial.println(httpResponseCode);
    }

    //libération des ressources
    http.end();
    client.stop();

    //attente de 5 secondes avant d'envoyer une nouvelle requête
    delay(5000);
  }
}
