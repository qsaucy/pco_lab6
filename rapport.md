## PCO Labo 6



### Étape 1

On a donc tout d’abord implémenté *requestComputation* et *getWork* pour permettre aux calculateurs de travailler. On a donc défini:



```c++
ComputationManager::ComputationManager(int maxQueueSize): MAX_TOLERATED_QUEUE_SIZE(maxQueueSize),notFull(NBR_OPTIONS),notEmpty(NBR_OPTIONS)
{
    id = 0;
    resultId=0;
    for(int i =0;i<NBR_OPTIONS;i++){
        queueBuffer.push_back(std::queue<Request>());
    }
}
```

Ce constructeur qui initialise plusieurs éléments:

- `id` et `resultId` : qui permettent respectivement de connaître garder une trace de l’ID qu’aura le prochain calcul à être initialisé et l’ID du prochain résultat à s’être terminé (servira surtout plus tard)
- `notFull` et `notEmpty` : sont des tableaux de *Condition* qui permettent de définir le nombre de types de calculateurs spécialisés et de leur associer une *Condition* chacun. Ces *Conditions* permettent alors l’attente du buffer dans le cas où il serait vide ou plein
- `queueBuffer` : est un tableau de la taille du nombre de types de calculateurs et contenant des queue pour assurer l’ordre d’arrivée pour chaque *Request* la composant 



```c++
int ComputationManager::requestComputation(Computation c) {
    monitorIn();
    size_t valueOfComputationType = (size_t)c.computationType;
    if (!stopped && 
        queueBuffer.at(valueOfComputationType).size() 
        == MAX_TOLERATED_QUEUE_SIZE){
        wait (notFull.at(valueOfComputationType));
  	}
    
    queueBuffer.at(valueOfComputationType).emplace(Request(c,id));
    int oldId =id;
    id++;
    signal(notEmpty.at(valueOfComputationType));
    monitorOut();

    return oldId;
}
```

On utilise le moniteur pour s’assurer une section protégée. Tout d’abord on vérifie si la queue a atteint sa taille maximale tolérée (qui vaut par défaut 10) pour le type de calcul passé. On le fait en utilisant *valueOfComputationType* comme index pour *queueBuffer*. Ayant lu tout le rapport, on sait qu’à la dernière étape, il va falloir permettre de stopper le buffer, donc on teste aussi la variable *stopped* même si pour le moment elle ne sert pas encore. Si le test est vrai, alors on fait attendre le calcul, sinon on l’ajoute à *queueBuffer* et on incrémente la valeur de *id* pour l’arrivée de la prochaine requête de calcul. Finalement, on signale qu’on peut libérer une place pour *getWork* et on sort de la section protégée et on retourne l’*id* de la requête que l’on a empilé.



```c++
Request ComputationManager::getWork(ComputationType computationType) {

    monitorIn();
    size_t valueOfComputationType = (size_t)computationType;
    if (queueBuffer.at(valueOfComputationType).size()==0){      
    	wait(notEmpty.at(valueOfComputationType));
    }
    
    
    Request r = queueBuffer.at(valueOfComputationType).front();
    queueBuffer.at(valueOfComputationType).pop();
    signal(notFull.at(valueOfComputationType));
    monitorOut();

    return r;
}
```

Encore une fois, on entre en section protégée on récupère le type de calcul qui va devoir commencer à être fait. On teste si un calcul du type obtenu précédemment est déjà en train de travailler. Si c’est le cas, on attend, sinon on peut pop ce fameux calcul de la queue et on l’utilise pour créer une requête pour commencer le calcul. On signale alors qu’une place s’est libérée pour *requestComputation*, on sort de la section protégée et on retourne la requête nouvellement créée.



![](./img/img1.png)



### Étape 2



### Étape 3



### Étape 4



### Conclusion



