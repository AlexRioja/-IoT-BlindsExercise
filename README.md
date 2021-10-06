# -IoT-BlindsExercise
[Master-IoT] First project for the Embedded Systems course

Specifications:
 Two working modes: summer (if light level is high blinds are down) and winter (the opposite).
 Ambience light should be measured each 1 sec. ~ 10 sec. using the light sensor (it is not necessary to make this parameter configurable).
 User button will be used to change between working modes.
 Consider a levelling up and down time of ~30 sec.
 Consider the use of a threshold for the ambience light measure.
 The states of the system are the following:
     Blinds states: raised, down, rising, lowering. 
     Working mode: summer and winter.
 The system states should be informed by using LEDs or Serial(received in the PC) and updated each 2 sec.