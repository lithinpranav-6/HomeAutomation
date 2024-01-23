/*Code By D.Lithinpranav

All Rights Reserved
Copyright content not to be used for commercial use.
copyright by D.Lithinpranav*/

#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <SimpleTimer.h>

const char *service_name = "Alpha_Cambridge_International_School";
const char *pop = "ACIS_Trichy";



#define gpio_0 0




char device1[] = "Bedroom Light";
char device2[] = "Garage Light";
char device3[] = "Kitchen Light";
char device4[] = "Living Room Light";

static uint8_t relay1 = 19;
static uint8_t relay2 = 21;
static uint8_t relay3 = 22;
static uint8_t relay4 = 23;

static uint8_t switch1 = 13;
static uint8_t switch2 = 12;
static uint8_t switch3 = 14;
static uint8_t switch4 = 27;

static uint8_t wifi_led = 2;

bool toggle_state1 = LOW;
bool toggle_state2 = LOW;
bool toggle_state3 = LOW;
bool toggle_state4 = LOW;

bool switch_state1 = LOW;
bool switch_state2 = LOW;
bool switch_state3 = LOW;
bool switch_state4 = LOW;

static Switch my_switch1(device1, &relay1);
static Switch my_switch2(device2, &relay2);
static Switch my_switch3(device3, &relay3);
static Switch my_switch4(device4, &relay4);

void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id) {
        case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32S2
            Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
            printQR(service_name, pop, "softap");
#else
            Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
            printQR(service_name, pop, "ble");
#endif
            break;
        case ARDUINO_EVENT_PROV_INIT:
            wifi_prov_mgr_disable_auto_stop(10000);
            break;
        case ARDUINO_EVENT_PROV_CRED_SUCCESS:
            wifi_prov_mgr_stop_provisioning();
            break;
        default:;
    }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx) {
  const char *device_name = device -> getDeviceName();
  const char *param_name = param -> getParamName();
  if (strcmp(device_name, device1) == 0) {
    Serial.printf("Switch = %s\n", val.val.b? "true" : "false");
    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
      toggle_state1 = val.val.b;
      (toggle_state1 == false) ? digitalWrite(relay1, LOW) : digitalWrite(relay1, HIGH);
      param -> updateAndReport(val);
    }
  } else if (strcmp(device_name, device2) == 0) {
    Serial.printf("Switch = %s\n", val.val.b? "true" : "false");
    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
      toggle_state2 = val.val.b;
      (toggle_state2 == false) ? digitalWrite(relay2, LOW) : digitalWrite(relay2, HIGH);
      param -> updateAndReport(val);
    }
  } else if (strcmp(device_name, device3) == 0) {
    Serial.printf("Switch = %s\n", val.val.b? "true" : "false");
    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
      toggle_state3 = val.val.b;
      (toggle_state3 == false) ? digitalWrite(relay3, LOW) : digitalWrite(relay3, HIGH);
      param -> updateAndReport(val);
    }
  } else if (strcmp(device_name, device4) == 0) {
    Serial.printf("Switch = %s\n", val.val.b? "true" : "false");
    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
      toggle_state4 = val.val.b;
      (toggle_state4 == false) ? digitalWrite(relay4, LOW) : digitalWrite(relay4, HIGH);
      param -> updateAndReport(val);
    }
  }
}
void setup() {
  uint32_t chip_id = 0;
  Serial.begin(115200);

  
  Serial.begin(9600);

  
  
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(wifi_led, OUTPUT);

  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  pinMode(switch4, INPUT_PULLUP);
  pinMode(gpio_0, INPUT);

  digitalWrite(relay1, toggle_state1);
  digitalWrite(relay2, toggle_state2);
  digitalWrite(relay3, toggle_state3);
  digitalWrite(relay4, toggle_state4);
  digitalWrite(wifi_led, LOW);

  Node Commercial_Node;
   Commercial_Node= RMaker.initNode("ESP_32 Node");


  

  WiFi.onEvent(sysProvEvent);

  my_switch1.addCb(write_callback);
  my_switch2.addCb(write_callback);
  my_switch3.addCb(write_callback);
  my_switch4.addCb(write_callback);

  Commercial_Node.addDevice(my_switch1);
  Commercial_Node.addDevice(my_switch2);
  Commercial_Node.addDevice(my_switch3);
  Commercial_Node.addDevice(my_switch4);

  RMaker.enableOTA(OTA_USING_PARAMS);
  RMaker.enableTZService();
  RMaker.enableSchedule();
  RMaker.enableScenes();


  for (int i = 0; i < 17; i = i+8) {
    chip_id = ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  Serial.printf("\nChip ID : %d, Service Name : %s\n", chip_id, service_name);
  Serial.printf("\nStarting ESP-Rainmaker\n");
  RMaker.start();
  WiFi.onEvent(sysProvEvent);

#if CONFIG_IDF_TARGET_ESP32
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
#else
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
#endif

  my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
  my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
  my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
  my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
}

void manual_control() {
  if (digitalRead(switch1) == LOW && switch_state1 == LOW) {
    digitalWrite(relay1, HIGH);
    toggle_state1 = 1 ;
    switch_state1 = HIGH;
    my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggle_state1);
    Serial.println("Fan is ON");
  }
  if (digitalRead(switch1) == HIGH && switch_state1 == HIGH) {
    digitalWrite(relay1, LOW);
    toggle_state1 = 1;
    switch_state1 = LOW;
    my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggle_state1);
    Serial.println("Fan is OFF");
  }
  
  if (digitalRead(switch2) == LOW && switch_state2 == LOW) {
    digitalWrite(relay2, HIGH);
    toggle_state2 = 1;
    switch_state2 = HIGH;
    my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggle_state2);
    Serial.println("Pink Night Bulb is ON");
  }
  if (digitalRead(switch2) == HIGH && switch_state2 == HIGH) {
    digitalWrite(relay2, LOW);
    toggle_state2 = 1;
    switch_state2 = LOW;
    my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggle_state2);
    Serial.println("Pink Night Bulb is OFF");
  }
  
  if (digitalRead(switch3) == LOW && switch_state3 == LOW) {
    digitalWrite(relay3, HIGH);
    toggle_state3 = 1;
    switch_state3 = HIGH;
    my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggle_state3);
    Serial.println("White Bulb is ON");
  }
  if (digitalRead(switch3) == HIGH && switch_state3 == HIGH) {
    digitalWrite(relay3, LOW);
    toggle_state3 = 1;
    switch_state3 = LOW;
    my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggle_state3);
    Serial.println("White Bulb is OFF");
  }

  if (digitalRead(switch4) == LOW && switch_state4 == LOW) {
    digitalWrite(relay4, HIGH);
    toggle_state4 = 1;
    switch_state4 = HIGH;
    my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggle_state4);
    Serial.println("Blue Night Bulb is ON");
  }
  if (digitalRead(switch4) == HIGH && switch_state4 == HIGH) {
    digitalWrite(relay4, LOW);
    toggle_state4 = 1;
    switch_state4 = LOW;
    my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggle_state4);
    Serial.println("Blue Night Bulb is OFF");
  }
}

void loop() {
  if (digitalRead(gpio_0) == LOW) {
    Serial.printf("Reset button pressed\n");
    delay(100);
    int start_time = millis();
    while (digitalRead(gpio_0) == LOW) delay(50);
    int end_time = millis();

    if ((end_time - start_time) > 10000) {
      Serial.printf("Reset to factory.\n");
      RMakerFactoryReset(2);
    } else if ((end_time - start_time) > 3000) {
      Serial.printf("Reset Wi-Fi.\n");
      RMakerWiFiReset(2);
    }
  }
  delay(100);
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(wifi_led, false);
  } else {
    digitalWrite(wifi_led, true);
  }
  manual_control();
}

