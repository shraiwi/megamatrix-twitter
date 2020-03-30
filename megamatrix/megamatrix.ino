#include <SmartLEDShieldV4.h>
#include <SmartMatrix3.h>
#include <Arduino.h>
#include <WiFi.h>
#include "webpage.h"
#include "ESPAsyncWebServer.h"

#define WIFI_SSID "YOUR_SSID"
#define WIFI_PWRD "YOUR_PWRD"

#define CW_DEPTH 8
#define CW_STEPS (1<<CW_DEPTH)
#define COLOR_DEPTH 24                  // known working: 24, 48 - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24
const uint8_t kMatrixWidth = 64;        // known working: 32, 64, 96, 128
const uint8_t kMatrixHeight = 64;       // known working: 16, 32, 48, 64
const uint8_t kRefreshDepth = 36;       // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN;   // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_BOTTOM_TO_TOP_STACKING);      // see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(canvas, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);

AsyncWebServer server(80);
uint8_t frame[kMatrixWidth*kMatrixHeight*3];
uint32_t head = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  matrix.addLayer(&canvas);
  matrix.begin();
  
  matrix.setBrightness(0x60);
  matrix.setRotation(rotation90);
  
  canvas.enableColorCorrection(true);
  canvas.fillScreen({0, 0, 0});
  canvas.setFont(font5x7);

  // initialize wifi
  Serial.println("Connecting to wifi...");

  /*WiFi.softAP(WIFI_SSID, WIFI_PWRD);
  IPAddress ip = WiFi.softAPIP();*/
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PWRD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!\n");
    return;
  }

  // print ip
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  // setup server
  server.on("/", HTTP_GET, handleGet);
  server.on("/img", HTTP_POST, handleGet, NULL, handleBody);

  server.begin();

  canvas.drawString(2, 2, {0xff, 0xff, 0xff}, ipToString(ip).c_str());
}

void loop() {
  // put your main code here, to run repeatedly:
  canvas.swapBuffers();
}

void handleGet(AsyncWebServerRequest *req) {
  Serial.println("handling request...");
  req->send(200, "text/html", WEBPAGE);  // send response
}

void handleBody(AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total) {
  if (!index) {                         // detect file upload start, reset buffers and pointers
    head = 0;                           // reset file head
    memset(frame, 0, sizeof(frame));    // clear framebuffer
    Serial.println("file upload start!");
  }
  Serial.printf("\tcopying chunk %d/%d...\n", head, total);

  rgb24 white = {0, 0, 0};

  canvas.fillRectangle(0, 0, head / (kMatrixHeight*3), kMatrixHeight, white);
  
  memcpy(frame + head, data, len);      // copy data chunk
  head += len;                          // advance data pointer
  if (index + len == total) {           // calculate sum on frame end
    for (uint8_t y = 0; y < kMatrixHeight; y++) {
      for (uint8_t x = 0; x < kMatrixWidth; x++) {
        uint32_t pxi = (y * kMatrixWidth + x) * 3;
        rgb24 color = *(rgb24*)(frame + pxi);
        canvas.drawPixel(x, y, color);
      }
    }
    uint32_t sum = 0;
    for (size_t i = 0; i < sizeof(frame); i++) {
      sum += frame[i];
    }
    Serial.printf("upload done!\n\tdatasum: %u\n", sum);
  }
}

String ipToString(IPAddress ip){
  String s = "";
  for (int i=0; i<4; i++)
    s += i ? "." + String(ip[i]) : String(ip[i]);
  return s;
}
