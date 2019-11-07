

#ifndef CPRIORITYQUEUE_H_
#define CPRIORITYQUEUE_H_

#include <omnetpp.h>

#include <vector>
#include <iostream>

using namespace omnetpp;

class myQueue : public cSimpleModule{
private:
    cQueue queue;                   // objekt, ki predstavlja cakalno vrsto
    std::vector<cMessage *> currentJobs;  // opravila, ki se trenutno procesirajo
    cMessage *endServiceMsg;        // sporocilo, ki ga modul posilja samemu sebi
    int capacity, length;           // capacity = kapaciteta cakalne vrste (parameter), length = trenutna dolzina cakalne vrste
    simtime_t serviceTime;          // cas procesiranja (parameter)
    int c;                          // c = stevilo streznikov (parameter)

public:
    myQueue();
    virtual ~myQueue();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void updateDisplay(int lenQueue, int lenProcessing);
};
#endif /* CPRIORITYQUEUE_H_ */

