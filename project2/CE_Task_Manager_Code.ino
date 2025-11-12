#define LED1 2
#define LED2 3

// Task timing parameters (ms)
#define FRAME_TIME 10  // base tick for scheduler, 10ms per frame

// Shared state variables
int ledSelect = 0;
int timing = 0;

unsigned long prevTime1 = 0;
unsigned long prevTime2 = 0;
long timeInt1 = 0;
long timeInt2 = 0;

int ledState1 = LOW;
int ledState2 = LOW;

bool flagPrint1 = true;
bool flagPrint2 = false;
bool flag3Check = true;
bool flag4Check = false;

// ---------- Task 1: Handle serial input ----------
void taskSerial() {
  if(flagPrint1) {
    Serial.println("What LED? (1 or 2)?");
    flag3Check = true;
    flagPrint1 = false;
  }

  if(flag3Check && Serial.available()) {
    ledSelect = Serial.parseInt();
    if(ledSelect != 0) {
      flagPrint2 = true;
      flag3Check = false;
    }
  }

  if(flagPrint2) {
    Serial.println("What interval (in msec)?");
    flagPrint2 = false;
  }

  if(!flag3Check && Serial.available()) {
    timing = Serial.parseInt();
    if(timing != 0) flag4Check = true;
  }

  if(flag4Check && ledSelect == 1) {
    timeInt1 = timing;
    if(timeInt1 < 0) ledState1 = LOW;
  }
  else if(flag4Check && ledSelect == 2) {
    timeInt2 = timing;
    if(timeInt2 < 0) ledState2 = LOW;
  }
  else if(flag4Check) {
    // invalid LED number
  }

  if(flag4Check) {
    flagPrint1 = true;
    flagPrint2 = false;
    flag3Check = true;
    flag4Check = false;
    ledSelect = 0;
    timing = 0;
  }
}

// ---------- Task 2: Blink LED1 ----------
void taskBlinkLED1() {
  unsigned long currentTime = millis();
  if(timeInt1 != 0 && (currentTime - prevTime1 >= timeInt1)) {
    prevTime1 = currentTime;
    ledState1 = (ledState1 == HIGH) ? LOW : HIGH;
    digitalWrite(LED1, ledState1);
  }
}

// ---------- Task 3: Blink LED2 ----------
void taskBlinkLED2() {
  unsigned long currentTime = millis();
  if(timeInt2 != 0 && (currentTime - prevTime2 >= timeInt2)) {
    prevTime2 = currentTime;
    ledState2 = (ledState2 == HIGH) ? LOW : HIGH;
    digitalWrite(LED2, ledState2);
  }
}

// ---------- Setup ----------
void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(9600);
  prevTime1 = millis();
  prevTime2 = millis();
  
}

typedef void (*TaskFunc)();
TaskFunc taskList[] = {
  taskSerial,
  taskBlinkLED1,
  taskBlinkLED2
};
const int NUM_TASKS = sizeof(taskList) / sizeof(TaskFunc);

// ---------- Cyclic Executive ----------
void loop() {
  unsigned long frameStart = millis();

  // Loop through all tasks in round-robin order
  for (int i = 0; i < NUM_TASKS; i++) {
    taskList[i]();  // Call each task function
  }

  // Enforce fixed frame duration
  while(millis() - frameStart < FRAME_TIME) {
    // idle until frame completes (keeps consistent timing)
  }
}