
#include "cPriorityQueue.h"

Define_Module(myQueue);     // makro, ki registrira modul

myQueue::myQueue() {
    endServiceMsg = NULL;
}

myQueue::~myQueue() {
    currentJobs.clear();
    cancelAndDelete(endServiceMsg);
}

// inicializacijska metoda
void myQueue::initialize()
{
    endServiceMsg = new cMessage("end");    // sporocilo, ki ga bo modul posiljal samemu sebi - modul si ga interpretira kot konec procesiranja trenutnega opravila
    capacity = par("capacity");             // kapaciteta cakalne vrste
    serviceTime = par("serviceTime");       // cas procesiranje
    c = par("c");                           // stevilo streznikov
    queue.setName("queue");                 // opcijsko lahko cakalni vrsti dolocimo ime
    length = 0;                             // na zacetku je cakalna vrsta prazna
    currentJobs.resize(c, nullptr);
}


// metoda, ki se klice ob sprejemu sporocila
// imamo dva tipa sporocil:
//   - sporocila, ki pridejo od zunaj in predstavljajo opravila, ki jih mora M/M/c sistem sprocesirati
//   - sporocila, ki jih modul poslje samemu sebi in pomenijo konec procesiranja trenutnega opravila
void myQueue::handleMessage(cMessage *msg)
{
    // Preveri, če je prispel zahtevek že v currentJobs
    int msg_server_idx = -1;
    for (int i = 0; i < c; i++)
    {
        if (currentJobs[i] != nullptr &&
                currentJobs[i]->getId() == msg->getId())
        {
            msg_server_idx = i;
            break;
        }
    }
    std::cout << msg_server_idx << std::endl;;

    // ali je prislo sporocilo o koncu procesiranja trenutnega opravila?
    if (msg_server_idx != -1)
    {
        send(currentJobs[msg_server_idx], "out");   // trenutno opravilo je obdelano, zato ga posljemo skozi izhodna vrata naprej v omrezje
        if (queue.isEmpty())            // ce je cakalna vrsta prazna, potem v izvajanje ne moremo vzeti nobenega opravila
        {
           currentJobs[msg_server_idx] = nullptr;
        }
        else                        // ce cakalna vrsta ni prazna, v izvajajnje vzamemo naslednje opravilo v cakalni vrsti
        {
            EV << "Cakalni cas:" << simTime() - currentJobs[msg_server_idx]->getTimestamp() << " s";        // v konzolo izpisemo cas, ki ga je opravilo prezivelo v cakalni vrsti

            cMessage *new_job = check_and_cast<cMessage *>(queue.pop());
            currentJobs[msg_server_idx] = new_job;  // novo zahtevo iz vrste vstavimo v streznik, ki je bil sproscen
            scheduleAt( simTime()+serviceTime, new_job);    // zabelezi zakljucek zahteve
            length--;               // cakalna vrsta se je skrajsala za 1
        }
    }
    // ali je skozi vhodna vrata prislo novo opravilo, ki ga mora M/M/c sistem obdelati?
    else
    {
        cMessage *job = msg;

        // v primeru, da je kaki streznik prost, lahko zacnemo procesirat zahtevo
        int empty_idx = -1;
        for (int i = 0; i < c; i++)
        {
            if (currentJobs[i] == nullptr)
            {
                empty_idx = i;
                break;
            }
        }


        if (empty_idx != -1)
        {
            scheduleAt( simTime()+serviceTime, job); // modul poslje sporocilo samemu sebi, ki ga bo obvestilo, da je izvajanja opravila konec
            currentJobs[empty_idx] = job;                                   // v spremenljivko currentJob shranimo prispelo opravilo
        }
        // v primeru, da procesna enota ni prosta, opravilo poskusimo shraniti v cakalno vrsto
        else
        {
            // ali je cakalna vrsta presegla svojo kapaciteto?
            if (capacity >=0 && length >= capacity)
            {
                delete job; // ce v cakalni vrsti ni prostora, opravilo zavrzemo
            }
            else
            {
                // sicer opravilo shranimo v cakalno vrsto
                job->setTimestamp();    // s pomocjo TimeStampa bomo lahko izmerili cas, ki ga je opravilo prezivelo v cakalni vrsti
                queue.insert( job );    // vstavljanje v vrsto
                length++;               // dolzina cakalne vrste se je povecala
            }
        }
    }
    updateDisplay(length, (int) currentJobs.size());      // v graficni vmesnik simulacije izpisemo dolzino cakalne vrste
}

void myQueue::updateDisplay(int lenQueue, int lenProcessing)
{
    char buf[100];
    sprintf(buf, "Q_length: %ld, Processing: %ld", (long) lenQueue, (long) lenProcessing);
    getDisplayString().setTagArg("t",0,buf);
}


