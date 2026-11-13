  #pragma once

  #ifdef ARDUINO

  #include <Arduino_GFX_Library.h>

  #if defined(ST7789_DRIVER) or defined(ST7789V_DRIVER)
    Arduino_HWSPI bus(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO, &SPI, false);
    Arduino_ST7789 tft(&bus, TFT_RST, 0 /* rotation */, false /* IPS */, TFT_WIDTH /* width */, TFT_HEIGHT /* height */, 0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col offset 2 */, 80 /* row offset 2 */);
    using TFT_eSPI = Arduino_ST7789;

  #elif defined(ILI9342_DRIVER)
    Arduino_HWSPI bus(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO, &SPI, true);
    Arduino_ILI9342 tft(&bus, TFT_RST, TFT_ROTATE /* rotation */, true /* IPS */);
    using TFT_eSPI = Arduino_ILI9342;

  #elif defined(DSI_DRIVER)
    Arduino_ESP32DSIPanel dsipanel(
        display_cfg.hsync_pulse_width,
        display_cfg.hsync_back_porch,
        display_cfg.hsync_front_porch,
        display_cfg.vsync_pulse_width,
        display_cfg.vsync_back_porch,
        display_cfg.vsync_front_porch,
        display_cfg.prefer_speed,
        display_cfg.lane_bit_rate);

    Arduino_DSI_Display tft(
        display_cfg.width,
        display_cfg.height,
        dsipanel,
        0,
        true,
        -1,
        display_cfg.init_cmds,
        display_cfg.init_cmds_size);

    using TFT_eSPI = Arduino_DSI_Display;

  #else
    #error Select on of the suported Arduino_GFX display drivers
  #endif

// #include <SPI.h>
// #include <TFT_eSPI.h>
// #include <xpt2046.h>

#endif