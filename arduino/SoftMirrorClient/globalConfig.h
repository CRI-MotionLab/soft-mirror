#ifndef _SOFTMIRROR_CLIENT_GLOBALCONFIG_H_
#define _SOFTMIRROR_CLIENT_GLOBALCONFIG_H_

// #define USE_SERIAL

#define BOARD_ID 1

#define CLIP(X, MIN, MAX) (X < MIN) ? MIN : ((X > MAX) ? MAX : X)
#define SCALE(X, MINI, MAXI, MINO, MAXO) ((X - MINI) / (MAXI - MINI)) * (MAXO - MINO) + MINO

#define NODE_LED 16
#define ESP_LED 2

// #define MAX_SOCKET_LINE_STRING_SIZE 32
#define MAX_TOTAL_SOCKET_MSG_LENGTH (PWM_OUTPUTS_PER_NODEMCU + 5)
#define MAX_TOTAL_SOCKET_STRING_SIZE (MAX_TOTAL_SOCKET_MSG_LENGTH * 10)

#define MAIN_PERIOD 5
#define PWM_CONTROL_MSG_PERIOD 100 // interpolation happens between messages

#define PWM_OUTPUTS_PER_NODEMCU 3

static const int PWM_PINS_ARRAY[] = { 5, 4, 0, 2, 14, 12, 13, 15, 3, 1 };

#endif /* _SOFTMIRROR_CLIENT_GLOBALCONFIG_H_ */
