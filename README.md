# Etude de l'ADC de l'ESP32 + acquisition de données

## Introduction
* Ce projet a pour but d'étudier la fiabilité de l'ADC de l'ESP32 et des méthodes de filtrages simples
* Le code a été réalisé sur le framework officiel d'espressif esp-idf ([ESP-IDF](https://www.espressif.com))

## Le matériel
* Les mesures de tensions se font aux bornes d'un potentiomètre
* Les mesures sont effectuées sur GPIO_36 (pin "SP") depuis l'ADC1 configuré en mode oneshot et atténuation 12dB
* Afin d'éviter des erreurs de mesures dues aux IEM j'ai utilisé des câbles courts et rigides
* La référence du multimètre utilisé est : LIFEDOM MAS830B
Le montage sur breadboard :
<img width="980" height="1080" alt="IMG_1510" src="https://github.com/user-attachments/assets/041f1bee-0a2b-491b-8e53-d92730defab4" />

## L'expérience
### Mesures
### Acquisition
### Protocole
(dépendances python)

## Limitations de l'ADC brut
### Linéarité et zones mortes
<img width="2560" height="1319" alt="linearity+voltage" src="https://github.com/user-attachments/assets/bf938a27-eee6-47ff-ad5c-6ffd04787a26" />
Montrer zones mortes + désaxage + linéarité acceptable
### Tension après calibration
<img width="2560" height="1319" alt="Precision_tension" src="https://github.com/user-attachments/assets/b3675d65-8dee-4ea3-bfd4-b3e67dc48a22" />
Montrer désaxage corrigé, zones mortes non

## Test des méthodes de filtrages
### Ecart-type avec vs sans condensateur
<img width="1000" height="500" alt="Comparaison_ecarts_types_avec_et_sans_condensateur" src="https://github.com/user-attachments/assets/48829699-1408-449e-b7c1-834c8574ea58" />
Explication de pourquoi c'est moins bien avec condensateur (10uF)

### Signal de sortie Average VS Raw VS Passe-bas numérique
<img width="2560" height="1319" alt="continuous-fluctuating-raw_filtered_averaged(alpha=0,1)" src="https://github.com/user-attachments/assets/16425f2f-d2a4-4868-abb6-c729babeab82" />


--- en cours d'écriture ---


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
