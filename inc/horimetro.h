#ifndef HORIMETRO_H
#define HORIMETRO_H

#include <zephyr/drivers/gpio.h>

struct {
  uint32_t ultimoPulso; // Ciclo em que o último pulso foi enviado
  uint32_t rpm; // RPM calculado
  uint32_t rpmMin; // RPM mínimo que o motor chega (abaixo disso o motor está desligado)
  const struct gpio_dt_spec* dev; // Dev
} sensor_hall;

// Essa função retorna quanto tempo faz desde o ultimo pulso
int hall_init();

#endif
