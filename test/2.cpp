#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "CESJT";
const char* password = "itisjtsmg";
WebServer server(80);

const int d7s1[7] = {2,3,4,5,6,7,8};
const int d7s2[7] = {9,10,11,12,13,14,15};

int display[10][7]={
                    {1,1,1,1,1,1,0}, //0
                    {0,1,1,0,0,0,0}, //1
                    {1,1,0,1,1,0,1}, //2
                    {1,1,1,1,0,0,1}, //3
                    {0,1,1,0,0,1,1}, //4
                    {1,0,1,1,0,1,1}, //5
                    {1,0,1,1,1,1,1}, //6
                    {1,1,1,0,0,0,0}, //7
                    {1,1,1,1,1,1,1}, //8
                    {1,1,1,0,0,1,1} //9
                    };


volatile int disp=0;
volatile int num=0;

// pagina
const char pagina_template[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Control Display 7 Segmentos</title>

<style>
    body{
    font-family: Arial, sans-serif;
    background:#f2f2f2;
    display:flex;
    justify-content:center;
    align-items:center;
    height:100vh;
    margin:0;
}

.contenedor{
    background:white;
    padding:25px;
    border-radius:15px;
    box-shadow:0 0 10px rgba(0,0,0,.2);
    text-align:center;
}

.display{
    margin-bottom:20px;
}

.display button{
    width:120px;
    height:45px;
    margin:5px;
    font-size:18px;
    border:none;
    border-radius:8px;
    background:#007bff;
    color:white;
    cursor:pointer;
}

.display button:hover{
    background:#005ec4;
}

.teclado{
    display:grid;
    grid-template-columns:repeat(3,70px);
    gap:10px;
    justify-content:center;
}

.teclado button{
    width:70px;
    height:70px;
    font-size:24px;
    border:none;
    border-radius:10px;
    background:#28a745;
    color:white;
    cursor:pointer;
}

.teclado button:hover{
    background:#218838;
}
</style>
</style>

</head>
<body>

<div class="contenedor">

    <h2>Display de 7 Segmentos</h2>

    <div class="display">
        <a href="/ds1"><button>Display A</button></a>
        <a href="/ds2"><button>Display B</button></a>
    </div>

    <div class="teclado">
        <a href="/1"><button>1</button></a>
        <a href="/2"><button>2</button></a>
        <a href="/3"><button>3</button></a>

        <a href="/4"><button>4</button></a>
        <a href="/5"><button>5</button></a>
        <a href="/6"><button>6</button></a>

        <a href="/7"><button>7</button></a>
        <a href="/8"><button>8</button></a>
        <a href="/9"><button>9</button></a>

        <a href="/0"><button>0</button></a>
    </div>

</div>

</body>
</html>
)rawliteral";

void ds1(){
  for(int i=0;i<7;i++){
    digitalWrite(d7s1[i],display[num][i]);
  }
}

void ds2(){
  for(int i=0;i<7;i++){
    digitalWrite(d7s2[i],display[num][i]);
  }      
}

void setup() {
  Serial.begin(115200);
  for(int i=2;i<9;i++) pinMode(d7s1[i], OUTPUT);
  for(int i=9;i<16;i++)pinMode(d7s2[i], OUTPUT);

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

  server.on("/ds1",[](){
    disp=1;
    server.sendHeader("Location", "/");             //para ds1
    server.send(302, "text/plain", ""); 
  });

  server.on("/ds2",[](){
    disp=2;
    server.sendHeader("Location", "/");             //para ds2
    server.send(302, "text/plain", "");
  });

  server.on("/1",[](){
    num=1;
    server.sendHeader("Location", "/");             //para 1
    server.send(302, "text/plain", "");
  });

  server.on("/2",[](){
    num=2;
    server.sendHeader("Location", "/");             //para 2
    server.send(302, "text/plain", "");
  });

  server.on("/3",[](){
    num=3;
    server.sendHeader("Location", "/");             //para 3
    server.send(302, "text/plain", "");
  });

  server.on("/4",[](){
    num=4;
    server.sendHeader("Location", "/");             //para 4
    server.send(302, "text/plain", "");
  });

  server.on("/5",[](){
    num=5;
    server.sendHeader("Location", "/");             //para 5
    server.send(302, "text/plain", "");
  });

  server.on("/6",[](){
    num=6;
    server.sendHeader("Location", "/");             //para 6
    server.send(302, "text/plain", "");
  });

  server.on("/7",[](){
    num=7;
    server.sendHeader("Location", "/");             //para 7
    server.send(302, "text/plain", "");
  });

  server.on("/8",[](){
    num=8;
    server.sendHeader("Location", "/");             //para 8
    server.send(302, "text/plain", "");
  });

  server.on("/9",[](){
    num=9;
    server.sendHeader("Location", "/");             //para 9
    server.send(302, "text/plain", "");
  });

  server.on("/0",[](){
    num=0;
    server.sendHeader("Location", "/");             //para 0
    server.send(302, "text/plain", "");
  });


  server.begin();
}

void loop() {
  server.handleClient();
  if(disp=1) ds1;
  if(disp=2) ds2; 
}