#include <sqlite3.h>
#include <iostream>
#include "../include/Deck.h"
#include "../include/DatabaseHandler.h"    
#include <vector>
#include <limits> //to use numeric_limits
#include "../include/CustomException.h"

void listDecks(std::vector<Deck> &Decks)
{
    int option;
    for (int i = 0; i < Decks.size(); i++)
    {
        std::cout << i+1 << "." << Decks[i].getTitle() << "\n";
    }
    try{
        do 
        {
            std::cout << "To choose a deck to review, type in the number of the deck!\n";
            std::cin >> option;
            if (option < 1) //handling cases when user input is not within boundaries
                throw CustomException("The chosen deck index can not be negative!\n");
            if (option > Decks.size())
                throw CustomException("The chosen deck index is within boundaries!\n");
        } while (option < 1 || option > Decks.size());
        std::system("clear");
        Decks[option-1].shuffleCards();
        Decks[option-1].reviewDeck();
        system("clear");
    }
    catch (const CustomException &e)
    {
        std::cerr << e.what();
    }
}

void addDeck(DatabaseHandler &db, std::vector <Deck> &Decks)
{
    std::string title;
    std::cout << "Type the name of the new deck(can not consist of 2 separate words!):";
    try
    {
        std::cin>>title;
        std::cout << std::endl;
        if (db.insertDeck(title.c_str())) //c_str() function converts from string to const char *
        {
            Deck newdeck(title);
            Decks.push_back(newdeck);
            std::cout << "The deck has been added successfully!\n";
        }
        else{
            throw CustomException("Deck insertion is invalid");
        }
    }
    catch(const CustomException& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void addCard(DatabaseHandler &db,std::vector <Deck> &Decks)
{
    int option;
    int deck_id;
    bool validInput = false;
    while (!validInput)
    {
        try
        {
            int deckchoice;
            std::cout << "What type of card would you prefer: 1.Flashcard or 2.ChoiceCard?\n";
            std::cin >> option;
            if (option != 1 && option != 2)
            {
                throw CustomException("The choice should be either 1 or 2!\n");
            }
            switch(option)
            {
                case 1:
                {
                    std::string question;
                    std::string answer;
                    std::cout << "Enter the question:\n";
                    std::cin.ignore(); 
                    std::getline(std::cin, question);
                    std::cout << "Enter the answer:\n";
                    std::getline(std::cin, answer);
                    Flashcard f(question, answer);

                    std::cout << "Choose which deck you would like to store this card in?\n";
                    for (int i = 0; i < Decks.size(); i++)
                    {
                        std::cout << i+1 << "." << Decks[i].getTitle() << "\n";
                    }
                    std::cin >> deckchoice;
                    if (deckchoice < 1) //handling cases when user input is not within boundaries
                       throw CustomException("The chosen deck index can not be negative!");
                    if (deckchoice > Decks.size())
                        throw CustomException("The chosen deck index cannot be greater than " + std::to_string(Decks.size()) + "!");
                    Decks[deckchoice-1].addFlashcard(f);

                    deck_id = db.getDeckId(Decks[deckchoice-1].getTitle().c_str());
                    db.insertFlashCard(deck_id, question.c_str(), answer.c_str());

                    std::cout << "The card has been added succesfully!\n";
                    validInput = true;
                    break;
                }
                case 2: 
                {
                    ChoiceCard newcard;
                    std::string question;
                    std::string choice;
                    char response;
                    std::string correctAnswer;
                    bool addingOptions = true;
                    std::cout << "Enter the question:\n";
                    std::cin.ignore(); 
                    std::getline(std::cin, question);
                    newcard.setqu(question);
                    while (addingOptions)
                    {
                        std::cout << "Enter the choice:\n";
                        std::getline(std::cin, choice);
                        newcard.addChoice(choice);
                        std::cout << "Press 'Y' to continue adding choices or 'N' otherwise\n";
                        std::cin >> response;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        if (tolower(response) != 'y')
                            addingOptions = false;
                        std::system("clear");
                    }
                    do
                    {
                        std::cout << "Enter the correct answer:\n";
                        std::getline(std::cin, correctAnswer);
                    } while (!newcard.checkExistenceClistAnswer(correctAnswer));

                    newcard.setans(correctAnswer);
                    
                    std::cout << "Choose which deck you would like to store this card in?\n";
                    for (int i = 0; i < Decks.size(); i++)
                    {
                        std::cout << i+1 << "." << Decks[i].getTitle() << "\n";
                    }
                    std::cin >> deckchoice;
                    if (deckchoice < 1 || deckchoice > Decks.size()) {
                        throw CustomException("Invalid deck choice. The deck index must be between 1 and " + std::to_string(Decks.size()) + ".");
                        break;
                    }
                    Decks[deckchoice-1].addChoicecard(newcard);

                    deck_id = db.getDeckId(Decks[deckchoice-1].getTitle().c_str());
                    db.insertChoiceCard(deck_id, question.c_str(), newcard.getans().c_str(),newcard.enumChoices());

                    std::cout << "The card has been added succesfully!\n";
                    validInput = true;
                    break;
                }
                default:break;
            }
        }
        catch (const CustomException &e)
        {
            std::cerr << e.what();
        }
    }
}

void deleteDeck(DatabaseHandler &db, std::vector <Deck> &decks)
{
    int option;
    int deckId;
    bool validOption = false;
    std::cout << "Choose which deck you would like to delete:\n";
    for (int i = 0; i < decks.size(); i++)
    {
       std::cout << i+1 << "." << decks[i].getTitle() << "\n";
    }
    std::cin >> option;
    while (!validOption) {
        std::cin >> option;
        if (option >= 1 && option <= decks.size()) {
            validOption = true;
        } else {
            std::cerr << "Invalid option! Please try again.\n";
        }
    }
    const char * deckTitle = decks[option-1].getTitle().c_str();
    deckId = db.getDeckId(deckTitle);
    decks.erase(decks.begin() + (option-1)); //the erase() function should call the destructor of Deck
    try{
        if (!db.deleteDeck(deckId))
            throw CustomException("Deck deletion was unsuccessful");
    }
    catch (const CustomException &e)
    {
        std::cerr << e.what() << "\n";
    }
}

void deleteCard(DatabaseHandler &db, std::vector <Deck> &decks)
{
    int option;
    bool validOption = false;
    int deckId;
    std::cout << "Choose the deck you would like to delete the card in:\n";
    for (int i = 0; i < decks.size(); i++)
    {
        std::cout << i+1 << "." << decks[i].getTitle() << "\n";
    }
    while (!validOption) {
        std::cin >> option;
        if (option >= 1 && option <= decks.size()) {
            validOption = true;
        } else {
            std::cerr << "Invalid option! Please try again.\n";
        }
    } 
    const char * deckTitle = decks[option-1].getTitle().c_str();
    deckId = db.getDeckId(deckTitle);

    std::cout << "The cards in this deck are the following:\n";
    decks[option-1].browseCards();
}

int main() {
    DatabaseHandler db("test.db");
    db.openDb();
    db.initializeDb();
    std::vector <Deck> Decks;
    db.initializeDecks(Decks);
    bool running = true;
    while (running)
    {
        std::cout << "1.Review Deck\n2.Add Deck\n3.Add new card\n4.Remove Deck\n5.Remove card\n0.Exit application\n";
        int option;
        std::cin >> option;
        switch (option)
        {
            case 1:std::system("clear");listDecks(Decks);break;
            case 2:std::system("clear");addDeck(db, Decks); break;
            case 3:std::system("clear");addCard(db, Decks);break;
            case 4:std::system("clear");deleteDeck(db, Decks);break;
            case 5:std::system("clear");deleteCard(db, Decks);break;
            case 0:running = false;break;
        }
    }
    return 0;
}