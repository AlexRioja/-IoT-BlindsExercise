#include "mbed.h"

enum blindsState { raised, down, rising, lowering };
InterruptIn user_btn(PB_2);
AnalogIn ldr(A0);
bool isSummer, operationLock = false;
bool infoLock, seasonInfoLock, readLdr = true;
Ticker ldr_5s, inform_2s;
uint16_t ldrThreshold = 2000L;
uint16_t ldrValue = 0;
blindsState state = down;
blindsState new_state;
Timeout toBlinds;
DigitalOut led_up(PB_5), led_middle(PB_6), led_down(PB_7), led_summer(PA_5);

void btn_handler(void) { seasonInfoLock = true; }

void read_ldr() { readLdr = true; }
void inform_state() { infoLock = true; }
void inform(blindsState new_state) {
  printf("Season---> %s", isSummer ? "Summer" : "Winter");
  printf("________Sensor readings--->%d", ldrValue);
  printf("________Current State---> ");
  switch (new_state) {
  case down:
    printf("down\n");
    led_down = 1;
    led_up = led_middle = 0;
    break;
  case lowering:
    printf("lowering\n");
    led_up = 0;
    led_down = led_middle = 1;
    break;
  case rising:
    printf("rising\n");
    led_up = led_middle = 1;
    led_down = 0;
    break;
  case raised:
    printf("raised\n");
    led_up = 1;
    led_middle = led_down = 0;
    break;
  }
}
void operationFinished() {
  operationLock = false;
  infoLock = true;
  if (state == lowering)
    state = down;
  else
    state = raised;
}

blindsState transitionState(bool up) {
  operationLock = true;
  switch (state) {
  case down:
    if (up) {
      toBlinds.attach(operationFinished, 10.0f);
      return rising;
    } else
      operationLock = false;
    return down;
    break;
  case raised:
    if (up) {
      operationLock = false;
      return raised;
    } else {
      toBlinds.attach(operationFinished, 10.0f);
      return lowering;
    }
    break;
  default:
    operationLock = false;
    return state;
  }
}

int main() {
  user_btn.mode(PullDown);
  user_btn.rise(btn_handler);
  ldr_5s.attach(read_ldr, 5.0f);
  inform_2s.attach(inform_state, 2.0f);
  while (true) {
    if (readLdr) {
      readLdr = false;
      ldrValue = ldr.read_u16();
    }
    if (infoLock) {
      infoLock = false;
      inform(state);
    }
    if (seasonInfoLock) {
      seasonInfoLock = false;
      isSummer = !isSummer;
      printf("\n-------------------------------------------------------\n");
      printf("We are in---->%s", isSummer ? "Summer" : "Winter");
      printf("\n-------------------------------------------------------\n\n");
      isSummer ? led_summer = 1 : led_summer = 0;
    }
    if (!operationLock) {
      if (isSummer) {
        if (ldrValue > ldrThreshold) {
          new_state = transitionState(false);
        } else {
          new_state = transitionState(true);
        }
      } else {
        if (ldrValue > ldrThreshold) {
          new_state = transitionState(true);
        } else {
          new_state = transitionState(false);
        }
      }
      if (new_state != state)
        infoLock = true;
      state = new_state;
    }
    sleep();
  }
}
/*
int main(){
        ldr_2s.attach(read_ldr, 2.0f);
        while (1) {
        if (readLdr) {
      readLdr = false;
      ldrValue = ldr.read_u16();
                        printf("%d\n", ldrValue);
    }
        }
}
*/

/*
#include "mbed.h"

enum blindsState { raised, down, rising, lowering };
InterruptIn user_btn(PB_2);
AnalogIn ldr(A0);
bool isSummer, readLdr, isStateChanged, operationLock = false;
bool infoLock, seasonInfoLock = true;
Ticker ldr_2s;
uint16_t ldrThreshold = 2000L;
uint16_t ldrValue = 0;
blindsState state = down;
blindsState new_state;
Timeout toBlinds;

void btn_handler(void) {
  isSummer = !isSummer;
  seasonInfoLock = true;
}

void read_ldr() { readLdr = true; }

void inform_state(blindsState new_state) {
  printf("Current State-->");
  switch (new_state) {
  case down:
    printf("down\n");
    break;
  case lowering:
    printf("lowering\n");
    break;
  case rising:
    printf("rising\n");
    break;
  case raised:
    printf("raised\n");
    break;
  }
}
void operationFinished() {
  operationLock = false;
  infoLock = true;
  if (state == lowering)
    state = down;
  else
    state = raised;
}

blindsState transitionState(bool up) {
  operationLock = true;
  switch (state) {
  case down:
    if (up) {
      toBlinds.attach(operationFinished, 10.0f);
      return rising;
    } else
      operationLock = false;
    return down;
    break;
  case raised:
    if (up) {
      operationLock = false;
      return raised;
    } else {
      toBlinds.attach(operationFinished, 10.0f);
      return lowering;
    }
    break;
  default:
    operationLock = false;
    return state;
  }
}

int main() {
  user_btn.mode(PullDown);
  user_btn.rise(btn_handler);
  ldr_2s.attach(read_ldr, 2.0f);
  while (true) {
    if (readLdr) {
      readLdr = false;
      ldrValue = ldr.read_u16();
      if (isStateChanged) {
        // operationLock=false;
        isStateChanged = false;
      }
      printf("%d\n", ldrValue);
    }
    if (seasonInfoLock) {
      seasonInfoLock = false;
      printf("\n-------------------------------------------------------\n");
      printf("We are in---->%s", isSummer ? "Summer" : "Winter");
      printf("\n-------------------------------------------------------\n\n");
    }
    if (infoLock) {
      infoLock = false;
      inform_state(state);
    }
    if (!operationLock) {
      if (isSummer) {
        if (ldrValue > ldrThreshold) {
          // printf("Detected high light levels\n");
          new_state = transitionState(false);
        } else {
          // printf("Detected low light levels\n");
          new_state = transitionState(true);
        }
      } else {
        if (ldrValue > ldrThreshold) {
          // printf("Detected high light levels\n");
          new_state = transitionState(true);
        } else {
          // printf("Detected low light levels\n");
          new_state = transitionState(false);
        }
      }
      if (new_state != state)
        infoLock = isStateChanged = true;
      state = new_state;
    }
    sleep();
  }
}
*/