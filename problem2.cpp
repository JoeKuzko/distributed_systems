
#include <iostream>
#include <pthread.h>
#include <string>
using namespace std;

/* Total number of customers. */
int NCUSTOMERS = 0;
int arrival = 0;
int bought_ticket = 0;
/* Global Variables. */
pthread_mutex_t mutex_arrival; // mutex to make sure all customers have arrived


pthread_mutex_t mutex_ticket; // mutex to make sure all tickets are bought
pthread_cond_t cond_ticket;

pthread_mutex_t mutex_cashier; // mutex for making sure cahsier is present

pthread_mutex_t Out = PTHREAD_MUTEX_INITIALIZER;
/* Insure that each output line has exclusive control of
console. */

void println (string s) {
    pthread_mutex_lock (&Out);
    cout << s << endl;
    pthread_mutex_unlock (&Out);
}
class Stadium {
private:
    int maxCustomers;
    int numberSeats;
    int numberSold;
    const static int baseTicket = 678;
    int nextTicket;
    bool cashierPresent;
    public:
    Stadium (int, int);
    void close ();
    void open ();
    int buyTicket (int);
    void enterStadium (int, int);
    bool getCashierPresent ();
};

Stadium::Stadium (int maxC, int numS) {
    maxCustomers = maxC;
    numberSeats = numS;
    numberSold = 0;
    cashierPresent = false;
    nextTicket = baseTicket + 1;
}

void Stadium::close () {
    cout << "Number of tickets sold: " << numberSold << endl;
}

void Stadium::open () {
    printf("stadium is now open!\n");
    cashierPresent = true;
}

int Stadium::buyTicket(int id) {
    int ticket;
    if(getCashierPresent() == true){ // checks for cashier
        pthread_mutex_unlock(&mutex_cashier); // unlocks mutex if cashier is present.
    }
    pthread_mutex_lock(&mutex_cashier); // mutex for waiting if cashier is not here
    numberSold++;
    ticket = baseTicket + numberSold;
    string sOut = "customer " + to_string(id) +
    " bought ticket " + to_string(ticket);
    println(sOut);
    bought_ticket++;
    pthread_mutex_unlock(&mutex_cashier);
    return (ticket);
}

void Stadium::enterStadium (int id, int ticket) {
    numberSeats--;
    string sOut;
    if ( numberSeats < 0 )
    sOut = "customer " + to_string(id) + " not admitted to stadium";
    else
    sOut = "customer " + to_string(id) +
    " admitted to stadium with ticket " + to_string(ticket);
    println (sOut);
    return;
    }
    bool Stadium::getCashierPresent () { return cashierPresent;}

    class Customer {
    private:
    int id;
    Stadium *stadium;
    int ticket;
    public:
    Customer ();
    Customer (int, Stadium*);
    void run ();
};

Customer::Customer () {
    id = 0;
    stadium = NULL;
}

Customer::Customer ( int i, Stadium* s ) {
    id = i;
    stadium = s;
}

void Customer::run () {
    string sOut = "Customer " + to_string(id) + " arrives at the Stadium.";
    println (sOut);
    arrival++;
    if(arrival == NCUSTOMERS){
        printf("all customers have arrived \n");
        pthread_mutex_unlock(&mutex_arrival); // unlocks mutex once all customers arrive
    }
    pthread_mutex_lock(&mutex_arrival); // locks in mutex for current thread unless already locked, then waits for unlock
    ticket = stadium->buyTicket(id);
    pthread_mutex_unlock(&mutex_arrival);// unlocks mutex for next thread
    if(bought_ticket == NCUSTOMERS){
        printf("all customers have bought tickets \n");
        pthread_cond_signal(&cond_ticket); // signals wait condition for the next thread to go in
    }
    pthread_cond_wait(&cond_ticket, &mutex_ticket); // waits for signal after all ticket have been bought
    stadium->enterStadium(id, ticket);
    pthread_cond_signal(&cond_ticket); // sends signal for next thread
    pthread_mutex_unlock(&mutex_ticket); // unlocks mutex for next thread
}

void *callCrun (void *c) {
    Customer *customer = (Customer *) c;
    customer->run();
    return NULL;
}

int main (int argc, const char * argv[]) { // arg 1 number of threads arg 2
    pthread_mutex_init(&mutex_arrival, NULL);

    pthread_mutex_init(&mutex_ticket, NULL);
    pthread_cond_init(&cond_ticket, NULL);

    pthread_mutex_init(&mutex_cashier, NULL);
    pthread_mutex_lock(&mutex_cashier);
    pthread_mutex_lock(&mutex_arrival);
    pthread_mutex_lock(&mutex_ticket);

    int err;
    int num_threads = stoi(argv[1]);
    pthread_t threads[num_threads];
    NCUSTOMERS = num_threads;
    Stadium* stadium = new Stadium (NCUSTOMERS, (NCUSTOMERS/2));
    Customer customers[num_threads];

    for(int i = 0 ; i < num_threads ; i++){
        Customer *customerss = new Customer(i, stadium);
        customers[i] = *(Customer*)customerss;
    }
    for(int i = 0; i < num_threads; i++){
        err = pthread_create ( &threads[i], NULL, callCrun, &customers[i]);
        if (err) { cout << "create of customer "<< i+1 <<" failed.\n"; }

    }
    stadium->open();
    /* Wait for all customers to complete. */

    for( int i = 0; i < num_threads;i++){
        if(pthread_join(threads[i], NULL) !=0 ){
            perror("failed to join thread");
        }
    }

    stadium->close();
    pthread_mutex_destroy(&mutex_arrival);
    pthread_mutex_destroy(&mutex_cashier);
    pthread_mutex_destroy(&mutex_ticket);
    pthread_cond_destroy(&cond_ticket);
    return 0;
}
