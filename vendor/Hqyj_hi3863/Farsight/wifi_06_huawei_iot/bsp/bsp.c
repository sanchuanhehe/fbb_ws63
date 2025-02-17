#include "bsp.h"


void my_gpio_init(pin_t gpio)
{
    uapi_pin_set_mode(gpio, HAL_PIO_FUNC_GPIO);

    uapi_gpio_set_dir(gpio, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(gpio, GPIO_LEVEL_LOW);
}

gpio_level_t my_io_readval(pin_t gpio)
{
   return uapi_gpio_get_output_val(gpio);
}

void my_io_setval(pin_t gpio, gpio_level_t level)
{
  uapi_gpio_set_val(gpio, level);
}