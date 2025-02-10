#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "ws2812.pio.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15 
#define endereco 0x3C 

#define NUM_PIXELS 25
#define WS2812_PIN 7
#define IS_RGBW false

static PIO pio = pio0;
static uint sm;
static uint offset;

const uint button_A = 5;
const uint button_B = 6;
const uint ledBlue_pin = 12;
const uint ledGreen_pin = 11;


static volatile int num = 0;
static volatile uint a = 0;
static volatile uint b = 0;
static volatile uint last_time = 0;

bool cor = true;
ssd1306_t ssd; // Inicializa a estrutura do display
 
int mapa_leds[25] = {
    24, 23, 22, 21, 20,  
    15, 16, 17, 18, 19,  
    14, 13, 12, 11, 10,  
    5,  6,  7,  8,  9,  
    4,  3,  2,  1,  0   
};

// Dígito 0
static bool led_digit0[NUM_PIXELS] = {
    0,1,1,1,0,
    1,0,0,0,1,
    1,0,0,0,1,
    1,0,0,0,1,
    0,1,1,1,0
};

// Dígito 1
static bool led_digit1[NUM_PIXELS] = {
    1,1,1,0,0,
    0,0,1,0,0,
    0,0,1,0,0,
    0,0,1,0,0,
    0,1,1,1,0
};

// Dígito 2
static bool led_digit2[NUM_PIXELS] = {
    1,1,1,1,0,
    0,0,0,0,1,
    0,1,1,1,0,
    1,0,0,0,0,
    1,1,1,1,1
};

// Dígito 3
static bool led_digit3[NUM_PIXELS] = {
    1,1,1,1,0,
    0,0,0,0,1,
    0,1,1,1,0,
    0,0,0,0,1,
    1,1,1,1,0
};

// Dígito 4
static bool led_digit4[NUM_PIXELS] = {
    1,0,0,1,0,
    1,0,0,1,0,
    1,1,1,1,1,
    0,0,0,1,0,
    0,0,0,1,0
};

// Dígito 5
static bool led_digit5[NUM_PIXELS] = {
    1,1,1,1,1,
    1,0,0,0,0,
    1,1,1,1,0,
    0,0,0,0,1,
    1,1,1,1,0
};

// Dígito 6
static bool led_digit6[NUM_PIXELS] = {
    0,1,1,1,0,
    1,0,0,0,0,
    1,1,1,1,0,
    1,0,0,0,1,
    0,1,1,1,0
};

// Dígito 7
static bool led_digit7[NUM_PIXELS] = {
    1,1,1,1,1,
    0,0,0,0,1,
    0,0,0,1,0,
    0,0,1,0,0,
    0,1,0,0,0
};

// Dígito 8
static bool led_digit8[NUM_PIXELS] = {
    0,1,1,1,0,
    1,0,0,0,1,
    0,1,1,1,0,
    1,0,0,0,1,
    0,1,1,1,0
};

// Dígito 9
static bool led_digit9[NUM_PIXELS] = {
    0,1,1,1,0,
    1,0,0,0,1,
    0,1,1,1,1,
    0,0,0,0,1,
    0,1,1,1,0
};

// Vetor de ponteiros para os dígitos
static bool* digitos[10] = {
    led_digit0, led_digit1, led_digit2, led_digit3, led_digit4,
    led_digit5, led_digit6, led_digit7, led_digit8, led_digit9
};

static inline void put_pixel(uint32_t pixel_grb); // Função para colocar um pixel
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);// Função para converter RGB para uint32_t
void set_one_led(bool *desenho, uint8_t r, uint8_t g, uint8_t b);// Função para acender um LED
static void init_display(void);// Função para inicializar o display
static void init_pin(void);// Função para inicializar os pinos
static void init_led5x5(void);// Função para inicializar o display 5x5

static void gpio_irq_handler(uint gpio, uint32_t events);// Função para tratar a interrupção


int main()
{
    stdio_init_all(); // Inicializa a comunicação serial

    init_display();
    init_pin();
    init_led5x5();
    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(button_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true) {
        
        if(stdio_usb_connected()){ /// Se o cabo USB estiver conectado
            char comando;
            puts("Digite um comando: ");
            comando = getchar(); // Recebe comando via terminal

            if(comando >= '0' && comando <= '9'){
                int digito = comando - '0';
                printf("Comando: %d\n", digito);
                set_one_led(digitos[digito], 0, 255, 0);
                ssd1306_fill(&ssd, !cor); // Limpa o display
                    ssd1306_draw_string(&ssd, &comando, 8, 10); // Desenha uma string
                    ssd1306_send_data(&ssd); // Atualiza o display
            }
            else if(comando >= 'A' && comando <= 'Z' || comando >= 'a' && comando <= 'z'){
                printf("Comando: %c\n", comando);
                ssd1306_fill(&ssd, !cor); // Limpa o display
                ssd1306_draw_char(&ssd, comando, 8, 10); // Desenha um caractere
                ssd1306_send_data(&ssd); // Atualiza o display
            }
            else{
                puts("Comando inválido\n");
            }
        }
    }
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time()); 
    
    if (current_time - last_time > 200000) 
    {
        last_time = current_time; 
        if(gpio == button_A){
            if(a == 0){
                a = 1;
                puts("Botao A pressionado. Led verde ligado\n");
                gpio_put(ledGreen_pin, 1); // Liga o led verde
                ssd1306_fill(&ssd, !cor); // Limpa o display
                ssd1306_draw_string(&ssd, "Led Verde", 8, 10); // Desenha uma string
                ssd1306_draw_string(&ssd, "Ligado", 20, 30); // Desenha uma string
                ssd1306_send_data(&ssd); // Atualiza o display
            }
            else{
                a = 0;
                puts("Botao A pressionado. Led verde desligado");
                gpio_put(ledGreen_pin, 0); // Desliga o led verde
                ssd1306_fill(&ssd, !cor); // Limpa o display
                ssd1306_draw_string(&ssd, "Led Verde", 8, 10); // Desenha uma string
                ssd1306_draw_string(&ssd, "Desligado", 20, 30); // Desenha uma string
                ssd1306_send_data(&ssd); // Atualiza o display
            }
        }
        else if(gpio == button_B){
            if(b == 0){
                b = 1;
                puts("Botao B pressionado. Led azul ligado\n");
                gpio_put(ledBlue_pin, 1);// Liga o led azul
                ssd1306_fill(&ssd, !cor); // Limpa o display
                ssd1306_draw_string(&ssd, "Led Azul", 8, 10); // Desenha uma string
                ssd1306_draw_string(&ssd, "Ligado", 20, 30); // Desenha uma string
                ssd1306_send_data(&ssd); // Atualiza o display
            }
            else{
                b = 0;
                puts("Botao B pressionado. Led azul desligado");
                gpio_put(ledBlue_pin, 0); // Desliga o led azul
                ssd1306_fill(&ssd, !cor); // Limpa o display
                ssd1306_draw_string(&ssd, "Led Azul", 8, 10); // Desenha uma string
                ssd1306_draw_string(&ssd, "Desligado", 20, 30); // Desenha uma string
                ssd1306_send_data(&ssd); // Atualiza o display
            }   
        }
    }
}

static void init_display(void) {
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set o pino para I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set o pino para I2C
    gpio_pull_up(I2C_SDA); // Pull up na linha de dados
    gpio_pull_up(I2C_SCL); // Pull up na linha de clock

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

static void init_pin(void) {
    gpio_init(ledGreen_pin);
    gpio_set_dir(ledGreen_pin, GPIO_OUT);

    gpio_init(ledBlue_pin);
    gpio_set_dir(ledBlue_pin, GPIO_OUT);

    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN);
    gpio_pull_up(button_A);

    gpio_init(button_B);
    gpio_set_dir(button_B, GPIO_IN);
    gpio_pull_up(button_B);
}

static void init_led5x5(void) {
    sm = pio_claim_unused_sm(pio, true);;
    offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    set_one_led(digitos[num], 255, 0, 0);
}

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void set_one_led(bool *led_buffer,uint8_t r, uint8_t g, uint8_t b)
{   
    float fator = 0.02;

    uint32_t color = urgb_u32(r * fator, g * fator, b * fator);

    for (int i = 0; i < NUM_PIXELS; i++)
    {
        int pos = mapa_leds[i];
        if (led_buffer[pos])
        {
            put_pixel(color); 
        }
        else
        {
            put_pixel(0); 
        }
    }
}