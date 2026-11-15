# Projet de mesure de fiabilité du CAN de l'ESP32
## 1

* Ce projet a pour but de déterminer la fiabilité de l'ADC de mon ESP32.
* Pour cela, j'ai réalisé plusieurs mesures de tensions depuis l'ESP que j'ai comparées à celles de mon voltmètre.
* Le code a été réalisé sur le framework officiel d'espressif esp-idf (https://www.espressif.com/en/products/sdks/esp-idf).
* La suite sera d'essayer de corriger les défauts de mesures en calibrant l'ADC.

## Le matériel
* Les mesures de tensions de l'ESP sont effectuées sur GPIO_36 (pin "SP") depuis l'ADC1 en mode one shot avec atténuation de 12dB aux bornes d'un potentiomètre ajustable.
* Afin d'éviter le plus possible les erreurs de mesures de l'ESP dues aux IEM j'ai utilisé des câbles courts et rigides.
* La référence du multimètre utilisé est : LIFEDOM MAS830B
<img width="980" height="1080" alt="IMG_1510" src="https://github.com/user-attachments/assets/041f1bee-0a2b-491b-8e53-d92730defab4" />

## Protocole
* Entre 0 et 2V :
* Mesurer la tension depuis le voltmètre avec le calibre 2V entre GPIO_36 et GND.
* Enlever le voltmètre
* Mesurer le retour de l'ADC de l'ESP en nombres de points ainsi que les données statistiques calculées.

* Entre 2 et 3.28V :
* Mesurer la tension depuis le voltmètre avec le calibre 20V (moins précis) entre GPIO_36 et GND.
* Enlever le voltmètre
* Mesurer le retour de l'ADC de l'ESP en nombres de points ainsi que les données statistiques calculées. 

## Résultats
Après une trentaine de mesures enregistrées dans un fichier csv [Mesures.csv](https://github.com/user-attachments/files/34872257/Mesures.csv)
On peut tracer une courbe de linéarité pour vérifier la stabilité des mesures de l'ADC.
<img width="1558" height="985" alt="Courbe_ADC_moyenne_VS_ADC_parfait" src="https://github.com/user-attachments/assets/c490f2d1-caca-49ff-b1b8-b3cc69d368d1" />
On remarque un décalage des courbes indiquant que l'ADC de l'ESP n'est pour l'instant pas très fiable par rapport aux mesures de mon voltmètre.
Ces décalages sont dues au fait que l'ADC ne détecte pas correctement les niveaux de tensions situés aux extrêmes.
Cependant, on observe graphiquement que la linéarité reste correcte.
Un calibrage pourrait donc aider à corriger ce problème et rendre l'ADC de l'ESP beaucoup plus fiable



## nuances / incertitudes / conclusion

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


* test
* test

Les mesures ont étaient faites aux bornes de la première résistance d'un potentiomètre contenant 2 résistances internes, la somme de ses 2 résistances est toujours égale à 10kΩ.
Quand on tourne le potentiomètre, les résistances s'ajustent de la sorte à ce qu'aux extrémités du potentiomètre, soit R1 = 10kΩ et R2 = 0Ω ou à l'inverse R1 = 0Ω et R2 = 10kΩ.
Voila le setup, on remarque des cables rigides car avec des cables longs et souples on capte trop de bruit (l'idéal étant des soudures).
On observe un probleme de décalage entre l'ADC parfait et celui de l'ESP, la courbe me parait plutot linaire. Seulement il ne détecte pas correctement les tensions aux extrémités.
Un calibrage pourrait corriger ce problème et le rendre plus précis.
Je n'ai pas eu de souci particulier du au bruit électrique, en effet lors de mes mesures les écarts types sont restés relativement faibles ainsi que les pics d'amplitude.

J'ai par la suite effectué quelques calculs supplémentaires sur tableur comme les incertitudes de mesures de mon voltmètre pour en faire un graphique :
<img width="1443" height="841" alt="Incertitudes_voltmetre" src="https://github.com/user-attachments/assets/be1e76da-e531-47a4-a99b-3673e37cd082" />
On observe surtout l'écart de précision entre le calibre 2V et 20V.

J'ai mesuré la tension de référence de mon ESP entre +3.3V et 0V/GND à 2,28V au voltmètre.
