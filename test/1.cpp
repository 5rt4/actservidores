#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "CESJT";
const char* password = "itisjtsmg";

const int led[5] = {13,12,14,27,26};

WebServer server(80);

int anim1[5][5]={
                {0,0,0,0,1},
                {0,0,0,1,0},
                {0,0,1,0,0},.
                {0,1,0,0,0},
                {1,0,0,0,0}
                };

int anim2[5][5]={
                {1,0,0,0,0},
                {0,1,0,0,0},
                {0,0,1,0,0},
                {0,0,0,1,0},
                {0,0,0,0,1}
                };

int off[5]={0,0,0,0,0};

volatile bool animacion1 = true;
volatile bool animacion2 = true;
volatile bool apagado = true;
// pagina
const char pagina_template[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset='utf-8'>
  <title>animaciones</title>
  <style>
    body { background-color: black; text-align: center; }
    .titulo { font-family: 'Courier', monospace; font-weight: bold; font-style: italic; font-size: 32px; color: white; }
    .boton { width: 80px; height: 45px; background-color: yellow; color: black; border: 2px solid black; border-radius: 80px; font-size: 14px; font-weight: bold; cursor: pointer; }
  </style>
</head>
<body>
  <h1 class='titulo'>ANIMACIONES DE LEDS</h1>
  <p>
    <a href="/an1"><button class="btn 1">Animacion 1</button></a>
  </p>
  <p>
    <a href="/an2"><button class="btn 2">Animacion 2</button></a>
  </p>
  <p>
    <a href="/off"><button class="btn off">Apagar</button></a>
  </p>
</body>
</html>
)rawliteral";

void anima1(){
  for(int i=0;i<5;i++){
    for(int j=0;j<5;j++){
      if(animacion1=true){
        digitalWrite(led[j],anim1[i][j]);
      }
    }
  }
}
void anima2(){
  for(int i=0;i<5;i++){
    for(int j=0;j<5;j++){
      if(animacion2=true){
        digitalWrite(led[j],anim2[i][j]);
      }
    }
  }
}
void apaga(){
  for(int i=0;i<5;i++){
    for(int j=0;j<5;j++){
      if(apagado=true){
        digitalWrite(led[j],off[j]);
      }
    }
  }
}
void setup() {
  Serial.begin(115200);
  for(int i=0;i<5;i++){
    pinMode(led[i], OUTPUT);
  };

  // conexion al wifi como antes
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int timeout = 20; // 20 intentos de 500ms = 10 segundos
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    Serial.print(".");
    timeout--;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFallo la conexion. Reiniciando...");
    delay(1000);
    ESP.restart();
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("Dirección IP: http://");
  Serial.println(WiFi.localIP());

  server.on("/an1",[](){
    animacion1=1;
    server.sendHeader("Location", "/"); // dice que la página se vaya a /
    server.send(302, "text/plain", ""); // 302 dice que el /toggle no está y que vaya a la locación de antes
  });

  server.on("/an2",[](){
    animacion2=1;
    server.sendHeader("Location", "/"); // dice que la página se vaya a /
    server.send(302, "text/plain", ""); // 302 dice que el /toggle no está y que vaya a la locación de antes
  });

  server.on("/off",[](){
    apagado=1;
    server.sendHeader("Location", "/"); // dice que la página se vaya a /
    server.send(302, "text/plain", ""); // 302 dice que el /toggle no está y que vaya a la locación de antes
  });
  server.begin();
}

void loop() {
  server.handleClient();
  if(animacion1==1) anima1;
  if(animacion2==1) anima2;
  if(apagado==1) apaga;  
}