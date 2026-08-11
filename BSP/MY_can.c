#include "can.h"

#include "can.h"   // 必须包含

// 1. 发送函数（和你原来用法完全一样）
void MyCAN_Transmit(uint32_t ID, uint8_t Length, uint8_t *Data)
{
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;

    TxHeader.StdId = ID;
    TxHeader.ExtId = ID;
    TxHeader.IDE = CAN_ID_STD;    // 标准帧
    TxHeader.RTR = CAN_RTR_DATA;  // 数据帧
    TxHeader.DLC = Length;        // 数据长度

    // 发送
    HAL_CAN_AddTxMessage(&hcan, &TxHeader, Data, &TxMailbox);

    // 等待发送完成（超时）
    uint32_t Timeout = 0;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0)
    {
        Timeout++;
        if (Timeout > 100000)
            break;
    }
}

// 2. 查询是否有接收到数据
uint8_t MyCAN_ReceiveFlag(void)
{
    if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) > 0)
    {
        return 1;
    }
    return 0;
}

// 3. 读取接收到的数据
void MyCAN_Receive(uint32_t *ID, uint8_t *Length, uint8_t *Data)
{
    CAN_RxHeaderTypeDef RxHeader;

    HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, Data);

    if (RxHeader.IDE == CAN_ID_STD)
    {
        *ID = RxHeader.StdId;
    }
    else
    {
        *ID = RxHeader.ExtId;
    }

    *Length = RxHeader.DLC;
}