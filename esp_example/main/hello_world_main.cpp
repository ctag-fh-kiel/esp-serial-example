#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <iostream>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace rapidjson;

extern "C" {
    void app_main();
}

#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/uart.h"
#define BUF_SIZE (1024)

void init_uart() {
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    //uart_set_pin(UART_NUM_0, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_set_pin(UART_NUM_0,UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);
}

// detects new line
int readline(int readch, char *buffer, int len) {
    static int pos = 0;
    int rpos;

    if (readch > 0) {
        switch (readch) {
            //case '\r': // Ignore CR
            //    break;
            case '\n': // Return on new-line
                rpos = pos;
                pos = 0;  // Reset position index ready for next time
                return rpos;
            default:
                if (pos < len-1) {
                    buffer[pos++] = readch;
                    buffer[pos] = 0;
                }
        }
    }
    return 0;
}

void app_main(void)
{
    init_uart();
    std::cout << "Starting echo listener" << std::endl;
    uint8_t data = 0;
    char buf[128];
    while(1) {
        //Read data from UART
        int len = uart_read_bytes(UART_NUM_0, &data, 1, 20 / portTICK_RATE_MS);
        //Write data back to UART
        //uart_write_bytes(UART_NUM_0, (const char*) data, len);
        if (len > 0 && readline(data, buf, 128) > 0) {
            Document d;
            d.Parse(buf);
            if(!d.HasParseError()){
                StringBuffer buffer;
                Writer<StringBuffer> writer(buffer);
                d.Accept(writer);
                std::cout << buffer.GetString() << std::endl;
            }
        }
    }
}