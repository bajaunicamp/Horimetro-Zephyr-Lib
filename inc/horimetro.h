#ifndef HORIMETRO_H
#define HORIMETRO_H

#include <zephyr/drivers/gpio.h>

extern struct horimetro_t {
  uint32_t ultimoPulso; // Ciclo em que o último pulso foi enviado
  uint32_t rpm; // RPM calculado
  uint32_t rpmMin; // RPM mínimo que o motor chega (abaixo disso o motor está desligado)
  bool motorLigado;
  const struct gpio_dt_spec* sensor_hall; // Dev
} horimetro;

// Essa função retorna quanto tempo faz desde o ultimo pulso
int horimetro_init();

#endif
