/*
  Step 4 test sketch for the 4x8 P10 matrix.
  Draws a timed radial rainbow that completes one clockwise rotation roughly every 12 seconds.
  This variant precomputes a quantized wheel position for each pixel during setup.
*/

#include <math.h>

#include <MatrixHardware_Teensy4_ShieldV5.h>
#include <SmartMatrix.h>

#define COLOR_DEPTH 24

const uint8_t kPanelColumns = 4;
const uint8_t kPanelRows = 8;
const uint16_t kSinglePanelWidth = 32;
const uint16_t kSinglePanelHeight = 16;

const uint16_t kMatrixWidth = kPanelColumns * kSinglePanelWidth;
const uint16_t kMatrixHeight = kPanelRows * kSinglePanelHeight;

//const uint8_t kRefreshDepth = 36;
const uint8_t kDmaBufferRows = 8;
const uint8_t kRefreshDepth = 24;
//const uint8_t kDmaBufferRows = 16;
const uint8_t kPanelType = SMARTMATRIX_HUB75_16ROW_MOD8SCAN;

const bool kUseCShapeStacking = true;
const bool kStartsFromBottomRow = true;
const rotationDegrees kMatrixRotation = rotation0;

const uint32_t kMatrixOptions =
    (kUseCShapeStacking ? SM_HUB75_OPTIONS_C_SHAPE_STACKING : SM_HUB75_OPTIONS_NONE) |
    (kStartsFromBottomRow ? SM_HUB75_OPTIONS_BOTTOM_TO_TOP_STACKING : SM_HUB75_OPTIONS_NONE);

const uint8_t kBackgroundLayerOptions = SM_BACKGROUND_OPTIONS_NONE;

const uint8_t kDefaultBrightness = 72;
//const uint8_t kDefaultBrightness = 48;
const uint16_t kFrameIntervalMs = 33;
const uint32_t kSerialSampleIntervalMs = 100;
//const uint32_t kSerialSampleIntervalMs = 100; // test
const uint32_t kRotationPeriodMs = 12000;
const uint16_t kWheelRangeSteps = 768;
const float kWheelRange = 768.0f;
const bool kEnableSerialBleach = true; //false;

SMARTMATRIX_ALLOCATE_BUFFERS(
    matrix,
    kMatrixWidth,
    kMatrixHeight,
    kRefreshDepth,
    kDmaBufferRows,
    kPanelType,
    kMatrixOptions);

SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(
    backgroundLayer,
    kMatrixWidth,
    kMatrixHeight,
    COLOR_DEPTH,
    kBackgroundLayerOptions);

uint32_t lastFrameMs = 0;
uint32_t lastSerialSampleMs = 0;
float currentHealth = 1.0f;
uint16_t cachedWheelPositions[kMatrixWidth * kMatrixHeight];

uint16_t mapY(uint16_t y) {
  //return y;
  const uint16_t panelRow = y / kSinglePanelHeight;
  const uint16_t localY = y % kSinglePanelHeight;
  return panelRow * kSinglePanelHeight + (kSinglePanelHeight - 1 - localY);
}

uint32_t pixelIndex(uint16_t x, uint16_t y) {
  return (uint32_t)y * kMatrixWidth + x;
}

void drawMappedPixel(uint16_t x, uint16_t y, const rgb24& color) {
  backgroundLayer.drawPixel(x, mapY(y), color);
}

rgb24 wheelToRgb(float wheelPosition) {
  while (wheelPosition < 0.0f) {
    wheelPosition += kWheelRange;
  }

  while (wheelPosition >= kWheelRange) {
    wheelPosition -= kWheelRange;
  }

  rgb24 color;

  if (wheelPosition < 256.0f) {
    color.red = (uint8_t)(255.0f - wheelPosition);
    color.green = 0;
    color.blue = (uint8_t)(wheelPosition);
    return color;
  }

  if (wheelPosition < 512.0f) {
    wheelPosition -= 256.0f;
    color.red = 0;
    color.green = (uint8_t)(wheelPosition);
    //color.green = 0;
    color.blue = (uint8_t)(255.0f - wheelPosition);
    return color;
  }

  wheelPosition -= 512.0f;
  color.red = (uint8_t)(wheelPosition);
  color.green = (uint8_t)(255.0f - wheelPosition);
  //color.green = 0;
  color.blue = 0;
  return color;
}

rgb24 applyBleach(float health, const rgb24& color) {
  rgb24 bleachedColor;
  if (health < 0.0f) {
    health = 0.0f;
  } else if (health > 1.0f) {
    health = 1.0f;
  }

  const float bleaching = 1.0f - health;
  const float softenWhite = 1.0f - 0.5f * bleaching;
  const uint8_t maxWhiteness = 150;
  //const float blueScale = powf(health, 4.0f); // seems unnecessary with these lights

  bleachedColor.red = (uint8_t)((health * color.red + bleaching * 255.0f) * softenWhite);
  bleachedColor.green = (uint8_t)((health * color.green + bleaching * 255.0f) * softenWhite);
  bleachedColor.blue = (uint8_t)((health * color.blue + bleaching * 255.0f) * softenWhite);
  //bleachedColor.red = (uint8_t)(health * color.red + min(maxWhiteness,bleaching * 255.0f));
  //bleachedColor.green = (uint8_t)(health * color.green + min(maxWhiteness,bleaching * 255.0f));
  //bleachedColor.blue = (uint8_t)(health * color.blue + min(maxWhiteness,bleaching * 255.0f));

  //bleachedColor.red = (uint8_t)(health * color.red + bleaching * 255.0f * softenWhite);
  //bleachedColor.green = (uint8_t)(health * color.green + bleaching * 255.0f * softenWhite);
  //bleachedColor.blue = (uint8_t)(health * color.blue + bleaching * 255.0f * softenWhite);

  return bleachedColor;
}

float sampleHealthFromSerial() {
  int latestByte = -1;

  while (Serial.available() > 0) {
    latestByte = Serial.read();
  }

  if (latestByte < 0) {
    return currentHealth;
  }

  return ((uint8_t)latestByte) / 255.0f;
}

float getEffectiveHealth() {
  if (!kEnableSerialBleach) {
    return 1.0f;
    //return 0.05f; // test
  }

  return currentHealth;
}

float angleToWheel(float angleRadians) {
  return (384.0f * angleRadians) / PI;
}

uint16_t quantizeWheelPosition(float wheelPosition) {
  while (wheelPosition < 0.0f) {
    wheelPosition += kWheelRange;
  }

  while (wheelPosition >= kWheelRange) {
    wheelPosition -= kWheelRange;
  }

  uint16_t quantized = (uint16_t)(wheelPosition + 0.5f);
  if (quantized >= kWheelRangeSteps) {
    quantized = 0;
  }

  return quantized;
}

void initializeWheelCache() {
  const float centerX = (kMatrixWidth - 1) * 0.5f;
  const float centerY = (kMatrixHeight - 1) * 0.5f;

  for (uint16_t x = 0; x < kMatrixWidth; ++x) {
    for (uint16_t y = 0; y < kMatrixHeight; ++y) {
      const float angle = atan2f(y - centerY, x - centerX);
      cachedWheelPositions[pixelIndex(x, y)] = quantizeWheelPosition(angleToWheel(angle));
    }
  }
}

void drawFrame(uint32_t elapsedMs, float health) {
  const uint16_t rotationOffset =
      (uint16_t)(((uint64_t)kWheelRangeSteps * (elapsedMs % kRotationPeriodMs)) / kRotationPeriodMs);

  for (uint16_t x = 0; x < kMatrixWidth; ++x) {
    for (uint16_t y = 0; y < kMatrixHeight; ++y) {
      uint16_t wheelPosition = cachedWheelPositions[pixelIndex(x, y)];
      if (wheelPosition >= rotationOffset) {
        wheelPosition -= rotationOffset;
      } else {
        wheelPosition = (uint16_t)(wheelPosition + kWheelRangeSteps - rotationOffset);
      }

      drawMappedPixel(x, y, applyBleach(health, wheelToRgb((float)wheelPosition)));
    }
  }
  backgroundLayer.swapBuffers(true);
  //backgroundLayer.swapBuffers(false);
}

void setup() {
  //Serial.begin(115200); // previous notes say this is ignored for Teensy

  matrix.addLayer(&backgroundLayer);
  matrix.begin();
  matrix.setRotation(kMatrixRotation);
  matrix.setBrightness(kDefaultBrightness);

  backgroundLayer.enableColorCorrection(true);
  initializeWheelCache();

  Serial.println("MatrixRadialRainbowQuantizedCacheTest");
  Serial.print("Resolution: ");
  Serial.print(kMatrixWidth);
  Serial.print("x");
  Serial.println(kMatrixHeight);
  Serial.print("Rotation period (ms): ");
  Serial.println(kRotationPeriodMs);
  Serial.print("Matrix options: 0x");
  Serial.println(kMatrixOptions, HEX);
  Serial.print("Cache bytes: ");
  Serial.println(sizeof(cachedWheelPositions));
  if (kEnableSerialBleach) {
    Serial.println("Send one byte 0-255 over Serial to change health/bleach.");
  }

  drawFrame(0, getEffectiveHealth());
}

void loop() {
  const uint32_t now = millis();

  if (kEnableSerialBleach && now - lastSerialSampleMs >= kSerialSampleIntervalMs) {
    lastSerialSampleMs = now;
    currentHealth = sampleHealthFromSerial();
    //Serial.print("Health: ");
    //Serial.println(currentHealth);
  }

  if (now - lastFrameMs < kFrameIntervalMs) {
    return;
  }

  lastFrameMs = now;
  drawFrame(now, getEffectiveHealth());
  //drawFrame(now, 1.0f); // test
}
