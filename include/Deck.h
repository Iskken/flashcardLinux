#ifndef DECK_H
#define DECK_H
#include "Flashcard.h"
#include <string>
#include "../include/CustomException.h"

class Deck {
    std::string deck_title;
    Flashcard *flist; 
    ChoiceCard *clist;
    int number_of_cards_flist;
    int number_of_cards_clist;
    int capacity_flist;
    int capacity_clist; 
public:
    Deck(const std::string &title);
    Deck(const Deck &other);
    std::string getTitle() const{
        return deck_title;
    }
    void setTitle(const std::string &new_title)
    {
        deck_title = new_title;
    }
    void addFlashcard(const Flashcard &f);
    void addChoicecard(const ChoiceCard &c);
    void increaseCapacityFlist(int newCapacity);
    void increaseCapacityClist(int newCapacity);
    void shuffleCards();
    void reviewDeck() const;
    void browseCards();
    ~Deck()
    {
        delete[] flist;
        delete[] clist;
    }
    friend class DatabaseHandler; //to be able to initialize the Decks using database handler
};

#endif