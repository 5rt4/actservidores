#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "CESJT";
const char* password = "itisjtsmg";
WebServer server(80);

int valorpotenciometro=0;
int valorpwm=0;

const int led=2;
const int pote=26;      //analogica

String texto_on_off="ON", tension;

volatile bool configuracion=false;

// pagina
const char pagina_template[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta http-equiv="refresh" content="1">
<title>Monitor Tensión</title>
<style>
  body {
    background-color: red;
    text-align: center;
  }

  .titulo {
    font-family: Courier, monospace;
    font-weight: bold;
    font-style: italic;
    font-size: 32px;
    text-transform: uppercase;
  }

  .borde {
    width: 170px;
    height: 100px;
    border: 5px solid black;
    border-radius: 18px;
    background-color: black;
    margin: 0 auto;
    display: flex;
    align-items: center;
    justify-content: center;
  }

  .pantalla {
    width: 160px;
    height: 90px;
    border: 1px solid black;
    border-radius: 10px;
    background-color: cyan;
    margin: 4px;
    display: flex;
    align-items: center;
    justify-content: center;
  }

  .pantalla.apagada {
    background-color: black;
  }

  .boton {
    width: 80px;
    height: 45px;
    background-color: yellow;
    color: black;
    border: 2px solid black;
    border-radius: 80px;
    margin-top: 20px;
  }
</style>
</head>
<body>

  <h1 class="titulo">Monitor Tensión</h1>

  <div class="borde">
    <div class="pantalla" id="pantalla">
      <span id="texto">&nbsp;&nbsp;Tensión: <strong id="valor">__TENSION__</strong>V</span>
    </div>
  </div>

  <a href="/off"><button class="boton" id="botonToggle">__TEXTO_ON_OFF__</button></a>

 
</body>
</html>
)rawliteral";

void off(){
  digitalWrite(led,LOW);
}

void on (){
  analogWrite(led, valorpwm);
}

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(pote, INPUT);

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

  server.on("/", [](){
    String pagina = pagina_template;
    pagina.replace("__TENSION__",tension);
    pagina.replace("__TEXTO_ON_OFF__",texto_on_off);
    server.send(200, "text/html", pagina);
  });

  server.on("/off",[](){
    configuracion=!configuracion;
    server.sendHeader("Location", "/");             //encendido/apagado
    server.send(302, "text/plain", ""); 
  });

  server.begin();
}

void loop() {
  server.handleClient();

  valorpotenciometro = analogRead(pote);
  valorpwm = map(valorpotenciometro, 0, 4095, 0, 255);
  tension = valorpwm;

  if(configuracion == false) {off(); texto_on_off="ON";}
  if(configuracion == true) {on(); texto_on_off="OFF";}
}