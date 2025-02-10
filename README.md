# Controle de LED WS2812, Display OLED via monitor serial e LEDs RGB via botões no Raspberry Pi Pico W

Este projeto demonstra o controle de uma matriz de LEDs WS2812 (RGB) organizada em uma grade 5x5 para exibir dígitos numéricos, do displaySSD1306 com a exibição de caracteres alfanumericos, alem de atualização dos estados do LED rgb via botões. A interação é realizada via botões e comandos enviados pelo serial monitor (quando o cabo USB está conectado).

---

## 📋 Funcionalidades

- **Exibição de Dígitos (0 a 9) e Caracteres:**
  - Cada dígito é representado por um desenho mapeado para os 25 LEDs da matriz.
  - Letras (A-Z ou a-z) também podem ser exibidas no display OLED.

- **Interação via Botões e Terminal USB:**
  - **Botão A (GPIO 5):** Alterna o estado do LED verde e atualiza o display, informando “Ligado” ou “Desligado”.
  - **Botão B (GPIO 6):** Alterna o estado do LED azul e atualiza o display, informando “Ligado” ou “Desligado”.
  - **Terminal USB:** Quando conectado, o usuário pode inserir um comando:
    - Se for dígito ('0'-'9'): o programa exibe o dígito na matriz LED (com a cor verde) e atualiza o display.
    - Se for caractere alfabético: o caractere é desenhado no display OLED.

- **Controle dos LEDs WS2812 via PIO:**
  - Utiliza o PIO para atualizar a matriz 5x5 com base no mapeamento definido.

- **Debounce de Botões:**
  - Implementado para evitar múltiplos acionamentos indesejados, considerando um intervalo mínimo de 200 ms.

---

## 🛠 Hardware

| Componente             | GPIO / Pino                           | Observações                                              |
|------------------------|---------------------------------------|----------------------------------------------------------|
| **Matriz LED WS2812**  | Pino 7                                | Grade de 25 LEDs em um formato 5x5                       |
| **Display OLED SSD1306**| I2C (SDA: pino 14, SCL: pino 15)      | Endereço 0x3C - utilizado para exibir comandos e status  |
| **Botão A**            | Pino 5                                | Configurado com resistor pull-up                        |
| **Botão B**            | Pino 6                                | Configurado com resistor pull-up                        |
| **LED Indicador Verde**| Pino 11                               | Feedback visual do estado do Botão A                     |
| **LED Indicador Azul** | Pino 12                               | Feedback visual do estado do Botão B                     |

---

## 🔢 Mapeamento da Matriz de LEDs e Desenhos dos Dígitos

A matriz LED é mapeada para corresponder a uma grade 5x5:

```c
int mapa_leds[25] = {
    24, 23, 22, 21, 20,  
    15, 16, 17, 18, 19,  
    14, 13, 12, 11, 10,  
    5,  6,  7,  8,  9,  
    4,  3,  2,  1,  0   
};
```

São definidos vetores booleanos para cada dígito (0 a 9). Por exemplo, para o dígito 0:
```c
static bool led_digit0[NUM_PIXELS] = {
    0,1,1,1,0,
    1,0,0,0,1,
    1,0,0,0,1,
    1,0,0,0,1,
    0,1,1,1,0
};
```
Os desenhos dos dígitos são definidos nos arrays led_digit0 a led_digit9.
```c
// Vetor de ponteiros para os dígitos
static bool* digitos[10] = {
    led_digit0, led_digit1, led_digit2, led_digit3, led_digit4,
    led_digit5, led_digit6, led_digit7, led_digit8, led_digit9
};
```

💻 Fluxo de Execução

1. **Inicialização:**
   - A função `main` inicializa a comunicação serial ( `stdio_init_all` ), o display OLED ( `init_display` ), os pinos ( `init_pin` ) e a matriz de LED ( `init_led5x5` ) via PIO.
   - As interrupções para os botões A e B são configuradas usando `gpio_set_irq_enabled_with_callback`.

2. **Interrupção e Debounce:**
   - A rotina `gpio_irq_handler` trata as interrupções dos botões, aplicando um debounce (intervalo mínimo de 200 ms) e alterna o estado dos LEDs indicadores.
   - O display OLED é atualizado para informar o status de acionamento dos botões.

3. **Entrada via Terminal USB:**
   - Se o cabo USB estiver conectado ( `stdio_usb_connected` ), o terminal permite ao usuário inserir um comando.
   - Se o comando for um dígito, é exibido na matriz LED com a cor verde e o display é atualizado.
   - Se o comando for um caractere alfabético, o display OLED é atualizado com esse caractere.

4. **Atualização dos LEDs WS2812:**
   - A função `set_one_led` percorre os 25 LEDs, utilizando o mapeamento definido, e envia os dados de cor convertidos pela função `urgb_u32` via PIO.

📂 Estrutura do Projeto

- **17.37M2_U4C6O12T_Tarefa_WLS.c:**  
  Contém a função `main`, a configuração do hardware, as rotinas de interrupção ( `gpio_irq_handler` ) e as funções de atualização dos LEDs e do display.

- **ws2812.pio e [`generated/ws2812.pio.h`](generated/ws2812.pio.h ):**  
  Responsáveis pela definição e controle via PIO dos LEDs WS2812.

- **inc/ssd1306.c e [`inc/font.h`](inc/font.h ):**  
  Implementam as funções de controle, desenho e atualização do display OLED.

🎥 Demonstração

- **Vídeo de Execução na Placa BitDogLab:**  
  https://youtu.be/mFvVInZJ87A

👥 Desenvolvedor

- Tárcio Santos
