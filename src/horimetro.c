#include "horimetro.h"
#include <stdlib.h>
#include "zephyr/drivers/gpio.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/timing/timing.h>
#include <zephyr/sys/time_units.h>

LOG_MODULE_REGISTER(Horimetro);

static void hall_callback(const struct device *dev, struct gpio_callback *cb,
                          uint32_t pins) {
  uint32_t cycle = k_cycle_get_32();
  sensor_hall.rpm = k_cyc_to_ms_near32(abs(sensor_hall.ultimoPulso - cycle));
  sensor_hall.ultimoPulso = cycle;
  LOG_INF("RPM: %u", sensor_hall.rpm);
}

int hall_init() {
  sensor_hall.ultimoPulso = -1; // Ciclo do último pulso
  sensor_hall.rpm = 0;
  sensor_hall.rpmMin = 1500; // E máximo por volta de 4200
  sensor_hall.dev =
      &(const struct gpio_dt_spec)GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), gpios);

  if (!gpio_is_ready_dt(sensor_hall.dev)) {
    LOG_ERR("A porta %s não está pronta\n", sensor_hall.dev->port->name);
    return 0;
  }

  int err;

  err = gpio_pin_configure_dt(sensor_hall.dev, GPIO_INPUT);
  if (err < 0) {
    LOG_ERR("Não foi possível configurar o pino do sensor (%d)", err);
    return err;
  }

  err =
      gpio_pin_interrupt_configure_dt(sensor_hall.dev, GPIO_INT_EDGE_TO_ACTIVE);

  if (err < 0) {
    LOG_ERR("Não foi possível configurar o pino %s %d (Erro %d)",
            sensor_hall.dev->port->name, sensor_hall.dev->pin, err);
  }
  return err;

  gpio_init_callback(NULL, hall_callback, BIT(sensor_hall.dev->pin));
  gpio_add_callback_dt(sensor_hall.dev, NULL);
}
