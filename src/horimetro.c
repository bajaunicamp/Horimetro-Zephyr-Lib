#include "horimetro.h"
#include "zephyr/drivers/gpio.h"
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/time_units.h>
#include <zephyr/timing/timing.h>

// quantos milissegundos de gordurinha adicionamos ao timer
#define GordurinhaTIMER 100

LOG_MODULE_REGISTER(Horimetro);

// Vou destacar tudo que tem a ver com o timer para que caso não funcione seja
// mais fácil de remover
// === Coisas to timer ===
// Verifica periodicamente se o motor está ligado (Se essa função está sendo
// executada, é porque o timer expirou e o motor está desligado)
static struct k_timer my_timer;
static void motor_desligado(struct k_timer *timer_id) {
  horimetro.motorLigado = false;
  LOG_INF("Motor Desligado");
};

// === \Coisas to timer ===

// Callback quando o pino vira HIGH
// Pegamos o ciclo atual e a partir dele e do ultimoPulso conseguimos descobrir
// o rpm. Então guardamos esse ciclo como o últimoPulso e definimos o motor como
// ligado e reiniciamos o timer que definiria o motor como desligado
static void hall_callback(const struct device *dev, struct gpio_callback *cb,
                          uint32_t pins) {
  uint32_t cycle = k_cycle_get_32();
  horimetro.rpm =
      60000 / k_cyc_to_ms_near32(abs(horimetro.ultimoPulso - cycle));
  horimetro.ultimoPulso = cycle;
  LOG_INF("RPM: %u", horimetro.rpm);

  // Resetamos o timer que verifica se o motor está ligado
  // não precisamos veriricar se o motor está ligado se acabamos de receber
  // sinal dele
  horimetro.motorLigado = true;
  k_timer_start(&my_timer,
                K_MSEC((60000 / horimetro.rpmMin) + GordurinhaTIMER),
                K_NO_WAIT);
}

int hall_init() {
  horimetro.ultimoPulso = -1; // Ciclo do último pulso
  horimetro.rpm = 0;
  horimetro.rpmMin = 1500; // E máximo por volta de 4200
                             // Talvez possamos configurar isso aqui na
                             // Desvice tree? 🤔
  horimetro.sensor_hall =
      &(const struct gpio_dt_spec)GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), gpios);
  horimetro.motorLigado = false;

  if (!gpio_is_ready_dt(horimetro.sensor_hall)) {
    LOG_ERR("A porta %s não está pronta\n", horimetro.sensor_hall->port->name);
    return 0;
  }

  int err;

  err = gpio_pin_configure_dt(horimetro.sensor_hall, GPIO_INPUT);
  if (err < 0) {
    LOG_ERR("Não foi possível configurar o pino do sensor (%d)", err);
    return err;
  }

  err =
      gpio_pin_interrupt_configure_dt(horimetro.sensor_hall, GPIO_INT_EDGE_TO_ACTIVE);

  if (err < 0) {
    LOG_ERR("Não foi possível configurar o pino %s %d (Erro %d)",
            horimetro.sensor_hall->port->name, horimetro.sensor_hall->pin, err);
  }
  return err;

  gpio_init_callback(NULL, hall_callback, BIT(horimetro.sensor_hall->pin));
  gpio_add_callback_dt(horimetro.sensor_hall, NULL);

  // Inicia o timer que verifica se o motor está ligado
  // __Esse timer não se reinicia sozinho__
  k_timer_init(&my_timer, motor_desligado, NULL);
  k_timer_start(&my_timer,
                K_MSEC((60000 / horimetro.rpmMin) + GordurinhaTIMER),
                K_NO_WAIT);
}
