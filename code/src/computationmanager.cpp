//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Prénom Nom, Prénom Nom


// A vous de remplir les méthodes, vous pouvez ajouter des attributs ou méthodes pour vous aider
// déclarez les dans ComputationManager.h et définissez les méthodes ici.
// Certaines fonctions ci-dessous ont déjà un peu de code, il est à remplacer, il est là temporairement
// afin de faire attendre les threads appelants et aussi afin que le code compile.

#include "computationmanager.h"
#include "iostream"
#include "algorithm"
#define NBR_OPTIONS 3
ComputationManager::ComputationManager(int maxQueueSize): MAX_TOLERATED_QUEUE_SIZE(maxQueueSize),notFull(NBR_OPTIONS),notEmpty(NBR_OPTIONS)
{
    id=0;
    resultId=0;
    for(int i =0;i<NBR_OPTIONS;i++){
        queueBuffer.push_back(std::queue<Request>());
    }
}

int ComputationManager::requestComputation(Computation c) {
    monitorIn();
    size_t valueOfComputationType = (size_t)c.computationType;
    if (!stopped && queueBuffer.at(valueOfComputationType).size()==MAX_TOLERATED_QUEUE_SIZE){
        wait (notFull.at(valueOfComputationType));
    }



    queueBuffer.at(valueOfComputationType).emplace(Request(c,id));
    int oldId =id;
    id++;
    signal(notEmpty.at(valueOfComputationType));
    monitorOut();

    if(stopped) throwStopException();
    // TODO
    return oldId;

}

void ComputationManager::abortComputation(int id) {
    monitorIn();

    aborted.emplace_back(id);
    if(resultMap.find(id)!=resultMap.end()){
        resultMap.erase(resultMap.find(id));
    }
    signal(notEmptyResult);
    monitorOut();
    // TODO
}

Result ComputationManager::getNextResult() {
    // TODO
    // Replace all of the code below by your code

    // Filled with some code in order to make the thread in the UI wait
    monitorIn();
    while (std::find(aborted.begin(),aborted.end(),resultId)!=aborted.end()) {
        resultId++;

    }

    while((resultMap.size()==0 or resultMap.find(resultId)==resultMap.end())){
        if(stopped) break;
        wait(notEmptyResult);
        while (std::find(aborted.begin(),aborted.end(),resultId)!=aborted.end()) {
            resultId++;
        }
    }

    Result r = resultMap.begin()->second;
    if(!stopped){
    resultId++;
    resultMap.erase(resultMap.begin());
    }
    monitorOut();


    if(stopped) throwStopException();
    //Result r = bufferRequest.front();
    return r;
}

Request ComputationManager::getWork(ComputationType computationType) {
    // TODO
    // Replace all of the code below by your code

    // Filled with arbitrary code in order to make the callers wait
    monitorIn();

    size_t valueOfComputationType = (size_t)computationType;
    bool f =true;
    while(f){
        if (queueBuffer.at(valueOfComputationType).size()==0){
            if(stopped) throwStopException();
            wait(notEmpty.at(valueOfComputationType));

        }
        if ( std::find(aborted.begin(),aborted.end(),queueBuffer.at(valueOfComputationType).front().getId())!=aborted.end())
             queueBuffer.at(valueOfComputationType).pop();
        else
             f =false;
    }
    Request r = queueBuffer.at(valueOfComputationType).front();
    queueBuffer.at(valueOfComputationType).pop();
    signal(notFull.at(valueOfComputationType));
    monitorOut();

    return r;
}

bool ComputationManager::continueWork(int id) {
    if(std::find(aborted.begin(),aborted.end(),id)==aborted.end() && !stopped)
        return true;
    return false;
}

void ComputationManager::provideResult(Result result) {
    monitorIn();
    resultMap.insert(std::pair<int,Result>(result.getId(),result));
    signal(notEmptyResult);
    monitorOut();
    // TODO
}

void ComputationManager::stop() {
    monitorIn();

    stopped = true;

    for(auto it: queueBuffer){
        for(unsigned i = 0; i < notEmpty.size(); ++i){
            for(unsigned j = 0; j < it.size(); ++j){
                signal(notEmpty.at(i));
            }
        }

        for(unsigned i = 0; i < notFull.size(); ++i){
            for(unsigned j = 0; j < it.size(); ++j){
                signal(notFull.at(i));
            }
        }

        for(unsigned i = 0; i < it.size(); ++i){
            signal(notEmptyResult);
        }
    }


    monitorOut();
}
