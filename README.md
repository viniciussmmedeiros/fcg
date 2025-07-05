# Escape da Vaca

Descrição:
O jogador controla uma vaca dentro de um corredor (um labirinto com caminho único). O objetivo será atravessar o corredor antes que o cronômetro zere e uma esfera a alcance.

Jogabilidade e elementos:
- O jogador controla a vaca com teclado (teclas WASD) para avançar no corredor.
- Deve haver ao menos uma porta bloqueando o caminho, para abrir o jogador deve rotacionar um pequeno cubo (usando um outro esquema de teclas), aproximando da posição/rotação de um cubo de exemplo.
- Duas câmeras: câmera lookat com visão aérea para navegar no corredor. Câmera livre em primeira pessoa, ativada nas barreiras para que o puzzle seja resolvido e vaca avance.
- Haverá um timer iniciado no começo da partida, e se o jogador demorar demais (tempo a ser definido) uma esfera gigante começa a rolar no corredor. Se a esfera alcançar a vaca, o jogador perde.

ToDo:
[ ] revisar os 15% de uso de código de externo / lab
[ ] revisar se os códigos prontos usados estão referenciados corretamente ("FONTE")
[X] usar um modelo geométrico (usamos cow.obj) a ser carregado com tiniobjloader
[ ] transformações geométricas realizada pelo usuário, além da câmera
[ ] implementar câmera look-at e câmera livre
[ ] duas ou mais instâncias de um mesmo objeto
[ ] três testes de intersecção (ex: cubo-cubo, cubo-plano, e ponto-esfera) --> collisions.cpp
[ ] iluminação de lambert e blinn-phong. 
[ ] ao menos um objeto com gouraud
[ ] ao menos um objeto com phong
[ ] todos os objetos devem ter textura
[ ] ao menos um objeto com movimentação relacionada a curva de bezier
[ ] todas as movimentações devem ser computadas baseado no tempo

Critérios técnicos:

[X] Malhas poligonais complexas (cow.obj)

[X] Transformações geométricas controladas pelo usuário

[X] Câmera livre e câmera look-at

[ ] Instâncias de objetos (duas ou mais model matrix aplicadas ao mesmo conjunto de vértices)

[ ] Três tipos de testes de intersecção (cubo-cubo, cubo-plano, e ponto-esfera) - collisions.

[ ] Modelos de Iluminação Difusa e Blinn-Phong

[ ] Modelos de Interpolação de Phong e Gouraud

[ ] Mapeamento de texturas em todos os objetos

[ ] Movimentação com curva Bézier cúbica

[ ] Animações baseadas no tempo ($\Delta t$) 