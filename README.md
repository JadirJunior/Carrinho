# Cart
A project of an automatic/manual cart with Esp32

This project was made by two students for Microcontroller discipline in College, and has the following features:

It has two operation modes, Automatic mode and Manual mode, which can be switched using a key on the protoboard.
It has two ultrasonic sensors and two touch sensors.
On Automatic mode, it moves forward until it finds an obstacle, then it activates the reverse track and adjust its the route.
On Manual mode, it can be controlled by a cellphone using an app called Dabble Gamepad.

# Carrinho
Projeto de um carrinho automático/manual utilizando a placa Esp32

Esse projeto foi realizado em dupla para a disciplina de microcontroladores do Instituto Federal de São Paulo.

- Ele possui dois modos, automático e manual, dos quais podem ser trocados utilizando uma chave no protoboard;
- Ele possui dois sensores ultrassônicos e dois sensores de toque;
- No modo automático, ele se move para frente até que encontre um obstáculo, em seguida ativa a ré e ajusta a rota;
- No modo manual ele é controlado pelo celular utilizando o Gamepad do aplicativo Dabble



# Relação e IO/s
| Nome        | Descrição    | Tipo  | Valor  |
| -----------: |:------------:| -----:| ------:|
|  SF         | Sensor Frontal  | Output/Input Digital | Parâmetro: 15-25cm |
|  ST         | Sensor Traseiro  | Output/Input Digital | Parâmetro: 15-25cm |
|  SLD         | Sensor Lateral Direito  | Input Digital | 0: Não ativado <br> 1: Ativado |
|  SLE         | Sensor Lateral Esquerdo  | Input Digital | 0: Não ativado <br> 1: Ativado |
|  MTD         | Motor da roda direita (Frente)  | Output Digital | 0: Não ativado <br> 1: Ativado |
|  MTE         | Motor da roda esquerda (Frente)  | Output Digital | 0: Não ativado <br> 1: Ativado |
|  VD        | Velocidade do motor esquerdo  | Output Digital (PWM) | Parâmetro: 0-100% |
|  VE         | Velocidade do motor direito  | Output Digital (PWM) | Parâmetro: 0-100% |
|  RE         | Ré do motor esquerdo  | Output Digital | 0: Não ativada <br> 1: Ativada |
|  RD         | Ré do motor direito  | Output Digital | 0: Não ativada <br> 1: Ativada |
|  MC        | Modo do carrinho  | Output Digital | 0: Modo manual <br> 1: Modo automático |





# Parâmetros: 
 | Nome | Descrição |
 | -------: |  ------: |
 | P_SENSOR_FRONTAL         | Parâmetro para regular a distância que o sensor deve ativar.         |
 | P_SENSOR_TRASEIRO         | Parâmetro para regular a distância que o sensor deve ativar.         |
 | P_T_RE         |  Tempo que o carrinho vai dar ré.         |
 | P_T_ROTA_1         |Tempo para rotacionar após a ré        |
 | P_V_RE         | Percentual de velocidade da ré         |
 | P_V_AJUSTE         | Percentual de velocidade do ajuste         |
 | P_V_FRENTE         | Percentual de velocidade frontal máxima        |
 | P_V_MIN         | Percentual da velocidade frontal mínima         |
 | P_T_ROTA_2         | Tempo para rotacionar ao detectar algum obstáculo lateral.        |
