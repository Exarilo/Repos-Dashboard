#include "HardwareSerial.h"
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h> 
class GitHubRepo {
public:
  GitHubRepo(String owner, String repo) : _owner(owner), _repo(repo) {}

  String getFiles() {
    WiFiClientSecure wifiClient;
    wifiClient.setInsecure(); // On autorise les connexions non sécurisées
    HttpClient httpClient = HttpClient(wifiClient, _host, _port);
    httpClient.beginRequest();
    Serial.print("/repos/" + _owner + "/" + _repo + "/contents");
    httpClient.get("/repos/" + _owner + "/" + _repo + "/contents");
    httpClient.sendHeader("User-Agent", "Arduino/1.0");
    httpClient.endRequest();

    String response;
    int statusCode = httpClient.responseStatusCode();
    if (statusCode == HTTP_SUCCESS) {
      response = httpClient.responseBody();
    } else {
      Serial.print("Erreur lors de la requête HTTP GET (code d'état ");
      Serial.print(statusCode);
      Serial.println(") :");
      Serial.println(httpClient.responseBody());
    }

    return response;
  }
 String* getFolders() {
  WiFiClientSecure wifiClient;
  wifiClient.setInsecure(); // On autorise les connexions non sécurisées
  HttpClient httpClient = HttpClient(wifiClient, _host, _port);
  httpClient.beginRequest();
  Serial.print("/repos/" + _owner + "/" + _repo + "/contents");
  httpClient.get("/repos/" + _owner + "/" + _repo + "/contents");
  httpClient.sendHeader("User-Agent", "Arduino/1.0");
  httpClient.endRequest();

  String response;
  int statusCode = httpClient.responseStatusCode();
  if (statusCode == 200) {
    response = httpClient.responseBody();
    DynamicJsonDocument jsonDoc(4096); // Créez un objet JSON dynamique de taille suffisante pour la réponse
    DeserializationError jsonError = deserializeJson(jsonDoc, response);
    if (jsonError) {
      Serial.print("Erreur lors de la désérialisation JSON : ");
      Serial.println(jsonError.c_str());
      return NULL; // Renvoyer NULL en cas d'erreur
    }
    JsonArray jsonArray = jsonDoc.as<JsonArray>(); // Transformez le document JSON en tableau
    String* folders = new String[jsonArray.size()]; // Allouer un tableau de chaînes de caractères pour stocker les noms des dossiers
       Serial.println(folders[0]);
    int folderCount = 0;
    for (JsonObject obj : jsonArray) { // Parcourir tous les objets du tableau
      if (obj["type"] == "dir") { // Vérifier si l'objet est un dossier
        String name = obj["name"]; // Récupérer le nom du dossier
        Serial.println(name);
        folders[folderCount++] = name; // Ajouter le nom du dossier au tableau et incrémenter le compteur
      }
    }
    return folders; // Renvoyer le tableau de noms de dossiers
  } else {
    Serial.print("Erreur lors de la requête HTTP GET (code d'état ");
    Serial.print(statusCode);
    Serial.println(") :");
    Serial.println(httpClient.responseBody());
    return NULL; // Renvoyer NULL en cas d'erreur
  }
}
private:
  const char* _host = "api.github.com";
  const int _port = 443;
  String _owner;
  String _repo;
};
