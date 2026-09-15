#pragma once

// XIAO ESP32C3 + 1.28" GC9A01 round panel version.
//
// Unlike the Core2 port, there's no PMIC involved here - GC9A01 modules
// take plain 3.3V/GND and are driven directly over SPI, so a hand-rolled
// LGFX_Device subclass (like the original hand-wired ESP32 DevKit build)
// is the right approach again, not M5GFX.
//
// Wiring (XIAO ESP32C3 pin -> GC9A01 pin):
//   D8  (GPIO8)  -> SCL/SCLK
//   D10 (GPIO10) -> SDA/MOSI
//   D0  (GPIO2)  -> CS
//   D1  (GPIO3)  -> DC
//   D2  (GPIO4)  -> RST
//   BLK -> 3V3 directly (no PWM dimming; wire to a GPIO instead if you want
//          brightness control, and add a Light_PWM config below)
//   VCC -> 3V3, GND -> GND

#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_GC9A01 _panel_instance;
    lgfx::Bus_SPI       _bus_instance;

public:
    LGFX(void)
    {
        {
            auto cfg = _bus_instance.config();

            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read  = 16000000;
            cfg.spi_3wire  = true;   // no MISO wired
            cfg.use_lock   = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;

            cfg.pin_sclk = 8;   // D8
            cfg.pin_mosi = 10;  // D10
            cfg.pin_miso = -1;  // not connected
            cfg.pin_dc   = 3;   // D1

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {
            auto cfg = _panel_instance.config();

            cfg.pin_cs   = 2;   // D0
            cfg.pin_rst  = 4;   // D2
            cfg.pin_busy = -1;

            cfg.panel_width  = 240;
            cfg.panel_height = 240;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.readable   = false; // GC9A01 read-back not used here
            cfg.invert     = true;  // most GC9A01 round panels need this
            cfg.rgb_order  = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = false; // nothing else on this SPI bus

            _panel_instance.config(cfg);
        }

        setPanel(&_panel_instance);
    }
};

extern LGFX tft;
