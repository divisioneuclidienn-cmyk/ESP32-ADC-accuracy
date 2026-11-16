# Projet de mesure des limitations du CAN de l'ESP32

## Introduction
* Ce projet a pour but de déterminer la fiabilité de l'ADC de mon ESP32
* Pour cela, j'ai comparé des mesures de tensions analogiques depuis l'ESP à celles de mon voltmètre
* Le code a été réalisé sur le framework officiel d'espressif esp-idf ([ESP-IDF](https://www.espressif.com))
* La suite sera d'essayer de corriger les défauts de ces mesures

## Le matériel
* Les mesures de tensions sont effectuées sur GPIO_36 (pin "SP") depuis l'ADC1 en mode one shot avec atténuation de 12dB aux bornes d'un potentiomètre ajustable
* Afin d'éviter des erreurs de mesures dues aux IEM j'ai utilisé des câbles courts et rigides
* La référence du multimètre utilisé est : LIFEDOM MAS830B
<img width="980" height="1080" alt="IMG_1510" src="https://github.com/user-attachments/assets/041f1bee-0a2b-491b-8e53-d92730defab4" />

## Le fonctionnement du code
Le code renvoie sur le moniteur série les résultats statistiques d'une série de 10000 mesures consécutives.
Premièrement on initialise l'ADC ainsi que le canal correspondant au gpio (ici gpio_36 soit ADC1_CHANNEL0).
Dans la boucle du firmware on effectue 10000 mesures analogiques, en stockant chacune des valeurs dans un tableau d'effectif.
Une fois le tableau rempli, on effectue des calculs mathématiques de statistiques simples pour déterminer la moyenne de la série, son amplitude, sa variance et son écart-type.
Enfin, on envoi les résultats sur le moniteur série, sans oublier de remettre l'entièreté des valeurs du tableau à 0.

## Protocole
* Mesurer la tension depuis le voltmètre avec le calibre 2V puis 20V (moins précis) entre GPIO_36 et GND
* Enlever le voltmètre
* Mesurer le retour de l'ADC de l'ESP en nombres de points ainsi que les données statistiques calculées

## Résultats
### Sans calibration
Mesures réalisées sans calibrage : [Mesures.csv](https://github.com/user-attachments/files/34872257/Mesures.csv)
On trace une courbe de linéarité pour vérifier la stabilité des mesures de l'ADC :
<img width="1558" height="985" alt="Courbe_ADC_moyenne_VS_ADC_parfait" src="https://github.com/user-attachments/assets/c490f2d1-caca-49ff-b1b8-b3cc69d368d1" />
On remarque un décalage des courbes du au fait l'ADC ne détecte pas les niveaux de tensions situés aux extrêmes.
On observe cependant une linéarité correcte
Un calibrage pourrait aider à corriger ce problème de désaxage.
### Avec calibration
Mesures réalisées après calibrage avec le driver ADC_cali d'espressif [Mesures_après_calibrage.csv](https://github.com/user-attachments/files/34879122/Mesures_apres_calibrage.csv)
On trace la courbe de la tension renvoyée par le mode de calibrage linéaire du driver en fonction de la tension réelle mesurée au voltmètre :
<img width="1708" height="1055" alt="tension_ADC_VS_tension_voltmetre" src="https://github.com/user-attachments/assets/b7fad671-7cab-41cf-b58a-20f0100c009d" />
On observe toujours les zones mortes de l'ADC aux extrémités des tensions qui ne peuvent pas être corrigées logiciellement.
La linéarité de la tension relevée est correcte.
Les tensions obtenues (sauf dans les zones mortes) sont plutôt fidèles car elles rentrent dans l'intervalle d'incertitude de mon voltmètre.

## Limitations de l'expérience
Il faut prendre en compte certaines incertitudes lors de l'expérience.
* Les mesures du voltmètre qui sont les références du jugement de la fiabilité de l'ADC de l'ESP.
En effet, passé le calibre 2V, mon voltmètre n'est plus assez précis pour réaliser une expérience fiable.
Voilà un graphique montrant l'écart des incertitudes pour chaque mesures au voltmètre.
<img width="1443" height="841" alt="Incertitudes_voltmetre" src="https://github.com/user-attachments/assets/be1e76da-e531-47a4-a99b-3673e37cd082" />
On observe principalement l'écart de la plage d'incertitudes entre le calibre 2V et 20V.
