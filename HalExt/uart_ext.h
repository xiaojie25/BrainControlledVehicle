#ifndef __HAL_UART_EXT_H__
#define __HAL_UART_EXT_H__


#include "ring_buffer.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef enum UartExtMode_ {
    UART_EXT_TX_ONLY = 0,
    UART_EXT_RX_ONLY = 1,
    UART_EXT_TX_RX = 2
} UartExtMode;

typedef struct UartExt_ {
    void * hal_instance;
    void * user;
    RingBuffer * tx_buffer;
    RingBuffer * rx_buffer;
    UartExtMode mode;
} UartExt;


int UartExtInit(
    UartExt * uart_ext, 
    void * hal_instance, 
    UartExtMode mode, 
    size_t buffer_size
);

int UartExtWrite(UartExt * uart_ext, const uint8_t * data, size_t length);

int UartExtRead(UartExt * uart_ext, uint8_t * data, size_t length);

int UartExtFlush(UartExt * uart_ext);

int UartExtSetUser(UartExt * uart_ext, void * user);






#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_EXT_H__ */
