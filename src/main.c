#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

const struct gpio_dt_spec *const led = &(const struct gpio_dt_spec)GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);

int main(){
  while (true) {
    k_sleep(K_USEC(100));
  }
}
