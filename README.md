# Projet de mesure de fiabilité du CAN de l'ESP32
## 1

* Ce projet a pour but de déterminer la fiabilité de l'ADC de mon ESP32.
* Pour cela, j'ai réalisé plusieurs mesures de tensions depuis l'ESP que j'ai comparées à celles de mon voltmètre.
* Le code a été réalisé sur le framework officiel d'espressif esp-idf ([ESP-IDF](https://www.espressif.com)).
* La suite sera d'essayer de corriger les défauts de mesures en calibrant l'ADC.

## Le matériel
* Les mesures de tensions de l'ESP sont effectuées sur GPIO_36 (pin "SP") depuis l'ADC1 en mode one shot avec atténuation de 12dB aux bornes d'un potentiomètre ajustable.
* Afin d'éviter le plus possible les erreurs de mesures de l'ESP dues aux IEM j'ai utilisé des câbles courts et rigides.
* La référence du multimètre utilisé est : LIFEDOM MAS830B
<img width="980" height="1080" alt="IMG_1510" src="https://github.com/user-attachments/assets/041f1bee-0a2b-491b-8e53-d92730defab4" />

## Le fonctionnement du code
* Le code renvoi sur le moniteur série les résultats statistiques d'une série de 10000 mesures consécutives.
Premièremement on initialise l'ADC ainsi que le canal qui correspond au gpio (ici gpio_36 soit ADC1_CHANNEL0).
Dans la boucle du firmware on effectue 10000 mesures analogiques de tensions sur GPIO_36, en stockant chacune des valeurs dans un tableau d'effectif.
Une fois le tableau remplit, on effectue des calculs mathématiques simples de statistiques pour déterminer la moyenne de la série, son amplitude, sa variance pour connaitre son écart-type.
Enfin, on envoi tout ça sur le moniteur série, sans oublier de remettre l'entièreté des valeurs du tableau à 0.

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


## incertitudes de mesures
Il faut prendre en compte certaines incertitudes lors de l'expérience.
* Les mesures du voltmètres qui sont les références du jugement de la fiabilité de l'ADC de l'ESP
En effet, passé le calibre 2V, mon voltmètre n'est plus assez précis pour réaliser une expérience fiable.
Voilà un graphique montrant l'écart des incertitudes pour chaque mesures au voltmètre
<img width="1443" height="841" alt="Incertitudes_voltmetre" src="https://github.com/user-attachments/assets/be1e76da-e531-47a4-a99b-3673e37cd082" />
On observe principalement l'écart de la plage d'incertitudes entre le calibre 2V et 20V.


## Notes
la linéarité de la courbe de l'ADC reste la même
Simplement la linéarité de la courbe de la tension retourné par la fonction adc_cali_raw_to_voltage() est bonne car pour 0 elle donne le palier de tension ou l'ADC n'est plus aveugle.
Problème pas vraiment réglé, car l'ADC reste aveugle dans ces zones.
On étudie plus la fiabilité de l'ADC raw mais la fiabilité de la détermination de la tension grâce au mode de calibrage linéaire fournit par esp-idf
