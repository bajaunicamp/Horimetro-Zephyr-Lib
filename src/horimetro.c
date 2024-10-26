#include "horimetro.h"
#include "zephyr/drivers/gpio.h"
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/time_units.h>
#include <zephyr/timing/timing.h>

LOG_MODULE_REGISTER(Horimetro);

// Vou destacar tudo que tem a ver com o timer para que caso não funcione seja
// mais fácil de remover
// === Coisas to timer ===
// Verifica periodicamente se o motor está ligado (Se essa função está sendo
// executada, é porque o timer expirou e o motor está desligado)
static struct k_timer my_timer;
static void motor_desligado(struct k_timer *timer_id) {
  sensor_hall.motorLigado = false;
};

// === \Coisas to timer ===

// Callback quando o pino vira HIGH
static void hall_callback(const struct device *dev, struct gpio_callback *cb,
                          uint32_t pins) {
  uint32_t cycle = k_cycle_get_32();
  sensor_hall.rpm =
      60000 / k_cyc_to_ms_near32(abs(sensor_hall.ultimoPulso - cycle));
  sensor_hall.ultimoPulso = cycle;
  LOG_INF("RPM: %u", sensor_hall.rpm);

  // Resetamos o timer que verifica se o motor está ligado
  // não precisamos veriricar se o motor está ligado se acabamos de receber
  // sinal dele
  sensor_hall.motorLigado = true;
  k_timer_start(
      &my_timer,
      K_MSEC((60000 / sensor_hall.rpmMin) + 100 // Uma gordurinha de 100ms
             ),
      K_NO_WAIT);
}

int hall_init() {
  sensor_hall.ultimoPulso = -1; // Ciclo do último pulso
  sensor_hall.rpm = 0;
  sensor_hall.rpmMin = 1500; // E máximo por volta de 4200
                             // Talvez possamos configurar isso aqui na
                             // Desvice tree? 🤔
  sensor_hall.dev =
      &(const struct gpio_dt_spec)GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), gpios);
  sensor_hall.motorLigado = false;

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

  // Inicia o timer que verifica se o motor está ligado
  // __Esse timer não se reinicia sozinho__
  k_timer_init(&my_timer, motor_desligado, NULL);
  k_timer_start(
      &my_timer,
      K_MSEC((60000 / sensor_hall.rpmMin) + 100 // Uma gordurinha de 100ms
             ),
      K_NO_WAIT);
}
