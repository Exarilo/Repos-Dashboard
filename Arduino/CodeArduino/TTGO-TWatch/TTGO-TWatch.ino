#include <WiFi.h>
#include "config.h"
#include <HTTPClient.h>
#include "CodeSnippet.cpp"
TTGOClass *ttgo;
static lv_obj_t *list;

const char* ssid = "WifiName";
const char* password = "Password";
static void event_handler_list(lv_obj_t *obj, lv_event_t event)
{

}

static void retour_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        createBtCode(); // Recreate the "CODE" button
        lv_obj_del(list); // Delete the "Retour" button
    }
}

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    GitHubRepo repo("Exarilo", "Reusable-Code");

    lv_obj_t *parent = lv_obj_get_parent(obj);
    lv_obj_del(obj); // Delete the clicked button

      // Create a list
    list = lv_list_create(parent, NULL);
    lv_obj_set_size(list, lv_obj_get_width(parent), lv_obj_get_height(parent));
    lv_obj_set_event_cb(list, event_handler_list);

    String* folders = repo.getFolders();
    Serial.print((sizeof folders));
    for (int i=0; i<sizeof folders; i++) {
      lv_obj_t *btn = lv_list_add_btn(list, NULL, folders[i].c_str());
      lv_obj_set_event_cb(btn, event_handler_list);
    }

    // Create the "Retour" button
    lv_obj_t *btn_retour = lv_list_add_btn(list, NULL, "Retour");
    lv_obj_set_event_cb(btn_retour, retour_handler);
  }
}



void createBtCode(){
  lv_obj_t *label;
  lv_obj_t *btn1 = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_LEFT , 10, 10);
  lv_obj_set_size(btn1, 60, 40);
  lv_obj_set_style_local_radius(btn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  label = lv_label_create(btn1, NULL);
  lv_label_set_text(label, "CODE");
  lv_btn_set_checkable(btn1, true);
  lv_btn_toggle(btn1);
  lv_btn_set_fit2(btn1, LV_FIT_NONE, LV_FIT_TIGHT);
  lv_obj_set_event_cb(btn1, event_handler);
}


void setup() {
  Serial.begin(115200);
  ttgo = TTGOClass::getWatch();
  lv_init();
  ttgo->begin();
  ttgo->openBL();
  ttgo->lvgl_begin();
  createBtCode();
  // Connexion au réseau WiFi
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au réseau WiFi...");
  }
  Serial.println("Connecté au réseau WiFi !");
}

void loop() {
  lv_task_handler();
  delay(5);

}



