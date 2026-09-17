#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "CESJT";
const char* password = "itisjtsmg";
WebServer server(80);

unsigned long t_delay_lento_rojo = 70000;
unsigned long t_delay_lento_amarillo = 3000;         //milis de lento
unsigned long t_delay_lento_verde = 50000;

unsigned long t_delay_normal_rojo = 45000;
unsigned long t_delay_normal_amarillo = 2000;        //milis de normal
unsigned long t_delay_normal_verde = 65000;

unsigned long t_delay_rapido_rojo = 20000;
unsigned long t_delay_rapido_amarillo = 1000;        //milis de rapido
unsigned long t_delay_rapido_verde = 40000;

unsigned long t_delay_emergencia = 1500;            //milis de emergencia

unsigned long t_previo = 0;
unsigned long t_actual = 0;

int estado=0;                                       //0:rojo | 1:amarillo | 2:verde
bool parpadeo;

const int rojo=2;
const int amarillo=3;
const int verde=4;

volatile int configuracion=0;

// pagina
const char pagina_template[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<title>Semáforo</title>
<style>
  body {
    background-color: white;
    text-align: center;
  }

  .titulo {
    font-family: Impact;
    font-weight: bold;
    text-decoration: underline;
    font-size: 45px;
    text-transform: uppercase;
  }

  .boton {
    width: 200px;
    height: 200px;
    color: black;
    font-size: 23px;
    border: 3px solid black;
    border-radius: 100px;
    display: block;
    margin: 20px auto;
  }

  .lento {
    background-color: red;
  }

  .normal {
    background-color: yellow;
  }

  .rapido {
    background-color: green;
  }

  .emergencia {
    background-color: orange;
  }
</style>
</head>
<body>

  <h1 class="titulo">Semáforo</h1>

  <button class="boton lento" onclick="cambiarModo('Lento')">Lento</button>
  <button class="boton normal" onclick="cambiarModo('Normal')">Normal</button>
  <button class="boton rapido" onclick="cambiarModo('Rápido')">Rápido</button>
  <button class="boton emergencia" onclick="cambiarModo('Emergencia')">Emergencia</button>

  <span>Modo: <strong id="modo">XXX</strong></span>

  <script>
    function cambiarModo(modo) {
      document.getElementById('modo').textContent = modo;
    }
  </script>

</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  pinMode(rojo, OUTPUT);
  pinMode(amarillo, OUTPUT);
  pinMode(verde, OUTPUT);

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
    server.send(200, "text/html", pagina);
  });

  server.on("/l",[](){
    configuracion=1;
    server.sendHeader("Location", "/");             //para lento
    server.send(302, "text/plain", ""); 
  });

  server.on("/n",[](){
    configuracion=2;
    server.sendHeader("Location", "/");             //para normal
    server.send(302, "text/plain", "");
  });

  server.on("r",[](){
    configuracion=3;
    server.sendHeader("Location", "/");             //para rapido
    server.send(302, "text/plain", "");
  });

  server.on("e",[](){
    configuracion=4;
    server.sendHeader("Location", "/");             //para emergencia
    server.send(302, "text/plain", "");
  });

  server.begin();
}

void loop() {
  server.handleClient();
  switch (configuracion) {
    case 1: // lento
            if(estado==0 && t_actual-t_previo>=t_delay_lento_rojo) {
            estado=1;
            t_previo=t_actual;
            } else if (estado==1 && t_actual-t_previo>=t_delay_lento_amarillo) {
            estado=2;
            t_previo=t_actual;
            } else if (estado==2 && t_actual-t_previo>=t_delay_lento_verde) {
            estado=0;
            t_previo=t_actual;
            }
            break;

    case 2: // normal
            if (estado==0 && t_actual-t_previo>=t_delay_normal_rojo) {
            estado=1;
            t_previo=t_actual;
            } else if (estado==1 && t_actual-t_previo>=t_delay_normal_amarillo) {
            estado=2;
            t_previo=t_actual;
            } else if (estado==2 && t_actual-t_previo>=t_delay_normal_verde) {
            estado=0;
            t_previo=t_actual;
            }
            break;

    case 3: // rapido
            if (estado==0 && t_actual-t_previo>=t_delay_rapido_rojo) {
            estado=1;
            t_previo=t_actual;
            } else if (estado==1 && t_actual-t_previo>=t_delay_rapido_amarillo) {
            estado=2;
            t_previo=t_actual;
            } else if (estado==2 && t_actual-t_previo>=t_delay_rapido_verde) {
            estado=0;
            t_previo=t_actual;
            }
            break;

    case 4: // emergencia
            if (t_actual - t_previo >= t_delay_emergencia) {
            t_previo = t_actual;
            parpadeo!=parpadeo;
            if(parpadeo) estado=1;
            else estado=3;
            }
            break;

    default:
            break;
  }
  if(estado==0) digitalWrite(rojo, HIGH);
  if(estado==1) digitalWrite(amarillo, HIGH);
  if(estado==2) digitalWrite(verde, HIGH);
  if(estado==3){
    digitalWrite(rojo, LOW);
    digitalWrite(amarillo, LOW);
    digitalWrite(verde, LOW);
  }
}