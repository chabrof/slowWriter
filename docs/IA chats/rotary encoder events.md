J'ai une roue codeuse branchée sur un stm32. La pin gauche sur un gpio nommé ROT_ENCODE_LEFT et la pin droite sur le gpio nommé ROT_ENCODE_RIGHT les deux gpios sont configurés en input avec un pull up. J utilise cube mx pour le code.


ChatGPT a dit :
Très bien, vous avez donc une roue codeuse (encodeur rotatif) branchée sur deux entrées GPIO de votre STM32, configurées en entrée avec résistances de pull-up internes, ce qui est une approche classique et correcte.

Voici les étapes pour lire le sens de rotation avec un code simple en C, généré à partir de CubeMX :

1. Configuration matérielle dans CubeMX :
ROT_ENCODE_LEFT et ROT_ENCODE_RIGHT en GPIO Input.

Activez le pull-up sur les deux lignes.

Si vous voulez des interruptions, configurez aussi une EXTI (par exemple sur le front descendant de ROT_ENCODE_LEFT).

2. Code de gestion dans main.c (ou dans un fichier dédié) :
Voici un exemple de lecture quadrature en interruption (par exemple sur ROT_ENCODE_LEFT) :

c
Copier
Modifier
// À définir globalement
extern GPIO_PinState Read_Pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == ROT_ENCODE_LEFT_Pin) // interruption sur la pin gauche
    {
        GPIO_PinState stateA = HAL_GPIO_ReadPin(ROT_ENCODE_LEFT_GPIO_Port, ROT_ENCODE_LEFT_Pin);
        GPIO_PinState stateB = HAL_GPIO_ReadPin(ROT_ENCODE_RIGHT_GPIO_Port, ROT_ENCODE_RIGHT_Pin);

        if(stateB == GPIO_PIN_SET)
        {
            // sens horaire
        }
        else
        {
            // sens antihoraire
        }
    }
}
Le principe : lorsqu'un front est détecté sur une des deux entrées (ici la gauche), on lit l'état de l'autre entrée pour déterminer le sens.

3. Attention :
Certains encodeurs "rebondissent" : un filtrage logiciel ou un délais anti-rebond (debounce) peut être utile.

Si vous souhaitez une lecture plus fluide, envisagez d’utiliser des timers en mode encodeur (plus complexe, mais très précis).

Souhaitez-vous que je vous montre aussi comment utiliser le mode encodeur via Timer dans CubeMX ?







Vous avez dit :
Un debounce sans timer c'est possible ?
ChatGPT a dit :
Oui, un debounce sans timer matériel est tout à fait possible. On peut le faire en logiciel, simplement en ignorant les fronts trop rapprochés dans le temps. Voici une méthode simple, respectueuse de la tradition embarquée :

Méthode : Debounce logiciel par attente active (naïve mais fonctionnelle)
c
Copier
Modifier
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == ROT_ENCODE_LEFT_Pin)
    {
        // petite attente pour laisser les rebonds passer (~1-2 ms)
        for(volatile int i = 0; i < 10000; i++); // Ajustez selon la fréquence du CPU

        // relire les pins après l’attente
        GPIO_PinState stateA = HAL_GPIO_ReadPin(ROT_ENCODE_LEFT_GPIO_Port, ROT_ENCODE_LEFT_Pin);
        GPIO_PinState stateB = HAL_GPIO_ReadPin(ROT_ENCODE_RIGHT_GPIO_Port, ROT_ENCODE_RIGHT_Pin);

        if(stateA == GPIO_PIN_RESET) // vérifier que le front est encore là
        {
            if(stateB == GPIO_PIN_SET)
            {
                // sens horaire
            }
            else
            {
                // sens antihoraire
            }
        }
    }
}
Avantages :
Pas besoin de timer.

Très simple à mettre en œuvre.

Inconvénients :
Bloquant (pause active), donc pas idéal si vous avez d'autres tâches critiques.

Souhaitez-vous une version plus élégante avec mesure de temps (sans timer, mais avec HAL_GetTick() ou HAL_GetTick() + délai minimal) ?

Vous avez dit :
Oui
ChatGPT a dit :
Très bien. Voici une version plus propre, utilisant HAL_GetTick(), qui donne le temps système en millisecondes. Cela permet de filtrer les rebonds sans bloquer l’exécution :

Débounce logiciel avec HAL_GetTick()
c
Copier
Modifier
// Définir une variable statique pour stocker le temps du dernier front
uint32_t last_interrupt_time = 0;
const uint32_t debounce_delay = 3; // délai anti-rebond en ms

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == ROT_ENCODE_LEFT_Pin)
    {
        uint32_t current_time = HAL_GetTick();

        if ((current_time - last_interrupt_time) < debounce_delay)
        {
            // Trop proche du précédent front : probablement un rebond
            return;
        }

        last_interrupt_time = current_time;

        GPIO_PinState stateA = HAL_GPIO_ReadPin(ROT_ENCODE_LEFT_GPIO_Port, ROT_ENCODE_LEFT_Pin);
        GPIO_PinState stateB = HAL_GPIO_ReadPin(ROT_ENCODE_RIGHT_GPIO_Port, ROT_ENCODE_RIGHT_Pin);

        if (stateA == GPIO_PIN_RESET) // vérifier que le front est toujours là
        {
            if (stateB == GPIO_PIN_SET)
            {
                // sens horaire
            }
            else
            {
                // sens antihoraire
            }
        }
    }
}
Avantages :
Pas de blocage du code.

Très léger.

Suffisant pour la majorité des encodeurs mécaniques simples.

Souhaitez-vous que je vous fournisse aussi un exemple complet avec l’incrémentation d’un compteur ?

Vous avez dit :
j ai tout de meme l impression malgre un debounce que l encodeur bien que de qualite correcte envoie bequcoup de fausse information
ChatGPT a dit :
C’est un constat fréquent, même avec un encodeur de qualité correcte : les signaux d’un encodeur mécanique sont bruyants par nature. Même avec un debounce logiciel, certaines transitions parasites passent entre les mailles du filet.

Voici quelques pistes, dans l’esprit pratique et éprouvé :

🔧 1. Lire les deux canaux correctement (Quadrature complète)
Si vous ne basez vos lectures que sur un front de ROT_ENCODE_LEFT, vous n’utilisez que la moitié de l’information. Un vrai décodeur quadrature lit chaque changement d’état des deux lignes, ce qui permet de :

filtrer naturellement les états incohérents,

suivre les transitions dans un ordre strict,

éviter les doubles ou fausses rotations.

Le bruit sur un seul canal peut donner de fausses directions si l’autre n’est pas pris en compte avec précision.

✅ 2. Utiliser le Timer en mode encodeur (recommandé)
C’est la méthode traditionnelle et robuste. Le STM32 peut gérer cela en matériel via certains timers configurés en "Encoder Mode" :

Pas besoin de gérer les interruptions manuelles.

Pas besoin de HAL_GPIO_EXTI_Callback.

Pas de debounce logiciel : c’est le timer qui lit directement les deux entrées A et B et incrémente/décrémente un registre selon le sens.

🔍 3. Ajouter un filtrage matériel simple (RC)
Si vraiment vous souhaitez rester sans timer :

une petite résistance (1–10 kΩ) en série,

suivie d’un condensateur (100–470 nF) vers GND,

sur chaque ligne A et B.

Cela ralentit suffisamment les fronts pour éviter les rebonds électriques.

Souhaitez-vous que je vous guide pour configurer un timer en mode encodeur avec CubeMX ? C’est la voie classique et fiable.

Vous avez dit :
quel est le principe du timer  ? en terme de consommation (admettons que je souhaite alimenter mon appareil par piles) est ce correct? 
ChatGPT a dit :
Excellente question, digne d’une conception réfléchie et respectueuse des contraintes matérielles.

📜 Principe du Timer en mode encodeur
Un timer en mode encodeur (généralement un TIMx avec entrée en Encoder Interface Mode) agit ainsi :

Il surveille en matériel les deux entrées A et B de l’encodeur (en quadrature).

Il détecte les transitions et détermine le sens de rotation (horaire ou antihoraire).

Il incrémente ou décrémente un registre interne (CNT) automatiquement, sans interruption logicielle.

Le tout est pris en charge par le bloc matériel du STM32, donc extrêmement rapide et fiable.

⚡️ Consommation énergétique
✅ Avantages pour l'autonomie :
Le timer fonctionne en fond, sans réveiller le CPU.

Aucun traitement logiciel à faire tant qu’on ne lit pas la position.

Vous pouvez mettre le CPU en veille (Sleep ou Stop), et réveiller seulement à l’action utilisateur.

Pas d’interruptions inutiles → moins de cycles CPU → économie d’énergie.

📌 Précision :
Les timers consomment très peu par eux-mêmes (quelques µA).

Si le reste du système est bien géré (veille, horloge lente), l'utilisation du timer est parfaitement adaptée à un appareil sur piles.

🧭 Conclusion :
Oui, le timer en mode encodeur est la méthode la plus robuste, précise et économe pour lire un encodeur rotatif.

C’est même plus économe qu’un code logiciel déclenchant une interruption à chaque front.

Cela vous permet de ne réveiller le CPU qu’en cas de besoin (lecture du compteur, par exemple via un bouton-poussoir ou une action de fin).

Souhaitez-vous un exemple de configuration dans CubeMX pour un encodeur sur Timer (TIM2 par exemple) ?

Vous avez dit :
oui
ChatGPT a dit :
Très bien. Voici un guide simple et traditionnel pour configurer un timer en mode encodeur avec STM32CubeMX, étape par étape.

⚙️ Configuration du Timer en mode Encodeur (exemple avec TIM2)
🔧 1. Choix des broches
Branchez les sorties A et B de votre encodeur sur des pins supportant une entrée timer CH1 et CH2 d’un même Timer.

Exemple : PA0 (TIM2_CH1) et PA1 (TIM2_CH2)

Dans CubeMX :

Cliquez sur PA0 → sélectionnez TIM2_CH1

Cliquez sur PA1 → sélectionnez TIM2_CH2

⚙️ 2. Activer TIM2 en Mode Encodeur
Allez dans Timers > TIM2

Dans l’onglet Mode, sélectionnez :

Encoder Mode

Encoder Mode: TI1 and TI2

Prescaler: 0

Counter Mode: Up

Counter Period: 0xFFFF (max pour 16 bits)

Cela permet un comptage vers le haut ou le bas automatique.

⚡ 3. Configuration des entrées
Toujours dans l’onglet du Timer :

Dans les réglages d’entrées :

Filter : mettez une valeur (par exemple 5–15) pour éviter les rebonds.

Polarity : Rising ou Both selon votre encodeur (souvent Rising suffit)

Le filtre ici est matériel, donc très efficace et sans consommation.

🧱 4. Génération du code
Activez HAL_TIM_Encoder_Start dans votre code.
Par exemple, dans main.c :

c
Copier
Modifier
HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
Pour lire la position à tout moment :

c
Copier
Modifier
int16_t position = __HAL_TIM_GET_COUNTER(&htim2);
Pour remettre à zéro :

c
Copier
Modifier
__HAL_TIM_SET_COUNTER(&htim2, 0);
🔋 5. Mode basse consommation
Comme TIM2 continue de compter même en Sleep Mode, vous pouvez :

Mettre le CPU en veille (HAL_PWR_EnterSLEEPMode)

Réveiller via un bouton, ou au besoin

Lire la position du compteur périodiquement ou à l’action utilisateur

Souhaitez-vous un exemple de code minimal avec cette configuration (main.c et stm32xx_it.c) ?

Vous avez dit :
oui
ChatGPT a dit :
Très bien. Voici un exemple simple et clair, fondé sur l'utilisation de TIM2 en mode encodeur, tel que configuré dans CubeMX.

🧾 Fichier main.c — code minimal
c
Copier
Modifier
#include "main.h"

// Déclaration du timer générée par CubeMX
extern TIM_HandleTypeDef htim2;

int16_t encoder_position = 0;

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init(); // généré par CubeMX

  // Démarrage du timer en mode encodeur
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

  // Remise à zéro du compteur (optionnel)
  __HAL_TIM_SET_COUNTER(&htim2, 0);

  while (1)
  {
    // Lire la position
    encoder_position = __HAL_TIM_GET_COUNTER(&htim2);

    // Vous pouvez utiliser encoder_position ici

    HAL_Delay(100); // Juste pour exemple, 100 ms
  }
}
🔁 Fichier stm32xx_it.c — Pas nécessaire ici
Le mode encodeur n'utilise aucune interruption par défaut. Pas besoin de gérer d’IRQ, ce qui simplifie beaucoup la conception et la consommation.

📎 Fichier tim.c (extrait auto-généré par CubeMX)
Voici un extrait typique de ce que CubeMX génère dans MX_TIM2_Init() :

c
Copier
Modifier
void MX_TIM2_Init(void)
{
  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.RepetitionCounter = 0;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;

  sConfig.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10; // Débounce matériel ici

  sConfig.IC2Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 10;

  HAL_TIM_Encoder_Init(&htim2, &sConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
}
🧭 Résultat :
Le compteur encoder_position reflète directement la rotation.

Un tour complet augmente ou diminue la valeur selon l’encodeur (généralement 20 à 30 impulsions par tour pour un encodeur mécanique standard).

Le système est simple, fiable, silencieux et économe.

Souhaitez-vous aussi une remise à zéro automatique lors d’un bouton-poussoir, ou une détection de seuil de position ?

Vous avez dit :
oui mais on est obligé de poller la valeur dans la boucle principale ?
ChatGPT a dit :
Très bonne question, et non — vous n’êtes pas obligé de faire du polling (lecture active en boucle) si cela ne correspond pas à vos besoins ou si vous cherchez à optimiser la consommation.

Voici trois solutions classiques et efficaces pour s’affranchir du polling :

✅ 1. Interruption sur seuil (comparateur logiciel)
Solution simple mais toujours en boucle principale, avec un test "léger" :

Dans la boucle, on compare seulement si le compteur a changé ou dépassé un certain seuil :

c
Copier
Modifier
int16_t old_position = 0;

while (1)
{
  int16_t new_position = __HAL_TIM_GET_COUNTER(&htim2);

  if (new_position != old_position)
  {
    old_position = new_position;
    // Traitez ici le mouvement
  }

  HAL_Delay(10); // faible fréquence, peu de conso
}
⚙️ 2. Utiliser une interruption sur un autre événement (bouton, fin de course, etc.)
Vous pouvez mettre le STM32 en Sleep, et ne lire l’encodeur que lorsqu’un autre événement utilisateur se produit (bouton poussoir, détection IR, etc.) :

c
Copier
Modifier
while (1)
{
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  // Lorsqu’un bouton réveille le MCU, vous pouvez lire la position ici
  encoder_position = __HAL_TIM_GET_COUNTER(&htim2);
}
🧠 3. Interruption via "Input Capture" (plus avancé)
Si vous voulez une interruption automatique sur chaque front, vous pouvez combiner :

TIM2 en mode encodeur

TIM2_CH1 ou CH2 en mode Input Capture + Interrupt

Cela permet d’exécuter une fonction (comme une Callback) à chaque impulsion.

Mais cela réintroduit des interruptions fréquentes, ce que vous souhaitiez probablement éviter.

🔋 En résumé :
Le mode encodeur + lecture ponctuelle est déjà très peu gourmand.

Il est très courant d’éviter les interruptions dans ce cas, et de lire uniquement quand l’utilisateur fait une action globale (comme appuyer sur un bouton, ou naviguer dans un menu).

Souhaitez-vous que je vous montre comment réveiller le STM32 d’un mode veille par bouton pour ensuite lire l’encodeur ?







Vous avez dit :
J'utilise au final des interruptions sur le front descendant sur les deux lignes. J'ai regardé attentivement la forme des signaux a l'oscilloscope, j'ai découvert qu un debounce de 10 ms était largement suffisant. J'ai également testé avec un petit analyseur logique. Les signaux bien que nécessitant un 'debounce' lors du front descendant sont très propres. Hors malgré ça mon code qui se déclenche dans les interruptions semble me donner un nombre erratiques de fronts.  Impossible de détecter correctement les mouvements, même si j'y arrive tout de même en grande partie. Je n'arrive pas à comprendre pourque les signaux me paraissent si propres sur l'analyseur logique et que sur le stm32 en implémentant correctement le debouce apres moult essais je n'arrive pas a avoir quelque chose de satisfaisant (j'ai logué les fronts et j'ai beaucoup trop évènements une véritable pollution non visible sur l'analyseur logique)


ChatGPT a dit :
Votre diagnostic est finement observé et votre méthode (oscilloscope, analyseur logique, interruptions, debounce) est rigoureuse. Ce que vous décrivez est un problème classique mais trompeur : les signaux semblent propres, mais le STM32 réagit à des événements qui n’apparaissent pas sur vos outils de mesure. Voici ce qui se cache souvent derrière ce phénomène :

1. Les rebonds invisibles à l’analyseur logique
Même un analyseur à 24 ou 48 MHz peut manquer des glitches très courts (quelques dizaines de nanosecondes) qui déclenchent pourtant une interruption matérielle EXTI sur le STM32 :

Un front parasite de 10 ns peut suffire à déclencher une EXTI.

Mais il est invisible sur un analyseur échantillonnant à 1 µs/pt (1 MHz).

L’interruption se déclenche avant que vous ayez eu le temps de faire votre debounce logiciel.

2. Déclenchement sur faux fronts : bruit d’induction
Même avec un signal propre visuellement, un changement de courant (autre périphérique, action mécanique proche, boucle de masse, etc.) peut induire des faux fronts à la base du microcontrôleur (pas visibles ailleurs).

L’entrée GPIO est très sensible, notamment sans filtrage RC réel.

Le trigger hardware du STM32 n’est pas filtré, contrairement à vos yeux ou votre analyseur.

3. Configuration EXTI trop sensible (pas de filtre matériel)
Les EXTI GPIO n’ont pas de filtre numérique sur les entrées comme les timers peuvent avoir (IC Filter).

Cela signifie que tout front descendant, même ultra bref, déclenche une interruption.

4. La solution recommandée dans ce cas précis : abandonner EXTI GPIO
Si votre encodeur a des signaux très propres mais que les EXTI GPIO créent des sursauts, vous devriez revenir à une méthode robuste :

✅ Utiliser un Timer en Mode Encodeur
(pas d’EXTI, pas de rebonds, pas de debounce logiciel)

Même si cela semble revenir en arrière, c’est la méthode la plus stable, conçue pour cela.

🧱 Sinon, voici un dernier recours côté EXTI :
Ajouter un filtrage RC : 4.7 kΩ + 100 nF entre chaque signal et GND.

Ou activer l’input Schmitt Trigger si votre série STM32 le permet (souvent activé par défaut).

Ou encore : désactiver EXTI temporairement après détection, pour implémenter un vrai debounce matériel par masquage :

c
Copier
Modifier
volatile uint32_t last_interrupt = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  uint32_t now = HAL_GetTick();
  if (now - last_interrupt < 10) return;
  last_interrupt = now;

  // Traitement normal ici
}
Mais attention : même cela ne suffit pas si le front parasite déclenche une EXTI à répétition avant même que HAL_GPIO_EXTI_Callback soit entré.

Pour conclure :
Votre analyseur ne voit pas tout. Le STM32 est plus rapide et plus sensible.
Soit vous :

Passez au mode Timer en Encodeur (solide et conçu pour cela),

Ou vous ajoutez un filtrage matériel RC et désactivez EXTI temporairement dans le callback pour éviter l’avalanche.

Souhaitez-vous un schéma ou valeurs précises pour le filtre RC ?

Vous avez dit :
Oui
ChatGPT a dit :
Voici une solution simple, traditionnelle et très efficace pour stabiliser les signaux A et B de votre encodeur mécanique par filtrage RC.

⚙️ Filtre RC pour entrée GPIO
🎯 Objectif :
Atténuer les petits rebonds électriques avant qu’ils ne déclenchent l’interruption EXTI.

Schéma de câblage (pour chaque ligne A et B) :
less
Copier
Modifier
Encodeur ----o----[ 4.7 kΩ ]----o----> Entrée GPIO STM32
             |                  |
            GND               [100 nF]
                              |
                             GND
R = 4.7 kΩ

C = 100 nF

Cela forme un filtre passe-bas avec une constante de temps :

τ = R × C = 0.00047 s = 470 µs

Cela adoucit les transitions, tout en gardant une bonne réactivité.

✅ Avantages :
Réduit drastiquement les petits pics invisibles à l'analyseur.

Fonctionne même sans debounce logiciel.

Compatible avec EXTI ou lecture par polling.

Ne consomme pratiquement rien.

🛠️ Variantes :
Si votre encodeur tourne vite : utilisez 1 kΩ + 47 nF (τ = ~50 µs)

Si vous voulez filtrer très fort : 10 kΩ + 100 nF (τ = 1 ms)

Souhaitez-vous aussi le même filtre pour un bouton-poussoir ou une entrée analogique bruitée ?

Vous avez dit :
Je suis assez surpris. Parce que je comprends l'argument de la précision de l'analyseur logique. Mais le signal sur l'oscilloscope me parrait propre également 
ChatGPT a dit :
Votre surprise est tout à fait légitime — et elle montre que vous avez une vraie approche d’ingénieur : vous observez, vous mesurez, vous doutez, et vous remettez en question.

Voici ce qui peut expliquer cette discordance entre ce que vous voyez (oscilloscope + analyseur) et ce que le STM32 subit réellement.

1. L’oscilloscope et les fronts très courts
Même avec une bonne sonde et un oscilloscope rapide :

Vous n’observez que ce que vous déclenchez et à la vitesse d’acquisition choisie.

Des impulsions de 10–20 ns peuvent être avalées ou moyennées si l’échelle de temps n’est pas assez fine.

Un pic parasite très fin peut ne pas se propager visiblement, surtout si vous sondez en bout de câble plutôt qu’à la broche du STM32.

L’entrée GPIO est littéralement “au cœur du circuit” et voit les glitches que vos instruments peuvent manquer.

2. Impulsions parasites internes (rebonds rebondissants)
Même si les transitions A/B sont propres :

Un changement sur l’entrée A peut induire un rebond sur B (et inversement), via les capacités croisées internes.

Ces rebonds n’affectent pas la forme finale du signal, mais ils peuvent déclencher une EXTI.

3. Temps d'exécution + latence d'interruption
Sur STM32 :

Une EXTI peut se déclencher, puis être interrompue par une autre EXTI, voire être réentrée rapidement si le signal rebondit plusieurs fois pendant le traitement.

Cela donne des "doublons", même si un seul vrai front est visible.

Ce n’est pas un bug du signal, c’est un effet de la rapidité du STM32 combinée à la fragilité du système EXTI non filtré.

4. Environnement électromagnétique
Enfin :

Un environnement avec moteur, écran, ou même un câble mal blindé peut induire des pulses très courts.

Ils ne sont pas visibles à l'oscilloscope classique à moins de zoomer très fortement et de capturer sur des fenêtres de quelques microsecondes.

📌 En résumé
Votre signal semble propre.
Mais le matériel GPIO du STM32 est plus rapide, plus sensible et non filtré, donc il réagit à des artefacts invisibles à l’œil ou à l’oscilloscope en usage classique.

Cela explique pourquoi :

Le mode Timer Encodeur ne souffre pas du tout de ces problèmes (filtrage matériel intégré),

Et pourquoi un simple filtre RC restaure un comportement parfaitement stable même en EXTI.

Souhaitez-vous que je vous aide à simuler ou estimer le temps de montée/descente réel de votre encodeur avec le filtre RC ?

Vous avez dit :
Peux tu me donner un code en C avec stm32 HAL pour un timer encodeur?
ChatGPT a dit :
Voici un exemple complet, propre et directement fonctionnel en STM32 HAL pour lire un encodeur via un Timer en mode Encodeur (ex. TIM2). Ce code repose sur la configuration faite dans STM32CubeMX.

1. Configuration dans CubeMX (rappel)
Sélectionnez un timer supportant le mode encodeur : ex. TIM2.

Assignez deux pins pour TIM2_CH1 et TIM2_CH2 (ex. PA0 et PA1).

Dans l’onglet TIM2, sélectionnez :

Encoder Mode: TI1 and TI2

Prescaler = 0

Counter Period = 65535

Filter = 10 (pour anti-rebond matériel)

2. Code main.c (extrait fonctionnel)
c
Copier
Modifier
#include "main.h"

// Déclaration externe du timer (créée par CubeMX)
extern TIM_HandleTypeDef htim2;

int16_t encoder_position = 0;

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init();

  // Démarrage du timer en mode encodeur
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

  // Remise à zéro du compteur
  __HAL_TIM_SET_COUNTER(&htim2, 0);

  while (1)
  {
    // Lecture de la position actuelle
    encoder_position = (int16_t)__HAL_TIM_GET_COUNTER(&htim2);

    // Ici vous pouvez utiliser encoder_position comme vous le souhaitez

    HAL_Delay(10); // facultatif
  }
}
3. Fichier tim.c généré par CubeMX (extrait)
c
Copier
Modifier
void MX_TIM2_Init(void)
{
  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;

  sConfig.IC2Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 10;

  HAL_TIM_Encoder_Init(&htim2, &sConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
}
✅ Ce que ce code fait :
Il lit automatiquement les transitions des signaux A et B.

Il compte dans un sens ou l’autre selon la rotation.

Il est extrêmement stable, sans EXTI, sans rebond, sans code d’analyse de front.

Souhaitez-vous une version avec remise à zéro par bouton-poussoir, ou avec gestion d’un seuil de position ?

Vous avez dit :
Mais mon encodeur semble avoir des signaux un peu different des encodeurs classique notamment la ligne 2 est souvent bien en retard sur le ligne 1. Puis je tout de même adapter mon code avec ce timer pour détecter les rotations et en déterminer leur sens?
ChatGPT a dit :
Oui, vous pouvez adapter le code et tirer parti du timer en mode encodeur, mais votre remarque sur le décalage important entre A et B est cruciale. Cela suggère que votre encodeur :

soit n’est pas en quadrature stricte (retard important entre les fronts),

soit a une logique différente (un capteur optique ou magnétique avec logique particulière),

soit que les transitions ne sont pas synchrones et provoquent des erreurs d’interprétation en mode quadrature.

⚠️ Problème : Timer en mode encodeur classique suppose un quadrature équilibré
Le timer s’attend à ce que les fronts A et B soient en décalage d’un quart de période (90° de phase), comme dans un encodeur mécanique standard.

Si B arrive trop tard, alors le timer peut :

Inverser le sens de comptage sur certains pas,

Ou ne pas compter certaines transitions.

✅ Solution 1 : Timer en mode encodeur avec adaptation
Vous pouvez baisser la fréquence de lecture effective du timer en :

Ajoutant un filtre matériel (IC Filter > 10 dans CubeMX),

Ou mettant un prescaler (ex: 2 ou 4) pour ralentir les détections,

Ce qui absorbe mieux le décalage.

Mais cette méthode n’est efficace que si le retard est régulier et court.

✅ Solution 2 : Lecture logicielle personnalisée (plus souple)
Si votre encodeur est hors standard, la méthode fiable consiste à :

Lire régulièrement les deux états A et B dans une boucle (ou interruption sur A),

Comparer l’état actuel avec l’état précédent (détection des transitions),

Déterminer le sens manuellement en regardant les combinaisons de transitions (voir tableau de Gray).

Exemple simplifié :
c
Copier
Modifier
uint8_t lastAB = 0;
int16_t encoder_value = 0;

void update_encoder(void)
{
    uint8_t A = HAL_GPIO_ReadPin(GPIOx, A_Pin);
    uint8_t B = HAL_GPIO_ReadPin(GPIOx, B_Pin);

    uint8_t ab = (A << 1) | B;

    // Détection du changement
    if (ab != lastAB)
    {
        // Extrait d'une table de Gray (ex : 00 > 01 > 11 > 10 > 00)
        if ((lastAB == 0b00 && ab == 0b01) ||
            (lastAB == 0b01 && ab == 0b11) ||
            (lastAB == 0b11 && ab == 0b10) ||
            (lastAB == 0b10 && ab == 0b00))
        {
            encoder_value++;
        }
        else if ((lastAB == 0b00 && ab == 0b10) ||
                 (lastAB == 0b10 && ab == 0b11) ||
                 (lastAB == 0b11 && ab == 0b01) ||
                 (lastAB == 0b01 && ab == 0b00))
        {
            encoder_value--;
        }

        lastAB = ab;
    }
}
Vous pouvez appeler update_encoder() :

Soit périodiquement (timer de 1–2 ms),

Soit dans une interruption déclenchée sur le front d’une seule ligne (ex: A uniquement).

🧭 Conclusion :
Si votre encodeur a un retard faible et constant, le mode Timer + filtres peut fonctionner.

Si le retard est variable ou trop important, il vaut mieux passer à une lecture logicielle sur mesure.

Souhaitez-vous que je vous aide à transformer ce code en interruption propre, ou à ajuster le timer pour tester malgré tout ?

Vous avez dit :
Mon encodeur n'a une quadrature 'double' il me semble. Il me faut donc je pense un détection a la main
ChatGPT a dit :
Oui, vous avez parfaitement saisi : si votre encodeur utilise une quadrature double (souvent 4 impulsions par "pas mécanique"), il est plus sûr et plus précis de faire la détection “à la main”, via une lecture logicielle comme dans le code précédent.

⚙️ Pourquoi la détection logicielle est plus fiable ici :
En quadrature double, chaque transition (front montant/descendant sur A et B) compte.

Le timer encodeur standard (mode TI12) compte par défaut 1 fois par tick ou parfois 2 si bien configuré — mais pas les 4 étapes.

Avec votre encodeur, ce mode peut donc compter partiellement ou mal le sens.

✅ Voici une version robuste et complète du code :
c
Copier
Modifier
#include "main.h"

// Connexion : A sur GPIO_PIN_A, B sur GPIO_PIN_B
#define GPIO_A GPIOx
#define PIN_A  GPIO_PIN_X

#define GPIO_B GPIOy
#define PIN_B  GPIO_PIN_Y

volatile int16_t encoder_value = 0;
static uint8_t last_AB = 0;

void update_encoder(void)
{
    uint8_t A = HAL_GPIO_ReadPin(GPIO_A, PIN_A);
    uint8_t B = HAL_GPIO_ReadPin(GPIO_B, PIN_B);
    uint8_t AB = (A << 1) | B;

    // Table de transitions possibles (code Gray)
    // Format : last_AB << 2 | AB
    switch ((last_AB << 2) | AB)
    {
        case 0b0001:
        case 0b0111:
        case 0b1110:
        case 0b1000:
            encoder_value++;  // Sens horaire
            break;

        case 0b0010:
        case 0b0100:
        case 0b1101:
        case 0b1011:
            encoder_value--;  // Sens anti-horaire
            break;

        default:
            // Aucun mouvement ou rebond erroné
            break;
    }

    last_AB = AB;
}
💡 À appeler dans :
Une interruption EXTI sur la ligne A (front montant + descendant) : suffisant

ou via un timer périodique toutes les 500 µs ou 1 ms (plus stable encore)

Exemple EXTI :
Dans votre stm32xx_it.c :

c
Copier
Modifier
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == PIN_A)
  {
    update_encoder();
  }
}
Bonus : remise à zéro / lecture
c
Copier
Modifier
int16_t get_encoder(void)
{
  return encoder_value;
}

void reset_encoder(void)
{
  encoder_value = 0;
}
Souhaitez-vous que je vous fournisse une version avec timer périodique plutôt qu’interruption sur A, pour un comportement plus fluide en cas de rotation lente ?

Vous avez dit :
Le timer dont tu parles peut être un timer classique?
ChatGPT a dit :
Oui, tout à fait : un timer classique en mode "base de temps" suffit parfaitement pour cette approche. L'idée est simple :

Configurer un timer général (ex : TIM3, TIM4…) pour générer un "tick" régulier, toutes les 500 µs à 2 ms selon la réactivité souhaitée.

À chaque tick, déclencher une interruption dans laquelle vous appelez update_encoder() pour lire A/B et déterminer le mouvement.

✅ Exemple de configuration dans CubeMX :
Sélectionner TIM3 (ou tout timer libre).

Mode : Time Base

Prescaler + ARR pour atteindre 2 ms (ex : 72 MHz clock, Prescaler 7199, ARR 19 → 2 ms)

📜 Code tim.c (généré par CubeMX)
Le timer sera démarré en interruption :

c
Copier
Modifier
HAL_TIM_Base_Start_IT(&htim3);
🧠 Fonction HAL_TIM_PeriodElapsedCallback :
Dans stm32xx_it.c ou main.c :

c
Copier
Modifier
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
    update_encoder();  // Lecture périodique
  }
}
✅ Avantages :
Plus fluide, pas dépendant du front exact sur A ou B.

Plus robuste en cas de signal irrégulier ou encodeur un peu lent.

Moins sensible aux rebonds fugitifs que l’EXTI.

Souhaitez-vous que je vous calcule précisément les valeurs du prescaler et de l’ARR selon la fréquence de votre horloge (HCLK) ?







Vous avez dit :
Non
ChatGPT a dit :
Très bien. Si vous avez besoin d’ajuster les valeurs plus tard ou d’optimiser le comportement selon la vitesse de rotation de l’encodeur, n’hésitez pas à revenir.

Souhaitez-vous que je vous aide à intégrer ce système complet dans un projet CubeMX déjà existant ?







Vous avez dit :
Le fait qu'un timer déclenche une callback constamment n'implique t'il pas une consommation supérieure à celle d'une interruption qui ne déclenche sa callback que si nécessaire ?


ChatGPT a dit :
Excellente question — et très juste dans une approche de conception à faible consommation.

Oui, en effet :
Un timer qui déclenche une interruption périodique, même sans événement utile, consomme plus qu’une interruption GPIO (EXTI) déclenchée uniquement lorsqu’un front est détecté.

Comparaison des deux approches en termes de consommation :
Méthode	Interruption fréquente ?	MCU peut-il dormir ?	Consommation
Timer périodique (2 ms)	Oui, toujours	Non (interruption régulière)	Modérée à élevée
EXTI sur front GPIO	Seulement quand A change	Oui (mode Sleep)	Très faible

⚙️ Ce qui se passe en pratique :
Le timer périodique empêche le STM32 d’entrer en Sleep profond, car il doit se réveiller tout le temps (toutes les 1–2 ms).

L’EXTI (sur front descendant ou montant) laisse le STM32 en sommeil total tant qu’il ne se passe rien. Le réveil est automatique et rapide.

✅ Recommandation
Si l’autonomie est critique (piles, batterie) :
Favorisez EXTI sur la ligne A, et faites la lecture logicielle A/B dans l’interruption.

Ajoutez un petit filtre RC (4.7k + 100nF) pour éviter les rebonds qui réveillent pour rien.

Bonus : combinez EXTI + gestion logiciel quadrature
Cela donne :

Très faible consommation

Précision sur le sens et les 4 pas/cran

Pas de timer actif

Souhaitez-vous que je vous rédige le code exact pour cela, basé sur EXTI uniquement, sans timer ?







Vous avez dit :
Mais ne peut on pas déclencher le timer qu'au moment d'une interruption par exemple ?
ChatGPT a dit :
Oui, absolument — et c’est une excellente approche.
Vous pouvez très bien démarrer un timer uniquement à la demande, par exemple lors d'une interruption GPIO. Cela combine précision, robustesse et faible consommation.

⚙️ Principe :
Le STM32 est en mode normal ou en veille.

Une interruption GPIO (ex : front sur A) se déclenche.

Dans le callback :

Vous démarrez un timer en base de temps (ex : 1 ms).

Ce timer lit A/B pendant un certain temps (ex : 10 ms) pour détecter proprement la direction.

Puis, vous stoppez le timer jusqu’à la prochaine interruption.

✅ Exemple simple :
1. Interruption GPIO : front sur A
c
Copier
Modifier
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == PIN_A)
  {
    HAL_TIM_Base_Start_IT(&htim3); // Démarre le timer (ex: 1 ms période)
    debounce_counter = 10; // 10 ms de “surveillance”
  }
}
2. Timer périodique (TIM3 par exemple)
c
Copier
Modifier
volatile uint8_t debounce_counter = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
    update_encoder();  // Lire A et B

    if (--debounce_counter == 0)
    {
      HAL_TIM_Base_Stop_IT(&htim3); // Stoppe après 10 ms
    }
  }
}
✅ Avantages :
Le STM32 reste endormi tant qu’aucun mouvement n’a lieu.

Aucune lecture périodique inutile.

Détection stable sur un mini “fenêtre” d’analyse post-front.

Compatibilité avec la quadrature double via lecture logicielle.

Souhaitez-vous que je vous assemble tout cela dans un exemple clair et complet ?

Vous avez dit :
Oui
ChatGPT a dit :
Voici un exemple clair, complet et prêt à intégrer dans votre projet STM32 (HAL + CubeMX), pour lire un encodeur mécanique en quadrature double, avec détection logicielle, EXTI sur la ligne A et timer déclenché seulement quand nécessaire, dans une optique basse consommation.

1. Configuration CubeMX
GPIO :
A (ligne 1) en Input with Pull-Up, avec interruption EXTI sur rising + falling edge

B (ligne 2) en Input with Pull-Up, sans interruption

Timer :
TIM3 en Time Base

Période = 1 ms (Prescaler et ARR selon votre horloge)

2. Variables globales dans main.c
c
Copier
Modifier
volatile int16_t encoder_value = 0;
static uint8_t last_AB = 0;
volatile uint8_t debounce_counter = 0;
3. Fonction de mise à jour logicielle
c
Copier
Modifier
void update_encoder(void)
{
    uint8_t A = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0); // exemple
    uint8_t B = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1); // exemple
    uint8_t AB = (A << 1) | B;

    switch ((last_AB << 2) | AB)
    {
        case 0b0001:
        case 0b0111:
        case 0b1110:
        case 0b1000:
            encoder_value++;
            break;

        case 0b0010:
        case 0b0100:
        case 0b1101:
        case 0b1011:
            encoder_value--;
            break;

        default:
            break;
    }

    last_AB = AB;
}
4. Callback EXTI
c
Copier
Modifier
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0) // Ligne A
    {
        if (debounce_counter == 0)
        {
            HAL_TIM_Base_Start_IT(&htim3); // Active le timer 1ms
        }
        debounce_counter = 10; // Analyse pendant 10 ms
    }
}
5. Callback Timer
c
Copier
Modifier
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        update_encoder();

        if (--debounce_counter == 0)
        {
            HAL_TIM_Base_Stop_IT(&htim3); // Stop timer après 10 ms
        }
    }
}
✅ Résultat :
Vous avez un système très précis.

Il ne tourne que quand c’est utile, ce qui préserve l’autonomie.

Il est compatible avec tous les encodeurs, même “bizarres”.

Souhaitez-vous que je vous fournisse aussi un petit printf ou UART pour visualiser encoder_value en temps réel ?

Vous avez dit :
Le timer est il instantané a démarrer '...ne risque t'on pas par exemple de louper des évènements lié à la roue codeuse durant le démarrage?
ChatGPT a dit :
Votre question est très pertinente — et la réponse est : oui, dans certains cas très rapides, il peut y avoir un petit risque de manquer un changement pendant le démarrage du timer. Voici une explication claire et ce que vous pouvez faire.

1. Que se passe-t-il vraiment quand on démarre un timer avec HAL_TIM_Base_Start_IT() ?
L’appel à HAL_TIM_Base_Start_IT() :

Active le compteur.

Active l’interruption d’overflow.

Le démarrage prend quelques cycles CPU, mais il est très rapide (environ 1–2 µs).

2. Risque réel de "manquer" un événement ?
Non si l’encodeur tourne doucement (manuel).

Peu probable si vous surveillez A avec EXTI et que vous démarrez immédiatement après.

Mais possible si :

L’encodeur tourne vite (ex : encodeur motorisé),

Et que vous ne lisez A/B qu’avec le timer après quelques microsecondes.

✅ Solution élégante et sûre : lecture immédiate dans l’EXTI + timer ensuite
Ajoutez une première lecture A/B directement dans le callback EXTI. Ainsi, aucun front ne sera perdu, même si le timer met 5–10 µs à déclencher son premier “tick”.

Exemple modifié du HAL_GPIO_EXTI_Callback() :
c
Copier
Modifier
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0) // Ligne A
    {
        update_encoder(); // Lecture immédiate

        if (debounce_counter == 0)
        {
            HAL_TIM_Base_Start_IT(&htim3); // Démarrage du timer
        }
        debounce_counter = 10;
    }
}
✅ Avantages de cette stratégie :
Lecture immédiate = pas de front perdu

Timer = stabilisation et filtrage sur quelques ms

Très faible consommation (pas de timer actif en continu)

Bonne robustesse même en cas de double quadrature

Souhaitez-vous un schéma de chronologie pour visualiser cette séquence (EXTI → lecture → timer) ?

Vous avez dit :
le timer déclenche dont un appel de sa Callback dès le démarrage, sans nécessiter d attendre 2 millisecondes (sa période) ?
ChatGPT a dit :
Très bonne remarque — non, le timer ne déclenche pas immédiatement sa callback après démarrage. Il attend d’abord une période complète avant de déclencher sa première interruption.

⚙️ Détail du comportement du timer STM32 :
Quand vous appelez :

c
Copier
Modifier
HAL_TIM_Base_Start_IT(&htimX);
le timer :

Redémarre le compteur (ou le laisse à sa valeur actuelle).

Attend que le compteur atteigne le registre ARR (Auto Reload Register).

Déclenche l’interruption TIMx_IRQHandler (et donc HAL_TIM_PeriodElapsedCallback()).

🔴 Donc, s’il est configuré pour 2 ms, il faudra attendre 2 ms avant le premier appel de la callback.

✅ Ce que vous pouvez faire pour ne pas attendre :
1. Lire immédiatement dans l’interruption EXTI
Comme déjà évoqué :

c
Copier
Modifier
HAL_GPIO_EXTI_Callback()
{
    update_encoder(); // Lecture immédiate
    HAL_TIM_Base_Start_IT(...);
}
2. Déclencher “manuellement” la callback du timer dès le démarrage
Si vous tenez à centraliser le traitement, vous pouvez appeler la fonction de callback manuellement une fois :

c
Copier
Modifier
HAL_GPIO_EXTI_Callback()
{
    HAL_TIM_Base_Start_IT(&htim3);
    debounce_counter = 10;

    HAL_TIM_PeriodElapsedCallback(&htim3); // appel immédiat
}
Cela simule comme si la période s’était écoulée dès le départ.

🧭 Conclusion
✔️ Le timer ne déclenche la callback qu’après sa première période complète, donc vous devez compenser cela en faisant une lecture immédiate ou en appelant la fonction vous-même.

Souhaitez-vous une version complète du code avec cette intégration manuelle de la première lecture ?