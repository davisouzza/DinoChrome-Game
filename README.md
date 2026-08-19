```markdown
# 🦖 Dino Chrome Game - ESP32

Um remake do clássico jogo do dinossauro do Google Chrome executado de forma nativa em um microcontrolador ESP32, utilizando um Display OLED SSD1306 (I2C) e controles físicos por botões tácteis em uma protoboard.

---

## 📌 Sumário
- [Demonstração e Recursos](#-demonstração-e-recursos)
- [Componentes Utilizados](#-componentes-utilizados)
- [Esquema de Ligação (Pinout)](#-esquema-de-ligação-pinout)
- [Mecânicas de Jogabilidade](#-mecânicas-de-jogabilidade)
- [Estrutura do Código e Arquitetura](#-estrutura-do-código-e-arquitetura)
- [Montagem Física e Solução de Problemas](#-montagem-física-e-solução-de-problemas)
- [Como Executar o Projeto](#-como-executar-o-projeto)

---

## 🎮 Demonstração e Recursos

- **Tela Inicial (Start Screen):** Exibe o título do jogo e a pontuação máxima gravada.
- **Física de Pulo e Gravidade:** Cálculo vetorial no eixo Y simulando aceleração gravitacional no pulo.
- **Mecânica de Esquiva (Agachar):** Ajuste dinâmico de hitbox para esquivar de obstáculos aéreos.
- **Obstáculos Dinâmicos:**
  - Cactos no chão: Exigem que o jogador faça pulos precisos.
  - Pássaros no ar: Voam na altura da cabeça, exigindo que o jogador se abaixe.
- **Dificuldade Escalável:** A velocidade do jogo inicia em 5 e aumenta dinamicamente a cada 15 pontos (até a velocidade máxima 9).
- **Persistência de Recorde (NVS):** O recorde (High Score) é salvo na memória flash interna do ESP32 utilizando a biblioteca `Preferences.h`.
- **Tela de Vitória:** Exibe um troféu desenhado em Pixel Art ao atingir a pontuação máxima de 100 pontos.
- **Animações em Pixel Art:** Animações para corrida do Dino, agachamento e bater de asas dos pássaros.

---

## 🛠️ Componentes Utilizados

| Componente | Quantidade | Descrição |
| :--- | :---: | :--- |
| ESP32 DevKit v1 | 1 | Microcontrolador principal de 32 bits |
| Display OLED SSD1306 | 1 | Tela 128x64 pixels com comunicação I2C |
| Push Buttons (Chave Tátil) | 2 | Botões para os comandos de Pulo/Start e Agachar |
| Protoboard | 1 | Base para montagem do circuito |
| Cabos Jumper | Vários | Conexões elétricas (Macho-Fêmea / Macho-Macho) |

---

## 🔌 Esquema de Ligação (Pinout)

### 1. Display OLED (I2C)
| Display OLED | Pino ESP32 | Função |
| :---: | :---: | :--- |
| VCC | 3V3 | Alimentação 3.3V |
| GND | GND | Terra |
| SDA | GPIO 21 (D21) | Linha de Dados I2C |
| SCL | GPIO 22 (D22) | Linha de Clock I2C |

### 2. Botões de Controle
Os botões utilizam os resistores internos do ESP32 via modo INPUT_PULLUP, dispensando resistores externos.

| Botão | Pino ESP32 | Conexão Secundária | Função no Jogo |
| :--- | :---: | :---: | :--- |
| Botão 1 (Direita) | GPIO 4 (D4) | GND | Iniciar Jogo (Start) / Pular |
| Botão 2 (Esquerda) | GPIO 5 (D5) | GND | Abaixar (Agachar) |

---

## 🎯 Mecânicas de Jogabilidade

```text
       ┌───────────┐
       │ DINO GAME │
       └─────┬─────┘
             │ (Aperte D4 para Iniciar)
             ▼
      ┌──────────────┐
      │  EM PARTIDA  │
      └──────┬───────┘
             ├──────────────────────────┐
             ▼                          ▼
   [ Pressione D4 ]            [ Pressione D5 ]
   • Salta sobre Cactos        • Abaixa a cabeça
   • Evita obstáculos no chão  • Evita Pássaros no ar

```

1. **Tela Inicial:** O jogo inicia pausado. Pressione o botão de Pulo (D4) para iniciar a partida.
2. **Cactos:** Surgem na parte inferior da tela. Pressione D4 para pular.
3. **Pássaros:** Voam na altura da cabeça do dinossauro. Pressione e segure D5 para se abaixar por baixo deles.
4. **Game Over:** Ao colidir com qualquer obstáculo, o jogo exibe o placar da partida e o recorde salvo, retornando para a tela inicial.

---

## 💻 Estrutura do Código e Arquitetura

O projeto foi desenvolvido no ecossistema PlatformIO / VS Code em linguagem C++ utilizando a arquitetura de estado e loop contínuo da plataforma Arduino.

### Principais Bibliotecas:

* `Adafruit_SSD1306.h` & `Adafruit_GFX.h`: Renderização gráfica e exibição de Bitmaps.
* `Preferences.h`: Gerenciamento do armazenamento persistente em memória não-volátil (NVS).
* `Wire.h`: Comunicação I2C com o display.

### Lógica de Colisão Ajustada:

A verificação de impacto avalia os limites do obstáculo e a posição atual do Dino, alterando os limites superiores do personagem quando o estado `isDucking` está ativo:

```cpp
int dinoTop = isDucking ? (dinoY + 8) : dinoY; // Reduz a área de impacto se estiver abaixado

```

---

## ⚡ Montagem Física e Solução de Problemas

Caso os botões apresentem leitura contínua (jogo travado abaixado ou dando pulos/start sozinho):

1. **Orientação dos Botões de 4 Pernas:**
* Os botões tácteis possuem conexão interna direta em pares. Eles devem ser montados atravessando a vala central (canal divisor) da protoboard.
* O cabo de sinal (D4 ou D5) deve ser conectado na parte superior do canal central e o cabo de GND na parte inferior.


2. **Resistores Internos:**
* O código ativa o modo `INPUT_PULLUP`. Quando o botão não está pressionado, a leitura lógica do pino é HIGH (1). Ao pressionar o botão e fechá-lo com o GND, a leitura vai para LOW (0).



---

## 🚀 Como Executar o Projeto

### Pré-requisitos

* VS Code instalado.
* Extensão PlatformIO IDE instalada no VS Code.

### Passos

1. Clone este repositório:
```bash
git clone [https://github.com/davisouzza/DinoChrome-Game.git](https://github.com/davisouzza/DinoChrome-Game.git)

```


2. Abra a pasta do projeto no VS Code.
3. Aguarde o PlatformIO carregar as dependências definidas no `platformio.ini`.
4. Conecte o ESP32 ao computador via cabo USB.
5. Clique no ícone de Upload (→) na barra inferior do PlatformIO para compilar e gravar o código no microcontrolador.

---

## 📜 Licença

Este projeto foi desenvolvido para fins educacionais e de aprendizado em sistemas embarcados e programação C++/ESP32. Sinta-se livre para clonar e modificar!

```

```
