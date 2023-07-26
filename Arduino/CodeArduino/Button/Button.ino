const int BUTTON_PIN = 7; 
int lastState = HIGH; 
int currentState;   

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  currentState = digitalRead(BUTTON_PIN);

  if(lastState == LOW && currentState == HIGH)
    Serial.println("The state changed from LOW to HIGH");

  lastState = currentState;
}
