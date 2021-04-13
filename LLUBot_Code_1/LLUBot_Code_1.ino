// Incluimos las librerías
#include <ESP8266WiFi.h>
#include <DMotor_mod.h>

// Datos de las conexiones
#define ledPin D2
#define voltagePin D6

// Creamos los motores gracias a las librerías
AF_Stepper motorR(256, 1);
AF_Stepper motorL(256, 2);

// Datos físicos del LLUBot
float wheelRadius = 32.0; //in mm
int chosenSpeed = 100;
float chosenLenght = 19.5;
float chosenAngle = 90;
float calculatedSteps = 0;
char chosenDirection = 0; //puede tener datos o no

// Introducimos usuario y contraseña del Wi-Fi
const char* ssid = "";
const char* password = "";

// Inicializamos el servidor web en el puerto 80
WiFiServer server(80);


//*******************************FUNCIONES***************************

// FUNCIÓN 1: FORMAS DE MOVER EL LLUBOT
void movement (char way, float steps){
  
  // Primero se lee la opción que nos marcan
  char way_ = way;
  float steps_ = steps;
  
  // F = Forward, avanzar
  if(way_ == 'F') {
    //Running the LLUBot:
    for(int i = 0; i < steps_; i++){
      motorR.step(1, FORWARD, SINGLE); 
      motorL.step(1, FORWARD, SINGLE); 
    }
  }
  // B = Backward, retroceder
  if(way_ == 'B') {
    //Running the LLUBot:
    for(int i = 0; i < steps_; i++){
      motorR.step(1, BACKWARD, SINGLE); 
      motorL.step(1, BACKWARD, SINGLE); 
    }
  }
  // R = Right, sentido horario
  if(way_ == 'R') {
    //Running the LLUBot:
    for(int i = 0; i < steps_; i++){
      motorR.step(1, BACKWARD, SINGLE); 
      motorL.step(1, FORWARD, SINGLE); 
    }
  }
  // L = Left, sentido anti-horario
  if(way_ == 'L') {
    //Running the LLUBot:
    for(int i = 0; i < steps_; i++){
      motorR.step(1, FORWARD, SINGLE); 
      motorL.step(1, BACKWARD, SINGLE);
    }
  }
  else {
  // No se hace nada en otros casos
  // Se puede incluir que diga error
  }
}

// FUNCIÓN 2: AVANZAR O RETROCEDER
void goStraight (char way, float cm){
  char way_ = way;
  float cm_ = cm;
  int steps = 0;

  // Choosing the cm to introduce as steps in the motor motion:
  steps = cm_ * 256 * 8 / (2 * 3.1416 * wheelRadius / 10);

  // Choosing a direction for the motors:
  movement(way_, steps);
}

// FUNCIÓN 3: GIRAR
void turn (char way, float angle){
  char way_ = way;
  float angle_ = angle;
  int steps = 0;

  //Choosing the cm to introduce as steps in the motor motion:
  steps = angle_ * (256 * 8 / (2 * 3.1416 * wheelRadius / 10)) * 2 * 3.1416 * (16 /2) / 360;

  //Choosing a direction for the motors:
  movement(way_, steps);
}

//***********************************CONFIGURACIÓN*************************************************

void setup() {
  Serial.begin(115200);
  delay(10);
 
  pinMode(voltagePin, INPUT); 
  pinMode(ledPin, OUTPUT);
 
  // Connect to WiFi network
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
 
  // Start the server
  server.begin();
  Serial.println("Server started");


//************IMPORTANTE*************//
 
  // IMPRIMIR LA DIRECCIÓN IP
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

//***********************************//

  //Steppers speed
  motorR.setSpeed(chosenSpeed);
  motorL.setSpeed(chosenSpeed);   
 
}
 
void loop() {
  
  // First read the voltage value for the correct functioning of the LLUBot.
  // Primero leemos el valor del voltaje para el correcto funcionamiento del LLUBot:
  int sensorVal = 1; // digitalRead(voltagePin);

  // Si NO tenemos suficiente batería:
  if (sensorVal == 0) {
    digitalWrite(ledPin, LOW);

    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
      return;
    }
   
    // Wait until the client sends some data
    Serial.println("new client");
    while(!client.available()){
      delay(1);
    }
   
    // Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

 
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.println("<!DOCTYPE html>");
    client.println("<html lang='es'>");
    client.println("  <head>");
    client.println("    <title> LLUBot </title>");
    client.println("    <style>");
    client.println("          body { background-color: #7c05a3; text-align: center; font: Verdana; font-size: 25px; color: white;}");
    client.println("    </style>");
    client.println("  </head>");
    client.println("  <body>");
    client.println("      <p> Low battery, please turn the LLUBot off and charge it completely.</p>");
    client.println("  </body>");
    client.println("</html>");
  }

  // Si SÍ tenemos suficiente batería:
  else {
  //***************************************NO TOCAR***************************//
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
      return;
    }
   
    // Wait until the client sends some data
    Serial.println("new client");
    while(!client.available()){
      delay(1);
    }
   
    // Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();
   
    // Match the request
    int value = 0;
    int mov = 0;
    int pos = request.lastIndexOf('-');

    //******************ALGORITMOS*******************//
  
    if (request.indexOf("/MOTOR=FORWARD") != -1) {
      value = 1;
      request.remove(0,pos+1);
      mov = request.toInt();
      }
    if (request.indexOf("/MOTOR=BACKWARD") != -1) {
      value = 2;
      request.remove(0,pos+1);
      mov = request.toInt();
    }
    if (request.indexOf("/MOTOR=RIGHT") != -1) {
      value = 3;
      request.remove(0,pos+1);
      mov = request.toInt();
    }
    if (request.indexOf("/MOTOR=LEFT") != -1) {
      value = 4;
      request.remove(0,pos+1);
      mov = request.toInt();
    } 
  
   //**************FIN ALGORITMOS****************//


   
 //***************************A PARTIR DE AQUÍ VAMOS A TRABAJAR**********************************************************************************************************************************************************************************  
    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("  <head>");
    client.println("    <title>LLUBot</title>");

//************************************************************** CSS ************************************************************************************************************************************************************//

    client.println("    <style>");
    client.println("      body {background-color: #7c05a3; text-align: center; color: white; font-family: Verdana;}");
    client.println("        .buttonforward {background-color: white; border-radius: 12px; border: none; color: #7c05a3; padding: 50px; text-align: center; text-decoration: none; display: inline-block; font-size: 40px}");
    client.println("        .buttonright {background-color: white; border-radius: 12px; border: none; color: #7c05a3; padding: 50px; text-align: center; text-decoration: none; display: inline-block; font-size: 40px; position: relative; left: 100px;}");
    client.println("        .buttondown {background-color: white; border-radius: 12px; border: none; color: #7c05a3; padding: 50px; text-align: center; text-decoration: none; display: inline-block; font-size: 40px}");
    client.println("        .buttonleft {background-color: white; border-radius: 12px; border: none; color: #7c05a3; padding: 50px; text-align: center; text-decoration: none; display: inline-block; font-size: 40px; position: relative; left: -100px;}");
    client.println("      h1 {font-size: 50px;}");
    client.println("      a {color: black; text-decoration: none}");
    client.println("      body {font-size: 30px;}");
    client.println("    </style>");

//************************************************************* HTLM ************************************************************************************************************************************************************//
    
    client.println("  </head>");
    client.println("  <body>");
    client.println("    <h1> LLUBot en el Laboratorio de Verano de Ciencia y Tecnolog&iacutea de Arona</h1>");
    client.println("    <p> Vamos a mover en equipos el LLUBot hasta la meta amarilla.</p>");
    client.println("    <button class= 'buttonforward'> <a href=\"/MOTOR=FORWARD\"> Avanzar </a></button><br></br>");
    client.println("    <button class= 'buttonleft'> <a href=\"/MOTOR=LEFT\"> Izquierda </a></button>");
    client.println("    <button class= 'buttonright'> <a href=\"/MOTOR=RIGHT\"> Derecha </a></button><br></br>");
    client.println("    <button class= 'buttondown'> <a href=\"/MOTOR=BACKWARD\"> Retroceder </a></button><br></br><br></br>");
    client.println("    <img src='https://luisanalarab.files.wordpress.com/2020/07/mapa-secundaria.png?w=1024' width='700' height='700'>"); 
    client.println("    <p> Ejemplo de ruta:</p>");
    client.println("    <img src='https://luisanalarab.files.wordpress.com/2020/07/ejemplo-ruta.png?w=1024' width='700' height='700'>"); 
    client.println("  </body>");
    client.println("</html>");
 
//************************* FIN DEL SERVIDOR ***************************************************//

    delay(1);
    Serial.println("Client disconnected");
    Serial.println("");
    

//**************ÓRDENES RECIBIDAS**********************//  

    if(value == 1) {
      goStraight('F', chosenLenght);  
    } if(value == 2) {
      goStraight('B', chosenLenght);  
    } if(value == 3) {
      turn('R', chosenAngle);  
    } if(value == 4) {
      turn('L', chosenAngle);  
    } else {
      
    }
  }
}  // FINITE INCANTATEM
