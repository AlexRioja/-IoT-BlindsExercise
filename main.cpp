#include "mbed.h"
#include <cstdint>

enum blindsState { raised, down, rising, lowering };
uint16_t ldrThreshold = 2000L;
uint16_t ldrValue = 0;
bool isSummer, operationLock = false;
bool infoLock, seasonInfoLock, readLdr = true;
float timeToOperate = 30.0f;
blindsState new_state;
Ticker ldr_5s, inform_2s;
Timeout toBlinds;
blindsState state;

DigitalOut led_up(PB_5), led_middle(PB_6), led_down(PB_7), led_summer(PA_5);
InterruptIn user_btn(PB_2);
AnalogIn ldr(A0);

void btn_handler(void) { seasonInfoLock = true; }
void read_ldr_handler() { readLdr = true; }
void inform_state_handler() { infoLock = true; }

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
      toBlinds.attach(operationFinished, timeToOperate);
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
      toBlinds.attach(operationFinished, timeToOperate);
      return lowering;
    }
    break;
  default:
    operationLock = false;
    return state;
  }
}

int main() {
  state=down;
  user_btn.mode(PullDown);
  user_btn.rise(btn_handler);
  ldr_5s.attach(read_ldr_handler, 5.0f);
  inform_2s.attach(inform_state_handler, 2.0f);
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