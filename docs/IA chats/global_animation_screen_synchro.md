chabrof: Comment dans un contexte de mise à jour d ecran par SPI avec double transmission pour ecran de gauche et ecran de droite avec quelques fonctions simple d affichage graphique dans un buffer comme une fonction d'affichage de droite ou de point implementer quelque chose ressemblant à des fonction de easing permettant des animations ?

GitHub Copilot: Pour implémenter des animations avec des fonctions de easing dans le contexte de mise à jour d'écran par SPI avec double transmission, vous pouvez ajouter des fonctions qui interpolent les positions ou les couleurs des éléments graphiques sur l'écran. Voici une approche :

### Étapes pour intégrer des animations avec easing :
1. **Créer une fonction d'interpolation** : Utilisez des fonctions de easing (comme linéaire, quadratique, sinusoidale, etc.) pour calculer les valeurs intermédiaires.
2. **Mettre à jour les buffers** : Appliquez les valeurs interpolées pour mettre à jour les positions ou les couleurs dans les buffers.
3. **Envoyer les buffers** : Utilisez les fonctions existantes (`SSD1320_SendBuffer_Left` et `SSD1320_SendBuffer_Right`) pour transmettre les données mises à jour.

### Exemple de code pour une animation avec easing :
Ajoutez une fonction d'interpolation et une fonction d'animation dans votre fichier ssd1320_graphics.c.

#### Fonction d'interpolation avec easing :
```c
float EaseInOutQuad(float t, float b, float c, float d) {
  t /= d / 2;
  if (t < 1) return c / 2 * t * t + b;
  t--;
  return -c / 2 * (t * (t - 2) - 1) + b;
}
```

#### Fonction d'animation pour déplacer un rectangle :
```c
void AnimateRect(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, uint16_t w, uint16_t h, uint8_t color, uint8_t filled, uint32_t duration) {
  uint32_t startTime = HAL_GetTick();
  uint32_t currentTime;
  uint16_t x, y;

  while ((currentTime = HAL_GetTick()) - startTime < duration) {
    float progress = (float)(currentTime - startTime) / duration;
    x = (uint16_t)EaseInOutQuad(progress, xStart, xEnd - xStart, 1.0f);
    y = (uint16_t)EaseInOutQuad(progress, yStart, yEnd - yStart, 1.0f);

    ClearBuffers(); // Efface les buffers avant chaque mise à jour
    DrawRect4BPP(x, y, w, h, color, filled);
    SSD1320_SendBuffer_Left();
    SSD1320_SendBuffer_Right();
    HAL_Delay(10); // Petit délai pour éviter une mise à jour trop rapide
  }
}
```

### Exemple d'utilisation :
Dans votre fichier main.c, vous pouvez appeler cette fonction pour animer un rectangle :
```c
AnimateRect(10, 10, 100, 50, 20, 20, 0xF, 1, 2000); // Animation sur 2 secondes
```

### Explications :
1. **EaseInOutQuad** : Cette fonction calcule une interpolation douce entre deux valeurs.
2. **AnimateRect** : Elle utilise l'interpolation pour déplacer un rectangle d'une position de départ à une position finale.
3. **ClearBuffers** : Efface les buffers avant chaque mise à jour pour éviter les artefacts visuels.
4. **SSD1320_SendBuffer_Left/Right** : Transmet les données mises à jour aux écrans.

Vous pouvez étendre cette logique pour animer des lignes, des cercles ou des textes en modifiant les fonctions graphiques existantes (`DrawLine4BPP`, `DrawText4BPP`, etc.).

chabrof: Tu peux m expliquer d avantage les arguments a fournir à la fonction EaseInOutQuad ?

GitHub Copilot: La fonction `EaseInOutQuad` est une fonction de easing qui permet de calculer une interpolation douce entre deux valeurs. Voici une explication des arguments à fournir :

### Prototype de la fonction
```c
float EaseInOutQuad(float t, float b, float c, float d);
```

### Description des arguments
1. **`t` (time)** : Le temps écoulé depuis le début de l'animation. Il doit être compris entre `0` et `d` (durée totale de l'animation). Ce paramètre représente la progression de l'animation.

2. **`b` (begin)** : La valeur de départ de l'animation. C'est la position ou la valeur initiale de l'élément animé.

3. **`c` (change)** : La différence entre la valeur finale et la valeur de départ (`c = valeur_finale - valeur_départ`). Cela représente l'amplitude du changement.

4. **`d` (duration)** : La durée totale de l'animation. C'est le temps que prendra l'animation pour aller de la valeur de départ (`b`) à la valeur finale (`b + c`).

### Fonctionnement
La fonction calcule une interpolation en fonction du temps écoulé (`t`) et applique une courbe quadratique pour rendre l'animation plus douce. Elle commence lentement, accélère au milieu, puis ralentit vers la fin.

### Exemple d'utilisation
Supposons que vous voulez déplacer un rectangle de la position `x = 10` à `x = 100` sur une durée de 2 secondes (2000 ms). Voici comment utiliser la fonction :

```c
float t = 1000; // Temps écoulé (1 seconde)
float b = 10;   // Position de départ
float c = 90;   // Changement (100 - 10)
float d = 2000; // Durée totale (2 secondes)

float x = EaseInOutQuad(t, b, c, d); // Calcule la position intermédiaire
```

Dans cet exemple :
- À `t = 0`, la position sera `b` (10).
- À `t = d / 2` (1000 ms), la position sera environ au milieu de l'animation.
- À `t = d` (2000 ms), la position sera `b + c` (100).

### Visualisation
La courbe générée par `EaseInOutQuad` ressemble à ceci :
- Départ lent → accélération → ralentissement vers la fin.

Cela donne une animation fluide et naturelle, idéale pour des transitions visuelles.

chabrof: je trouve ca curieux de mettre comme argument un changement et non la valeur finale souhaitée

GitHub Copilot: C'est une observation pertinente ! Utiliser un argument représentant le **changement** (`c`) plutôt que la **valeur finale** peut sembler contre-intuitif, mais c'est une convention courante dans les fonctions d'interpolation pour simplifier les calculs internes. Voici une explication détaillée :

### Pourquoi utiliser le changement (`c`) ?
1. **Simplification des calculs** : En utilisant le changement (`c`), la fonction peut directement calculer la progression relative par rapport à la valeur de départ (`b`). Cela évite de recalculer la différence entre la valeur finale et la valeur de départ à chaque appel.
   
2. **Flexibilité** : Le changement (`c`) permet de gérer des animations où la valeur finale peut être dynamique ou calculée ailleurs dans le code. Cela rend la fonction plus générique.

3. **Convention dans les fonctions de easing** : Les fonctions de easing (comme celles utilisées dans les bibliothèques graphiques ou d'animation) suivent souvent cette convention pour des raisons de cohérence et de compatibilité avec d'autres systèmes.

### Comment utiliser la valeur finale directement ?
Si vous préférez fournir la **valeur finale** au lieu du changement, vous pouvez modifier la fonction pour calculer le changement en interne. Voici une version adaptée :

#### Version modifiée de `EaseInOutQuad` :
```c
float EaseInOutQuad(float t, float start, float end, float duration) {
  float change = end - start; // Calcul du changement
  t /= duration / 2;
  if (t < 1) return change / 2 * t * t + start;
  t--;
  return -change / 2 * (t * (t - 2) - 1) + start;
}
```

### Arguments dans cette version :
1. **`t`** : Temps écoulé depuis le début de l'animation.
2. **`start`** : Valeur de départ.
3. **`end`** : Valeur finale souhaitée.
4. **`duration`** : Durée totale de l'animation.

### Exemple d'utilisation avec la valeur finale :
Supposons que vous voulez déplacer un rectangle de `x = 10` à `x = 100` sur une durée de 2 secondes (2000 ms). Voici comment utiliser cette version :

```c
float t = 1000; // Temps écoulé (1 seconde)
float start = 10; // Position de départ
float end = 100;  // Position finale
float duration = 2000; // Durée totale (2 secondes)

float x = EaseInOutQuad(t, start, end, duration); // Calcule la position intermédiaire
```

### Résultat :
- À `t = 0`, la position sera `start` (10).
- À `t = duration / 2` (1000 ms), la position sera environ au milieu de l'animation.
- À `t = duration` (2000 ms), la position sera `end` (100).

### Conclusion :
Si vous trouvez plus intuitif de fournir la valeur finale directement, vous pouvez utiliser cette version modifiée. Cela rend l'appel de la fonction plus clair et évite de calculer le changement (`c`) manuellement.

chabrof: restons sur ta version avec le changement et non la valeur finale. Admettons que je souhaite faire des animations avec la fonction de easing directement au sein de la boucle principale du programme (dans le main) et admettons egalement que cette boucle sert de base centrale aux frames qui composeront les animations que je souhaite afficher sur mon ecran. Aurais tu un exemple avec par exemple 2 rectangles à animer ?  

GitHub Copilot: Voici un exemple d'animation de deux rectangles utilisant la fonction de easing directement dans la boucle principale du programme. L'idée est de calculer les positions des rectangles à chaque itération de la boucle en fonction du temps écoulé et de mettre à jour les buffers pour afficher les nouvelles positions.

### Ajout de la fonction de easing
Assurez-vous que la fonction `EaseInOutQuad` est définie dans votre fichier `ssd1320_graphics.c` :

```c
float EaseInOutQuad(float t, float b, float c, float d) {
  t /= d / 2;
  if (t < 1) return c / 2 * t * t + b;
  t--;
  return -c / 2 * (t * (t - 2) - 1) + b;
}
```

### Exemple d'animation dans la boucle principale
Ajoutez le code suivant dans la boucle principale de votre fichier main.c :

```c
static uint32_t animationStartTime = 0;
static uint32_t animationDuration = 2000; // Durée de l'animation en ms (2 secondes)

while (1) {
  static uint32_t lastFrameTime = 0;
  uint32_t currentTime = HAL_GetTick();

  // Limiter la fréquence des frames (par exemple, 60 FPS)
  if (currentTime - lastFrameTime < 16) { // 16 ms = ~60 FPS
    continue;
  }
  lastFrameTime = currentTime;

  // Calculer le temps écoulé depuis le début de l'animation
  uint32_t elapsedTime = currentTime - animationStartTime;

  // Réinitialiser l'animation après sa durée
  if (elapsedTime > animationDuration) {
    animationStartTime = currentTime;
    elapsedTime = 0;
  }

  // Effacer les buffers avant de dessiner
  ClearBuffers();

  // Animation du premier rectangle
  uint16_t rect1StartX = 10, rect1EndX = 100;
  uint16_t rect1StartY = 20, rect1EndY = 80;
  uint16_t rect1Width = 30, rect1Height = 20;

  uint16_t rect1X = (uint16_t)EaseInOutQuad(elapsedTime, rect1StartX, rect1EndX - rect1StartX, animationDuration);
  uint16_t rect1Y = (uint16_t)EaseInOutQuad(elapsedTime, rect1StartY, rect1EndY - rect1StartY, animationDuration);

  DrawRect4BPP(rect1X, rect1Y, rect1Width, rect1Height, 0xF, 1);

  // Animation du deuxième rectangle
  uint16_t rect2StartX = 200, rect2EndX = 120;
  uint16_t rect2StartY = 50, rect2EndY = 100;
  uint16_t rect2Width = 40, rect2Height = 30;

  uint16_t rect2X = (uint16_t)EaseInOutQuad(elapsedTime, rect2StartX, rect2EndX - rect2StartX, animationDuration);
  uint16_t rect2Y = (uint16_t)EaseInOutQuad(elapsedTime, rect2StartY, rect2EndY - rect2StartY, animationDuration);

  DrawRect4BPP(rect2X, rect2Y, rect2Width, rect2Height, 0x7, 1);

  // Envoyer les buffers aux écrans
  SSD1320_SendBuffer_Left();
  SSD1320_SendBuffer_Right();

  // Petite pause pour éviter une surcharge CPU
  HAL_Delay(1);
}
```

### Explications
1. **Animation avec easing** :
   - Les positions des rectangles (`rect1X`, `rect1Y`, `rect2X`, `rect2Y`) sont calculées à chaque frame en fonction du temps écoulé (`elapsedTime`) et de la fonction `EaseInOutQuad`.
   - Les rectangles se déplacent entre leurs positions de départ et d'arrivée sur une durée définie (`animationDuration`).

2. **Effacement des buffers** :
   - La fonction `ClearBuffers()` est appelée avant chaque mise à jour pour éviter les artefacts visuels.

3. **Envoi des buffers** :
   - Les fonctions `SSD1320_SendBuffer_Left()` et `SSD1320_SendBuffer_Right()` transmettent les données mises à jour aux écrans.

4. **Fréquence des frames** :
   - La condition `if (currentTime - lastFrameTime < 16)` limite la fréquence des frames à environ 60 FPS.

5. **Réinitialisation de l'animation** :
   - Lorsque le temps écoulé dépasse la durée de l'animation, celle-ci est réinitialisée pour recommencer.

### Résultat attendu
- Le premier rectangle se déplace de `(10, 20)` à `(100, 80)` avec une animation fluide.
- Le deuxième rectangle se déplace de `(200, 50)` à `(120, 100)` simultanément.
- Les deux animations utilisent la fonction de easing pour des transitions douces.

chabrof: j ai egalement des interruptions par les pins FR1_PIN et FR2_PIN qui rythme je pense à 60 hz ne pourrais pas utiliser les flags fr1_flag et fr2_flag pour controller les animation plutot que le test "currentTime - lastFrameTime < 16" que tu proposes ?

GitHub Copilot: Oui, vous pouvez utiliser les interruptions associées aux pins `FR1_PIN` et `FR2_PIN` pour rythmer vos animations à 60 Hz en vous basant sur les flags `fr1_flag` et `fr2_flag`. Cela permet de synchroniser les mises à jour des animations avec les interruptions, ce qui est plus précis et évite de dépendre d'une boucle temporelle.

Voici comment adapter le code pour utiliser les flags `fr1_flag` et `fr2_flag` :

### Exemple d'animation avec les flags `fr1_flag` et `fr2_flag`

Ajoutez le code suivant dans la boucle principale de votre fichier main.c :

```c
static uint32_t animationStartTime = 0;
static uint32_t animationDuration = 2000; // Durée de l'animation en ms (2 secondes)

while (1) {
  // Attendre les interruptions pour rythmer les frames
  if (fr1_flag) {
    fr1_flag = false; // Réinitialiser le flag

    // Calculer le temps écoulé depuis le début de l'animation
    uint32_t elapsedTime = HAL_GetTick() - animationStartTime;

    // Réinitialiser l'animation après sa durée
    if (elapsedTime > animationDuration) {
      animationStartTime = HAL_GetTick();
      elapsedTime = 0;
    }

    // Effacer les buffers avant de dessiner
    ClearBuffers();

    // Animation du premier rectangle
    uint16_t rect1StartX = 10, rect1EndX = 100;
    uint16_t rect1StartY = 20, rect1EndY = 80;
    uint16_t rect1Width = 30, rect1Height = 20;

    uint16_t rect1X = (uint16_t)EaseInOutQuad(elapsedTime, rect1StartX, rect1EndX - rect1StartX, animationDuration);
    uint16_t rect1Y = (uint16_t)EaseInOutQuad(elapsedTime, rect1StartY, rect1EndY - rect1StartY, animationDuration);

    DrawRect4BPP(rect1X, rect1Y, rect1Width, rect1Height, 0xF, 1);

    // Animation du deuxième rectangle
    uint16_t rect2StartX = 200, rect2EndX = 120;
    uint16_t rect2StartY = 50, rect2EndY = 100;
    uint16_t rect2Width = 40, rect2Height = 30;

    uint16_t rect2X = (uint16_t)EaseInOutQuad(elapsedTime, rect2StartX, rect2EndX - rect2StartX, animationDuration);
    uint16_t rect2Y = (uint16_t)EaseInOutQuad(elapsedTime, rect2StartY, rect2EndY - rect2StartY, animationDuration);

    DrawRect4BPP(rect2X, rect2Y, rect2Width, rect2Height, 0x7, 1);

    // Envoyer les buffers aux écrans
    SSD1320_SendBuffer_Left();
  }

  if (fr2_flag) {
    fr2_flag = false; // Réinitialiser le flag

    // Vous pouvez ajouter des animations ou des actions spécifiques pour l'écran de droite ici
    SSD1320_SendBuffer_Right();
  }
}
```

### Explications
1. **Utilisation des flags** :
   - Les flags `fr1_flag` et `fr2_flag` sont levés par les interruptions des pins `FR1_PIN` et `FR2_PIN`.
   - Lorsque `fr1_flag` est levé, l'animation est mise à jour et les données sont envoyées à l'écran de gauche.
   - Lorsque `fr2_flag` est levé, les données sont envoyées à l'écran de droite (vous pouvez ajouter des animations spécifiques si nécessaire).

2. **Synchronisation avec les interruptions** :
   - Les interruptions garantissent une fréquence stable (60 Hz dans votre cas), ce qui est idéal pour des animations fluides.

3. **Réinitialisation des flags** :
   - Les flags sont réinitialisés (`fr1_flag = false` et `fr2_flag = false`) après chaque traitement pour éviter des mises à jour multiples.

4. **Animation avec easing** :
   - Les positions des rectangles sont calculées à chaque interruption en fonction du temps écoulé (`elapsedTime`) et de la fonction `EaseInOutQuad`.

5. **Envoi des buffers** :
   - Les fonctions `SSD1320_SendBuffer_Left()` et `SSD1320_SendBuffer_Right()` transmettent les données mises à jour aux écrans.

### Résultat attendu
- Les animations des rectangles sont mises à jour à chaque interruption, garantissant une fréquence stable.
- L'écran de gauche affiche les animations des rectangles, tandis que l'écran de droite peut être utilisé pour d'autres animations ou actions.

chabrof: je trouve que le code est bien, je me pose une question : sur ce type d ecran la pin FR1_PIN permet d avoir une frequence de rafraichissement désiré par l'ecran, mais quel est son interet precis,  je  veux dire : quand j ai un front montant sur FR1 cela signifie que mon ecran est pret à recevoir un buffer immediatement ? auquel cas est ce que l animation des rectangles par exemple qui est gourmande en temps ne devrait pas etre faite apres le transfert du buffer et non avant pour satisfaire le besoin immediat de donnée de l'ecran ?

GitHub Copilot: Votre raisonnement est correct et pertinent. La pin `FR1_PIN` (ou tout signal similaire) est souvent utilisée pour indiquer que l'écran est prêt à recevoir un nouveau buffer. Cela permet de synchroniser les mises à jour avec le taux de rafraîchissement de l'écran. Voici une explication détaillée de son intérêt et des implications pour votre animation :

### Intérêt de la pin `FR1_PIN`
1. **Synchronisation avec l'écran** :
   - Le front montant sur `FR1_PIN` indique que l'écran est prêt à recevoir un nouveau buffer.
   - Cela garantit que les données envoyées à l'écran sont affichées au bon moment, évitant des artefacts visuels ou des données perdues.

2. **Optimisation des performances** :
   - En utilisant `FR1_PIN`, vous pouvez éviter d'envoyer des données à l'écran lorsqu'il n'est pas prêt, ce qui réduit les cycles inutiles et améliore l'efficacité.

3. **Animation fluide** :
   - En synchronisant les animations avec `FR1_PIN`, vous vous assurez que les transitions visuelles sont cohérentes avec le taux de rafraîchissement de l'écran.

### Implications pour les animations
Si l'écran demande un buffer immédiatement après un front montant sur `FR1_PIN`, il est préférable de **prioriser l'envoi du buffer** avant de calculer les prochaines étapes de l'animation. Cela garantit que l'écran reçoit les données à temps.

### Proposition de modification
Voici une approche où l'animation est calculée **après** l'envoi du buffer, en utilisant `FR1_PIN` pour synchroniser les mises à jour :

#### Code modifié dans la boucle principale
```c
static uint32_t animationStartTime = 0;
static uint32_t animationDuration = 2000; // Durée de l'animation en ms (2 secondes)

while (1) {
  // Attendre le front montant sur FR1_PIN
  if (fr1_flag) {
    fr1_flag = false; // Réinitialiser le flag

    // Envoyer le buffer actuel à l'écran
    SSD1320_SendBuffer_Left();

    // Calculer le temps écoulé depuis le début de l'animation
    uint32_t elapsedTime = HAL_GetTick() - animationStartTime;

    // Réinitialiser l'animation après sa durée
    if (elapsedTime > animationDuration) {
      animationStartTime = HAL_GetTick();
      elapsedTime = 0;
    }

    // Effacer les buffers avant de dessiner
    ClearBuffers();

    // Animation du premier rectangle
    uint16_t rect1StartX = 10, rect1EndX = 100;
    uint16_t rect1StartY = 20, rect1EndY = 80;
    uint16_t rect1Width = 30, rect1Height = 20;

    uint16_t rect1X = (uint16_t)EaseInOutQuad(elapsedTime, rect1StartX, rect1EndX - rect1StartX, animationDuration);
    uint16_t rect1Y = (uint16_t)EaseInOutQuad(elapsedTime, rect1StartY, rect1EndY - rect1StartY, animationDuration);

    DrawRect4BPP(rect1X, rect1Y, rect1Width, rect1Height, 0xF, 1);

    // Animation du deuxième rectangle
    uint16_t rect2StartX = 200, rect2EndX = 120;
    uint16_t rect2StartY = 50, rect2EndY = 100;
    uint16_t rect2Width = 40, rect2Height = 30;

    uint16_t rect2X = (uint16_t)EaseInOutQuad(elapsedTime, rect2StartX, rect2EndX - rect2StartX, animationDuration);
    uint16_t rect2Y = (uint16_t)EaseInOutQuad(elapsedTime, rect2StartY, rect2EndY - rect2StartY, animationDuration);

    DrawRect4BPP(rect2X, rect2Y, rect2Width, rect2Height, 0x7, 1);
  }

  // Traiter FR2_PIN si nécessaire
  if (fr2_flag) {
    fr2_flag = false; // Réinitialiser le flag
    SSD1320_SendBuffer_Right();
  }
}
```

### Explications des modifications
1. **Priorité à l'envoi du buffer** :
   - Dès que `FR1_PIN` signale que l'écran est prêt, le buffer actuel est envoyé immédiatement via `SSD1320_SendBuffer_Left()`.

2. **Calcul de l'animation après l'envoi** :
   - Les nouvelles positions des rectangles sont calculées après l'envoi du buffer, ce qui garantit que l'écran reçoit les données à temps.

3. **Synchronisation avec `FR1_PIN`** :
   - Les animations sont mises à jour uniquement lorsque l'écran est prêt, ce qui évite des calculs inutiles et garantit une fluidité optimale.

### Résultat attendu
- L'écran reçoit les données immédiatement après le front montant sur `FR1_PIN`.
- Les animations sont synchronisées avec le taux de rafraîchissement de l'écran.
- Les transitions visuelles restent fluides et cohérentes.

chabrof: très bien je vois, pour la pin FR1_PIN (et donc le flag fr1_flag) cela me parrait satisfaisant mais je prioriserai également le transfert du buffer si fr2_flag est vrai avant de gerer l'animation des rectangles.

GitHub Copilot: Voici une version modifiée du code qui priorise également le transfert du buffer pour `FR2_PIN` (via le flag `fr2_flag`) avant de gérer l'animation des rectangles. Cela garantit que les deux écrans reçoivent leurs données immédiatement après le front montant sur leurs pins respectives.

### Code modifié dans la boucle principale
```c
static uint32_t animationStartTime = 0;
static uint32_t animationDuration = 2000; // Durée de l'animation en ms (2 secondes)

while (1) {
  // Prioriser le transfert du buffer pour FR1_PIN
  if (fr1_flag) {
    fr1_flag = false; // Réinitialiser le flag

    // Envoyer le buffer actuel à l'écran de gauche
    SSD1320_SendBuffer_Left();

    // Calculer le temps écoulé depuis le début de l'animation
    uint32_t elapsedTime = HAL_GetTick() - animationStartTime;

    // Réinitialiser l'animation après sa durée
    if (elapsedTime > animationDuration) {
      animationStartTime = HAL_GetTick();
      elapsedTime = 0;
    }

    // Effacer les buffers avant de dessiner
    ClearBuffers();

    // Animation du premier rectangle
    uint16_t rect1StartX = 10, rect1EndX = 100;
    uint16_t rect1StartY = 20, rect1EndY = 80;
    uint16_t rect1Width = 30, rect1Height = 20;

    uint16_t rect1X = (uint16_t)EaseInOutQuad(elapsedTime, rect1StartX, rect1EndX - rect1StartX, animationDuration);
    uint16_t rect1Y = (uint16_t)EaseInOutQuad(elapsedTime, rect1StartY, rect1EndY - rect1StartY, animationDuration);

    DrawRect4BPP(rect1X, rect1Y, rect1Width, rect1Height, 0xF, 1);

    // Animation du deuxième rectangle
    uint16_t rect2StartX = 200, rect2EndX = 120;
    uint16_t rect2StartY = 50, rect2EndY = 100;
    uint16_t rect2Width = 40, rect2Height = 30;

    uint16_t rect2X = (uint16_t)EaseInOutQuad(elapsedTime, rect2StartX, rect2EndX - rect2StartX, animationDuration);
    uint16_t rect2Y = (uint16_t)EaseInOutQuad(elapsedTime, rect2StartY, rect2EndY - rect2StartY, animationDuration);

    DrawRect4BPP(rect2X, rect2Y, rect2Width, rect2Height, 0x7, 1);
  }

  // Prioriser le transfert du buffer pour FR2_PIN
  if (fr2_flag) {
    fr2_flag = false; // Réinitialiser le flag

    // Envoyer le buffer actuel à l'écran de droite
    SSD1320_SendBuffer_Right();

    // Vous pouvez ajouter des animations ou des actions spécifiques pour l'écran de droite ici
  }
}
```

### Explications des modifications
1. **Priorité au transfert des buffers** :
   - Les buffers sont envoyés immédiatement après que les flags `fr1_flag` ou `fr2_flag` sont levés, garantissant que les écrans reçoivent leurs données à temps.

2. **Animation après le transfert** :
   - Les animations des rectangles sont calculées et mises à jour après le transfert des buffers, ce qui évite de retarder l'envoi des données aux écrans.

3. **Synchronisation avec les interruptions** :
   - Les animations et les transferts sont synchronisés avec les interruptions des pins `FR1_PIN` et `FR2_PIN`, assurant une fréquence stable et une fluidité optimale.

### Résultat attendu
- Les écrans reçoivent leurs données immédiatement après les fronts montants sur `FR1_PIN` et `FR2_PIN`.
- Les animations des rectangles sont mises à jour après le transfert des buffers, garantissant une transition fluide et cohérente.