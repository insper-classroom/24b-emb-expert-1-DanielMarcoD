#include <stdio.h>
#include "pico/stdlib.h"   // stdlib 
#include "hardware/pwm.h"  // pwm 
#include "hardware/adc.h"  // adc

// Definições de pinos e constantes
#define AUDIO_PIN 28
#define ADC_NUM 0
#define ADC_PIN (26 + ADC_NUM)
#define BUTTON_PIN 14

int main(void) {
    // Inicialização
    stdio_init_all();
    set_sys_clock_khz(176000, true);

    // Configuração do pino de áudio como PWM
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);
    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

    // Configuração do PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 8.0f);
    pwm_config_set_wrap(&config, 250);
    pwm_init(audio_pin_slice, &config, true);
    pwm_set_gpio_level(AUDIO_PIN, 0);

    // Configuração do ADC
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_NUM);

    // Configuração do botão
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Variáveis locais
    uint16_t adc_raw;
    int button_state = 1;
    int speaking = 0;

    while (true) {
        // Leitura do estado do botão
        button_state = gpio_get(BUTTON_PIN);

        // Verifica se o botão está pressionado
        if (button_state == 0) {
            if (!speaking) {
                printf("Botão pressionado, iniciando reprodução em tempo real...\n");
                speaking = 1;
            }
            // Captura e reproduz o áudio em tempo real
            adc_raw = adc_read();
            uint16_t pwm_value = adc_raw >> 4; // Convertendo 12 bits para 8 bits
            pwm_set_gpio_level(AUDIO_PIN, pwm_value);
            // Controle da taxa de amostragem
            sleep_us(90); // Ajuste este valor conforme necessário
        } else {
            if (speaking) {
                printf("Botão solto, parando reprodução.\n");
                speaking = 0;
                pwm_set_gpio_level(AUDIO_PIN, 0); // Desliga o áudio
            }
            // Aguarda próximo evento
            sleep_ms(1);
        }
    }

    return 0;
}
