#ifndef __PARALLEL_MODEL_HPP
#define __PARALLEL_MODEL_HPP

#include <vector>

using namespace std;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Klasa opisująca wektor akcji w obrębie danego stanu, czyli zbiór akcji dla agentów.
 * Roboczo - jest to wektor akcji dla każdego agenta z osobna.
 * Docelowo - zakodowany wektor akcji tak, aby jak najprościej można było
 *            porównywać/wyszukiać zbiory.
 * Akcja jest identyfikowana przez liczbę typu int (powinno wystarczyć).
 */
class action_vector : public vector<int> {
public:
    // Metoda do porównań
//    virtual bool equals(action_vector &v);
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Klasa abstrakcji relacji nierozróżnialności.
 * Instancja reprezentuje zbiór stanów bedących w jednej klasie abstrakcji
 * z punktu widzenia relacji nierozróżnialności.
 */
class equivalence_class {
public:
    /* Identyfikator klasy tożsamy z jej numerem w tablicy - ale może się przydać */
    long long int class_id;
    
    /* Wektor identyfikatorów węzłów należących do tej klasy abstracji */
    vector<long long int> members;
    
    /* Dodatkowy licznik potrzebny podczas pracy nad modelem */
    long long int counter;
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Klasa opisująca stan.
 * Najważniejsze elementy z punktu widzenia modelu:
 * - identyfikator stanu czyli liczba 64-bitowa
 * - wektor akcji możliwych do wykonania w tym stanie
 * - skojarzony z nim wektor stanów, do których się przechodzi
 *   po wybraniu danej akcji.
 * Poza tym są atrybuty potrzebne do obchodzenia grafu i szukania
 * strategii.
 */
class ParallelState {
public:
    /* - - - - - - - - - - - - -
     * Informacje opisujące stan 
     */
    
    /* Identyfikator stanu równy pozycji tego stanu w wektorze stanów modelu
     */
    long long int state_id;

    /* Czy stan jest wygrywajacy */
    bool winning;
    
    /* Wektor identyfikatorów klas abstrakcji, do której węzeł przynależy
     * zależnie od agenta.
     * (-1 == węzeł sam stanowi jednoelementową klasę abstrakcji)
     */
    vector<long long int> equivalence_vector;
    
    /* Wektory akcji i stanów. W wektorze akcji dana akcja może wystapić
     * wielokrotnie, ale wszystkie wystąpienia muszą następować po sobie
     * w celu zapewnienia poprawności działania.
     */
    // wektor z akcjami
    vector<action_vector> action_list;
    // i odpowiadającymi im przejściami
    vector<long long int> state_list;
    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Informacje wymagane podczas obchodzenia grafu (na pewno nie wszystkie)
     */
    
    /* Identyfikator wątku, który jest aktualnie odpowiedzialny za ten węzeł.
     * Jeśli węzeł nie jest aktualnie sprawdzany pod kątem przydatnosci
     * dla strategii, wartość ta to -1. W każdym innym przypadku jest to
     * identyfikator wątku, który szuka strategii od tego węzła. */
    long long int strider_id;
    
    /* Identyfikator węzła, z którego dany węzeł został odwiedzony.
     * Wartość ta jest potrzebna przy powrotach.
     */
    long long int previous_node;
    
    /* Jeśli identyfikator wątku nie jest -1, kolejne pole określa akcję,
     * która jest aktualnie analizowana. Akcja ta to numer pozycji w wektorze
     * action_list. Jeśli stan nie jest analizowany, wartość ta jest
     * ustawiana na -1.
     */
    int current_action;

    /* Pole ustawiane na:
     * - 3 gdy istnieje strategia wygrywająca dla tego stanu,
     * - 2 gdy rozpatrzono ścieżki i nie ma strategii wygrywającej,
     * - 1 gdy stan jeszcze jest w obróbce,
     * - 0 gdy stan nie był jeszcze rozpatrywany.
     */
    int search_status;
    
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Klasa opisująca model.
 * Model to w zasadzie zbiór stanów, dla uproszczenia uporządkowany i zapakowany
 * w wektor indeksowany numerem stanu oraz dodatkowy wektor z numerami klas abstrakcji.
 * 
 * Dojdzie jeszcze trochę pól typu semafory. Z punktu widzenia samego modelu jest to 
 * zupełnie nieistotne - będzie potrzebne do przeszukiwania.
 */
class ParallelModel {
public:
    /* wektor wszystkich stanów
     * Pozycja w wektorze jest tożsama z identyfikatorem węzła.
     */
    vector<ParallelState> states;
    
    /* Wektor klas abstrakcji*/
    vector<equivalence_class> abstraction_classes;
    
    /* Metoda sprawdzajaca, czy w grafie nie ma ścieżki od pierwszego węzła 
     * do drugiego etykietowanej konkretnym identyfikatorem węzła.
     */
//    virtual bool path_search(long long int strider_id, long long int first, long long int last);

};

#endif // __PARALLEL_MODEL_HPP
