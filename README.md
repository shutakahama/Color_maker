# Color_maker

これは私が製作した，身の周りのものの色を自動で再現する装置です．  
センサーで身の周りのものの色を読み取って，その色を使って画面上に線を描けたり，絵の具を混ぜ合わせてその色を作り出したりすることができます．  

詳細は記事になっていますのでご覧ください．  
[身の周りのものの色を自動で再現する装置を作ってみた](https://qiita.com/grouse324/items/7b44aa416243a2200f95)

製作した装置の外観

<img src="https://github.com/shutakahama/Color_maker/blob/master/img/pic_all.png" width="480">

オレンジ色を読み取って色を生成した結果

<img src="https://github.com/shutakahama/Color_maker/blob/master/img/pic_color.png" width="480">

## 使ったもの

- ノートPC
- Arduino Uno R3
- サーボモーター(マイクロサーボ SG92R) 4個
- カラーセンサー(Adafruit TCS34725)
- フルカラーLED(OSTA5131A)
- 抵抗(100Ω 2個，150Ω 1個，10kΩ 3個)
- タクトスイッチ 3個
- ユニバーサル基板(AE-ARDUINO_UNI-G)
- 外部電源(単3乾電池 2個)
- その他ワイヤー類、ピンソケットなどの電子工作部品
- ゴムチューブ(熱収縮チューブ Φ1.5×0.2×1m) 2本
- プラスチック容器(4分割 box型)
- ジュラルミン製外枠(大学の工房で自分で切削)
- カラーセンサーカバー(3Dプリンターで作成)
- 絵の具(中善画廊 3原色カラー，ぺんてる 黒)
- 絵の具用筆

## プログラム
#### **color_sensor.ino**
Arduinoで実行するためのコード．色センサーからの値を入力として得て，Processingと通信を行って色をディスプレイ上に表示する．  
またサーボモータを動作させて絵の具を混ぜ合わせる．

#### **colordraw.pde**
Processingで実行するためのコード．Arduinoとの通信で得た色情報を使ってディスプレイに線を表示する．  

## 実行環境

- Arduino-1.8.5 (Arduino IDE)
  - ライブラリ Adafruit TCS34725 by Adafruit が必要
- Processing-3.3.6

## Arduinoボード上の配線

- 色を読み取るカラーセンサー
- 絵の具の量を制御するサーボモータ
- 状態を指定するスイッチ
- 状態を表現するLED

が接続されている．カラーセンサは3.3V，スイッチ類は5Vピンに繋いでいる．サーボモータは出力を安定させるため外部電源(乾電池2個)を繋いでいる．  
左がはんだ付けしたボードの様子．右が接続時の配線図．  

<img src="https://github.com/shutakahama/Color_maker/blob/master/img/pic_board.png" width="600">
