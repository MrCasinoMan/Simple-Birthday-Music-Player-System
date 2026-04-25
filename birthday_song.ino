// Global Variables
const byte PLAY_BUTTON = 2;
unsigned long last_interrupt_time = 0;

const byte BUZZER_PIN = 10;

const byte RED_PIN = 5;
const byte BLUE_PIN = 6;
const byte GREEN_PIN = 9;

volatile bool play_melody = false;

// Notes
const int NOTE_C4 = 262;
const int NOTE_D4 = 294;
const int NOTE_E4 = 330;
const int NOTE_FS4 = 370;
const int NOTE_G4 = 392;
const int NOTE_A4 = 440;
const int NOTE_B4 = 494;
const int NOTE_C5 = 523;
const int NOTE_D5 = 587;

// Timing
const int BPM = 130.0;

const int QUARTER = 60000 / BPM; // 1 beat = 1 min/BPM
const int HALF = QUARTER * 2.0;
const int DOTTED = QUARTER * 3.0;

const int DOTTED_QUARTER = QUARTER * 1.5;
const int EIGHTH = QUARTER / 2.0;

int current_note = 0;

unsigned long last_note_time = 0;
unsigned long last_led_time = 0;

const int melody[25] = {NOTE_D4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_G4, NOTE_FS4,
                      NOTE_D4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_A4, NOTE_G4,
                      NOTE_D4, NOTE_D4, NOTE_D5, NOTE_B4, NOTE_G4, NOTE_FS4, NOTE_E4,
                      NOTE_C5, NOTE_C5, NOTE_B4, NOTE_G4, NOTE_A4, NOTE_G4};

const int melody_durations[25] = {DOTTED_QUARTER, EIGHTH, QUARTER, QUARTER, QUARTER, HALF,
                                  DOTTED_QUARTER, EIGHTH, QUARTER, QUARTER, QUARTER, HALF,
                                  DOTTED_QUARTER, EIGHTH, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER,
                                  DOTTED_QUARTER, EIGHTH, QUARTER, QUARTER, QUARTER, DOTTED};

void PlayISR() {
  unsigned long current_time = millis();
  if (current_time - last_interrupt_time > 200 && !play_melody) {
    play_melody = true;
    Serial.println("playing");

    last_note_time = 0; // for playing the first note
    last_interrupt_time = current_time;
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(PLAY_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PLAY_BUTTON), PlayISR, FALLING);

  randomSeed(analogRead(A0));
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  unsigned long time = millis();

  if (play_melody) {
    if (last_note_time == 0) {
      tone(BUZZER_PIN, melody[current_note]);
      last_note_time = time;
    }

    else if (time - last_note_time >= melody_durations[current_note]) { // if full duration passed, play the next note
      current_note++;
      
      if (current_note > 24) { // index check
        noTone(BUZZER_PIN);
        setColor(0, 0, 0);
        randomSeed(analogRead(A0));

        current_note = 0;
        last_note_time = time;
        play_melody = false;
      }

      else {
        tone(BUZZER_PIN, melody[current_note]);
        last_note_time = time;
      }
    }

    else if (time - last_note_time >= melody_durations[current_note] * 0.9) { // stop the sound after 90% of the duration has passed to distinguish seperate notes
      noTone(BUZZER_PIN);
    }
  }

  if (play_melody && time - last_led_time > QUARTER) {
      setColor(random(0, 256), random(0, 256), random(0, 256));
      last_led_time = time;
    }
}

void setColor(int red_value, int green_value, int blue_value) {
  analogWrite(RED_PIN, red_value);
  analogWrite(GREEN_PIN, green_value);
  analogWrite(BLUE_PIN, blue_value);
}