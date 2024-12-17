#include "uart_ext.h"
#include "stm32f4xx_hal.h"

// TODO: Implement read function.

static void UartExtTxCpltCallback_0(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_1(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_2(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_3(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_4(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_5(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_6(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_7(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_8(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_9(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_10(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_11(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_12(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_13(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_14(UART_HandleTypeDef *huart);
static void UartExtTxCpltCallback_15(UART_HandleTypeDef *huart);

void (*mUartExtTxCpltCallbacks[])(UART_HandleTypeDef *) = {
    UartExtTxCpltCallback_0,  UartExtTxCpltCallback_1,  UartExtTxCpltCallback_2,
    UartExtTxCpltCallback_3,  UartExtTxCpltCallback_4,  UartExtTxCpltCallback_5,
    UartExtTxCpltCallback_6,  UartExtTxCpltCallback_7,  UartExtTxCpltCallback_8,
    UartExtTxCpltCallback_9,  UartExtTxCpltCallback_10, UartExtTxCpltCallback_11,
    UartExtTxCpltCallback_12, UartExtTxCpltCallback_13, UartExtTxCpltCallback_14,
    UartExtTxCpltCallback_15
};

UartExt * mUartExts[16] = {NULL};

int mUartExtTxCpltCallbackUsageCount = 0;



//UartExtInit(&mLogUartExt, &huart1, UART_EXT_TX_ONLY, LOG_PRINT_BUFFER_SIZE);
int UartExtInit(UartExt * uart_ext, void * hal_instance, UartExtMode mode, size_t buffer_size) {

    if(uart_ext == NULL || hal_instance == NULL || buffer_size == 0) {
        return -1;
    }


    uart_ext->hal_instance = hal_instance;
    uart_ext->mode = mode;

    uart_ext->tx_buffer = NULL;
    uart_ext->rx_buffer = NULL;

    int is_tx_enable = 0;
    int is_rx_enable = 0;   

    if(mode == UART_EXT_RX_ONLY) {
        is_rx_enable = 1;
    } else if(mode == UART_EXT_TX_ONLY) {
        is_tx_enable = 1;
    } else {
        is_rx_enable = 1;
        is_tx_enable = 1;
    }

    mUartExts[mUartExtTxCpltCallbackUsageCount] = uart_ext;

    if(is_rx_enable) {
        RingBuffer * rx_buffer = malloc(sizeof(RingBuffer));
        if(rx_buffer == NULL) {
            return -1;
        }
        if(0 != RingBufferInit(rx_buffer, buffer_size)) {
            return -1;
        }
        uart_ext->rx_buffer = rx_buffer;
    }
    if(is_tx_enable) {
        RingBuffer * tx_buffer = malloc(sizeof(RingBuffer));
        if(tx_buffer == NULL) {
            return -1;
        }
        if(0 != RingBufferInit(tx_buffer, buffer_size)) {
            return -1;
        }
        uart_ext->tx_buffer = tx_buffer;
        HAL_UART_RegisterCallback(hal_instance, HAL_UART_TX_COMPLETE_CB_ID, mUartExtTxCpltCallbacks[mUartExtTxCpltCallbackUsageCount]);
    }

    mUartExtTxCpltCallbackUsageCount++;
    return 0;
}


int UartExtWrite(UartExt * uart_ext, const uint8_t * data, size_t length) {
    if(uart_ext == NULL || uart_ext->mode == UART_EXT_RX_ONLY || data == NULL) {
        return -1;
    }
    RingBuffer * tx_buffer = uart_ext->tx_buffer;
    return RingBufferPush(tx_buffer, data, length);
}


int UartExtRead(UartExt * uart_ext, uint8_t * data, size_t length) {
    return 0;
    // if(uart_ext == NULL || uart_ext->mode == UART_EXT_TX_ONLY || data == NULL) {    
    //     return -1;
    // }
    // RingBuffer * rx_buffer = uart_ext->rx_buffer;
    // return RingBufferPop(rx_buffer, data, length);
}


int UartExtFlush(UartExt * uart_ext) {
    if(uart_ext == NULL) {
        return -1;
    }

    if(uart_ext->mode == UART_EXT_RX_ONLY) {
        return 0;
    }

    UART_HandleTypeDef * uart_handle = (UART_HandleTypeDef *)uart_ext->hal_instance;

    if(uart_handle->gState != HAL_UART_STATE_READY) {
        return -1;
    }
    
    if(RingBufferIsEmpty(uart_ext->tx_buffer)) {
        return 0;
    }

    void * tail_addr = RingBufferGetTail(uart_ext->tx_buffer);
    int distance_to_boundary = RingBufferGetDistanceToBoundary(uart_ext->tx_buffer);
    
    HAL_UART_Transmit_DMA(uart_handle, tail_addr, distance_to_boundary);
    return 0;
}


int UartExtSetUser(UartExt * uart_ext, void * user) {
    if(uart_ext == NULL) {
        return -1;
    }
    uart_ext->user = user;
    return 0;
}


static void UartExtTxCpltCallback(UART_HandleTypeDef *huart, UartExt * uart_ext) {
    size_t tx_size = (size_t)huart->TxXferSize;
    RingBufferPop(uart_ext->tx_buffer, NULL, tx_size);
    UartExtFlush(uart_ext);
}


#define DEFINE_UART_EXT_TX_CLPT_CALLBACK(index)                         \
static void UartExtTxCpltCallback_##index(UART_HandleTypeDef *huart) {  \
    UartExtTxCpltCallback(huart, mUartExts[index]);                     \
}


DEFINE_UART_EXT_TX_CLPT_CALLBACK(0)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(1)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(2)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(3)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(4)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(5)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(6)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(7)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(8)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(9)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(10)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(11)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(12)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(13)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(14)

DEFINE_UART_EXT_TX_CLPT_CALLBACK(15)



