# CODEXION – Liste des modifications

Correctifs appliqués **uniquement à l'intérieur des fonctions existantes**.

- Aucun nom de fonction ni de variable existante n'a été changé.
- Aucun fichier ajouté. **Une seule fonction `static` ajoutée** : `wait_dongle` dans `dongles.c` (l'attente avec `pthread_cond_timedwait` ne tient pas dans les 25 lignes de `take_one`).
- **Version avec `pthread_cond_wait`** : l'attente des dongles repose sur les variables de condition (pas d'attente active).
- `codexion.h`, `struct.h`, `Makefile`, `init.c` et `time.c` sont **inchangés**.
- Norminette : OK. Compilation `-Wall -Wextra -Werror` : OK.

| Fichier | Fonction | Problème corrigé |
|---|---|---|
| `dongles.c` | `wait_dongle` (nouvelle, static) | Attente `cond_wait` / `cond_timedwait` (cooldown) |
| `dongles.c` | `take_one` | Blocage cooldown, blocage après burnout, mauvais `heap_pop` |
| `dongles.c` | `take_dongle` | Retour toujours à 1 |
| `simulation.c` | `routine` | Boucle après stop, relâche de dongles non possédés |
| `useful.c` | `check_burnout` | Data race, faux burnout |
| `simulation.c` | `monitor` | Réveil de tous les dongles après l'arrêt |
| `useful.c` | `log_msg` | Messages affichés après « burned out » |
| `action.c` | `compiles` | Ordre `last_compile` / log |
| `scheduler.c` | `heap_push` | Data race sur `last_compile` |
| `thread.c` | `create_threads` | `pthread_create` non vérifié (segfault) |
| `parsing.c` | `is_positive` | Débordement d'`atoi` |
| `main.c` | `main` | `stop_mutex` jamais détruit |

---

## 1. `dongles.c` – `take_one` et nouvelle fonction `wait_dongle`

**Problèmes**

1. **Cooldown jamais « réveillé »** : le coder attendait avec `pthread_cond_wait`. Après un `release_dongle`, le dongle est disponible mais encore en cooldown. Le coder se réveille, voit que le cooldown n'est pas fini, se rendort, et personne ne le réveille à la fin du cooldown. Il dort jusqu'au burnout.
2. **Blocage après burnout** : `check_burnout` ne faisait le `broadcast` que sur les dongles du coder 0. Les autres threads restaient bloqués dans `cond_wait` et le `pthread_join` ne se terminait jamais.
3. **Mauvais `heap_pop`** : sur le chemin « stop », `heap_pop` retirait toujours la tête de file, même si ce n'était pas la requête du coder courant.

**Avant** (`take_one`)

```c
while (!is_stopped(coder))
{
    if (dongle_ready(dongle, coder) && is_front(dongle, coder))
        break ;
    pthread_cond_wait(&dongle->cond, &dongle->lock);
}
if (is_stopped(coder))
{
    heap_pop(dongle);
    pthread_mutex_unlock(&dongle->lock);
    return ;
}
heap_pop(dongle);
dongle->available = 0;
pthread_mutex_unlock(&dongle->lock);
log_msg(coder, "has taken a dongle");
```

**Après** (`take_one`)

```c
while (!is_stopped(coder))
{
    if (dongle_ready(dongle, coder) && is_front(dongle, coder))
    {
        heap_pop(dongle);
        dongle->available = 0;
        pthread_mutex_unlock(&dongle->lock);
        log_msg(coder, "has taken a dongle");
        return ;
    }
    wait_dongle(dongle, coder);
}
pthread_mutex_unlock(&dongle->lock);
```

**Nouvelle fonction** `wait_dongle` (static)

```c
static void	wait_dongle(t_dongle *dongle, t_coder *coder)
{
    struct timeval  tv;
    struct timespec ts;
    long            remain;

    remain = coder->config->dongle_cooldown
        - (get_timestamp_ms(coder->config->start_time) - dongle->released_at);
    if (dongle->available == 0 || remain <= 0)
    {
        pthread_cond_wait(&dongle->cond, &dongle->lock);
        return ;
    }
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + remain / 1000;
    ts.tv_nsec = tv.tv_usec * 1000 + (remain % 1000) * 1000000;
    ts.tv_sec += ts.tv_nsec / 1000000000;
    ts.tv_nsec %= 1000000000;
    pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
}
```

- **Dongle pris par un autre coder** (ou prêt mais pas en tête de file) : `pthread_cond_wait`. Le coder est réveillé par le `broadcast` de `release_dongle`.
- **Dongle libre mais encore en cooldown** : `pthread_cond_timedwait` avec une échéance égale au temps de cooldown restant. Le coder se réveille tout seul à la fin du cooldown.
- `heap_pop` n'est appelé que lorsque le coder est bien en tête de file (`is_front`).

---

## 2. `dongles.c` – `take_dongle`

**Problème** : la fonction renvoyait toujours `1`, même si la simulation était stoppée avant d'avoir pris les dongles.

**Avant** : `return (1);`

**Après** : `return (!is_stopped(coders));`

---

## 3. `simulation.c` – `routine`

**Problèmes**

1. Après un stop, la boucle continuait jusqu'à `compiles_required` et appelait `release_dongle` sur des dongles qui n'avaient pas été pris.
2. Avec un grand `compiles_required`, ces tours à vide pouvaient durer longtemps.

**Avant**

```c
while (i < coder->config->compiles_required)
{
    take_dongle(coder);
    compiles(coder);
```

**Après**

```c
while (i < coder->config->compiles_required && !is_stopped(coder))
{
    if (!take_dongle(coder))
        break ;
    compiles(coder);
```

---

## 4. `useful.c` – `check_burnout`

**Problèmes**

1. **Data race** : `last_compile` était lu sans `activity_mutex`, alors que `compiles` l'écrit sous ce mutex.
2. **Faux burnout** : un coder qui a déjà fini tous ses compiles continuait d'être surveillé. Son `last_compile` vieillissait et il était déclaré « burned out » alors qu'il avait terminé.
3. **Broadcast incomplet** : seuls `coders->left->cond` et `coders->right->cond` (coder 0) étaient réveillés. Ces deux lignes sont retirées ; le réveil de **tous** les dongles est fait par `monitor` (voir section suivante).

**Après** (extrait)

```c
pthread_mutex_lock(&coders[i].activity_mutex);
done = coders[i].compile_done;
last = coders[i].last_compile;
pthread_mutex_unlock(&coders[i].activity_mutex);
now = get_timestamp_ms(coders[i].config->start_time);
if (done < coders->config->compiles_required
    && now - last > coders->config->time_to_burnout)
{
    pthread_mutex_lock(&coders[i].config->stop_mutex);
    coders[i].config->stop = 1;
    pthread_mutex_unlock(&coders[i].config->stop_mutex);
    log_msg(&coders[i], "burned out");
    return (1);
}
```

Deux variables locales ont été ajoutées : `done` et `last`.

---

## 4 bis. `simulation.c` – `monitor`

**Problème** : après un burnout, les coders bloqués dans `pthread_cond_wait` sur d'autres dongles n'étaient jamais réveillés, donc le programme ne se terminait pas.

**Correctif** : une fois sorti de la boucle de surveillance (burnout ou fin normale), le monitor fait un `broadcast` sur le dongle de chaque coder, **sous le mutex du dongle**. Ce verrou évite qu'un réveil soit perdu entre le test `is_stopped` et le `cond_wait`. Une variable locale `i` a été ajoutée.

```c
i = 0;
while (i < coder->config->number_of_coder)
{
    pthread_mutex_lock(&coder[i].left->lock);
    pthread_cond_broadcast(&coder[i].left->cond);
    pthread_mutex_unlock(&coder[i].left->lock);
    i++;
}
```

Chaque dongle est le dongle `left` d'exactement un coder, donc cette boucle couvre tous les dongles.

---

## 5. `useful.c` – `log_msg`

**Problème** : après le burnout, d'autres coders pouvaient encore afficher des messages, parfois après la ligne « burned out ».

**Avant**

```c
timestamp = get_timestamp_ms(coder->config->start_time);
printf("%ld %d %s\n", timestamp, coder->id, msg);
```

**Après**

```c
if (!is_stopped(coder) || strcmp(msg, "burned out") == 0)
{
    timestamp = get_timestamp_ms(coder->config->start_time);
    printf("%ld %d %s\n", timestamp, coder->id, msg);
}
```

- `check_burnout` positionne `stop` **avant** d'appeler `log_msg`.
- Une fois `stop` positionné, plus aucun message n'est affiché, sauf « burned out » lui-même, qui est donc toujours la dernière ligne.

---

## 6. `action.c` – `compiles`

**Problème** : le log « is compiling » était affiché avant la mise à jour de `last_compile`. Le monitor pouvait détecter un burnout entre les deux.

**Correctif** : le bloc `lock / compile_done++ / last_compile = ... / unlock` est maintenant exécuté **avant** `log_msg(coder, "is compiling")`.

---

## 7. `scheduler.c` – `heap_push`

**Problème** : data race. `coder->last_compile` était lu sans mutex pour calculer la `deadline` (utilisée par EDF).

**Après**

```c
pthread_mutex_lock(&coder->activity_mutex);
dongle->heap[i].deadline = coder->last_compile
    + coder->config->time_to_burnout;
pthread_mutex_unlock(&coder->activity_mutex);
```

---

## 8. `thread.c` – `create_threads`

**Problème** : `pthread_create` n'était jamais vérifié. Si un thread n'était pas créé (par exemple avec un nombre de coders énorme), `pthread_join` était appelé sur un `pthread_t` non initialisé, ce qui provoque un segfault. C'est probablement la cause du segfault noté dans `concept.md` avec `100000` coders.

**Correctif**

- Une variable locale `n` compte les threads réellement créés.
- La boucle de création s'arrête au premier échec.
- Seuls les `n` threads créés sont joints.

```c
n = 0;
while (n < config->number_of_coder)
{
    if (pthread_create(&coders->thread[n], NULL, routine, &coders[n]))
        break ;
    n++;
}
pthread_create(&monitor_t, NULL, monitor, coders);
i = 0;
while (i < n)
{
    pthread_join(coders->thread[i], NULL);
    i++;
}
```

Si un thread n'a pas pu démarrer, le monitor le voit comme un coder qui n'a jamais compilé et déclenche un burnout. La simulation s'arrête alors proprement.

---

## 9. `parsing.c` – `is_positive`

**Problème** : `atoi` déborde sur les grands nombres (par exemple `99999999999`), ce qui donne un comportement indéfini ou une valeur négative.

**Correctif**, ajouté juste avant le `return (1)` final :

```c
if (i > 10 || atol(str) > 2147483647)
    return (0);
```

---

## 10. `main.c` – `main`

**Problème** : `stop_mutex` est initialisé dans `main` mais n'était jamais détruit.

**Correctif** : ajout de `pthread_mutex_destroy(&config.stop_mutex);` après la destruction de `print_mutex`.

---

## Vérifications effectuées

| Test | Résultat |
|---|---|
| Compilation `-Wall -Wextra -Werror` | OK |
| `norminette` | OK |
| ThreadSanitizer (4 configurations, FIFO et EDF) | 0 data race |
| AddressSanitizer + UBSan | 0 erreur |
| 144 exécutions de stress (cas limites, burnout rapide, gros cooldown) | 0 blocage, 0 message après « burned out » |
| Burnout avec cooldown de 5000 ms | Arrêt à ~105 ms (pas d'attente du cooldown) |
| Cooldown de 300 ms | Respecté (le 2e coder compile à 400 ms) |
| `4 800 200 200 200 5 0` et `5 800 200 200 200 5 0` (FIFO et EDF) | 5/5 sans burnout |
| 1 coder | 1 dongle pris, burnout à ~801 ms |
| Arguments invalides (négatif, trop grand, `lifo`, mauvais nombre) | Rejetés, code retour 1 |

## Limites connues (non modifiées)

- **Configurations très tendues** : par exemple `5 800 200 100 100 3 10`. Chaque coder garde un dongle pendant qu'il attend le second, ce qui limite le débit. Un burnout peut survenir même si la configuration est théoriquement faisable. Le corriger demanderait de réserver les deux dongles ensemble, donc plus de code.
- **Limite de 300 coders** dans `parsing.c` : conservée telle quelle.
