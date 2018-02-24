#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include <os_type.h>
#include <user_config.h>
#include <mem.h>

#include "../esp_nano_httpd/esp_nano_httpd.h"
#include "../html/include/index.h"
#include "../html/include/wifi_connect.h"
#include "../html/include/demo.h"

static volatile os_timer_t blink_timer;

void blink_fun(void *arg)
{	//Do blinky stuff
    if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT2)
    	gpio_output_set(0, BIT2, BIT2, 0);
    else
        gpio_output_set(BIT2, 0, BIT2, 0);
}

// HTTP request callbacks
void ICACHE_FLASH_ATTR wifi_config_cb(struct espconn *conn, void *arg, uint32_t len)
{
    struct station_config station_conf = {0};
    http_request_t *req = conn->reverse;
    char *param;

    if(req == NULL) return;
    //We only handle POST requests
    if(req->type != TYPE_POST || req->content == NULL){
        resp_http_error(conn);
        return;
    }
    /* in request content We expect serialized input form query like: ssid=MY_SSID&passwd=MY_PASSWD
    Use strtok to divide query into tokens*/
    param=strtok(req->content,"&");
    do {
        if( os_memcmp(param,"ssid=",5) == 0 )         //ssid value found
            ets_strncpy(station_conf.ssid, strchr(param,'=')+1,32);
        else if( os_memcmp(param,"passwd=",7) == 0 )  //password value found
            ets_strncpy(station_conf.password, strchr(param,'=')+1,64);
    } while( (param=strtok(NULL,"&")) != NULL);

    station_conf.bssid_set = 0;               //do not look for specific router MAC address
    wifi_station_set_auto_connect(0);		  //disable autoconnect
    wifi_station_set_config(&station_conf);   //save new WiFi settings
    wifi_station_connect();					  //connect to network

    send_html(conn, wifi_connect_html, sizeof(wifi_connect_html)); //show HTML page
}

void ICACHE_FLASH_ATTR led_demo_cb(struct espconn *conn, void *arg, uint32_t len)
{
	uint32_t freq;
    char *param;
    http_request_t *req = conn->reverse;

    if(req == NULL) return;
    if(req->type == TYPE_GET){   //handle GET request
    	send_html(conn, demo_html, sizeof(demo_html)); //return HTML demo page
    	return;
    }

    if(req->type != TYPE_POST || req->content == NULL){    //handle POST request
        resp_http_error(conn);
        return;
    }

    param=strtok(req->content,"&");
	if( os_memcmp(param,"led_freq=",9) == 0 ){         //led frequency
		freq = atoi(strchr(param,'=')+1);
		if(freq != 0){
			os_timer_disarm(&blink_timer);
			os_timer_setfn(&blink_timer, (os_timer_func_t *)blink_fun, NULL);
			os_timer_arm(&blink_timer, 1000/freq, 1); //set new frequency
			os_printf("new LED frequency set. f=%dHz\n", freq);
		}
	}
    send_html(conn, demo_html, sizeof(demo_html)); //return HTML demo page
}

// URL config table
const http_callback_t url_cfg[] = {
	{"/", send_html, index_html, sizeof(index_html)},
	{"/demo", led_demo_cb, NULL, 0},
	{"/wifi_conf", wifi_config_cb, NULL, 0 },
	{0,0,0}
};

void ICACHE_FLASH_ATTR user_init()
{
	uart_div_modify(0, UART_CLK_FREQ / 115200);
    gpio_init();

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);

    os_timer_disarm(&blink_timer);
    os_timer_setfn(&blink_timer, (os_timer_func_t *)blink_fun, NULL);
    os_timer_arm(&blink_timer, 1000, 1);
    
    esp_nano_httpd_register_content(url_cfg);
    esp_nano_httpd_init(STATIONAP_MODE);
}
