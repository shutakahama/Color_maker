#include<Wire.h>
#include<Servo.h>
#include "Adafruit_TCS34725.h"

#define button1 7
#define button2 8
#define button3 9

#define redpin 3
#define greenpin 5
#define bluepin 6

#define commonAnode false
byte gammatable[256];

Servo Servocyan;
Servo Servomagenta;
Servo Servoyellow;
Servo Servoblack;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS,TCS34725_GAIN_4X);
int buttonState1 = 0; //buttonState 押されたら１
int buttonState2 = 0;
int buttonState3 = 0;
int State = 0; //State定義

void setup() {
  //通信の初期設定
  Serial.begin(9600);
  Serial.println("Color View Test!");
  if (tcs.begin()){
    Serial.println("Found sensor");
  }else{
    Serial.println("No TCS34725 found ... check your connctions");
    while(1);
  }

  //ピンの指定
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

  Servocyan.attach(10);
  Servomagenta.attach(11);
  Servoyellow.attach(12);
  Servoblack.attach(13);
  
  //フルカラーLED用LUTの作成（ガンマ補正）
  for (int i=0; i<256; i++){
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if(commonAnode){
      gammatable[i] = 255 - x;
    }else{
      gammatable[i] = x;
    }
  }
}

void loop() {
  uint16_t clear, red, green, blue;
  tcs.setInterrupt(false);
  delay(60);
  
  //カラーセンサーデータの取得
  tcs.getRawData(&red, &green, &blue, &clear);
  tcs.setInterrupt(true);

  //色情報の正規化
  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256; 

  //CMYKへの変換，変換式のまま（100を掛けているのは％表示にするため）
  float c, m, y, k;
  k = 256 - r;
  if(k > 256 - g) k = 256 - g;
  if(k > 256 - b) k = 256 - b;

  c = (256 - k - r); c /= 256-k; c *= 100;
  m = (256 - k - g); m /= 256-k; m *= 100;
  y = (256 - k - b); y /= 256-k; y *= 100;
  k /= 256; k *= 100;
  
  //シリアルモニターへ表示
  Serial.print("R:\t"); Serial.print((int)r);
  Serial.print("\tG:\t"); Serial.print((int)g);
  Serial.print("\tB:\t"); Serial.print((int)b);
  Serial.print("\tC:\t"); Serial.print((int)c);
  Serial.print("\tM:\t"); Serial.print((int)m);
  Serial.print("\tY:\t"); Serial.print((int)y);
  Serial.print("\tK:\t"); Serial.print((int)k);

  Serial.print("\tState:\t"); Serial.print(State);
  Serial.println();

  //ボタン状態の取得
  buttonState1 = digitalRead(button1); 
  buttonState2 = digitalRead(button2); 
  buttonState3 = digitalRead(button3); 

  //フルカラーLEDを発光  
  //analogWrite(redpin, gammatable[(int)r]);
  //analogWrite(greenpin, gammatable[(int)g]);
  //analogWrite(bluepin, gammatable[(int)b]);

  //ボタンを押すと指定したStateに移る
  if(buttonState1 == HIGH){
    State = 1;
    delay(500);
  }
  if(buttonState2 == HIGH){
    State = 2; 
    delay(500);
  }
  if(buttonState3 == HIGH){
    State = 3;
    delay(500);
  }

  //State1になっている間，Proessingとの通信を行う
  if(State == 1){
    analogWrite(redpin, 255);  // 赤色LEDの点灯
    if(Serial.available() > 0) {
      Serial.read();
      //RGB値の送信
      Serial.println(String((int)r) + "," + String((int)g) + "," + String((int)b));
    }
  }else{
    analogWrite(redpin, 0);
  }

  //State2の間，全てのモータが絵の具が流れる位置に回転する
  if(State == 2){
    analogWrite(greenpin, 255);  // 緑色LEDの点灯

    Servocyan.write(10);  //各モータの絵の具を流す角度は実際にやってみて調節
    Servomagenta.write(20);
    Servoyellow.write(170);
    Servoblack.write(150);
    delay(1000);
  }else{
    //State2以外のときは全て絵の具を流さない
    Servocyan.write(180);
    Servomagenta.write(180);
    Servoyellow.write(0);
    Servoblack.write(-10);
    analogWrite(greenpin, 0);
  }

  //State3のとき，各モータが指定した時間だけ絵の具を流す
  if(State == 3){
    analogWrite(bluepin, 255);  // 青色LEDの点灯

    Servocyan.write(40);  //絵の具を流す回転位置
    if(c != 0) delay(100);
    delay(c*40);  //cがシアンの値(量の調整のため40倍している)
    Servocyan.write(180);  //絵の具を流さない回転位置
    delay(1000);

    //他の色も同様
    Servomagenta.write(20);
    if(m !=0) delay(100);
    delay(m*40);
    Servomagenta.write(180);
    delay(1000);

    Servoyellow.write(170);
    if(y !=0) delay(100);
    delay(y*40);
    Servoyellow.write(0);
    delay(1000);

    Servoblack.write(150);
    if(k !=0) delay(100);
    delay(k*20);
    Servoblack.write(-10);
    State = 0;
  }else{
    //State3以外のときは全て絵の具を流さない
    Servocyan.write(180);
    Servomagenta.write(180);
    Servoyellow.write(0);
    Servoblack.write(-10);
    analogWrite(bluepin, 0);
  }

  delay(1);
}
