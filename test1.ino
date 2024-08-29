#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>  // Thêm thư viện DHT

// WiFi 
const char* ssid = "CuongDZ"; // Wifi SSID
const char* password = "0901926570"; // Wifi Password

// MQTT Broker
const char* mqtt_server = "192.168.1.82";
unsigned int mqtt_port = 1883; //SSL 8883 NoneSSL 1883
const char* username = "master:mqtt"; // Service User Realm:Serviceuser
const char* mqttpass = "hCFzb4XBXv4qvK5P2jZAIgiGh35HpJDP"; // Service User Secret
const char* ClientID = "client123"; 
// LastWill
const char* lastwill = "master/client123/writeattributevalue/writeAttribute/28Bt1qsMpKCx9qvWEDLaLn";
const char* lastwillmsg = "0";

// Subscribing Topic
const char *topic = "master/client123/attribute/writeAttribute/28Bt1qsMpKCx9qvWEDLaLn";

const char* local_root_ca = \
                            "-----BEGIN CERTIFICATE-----\n" \
                            "MIIDnzCCAoegAwIBAgIUE3jYzxKpepVM0CSLZd9GNv6BHj8wDQYJKoZIhvcNAQEL\n" \
                            "BQAwUDELMAkGA1UEBhMCR0IxHTAbBgNVBAMMFE9wZW5SZW1vdGUgRGVtbyBDZXJ0\n" \
                            "MRMwEQYDVQQKDApPcGVuUmVtb3RlMQ0wCwYDVQQLDAREZW1vMCAXDTIwMDYwODE5\n" \
                            "MTc1MVoYDzIwNTAwNjAxMTkxNzUxWjBQMQswCQYDVQQGEwJHQjEdMBsGA1UEAwwU\n" \
                            "T3BlblJlbW90ZSBEZW1vIENlcnQxEzARBgNVBAoMCk9wZW5SZW1vdGUxDTALBgNV\n" \
                            "BAsMBERlbW8wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCrKEk77HcJ\n" \
                            "B5SqvoN2UbRsDh9d0ECN8tOU5hC2poih+6XBJgikQ8gdy7ptt477KRh3ZIiw3ZTX\n" \
                            "Hg0//Ju71D/4EDBYwHxoSK9WehP5Kz/LrBHhtArXK3RYH8pFS13CDOPjXnm6LMN5\n" \
                            "2mRGwm2gCwKwRTbfm+D9hjpVuwt0sfHaXVETlUc4JystlfYVurMcfsox9tsbRuzl\n" \
                            "EakyK9Cr1V7bgaLMosHDX3NSuEyzb9DQZ3PBK3JjJhSeYkGNuP/NocMrWy/JHd2v\n" \
                            "2Wev9W+D1Pv46Sqfrvd6K7oP00FL0CdODkMRBVTlb1wq/6uJdRbnVUM0PGA9enrQ\n" \
                            "vMB11fFglHa3AgMBAAGjbzBtMB0GA1UdDgQWBBT0ixs03BOrns+E2+xSU+nfP9KX\n" \
                            "iTAfBgNVHSMEGDAWgBT0ixs03BOrns+E2+xSU+nfP9KXiTAPBgNVHRMBAf8EBTAD\n" \
                            "AQH/MBoGA1UdEQQTMBGCCWxvY2FsaG9zdIcEfwAAATANBgkqhkiG9w0BAQsFAAOC\n" \
                            "AQEAawmLoD7bzFTM0Z58PR6jQR3ypD6IAyei6xiBI7wvxbjyxqQrk1i0rK2Aexjk\n" \
                            "v2ZsAUmtrm5k5pWpBsokNuRddPV1K2OZjTj9HPc9AxqjyHKyqRXmVKWkzbWQDLVS\n" \
                            "lGRk7yviUFS8nRuY0vLfqZzF7e2HeasThILJibY8rUVLuq+iMS35RDwQ9usIOiYz\n" \
                            "dF4CO3HFZ6NtDheM1mPAy4Q76H1P1fINuA8mp/by9J8heexqjgpBKYexiQhjb1A7\n" \
                            "NBdWbJPXoNJplGXjGIbj8KxW61ih1wDRE2ZseOflRstO9/Txm7+Cuqo+WBOK39cU\n" \
                            "CXPKre2pqmkIu65wJ6VcTKeSqw==" \
                            "-----END CERTIFICATE-----";

// DHT Sensor Settings
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); 

// Objects
WiFiClient askClient; // WiFi Client
PubSubClient client(askClient);

void setup() {
  Serial.begin(115200);
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  dht.begin();

  delay(10000); // Delay 10 giây trước khi gửi dữ liệu tiếp theo
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Đọc dữ liệu từ cảm biến DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Kiểm tra xem dữ liệu có hợp lệ không
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Chuyển đổi dữ liệu thành chuỗi để publish lên MQTT
  String temp = String(t);
  String hum = String(h);

  // Publish dữ liệu lên các topic MQTT
  client.publish("master/client123/writeattributevalue/temp/28Bt1qsMpKCx9qvWEDLaLn", temp.c_str());
  client.publish("master/client123/writeattributevalue/hum/28Bt1qsMpKCx9qvWEDLaLn", hum.c_str());
  Serial.print("Temp: ");
  Serial.println(temp.c_str());
  Serial.print("Hum: ");
  Serial.println(hum.c_str());
  Serial.println("- - - - -");

  delay(10000); // Delay 10 giây trước khi gửi dữ liệu tiếp theo
}

// MQTT callback
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// MQTT reconnect
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(ClientID, username, mqttpass, lastwill, 1, 1, lastwillmsg)) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
