#include <Arduino.h>

/* ============================================================
   ピン定義
   ------------------------------------------------------------
   SW_PIN   : プッシュスイッチ（INPUT_PULLUPで使用）
   LED1_PIN : LED1（点灯ステップ1で点灯）
   LED2_PIN : LED2（点灯ステップ2で点灯）
   LED3_PIN : LED3（点灯ステップ3で点灯）
   ============================================================ */
const int SW_PIN   = 2;
const int LED1_PIN = 8;
const int LED2_PIN = 9;
const int LED3_PIN = 10;

/* ============================================================
   デバウンス設定
   ------------------------------------------------------------
   スイッチは機械的にチャタリングが発生するため、
   押した瞬間のノイズを無視する時間（20ms）を設定。
   ============================================================ */
const unsigned long DEBOUNCE_MS = 20;

/* ============================================================
   LED 点灯ステップ定義
   ------------------------------------------------------------
   STEP_OFF  : 全消灯
   STEP_LED1 : LED1 点灯
   STEP_LED2 : LED2 点灯
   STEP_LED3 : LED3 点灯
   STEP_ALL  : 全点灯
   ============================================================ */
const int STEP_OFF  = 0;
const int STEP_LED1 = 1;
const int STEP_LED2 = 2;
const int STEP_LED3 = 3;
const int STEP_ALL  = 4;

/* ============================================================
   LED 点灯間隔
   ------------------------------------------------------------
   点灯モード中にステップを進める間隔（300ms）
   ============================================================ */
const unsigned long LED_INTERVAL_MS = 300;

/* ============================================================
   状態管理用の変数
   ------------------------------------------------------------
   mode        : false=消灯モード, true=点灯モード
   lastSwState : 前回読み取ったスイッチ状態
   lastSwTime  : デバウンス用に状態が変わった時刻
   ledTimer    : LEDステップ更新用タイマー
   stepIndex   : 現在のLEDステップ
   ============================================================ */
bool mode = false;
bool lastSwState = HIGH;
unsigned long lastSwTime = 0;

unsigned long ledTimer = 0;
int stepIndex = STEP_OFF;


/* ============================================================
   readSwitch()
   ------------------------------------------------------------
   スイッチ入力を読み取り、デバウンス処理を行う。
   ・スイッチが押された瞬間だけ true を返す
   ・INPUT_PULLUP のため、押すと LOW になる
   ============================================================ */
bool readSwitch() {
  bool now = digitalRead(SW_PIN);

  // 状態が変わった瞬間に時刻を記録（デバウンス開始）
  if (now != lastSwState) {
    lastSwTime = millis();
  }

  bool pressed = false;

  // 一定時間経過後に「本当に押されたか」を判定
  if (millis() - lastSwTime > DEBOUNCE_MS) {
    // HIGH → LOW の変化を検出（押された瞬間）
    if (lastSwState == HIGH && now == LOW) {
      pressed = true;
    }
  }

  lastSwState = now;
  return pressed;
}


/* ============================================================
   updateState()
   ------------------------------------------------------------
   スイッチ押下に応じてモードを切り替え、
   点灯モード中はLEDステップを進める。
   ------------------------------------------------------------
   swPressed : readSwitch() の結果（押された瞬間のみ true）
   ============================================================ */
void updateState(bool swPressed) {

  // スイッチが押されたらモード切り替え
  if (swPressed) {
    mode = !mode;          // ON/OFF トグル
    stepIndex = STEP_OFF;  // ステップをリセット
    ledTimer = millis();   // タイマー初期化
  }

  // 点灯モード中は一定間隔でステップを進める
  if (mode) {
    unsigned long now = millis();
    if (now - ledTimer >= LED_INTERVAL_MS) {
      ledTimer = now;

      stepIndex++;
      if (stepIndex > STEP_ALL) {
        stepIndex = STEP_OFF;  // ループさせる
      }
    }
  }
}


/* ============================================================
   outputLEDs()
   ------------------------------------------------------------
   現在のステップに応じてLEDを点灯・消灯する。
   ・mode=false のときはすべて消灯
   ・mode=true のときは stepIndex に応じて点灯
   ============================================================ */
void outputLEDs() {

  // 消灯モードなら全部消す
  if (!mode) {
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    return;
  }

  // ステップに応じて点灯
  digitalWrite(LED1_PIN, (stepIndex == STEP_LED1 || stepIndex == STEP_ALL) ? HIGH : LOW);
  digitalWrite(LED2_PIN, (stepIndex == STEP_LED2 || stepIndex == STEP_ALL) ? HIGH : LOW);
  digitalWrite(LED3_PIN, (stepIndex == STEP_LED3 || stepIndex == STEP_ALL) ? HIGH : LOW);
}


/* ============================================================
   setup()
   ------------------------------------------------------------
   起動時に1回だけ実行される初期化処理。
   ・スイッチは INPUT_PULLUP（内部プルアップ）
   ・LEDピンは出力に設定
   ============================================================ */
void setup() {
  pinMode(SW_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
}


/* ============================================================
   loop()
   ------------------------------------------------------------
   メインループ。
   1. スイッチ入力を読む
   2. 状態を更新する
   3. LED を出力する
   ------------------------------------------------------------
   この3つを高速で繰り返すことで、
   スイッチ操作とLED制御を滑らかに実現する。
   ============================================================ */
void loop() {
  bool swPressed = readSwitch();   // 入力処理
  updateState(swPressed);          // 状態遷移処理
  outputLEDs();                    // 出力処理
}