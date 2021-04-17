// EL PRESENTE FUENTE FUE DESARROLLADO POR SEAMAN SRL Y SE 
// ENTREGA SIN GARANTIAS Y A MODO DE PRUEBA UNICAMENTE

// Definimos las librerias a usar en el proyecto
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> 
#include <ArduinoJson.h>

// Agregamos nuestro SSID WiFi y clave
const char* ssid = "SSID WiFi";
const char* password = "Clave Wifi";

// Agregamos los valores de API Token y ChatID entregados por Telegram
// Para obtenerlos deberemos abrir nuestro cliente Telegram tanto sea 
// en nuestro movil o en nuestra PC y buscaremos entre nuestros contactos
// al contacto BotFather.
// Una vez que lo seleccionemos como para iniciar una charla se nos abrirá
// una ventana pero en lugar de un campo de texto para escribir aparecerá un
// botón "INICIAR", al cual le haremos clic. Esto nos iniciara una instancia
// con BotFather el cual nos tirara una serie de comandos disponibles para crear
// o administrar nuestro bot. De ellas deberemos hacer clic o bien escribir el 
// comando /newbot. 
// A continuación nos solicitara que escribamos un nombre para el nuevo bot y
// de estar el nombre disponible nos dirá que le asignemos un nombre publico 
// terminado con '_bot' ejemplo: DEMO100_bot
// Si todo salió bien BotFather nos enviara un mensaje indicando cual es el 
// nombre del bot y cual es nuestro API Token. Haremos clic en el link del 
// chat, y apretaremos el botón iniciar.
// Ya tenemos nuestro API Token por lo que falta obtener el ChatID, para ello
// iremos a la página:
// https://api.telegram.org/bot000000:000000000000/getUpdates en donde 
// 000000:000000000000 es el API Token. De estar todo OK nos abrirá un texto en
// formato JSON con un código 200 y la palabra OK.
// Ahora vamos al chat de Telegram en donde esta nuestro bot y escribiremos cualquier
// cosa, no importa el contenido del mensaje, es solo para actualizar el estatus del 
// chat, hecho esto volveremos a la página antes abierta y daremos refrescar para ver
// que el JSON ahora es diferente, contiene más información, entre ella una clave llamada
// "ID": NUMERO DEL CHAT ID, siendo ese número el que deberemos copiar a continuación.

 
#define BOTtoken "API Token" 
#define CHAT_ID "Chat ID"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Revisa si hay mensajes cada 1 segundo
int botRequestDelay = 100;
unsigned long lastTimeBotRan;

const int ledPin = 2;
bool ledState = LOW;

// Cuando recibe un mensaje
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) 
  {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID)
    {
      bot.sendMessage(chat_id, "USUARIO NO AUTORIZADO", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "BIENVENIDO, " + from_name + ".\n";
      welcome += "USE LOS SIGUIENTES COMANDOS PARA CONTROLAR SUS LUCES.\n\n";
      welcome += "/prender_luz PARA PRENDER LA LUZ \n";
      welcome += "/apagar_luz PARA APAGARLA \n";
      welcome += "/estado PARA SABER EL ESTADO DE LA LUZ \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/prender_luz") {
      bot.sendMessage(chat_id, "PRENDIDO", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/apagar_luz") {
      bot.sendMessage(chat_id, "APAGADO", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/estado") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "LA LUZ ESTA PRENDIDA", "");
      }
      else{
        bot.sendMessage(chat_id, "LA LUZ ESTA APAGADA", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");
    client.setTrustAnchors(&cert); 
  #endif

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  #endif
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("CONECTANDO A SU RED WI-FI..");
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("RESPUESTA OBTENIDA");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
