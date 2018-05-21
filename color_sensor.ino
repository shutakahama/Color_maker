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
  tcs.getRawData(&red, &green, &blue, &clear); //カラーセンサーデータの取得
  tcs.setInterrupt(true);

  //RGBを256段階で表示
  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /=sum;
  b = blue; b/=sum;
  r *= 256; g *= 256; b *= 256; 

  //CMYKへの変換
  float c, m, y, k, sum2;
  sum2= 256 - r;
  if(sum2 > 256 - g) sum2 = 256 - g;
  if(sum2 > 256 - b) sum2 = 256 - b;

  k=sum2;
  c = (256 - k - r); c /= 256-k; c *= 100;
  m = (256 - k - g); m /= 256-k; m *= 100;
  y = (256 - k - b); y /= 256-k; y *= 100;
  k = sum2; k /= 256; k *= 100;
  
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

  //ボタン状態でStateを変更
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

  //State1　Processingとの通信
  if(State == 1){
    analogWrite(redpin, 255); //LED赤色発光
    if(Serial.available() > 0) {
      Serial.read(); //Processingから仮データを受け取る
      Serial.println(String((int)r) + "," + String((int)g) + "," + String((int)b)); //ProcessingにRGBデータを送信
    }
  }else{
    analogWrite(redpin, 0);
  }

  //State2 全てのモーターを「水が流れる角度」に設定
  if(State == 2){
    analogWrite(greenpin, 255); //LED緑色発光
    
    Servocyan.write(10);
    Servomagenta.write(20);
    Servoyellow.write(170);
    Servoblack.write(160);
    delay(1000); //１秒待機
  }else{
    Servocyan.write(180);
    Servomagenta.write(180);
    Servoyellow.write(0);
    Servoblack.write(-10);
    analogWrite(greenpin, 0);
  }

  //State3　色生成
  if(State == 3){
    analogWrite(bluepin, 255); //LED青色発光
    
    Servocyan.write(10); //「水が流れる角度」に回転
    if(c != 0) delay(100);
    delay(c*40); //C値*40(ms)待機
    Servocyan.write(180); //「水が流れない角度」に戻る
    delay(1000);
    
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
    
    Servoblack.write(160);
    if(k !=0) delay(100);
    delay(k*10);//インクの濃さに応じて割合は調整する
    Servoblack.write(0);
    
    State = 0; //State=0に戻る

  }else{
    Servocyan.write(180);
    Servomagenta.write(180);
    Servoyellow.write(0);
    Servoblack.write(-10);
    analogWrite(bluepin, 0);
  }

  delay(1);
}
