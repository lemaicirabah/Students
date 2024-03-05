# INF3173-241-TP2 - Compression de fichier

Ce TP vise à réaliser le logiciel de compression de fichier `kompress`. Le travail comprend deux étapes. La première consiste à compléter la fonctionnalité, et la seconde à analyser la performance de votre implémentation.

## Étape 1: Réalisation de la compression

Le programme est basé sur la librairie de compression [miniz](https://github.com/richgel999/miniz). La librairie est déjà intégrée dans les sources. Le principe est simple. Le fichier d'entrée est lu en mémoire, les données sont compressées puis écrites dans un fichier de sortie. La décompression est l'opération inverse. Fait à noter, on peut déduire que le fichier compressé sera de taille égale ou inférieure à celui de départ, mais la librairie n'offre pas (à notre connaissance) de moyen de déterminer à l'avance la taille que prendra un fichier compressé une fois décompressé.

La compression correspond à l'opération `deflate`, tandis que la décompression correspond à `inflate`. Un fichier ayant été compressé, puis décompressé devrait être identique à l'original. Une façon de le vérifier consiste à utiliser la commande `md5sum` généralement disponible.

Voici un exemple d'exécution du programme en ligne de commande: 

```
# Astuce : charger le fichier env.sh se trouvant dans le répertoire de compilation permet d'ajouter l'utilitaire dans le PATH
source env.sh

# création d'un fichier plein de zéros, mais vous pourriez utiliser n'importe quel fichier d'entrée
dd if=/dev/zero of=file.v1 bs=1k count=10

# Compression
kompress deflate file.v1 file.z --method stdio --block-size 42

# Décompression (options par défaut)
kompress inflate file.z file.v2

# Vérification des fichiers
md5sum file.v1 file.v2
```

La librairie supporte deux modes d'opérations. Le premier considère que les données sont complètement chargées en mémoire. Ce mode ne fonctionne que pour les petits fichiers. Pour les fichiers volumineux, on pourrait utiliser ou dépasser toute la mémoire sur l'ordinateur, ce qui n'est pas une situation souhaitable. L'autre mode consiste à compresser les données par blocs. La mémoire requise est donc bornée et indépendante de la taille du fichier d'entrée. Votre implémentation doit être basé sur ce mode par bloc. La librairie utilise la structure `stream` pour conserver l'état de la compression du fichier.

Le programme doit comprend les trois implémentations suivantes :  

* `kompress_stdio.c` : utilise `fread()` et `fwrite()`. Cette implémentation est fournie. Cela vous donne une référence sur laquelle baser les deux autres variantes. Étudiez attentivement ce code, car il est commenté et indique le fonctionnement en détail de la librairie et le passage des options à l'aide de `struct kompress_app`.
* `kompress_readwrite.c` : utilise `read()` et `write()`. À compléter. Cette implémentation est facile à réaliser à partir de la variante précédente. La taille de bloc passée en paramètre doit être respectée pour obtenir tous les points.
* `kompress_mmap.c` : utilise une projection en mémoire des fichiers. À compléter. Cette implémentation est d'un niveau de difficulté plus élevé. Considérant que le système d'exploitation s'occupe de la pagination, il est autorisé de réaliser un appel à `mmap` avec une taille supérieure à la taille des bloc.

Des tests sont fournis pour que vous puissiez vérifier votre code. Ceux-ci valident que les implémentations produisent le bon résultat, autant pour la compression que la décompression et pour chaque variante. Deux éléments sont vérifiés, soit la taille du fichier de sortie et une somme de contrôle basé sur l'algorithme [CRC32](https://fr.wikipedia.org/wiki/Contr%C3%B4le_de_redondance_cyclique).

## Étape 2: Analyse de performance

Quelle est la méthode la plus performante? Un aspect essentiel de la réalisation d'un programme est de mesurer le temps d'exécution.

> Avant toute chose, assurez-vous que votre programme soit fonctionnel et exempt de fuite mémoire ou de descripteur de fichier. Le banc d'essai de performance va faire un appel répété à chaque fonction, et donc pourrait planter en cas de fuite et donc fausser les résultats.

Pour mesurer les performances avec le banc d'essai, le programme doit être compilé en mode `Release`. Ce mode active les optimisations du compilateur et améliore la performance. Avec CMake, il faut spécifier l'option `CMAKE_BUILD_TYPE`, par exemple : 

```
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Le banc d'essai de performance basé sur [Google Benchmark](https://github.com/google/benchmark) est déjà fourni dans le fichier `bench/bench_kompress.cpp`. Le temps d'exécution est mesurée en fonction de la taille de bloc (en puissance de 2). Une métrique est calculée, soit le débit d'octets par seconde traités. Une valeur plus élevée est meilleure.

Réaliser un graphique des données avec l'outil de votre choix. Pour obtenir les données brutes, vous pouvez utiliser l'option pour écrire la sortie au format CSV:

```
bench_kompress --benchmark_format=csv
```

Observez l'effet de la taille de bloc sur la vitesse de compression. En général, quelle variante est la plus rapide? Quelle est la taille de bloc la plus avantageuse? Quelle est le débit de compression maximal?

Avant la remise de votre code, roulez la commande suivante pour générer votre fichier de banc d'essai:

```
make run_benchmark
```

Ceci permet de valider la portion du banc d'essai sur Hopper.

> Le graphique n'est pas corrigé. Il s'agit d'un exercice essentiel à faire pour votre compréhension, donc faites-le.

## Correction sur Hopper

Votre travail sera corrigé sur le système [Hopper](https://hopper.info.uqam.ca). Le système vous permet de former votre équipe pour ce TP. Faire l'archive avec `make dist`, puis envoyer l'archive sur le serveur de correction. Votre note sera attribuée automatiquement. Vous pouvez soumettre votre travail plusieurs fois et la meilleure note sera conservée. D'autres détails et instructions pour l'utilisation de ce système seront fournis.

Barème de correction

 * Implémentation readwrite: 30
 * Implémentation mmap: 50
 * Résultat du banc d'essai de performance : 20
 * Qualité du code (fuite mémoire, gestion d'erreur, etc): pénalité jusqu'à 10 points
 * Total sur 100 points

Le non-respect de la mise en situation pourrait faire en sorte que des tests échouent. Il est inutile de modifier les tests pour les faire passer artificiellement, car un jeu de test privé est utilisé. Se référer aux résultats de test pour le détail des éléments vérifiés et validés.

Bon travail !