Projet de mesure de linéarité du CAN d'un ESP32
L'objectif était de mesurer la fiabilité du CAN de mon ESP32
Code réaliser depuis le framework officiel d'espressif esp-idf (https://www.espressif.com/en/products/sdks/esp-idf)
Pour cela, j'ai comparé une trentaine de mesures échantillonnées depuis l'ESP avec celle de mon multimètre : LIFEDOM MAS830B
-Les mesures de l'ESP étaient traitées comme suivant : 
Voltmètre débranché pour éviter toute variation de courant pouvant interagir avec la tension.
10000 mesures analogiques depuis l'ADC_1 configuré en mode oneshot sur GPIO_36 avec un intervalle de 100us entre chaque mesures pour éviter la sur utilisation de l'ADC
Chaque mesures étaient stockées dans un tableau d'entiers décrivant l'effectif pour chacune des 4096 valeurs possibles (la résolution du CAN définit sur 12bit).
Une fois le tableau rempli. La fonction sample_data() effectue des calculs mathématiques simples pour déterminer la moyenne des valeurs, l'amplitude, la variance (étant le carré de l'écart-type) et l'écart-type permettant de juger de la fiabilité d'une mesure complète.
Enfin, remise à 0 de l'entièreté des valeurs du tableau grâce à la fonction memset().
-les mesures effectuées au voltmètre se déroulaient comme suivant :
Dans la plage la plus précise du voltmètre lors de l'expérience soit de 0 à 2V (3 décimales)
Puis passé 2V, avec le calibre 20V du voltmètre soit de 2 à 3,28V (2 décimales)

Les mesures ont étaient faites aux bornes de la première résistance d'un potentiomètre contenant 2 résistances internes, la somme de ses 2 résistances est toujours égale à 10kΩ.
Quand on tourne le potentiomètre, les résistances s'ajustent de la sorte à ce qu'aux extrémités du potentiomètre, soit R1 = 10kΩ et R2 = 0Ω ou à l'inverse R1 = 0Ω et R2 = 10kΩ.
Voila le setup, on remarque des cables rigides car avec des cables longs et souples on capte trop de bruit (l'idéal étant des soudures).
<img width="1558" height="985" alt="Courbe_ADC_moyenne_VS_ADC_parfait" src="https://github.com/user-attachments/assets/c490f2d1-caca-49ff-b1b8-b3cc69d368d1" />

J'ai effectué 31 mesures, [Mesures.csv](https://github.com/user-attachments/files/34872257/Mesures.csv)
