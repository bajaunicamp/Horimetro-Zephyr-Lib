#ifndef HORIMETRO_H
#define HORIMETRO_H

#include <zephyr/drivers/gpio.h>

/**
 * @file
 * @brief Biblioteca para a utilização do horímetro
 *
 * Este arquivo contém as definições e funções necessárias para inicialiazar 
 * e ler dados do horímetro desenvolvido pela equipe UNICAP Baja SAE
 *
 * @author Giancarlo Bonvenuto
 */

/**
 * @brief struct que contém todos os dados do horímetro
 *
 * Essa biblioteca não tem o propósito de possuir getters para
 * garantir que o código não tenha o overhead da função get().
 *
 * Então os valores devem ser acessados diretamente pelo struct, mas
 * não devem ser modificados!
 *
 */
extern struct horimetro_t {

  /**
   * @brief guarda o ciclo em que ocorreu o último pulso. **Não deve ser
   * alterado!**
   */
  uint32_t ultimoPulso;

  /**
   * @brief contém o RPM atual do motor
   */

  uint32_t rpm;
  /**
   * @brief contém o RPM mínimo que o motor consegue chegar e é definido no
   * `prj.conf`
   *
   * Esse campo auxilia as funções de calcular o RPM para saber se o motor está
   * parado ou ainda está girando.
   */

  uint32_t rpmMin; 
  /**
   * @brief um booleano que contém o estado do motor
   *
   * - `true`: o motor está ligado
   * - `false`: o motor está desligado
   */

  bool motorLigado;
  /**
   * @brief contém o device do sensor_hall e é definido na DeviceTree
   */

  const struct gpio_dt_spec *sensor_hall;
} horimetro;

/**
 * @brief essa função deve ser executada antes de qualquer acesso ao struct do
 * horímetro
 */
int horimetro_init();

#endif
