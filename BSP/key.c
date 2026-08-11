#include "key.h"
#include "usart.h"

#define KEY_DEBOUNCE_COUNT 3

static const uint16_t key_pins[3] = {KEY1_PIN, KEY2_PIN, KEY3_PIN};
static uint8_t key_states[3];
static uint8_t key_counters[3];
static uint8_t key_last_state[3];

static uint8_t current_key_id = 0;
static uint8_t current_mode = 0;
static uint8_t mode_changed = 0;

void Key_Init(void)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        key_states[i] = 1;
        key_last_state[i] = 1;
        key_counters[i] = 0;
    }
    current_key_id = 0;
    current_mode = 0;
    mode_changed = 0;
}

uint8_t Key_GetState(uint8_t key_index)
{
    if (key_index < 1 || key_index > 3)
    {
        return 1;
    }
    return HAL_GPIO_ReadPin(KEY_GPIO_PORT, key_pins[key_index - 1]);
}

void Key_ProcessAndSend(void)
{
    const uint8_t frame_head[2] = {0xAA, 0x55};
    const uint8_t frame_tail[2] = {0x0D, 0x0A};

    for (uint8_t i = 0; i < 3; i++)
    {
        uint8_t raw_state = Key_GetState(i + 1);

        if (raw_state != key_states[i])
        {
            key_counters[i]++;
            if (key_counters[i] >= KEY_DEBOUNCE_COUNT)
            {
                key_counters[i] = 0;
                key_states[i] = raw_state;
                if (key_last_state[i] == 1 && key_states[i] == 0)
                {
                    current_key_id = i + 1;
                    uint8_t packet[5] = {frame_head[0], frame_head[1], current_key_id, frame_tail[0], frame_tail[1]};
                    HAL_UART_Transmit(&huart2, packet, sizeof(packet), 10);
                }
                key_last_state[i] = key_states[i];
            }
        }
        else
        {
            key_counters[i] = 0;
        }
    }
}

void Key_AckByteHandler(uint8_t byte)
{
    static uint8_t rx_state = 0;
    static uint8_t rx_key_id = 0;

    switch (rx_state)
    {
        case 0:
            if (byte == 0xAA)
            {
                rx_state = 1;
            }
            break;
        case 1:
            if (byte == 0x55)
            {
                rx_state = 2;
            }
            else
            {
                rx_state = 0;
            }
            break;
        case 2:
            if (byte >= 1 && byte <= 3)
            {
                rx_key_id = byte;
                rx_state = 3;
            }
            else
            {
                rx_state = 0;
            }
            break;
        case 3:
            if (byte == 0x0D)
            {
                rx_state = 4;
            }
            else
            {
                rx_state = 0;
            }
            break;
        case 4:
            if (byte == 0x0A)
            {
                current_mode = rx_key_id;
                mode_changed = 1;
            }
            rx_state = 0;
            break;
        default:
            rx_state = 0;
            break;
    }
}

uint8_t Key_GetMode(void)
{
    return current_mode;
}

uint8_t Key_GetModeChanged(void)
{
    return mode_changed;
}

void Key_ClearModeChanged(void)
{
    mode_changed = 0;
}
