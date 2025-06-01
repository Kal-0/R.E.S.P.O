# 🤖 RESPO: Murphy, o Robô inspirado no jogo REPO

**RESPO (REPO + ESP)** é um projeto de robótica interativa inspirado no universo do jogo de terror *REPO*, uma experiência cooperativa no estilo *Lethal Company*. Nosso robô, carinhosamente chamado de **Murphy**, homenageia a **Lei de Murphy** — pois, durante o desenvolvimento, tudo que podia dar errado… deu! 😅

Murphy é mais do que um robô: é uma criatura interativa que reage ao ambiente e aos comandos do usuário, replicando de forma física a experiência social e caótica dos personagens do jogo.

---

## 🧠 Funcionalidades Principais

### 🩸 Sistema de Vida com Feedback Visual e Auditivo
- Dois botões permitem **adicionar ou remover vida**.
- Os LEDs **verdes e vermelhos** indicam o nível de vida atual.
- Os **olhos RGB** piscam em vermelho ao perder vida.
- Um **servo motor** abre a boca do robô.
- Um **buzzer** emite um som diferente para adicionar ou remover vida.

### 🔦 Lanterna com Sensor de Luminosidade
- Um **LDR** monitora o nível de luz no ambiente.
- Em ambientes escuros, um **LED azul de alto brilho** acende automaticamente, como uma lanterna.

### 😠 Emoções via Potenciômetro
- O **potenciômetro** ajusta o “estado emocional” do robô:
  - Altera a cor dos olhos RGB.
  - Abre a boca com diferentes ângulos via servo.
  - Modifica a frequência do som emitido pelo buzzer.
  - Em níveis altos, Murphy fica "furioso": olhos vermelhos e som agudo.

### 🌐 Controle Remoto via Firebase
- O robô está conectado à internet através do **Wi-Fi da ESP32**.
- Integramos o projeto ao **Firebase** para controle remoto.
- Criamos um **frontend web** onde é possível acessar remotamente o robô.
- No site, é possível ativar o **modo online** e controlar todas as ações remotamente.

---

## 🖥️ Monitoramento via Serial e Firebase

O robô **Murphy** permite **monitoramento completo em tempo real** via terminal serial e nuvem:

- Sempre que a **vida é alterada**, o novo valor é exibido no terminal.
- O valor do **potenciômetro** e do **sensor de luminosidade (LDR)** são monitorados e atualizados.
- Esses dados são **sincronizados com o Firebase** automaticamente.
- Toda vez que há uma atualização no Firebase, a lista completa dos dados é **printada no terminal serial**.

---

## 🔧 Componentes Utilizados

- 1x ESP32 AM032 (Ants Make)
- 1x Buzzer piezoelétrico
- 1x Servo motor
- 2x LEDs RGB (olhos)
- 1x LED azul de alto brilho (lanterna)
- 3x LEDs verdes (indicador de vida)
- 2x LEDs vermelhos (indicador de vida)
- 1x Sensor LDR
- 1x Potenciômetro
- 2x Botões táteis
- Resistores de 220Ω
- Jumpers diversos
- 1x Placa perfurada para distribuir 3V3 e GND

### 🖨️ Carcaça do Robô
- Modelada em **Blender**
- Impressa em 3D na **Creality Ender S3 V1** com **PLA**
- Tempo de impressão: **~40 horas**

---

## 🚀 Próximos Passos

- [ ] Adicionar **reconhecimento de fala** para comandos como “Murphy, fala!” ou “Murphy, acende a luz!”
- [ ] Adicionar **sensor de presença ou movimento** (PIR ou ultrassônico) para detectar aproximação
- [ ] Implementar **comunicação entre múltiplos robôs via ESP-NOW**
- [ ] Adicionar **braços articuláveis com servos** para gestos
- [ ] Substituir alimentação USB por **bateria LiPo com carregamento via USB-C**

---

## 🤓 Saiba Mais

Para saber mais sobre o projeto, visite a nossa publicação no [Hackster.io](https://www.hackster.io/r-e-s-p-o/r-e-s-p-o-emotional-environment-sensing-bot-with-esp-673588)! 
Caso deseje ver o nosso painel de controle do R.E.S.P.O, [clique aqui](https://embarcados-front-git-master-diogo-henriques-projects-d9773d64.vercel.app/).

---

## 👥 Equipe

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/Kal-0">
        <img src="https://avatars.githubusercontent.com/u/106926790?v=4" width="150px;" alt="Caio Hirata"/><br>
        <sub><b>Caio Hirata</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/DiogoHMC">
        <img src="https://avatars.githubusercontent.com/u/116087739?v=4" width="150px;" alt="Diogo Henrique"/><br>
        <sub><b>Diogo Henrique</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/pedro-coelho-dr">
        <img src="https://avatars.githubusercontent.com/u/111138996?v=4" width="150px;" alt="Pedro Coelho"/><br>
        <sub><b>Pedro Coelho</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/theomilll">
        <img src="https://avatars.githubusercontent.com/u/99195030?v=4" width="150px;" alt="Theo Moura"/><br>
        <sub><b>Theo Moura</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/virnaamaral">
        <img src="https://avatars.githubusercontent.com/u/116957619?v=4" width="150px;" alt="Virna Amaral"/><br>
        <sub><b>Virna Amaral</b></sub>
      </a>
    </td>
  </tr>
</table>
