import processing.serial.*;

Serial myPort;
color myColor = color(0);

void setup(){
  fullScreen();
  background(255);
  noStroke();
  
  println(Serial.list());
  myPort = new Serial(this, Serial.list()[0], 9600);
  myPort.bufferUntil('\n');
  
  myPort.write('A'); //仮データ送信
}

//マウスで円を書く操作
void draw(){
  if(mousePressed && mouseButton == LEFT){ //左ボタンで指定した色の円
    fill(myColor);
    ellipse(mouseX, mouseY, 50, 50);
  }else if(mousePressed && mouseButton == RIGHT){ //右ボタンで白色円
    fill(0,0,0);
    ellipse(mouseX, mouseY, 50, 50);
  }else if(mousePressed && mouseButton == CENTER){ //中央ボタンで画面クリア
    background(0,0,0);
  }
}

//データを受け取った際に呼び出される
void serialEvent(Serial myPort){
  String received = myPort.readStringUntil('\n');
  println(received);
  
  if(received != null){
    received = trim(received); //改行コードなどを除く
    int sensorColor[] = int(split(received, ',')); //カンマの区切りごとに配列に格納
    if(sensorColor.length == 3){ //各値に格納
      int red = sensorColor[0];
      int green = sensorColor[1];
      int blue = sensorColor[2];
      
      colorMode(RGB, 255, 255, 255, 255); //カラーモードRGBでmycolorに格納
      myColor = color(red, green, blue);
      
      colorMode(HSB, 1.0, 1.0, 1.0, 1.0); //カラーモードHSBで変換
      myColor = color(hue(myColor), saturation(myColor)*1.5, brightness(myColor)*1.5);
    }
    myPort.write('A'); //仮データ送信
  }
}
