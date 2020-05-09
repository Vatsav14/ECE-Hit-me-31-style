/************************************************************************************************************************

PROJECT NAME    : Hit Me, 31 Style! (ECE 175: Final Project)
CLASS           : ECE 175 - Computer Programming for Engineering Applications (Spring 2020)
AUTHORS         : Shambhavi Singh and Vatsav Sethupathi
DATE            : 3rd May 2020
IDE USED        : Microsoft Visual Studio Enterprise

BRIEF DESCRIPTION:
    This program simulates a variation of the popular card game Blackjack, called 31.
    The objective of the game is to get a score as close to 31 as possible. The player
    bets a certain amount based on the cards that the dealer has after one card is dealed
    to both. Once either one of the two get a total of 31 or as close to 31 as possible,
    the round resets. This continues until either the player wishes to withdraw or when
    the player runs out of money.

RULES:
    - The player starts out with $100 and can bet any amount between $20 and $200
    - One card each is dealt to the dealer (face-down) and the player (face-up).
    - Once the cards are dealt, the player makes a bet.

    - Conditions for dealer to win:
        - The dealer hits 14 points and the player doesn't hit 31 points
        - The dealer hits 31 points, he wins instantly
        - The dealer hits a value between 27 and 31 and the player goes above 31 points
        - The dealer scores more than the player but less than 31

    - Conditions for the player to win:
        - The dealer goes above 31 points
        - The player hits 14 points while the dealer doesn't
        - The player hits 31 points
        - The player scores more than the dealer, but less than 31

    - If the dealer doesn't bust or hit 14 or 31, the player has an option to hit or stand
    - If the player stands, then all cards are instantly revealed and the person with a 
      score wins the round

DATA STRUCTURES USED: 
    The only data structures used throughout the entire project are linked lists

EXTRA CREDIT ATTEMPTED:
    - STATISTICS : Displaying the statistics of the games played at the end of each round
    - GRAPHICS   : Added a graphical element to the game by making the cards using ASCII art

*************************************************************************************************************************/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct card_s
{
    char suit[9];
    char displayChar[3];
    int value;
    struct card_s* next;
    struct card_s* prev;
} card;

void printIntro();
int getBet(int wallet);
int allButOne(card* head);
int getNewValue(int n);
void repurposeValue(card* head);
void mainGame(card** hdl, card** tl, card* head, card* dealer_head, card* dealer_tail, card* player_head, card* player_tail);
int getLength(card* head);
card* goToNode(card* head, int i);
char* getSuitFromNumber(int i);
char* getCardNumber(int i);
void createDefaultDeck(card** head, card** tail);
void inputFromFile(card** head, card** tail, FILE* inp);
void insertAtHead(card* toInsert, card** head, card** tail);
void shuffleDeck(card* head);
void printHand(card* head, int length);
int decideAceDealer(card* head, int s);
int decideAcePlayer(card* head, int s, int ace_val);
void moneyCalculations(int* wallet, int bet, int winLoss);
int hasAce(card* head);
int getLengthRow(card* head, int i);

/*
PURPOSE:
    This is the main function for the program. It serves as the point of control for the entire program

PARAMETERS / INPUTS: 
    No parameters. Takes in the choice to either get the shuffled cards from a file or randomly generated

RETURN VALUES / OUTPUT:
    No visible feedback in this part of the game. Passes control to the function containing the mechanics of the game.
*/
int main(void)
{
    printIntro();

    card* head = NULL, * tail = NULL, * dealer_head = NULL, * dealer_tail = NULL, * player_head = NULL, * player_tail = NULL;
    int ch;

    // Ask the user whether they want to use a pre determined card order or shuffle on the spot
    do {
        printf("Choose from the below options\n1. Shuffle the deck\n2. Load the cards from a file\nEnter 1 or 2: ");
        scanf("%d", &ch);
    } while (!(ch == 1 || ch == 2));

    // Case when the user wants a random shuffle
    if (ch == 1) {
        createDefaultDeck(&head, &tail);
        printf("\n\n");
        srand((int)time(0));
        for (int i = 0; i < 10000; i++) // Swap two random cards 10000 times
            shuffleDeck(head);
    }

    // Case when the user wants to use a pre-determined card sequence
    else {
        char file_name[1000];

    label:

        printf("Enter the name of the file: ");

        do {
            fgets(file_name, 1000, stdin);
        } while (file_name[0] == '\n' || file_name[0] == ' ');

        file_name[strlen(file_name) - 1] = '\0';

        FILE* FileIn = fopen(file_name, "r");

        if (FileIn == NULL) {
            printf("Unable to open file. Please enter a file that exists.\n");
            goto label; // If the filename is invalid, re-accept the input
        }

        inputFromFile(&head, &tail, FileIn);
    }

    repurposeValue(head); // Changes the values of the cards to make them as per their points

    mainGame(&head, &tail, head, dealer_head, dealer_tail, player_head, player_tail);

    return 0;
}

/*
PURPOSE:
    This function changes the value attribute of each card to represent the number of points they depict

PARAMETERS / INPUTS:
    - The head of the card deck

RETURN VALUES / OUTPUT:
    - Just changes the value attributes of the cards
*/
void repurposeValue(card* head) {
    while (head != NULL) {
        int newVal = getNewValue(head->value);
        head->value = newVal; // Change the value to the number of points
        head = head->next;
    }
}

/*
PURPOSE:
    This function gets the number of points of the card as per its original value

PARAMETERS / INPUTS:
    - The original value of the card

RETURN VALUES / OUTPUT:
    - Returns the value converted to the number of points that the value holds
*/
int getNewValue(int n) {
    switch (n) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9: return n; // Fallthrough in the switch case
    case 10:
    case 11:
    case 12:
    case 13: return 10;
    }
}

/*
PURPOSE:
    This function deletes a card from a hand of cards and frees the memory that it occupies

PARAMETERS / INPUTS:
    - A reference to the head of the linked list
    - Double pointers to the head and tail of the linked list
RETURN VALUES / OUTPUT:
    - Just deletes a card from the hand of cards and frees the memory
*/
card* deleteCard(card* p, card** hl, card** hr) {

    if (p == *hl)         // if deleting the first element
        *hl = p->next;     // update the left headp
    else
        p->prev->next = p->next;

    if (p == *hr)
        *hr = p->prev;     // if deleting the last element
    else
        p->next->prev = p->prev;

    card* new_p = p->next;
    free(p); // free memory
    return new_p;
}

/*
PURPOSE:
    This function simply returns the position of the node to which we wish to travel to

PARAMETERS / INPUTS:
    - The head of the linked list 
    - The index which we want to travel to

RETURN VALUES / OUTPUT:
    - The position of the node which we want to travel to in the list
    
*/
card* goToNode(card* head, int i) {
    while (i > 0) {
        head = head->next;
        i--;
    }

    return head;
}

/*
PURPOSE:
    This function shuffles the deck of cards that is formed by swapping two random cards

PARAMETERS / INPUTS:
    - The head of the deck of cards

RETURN VALUES / OUTPUT:
    - Doesn't return any value, simply swaps two cards in the list
*/
void shuffleDeck(card* head) {
    int n1 = rand() % 104, n2 = rand() % 104;
    if (n1 != n2) {
        char string_temp[9];
        int value_temp;

        // Swap the contents of the cards and not the positions in the linked list
        card* temp1 = goToNode(head, n1);
        card* temp2 = goToNode(head, n2);
        strcpy(string_temp, temp1->suit);
        strcpy(temp1->suit, temp2->suit);
        strcpy(temp2->suit, string_temp);

        strcpy(string_temp, temp1->displayChar);
        string_temp[strlen(string_temp)] = '\0';
        strcpy(temp1->displayChar, temp2->displayChar);
        strcpy(temp2->displayChar, string_temp);

        value_temp = temp1->value;
        temp1->value = temp2->value;
        temp2->value = value_temp;
    }
}

/*
PURPOSE:
    This function creates a deck of 104 cards

PARAMETERS / INPUTS:
    - The head and tail double pointers of the empty deck of cards

RETURN VALUES / OUTPUT:
    - Populates the empty deck of cards with 104 freshly created cards
*/
void createDefaultDeck(card** head, card** tail) {
    card* toAdd = (card*)malloc(sizeof(card));

    for (int i = 0; i < 8; i++) { // Two for each suit
        for (int j = 1; j < 14; j++) {
            // Populate the data members of the newly created cards
            strcpy(toAdd->suit, getSuitFromNumber(i % 4)); 
            toAdd->value = j;
            strcpy(toAdd->displayChar, getCardNumber(j));
            insertAtHead(toAdd, head, tail);
        }
    }
}

/*
PURPOSE:
    This function populates the deck of cards using a file provided by the user

PARAMETERS / INPUTS:
    - The head and tail double pointers of the deck of cards
    - A pointer to the file to be used to read the input

RETURN VALUES / OUTPUT:
    - Simply populates the empty deck of cards
*/
void inputFromFile(card** head, card** tail, FILE* inp) {
    card* toAdd = (card*)malloc(sizeof(card));

    while (!(feof(inp))) {
        // Populate the data members of the cards using the user's input file
        fscanf(inp, "%d ", &toAdd->value);
        strcpy(toAdd->displayChar, getCardNumber(toAdd->value));
        
        do {
            fgets(toAdd->suit, 9, inp);
        } while (toAdd->suit[0] == '\n' || toAdd->suit[0] == ' ');

        strcpy(toAdd->suit, getSuitFromNumber((int)(toAdd->suit[0]) - 48));
        insertAtHead(toAdd, head, tail);
    }
}

/*
PURPOSE:
    This method gets the length of a linked list

PARAMETERS / INPUTS:
    - The head pointer of a linked list

RETURN VALUES / OUTPUT:
    - The length of the given linked list
*/
int getLength(card* head)
{
    int i = 0;
    while (head != NULL) {
        i++;
        head = head->next;
    }

    // If the list is empty, just return the length as 13
    if (i / 13 == 0) {
        i = 13;
    }
    return i;
}

/*
PURPOSE:
    This method gets the length of any required row given the row number

PARAMETERS / INPUTS:
    - The head pointer of a deck/hand of cards
    - The row number whose length is to be found

RETURN VALUES / OUTPUT:
    - The length of the row to be calculated
*/
int getLengthRow(card* head, int i) {
    int l = 0;
    head = goToNode(head, i);

    while (head != NULL && l < 13) {
        l++;
        head = head->next;
    }

    return l;
}

/*
PURPOSE:
    This function gets the display character (the character to be displayed on the screen) 
    for a given card number

PARAMETERS / INPUTS:
    - The number whose display character is to be found

RETURN VALUES / OUTPUT:
    - A character representing the display character of a given number
*/
char* getCardNumber(int i) {
    switch (i) {
    case 1: return "A";
    case 2: return "2";
    case 3: return "3";
    case 4: return "4";
    case 5: return "5";
    case 6: return "6";
    case 7: return "7";
    case 8: return "8";
    case 9: return "9";
    case 10: return "10";
    case 11: return "J";
    case 12: return "Q";
    case 13: return "K";
    }
}

/*
PURPOSE:
    This function returns a suit corresponding to a specific number

PARAMETERS / INPUTS:
    - The number whose suit character is to be found

RETURN VALUES / OUTPUT:
    - 0 corresponds to spade
    - 1 corresponds to club
    - 2 corresponds to diamond
    - 3 corresponds to heart
*/
char* getSuitFromNumber(int i) {
    switch (i) {
    case 0: return "\x03";
    case 1: return "\x04";
    case 2: return "\x05";
    case 3: return "\x06";
    }
}

/*
PURPOSE:
    This function accepts a card node and adds it to a linked list

PARAMETERS / INPUTS:
    - A reference to the node to be inserted
    - Double pointers to the head and the tail of the linked list to insert into

RETURN VALUES / OUTPUT:
    - 
*/
void insertAtHead(card* temp, card** head, card** tail) {
    card* toInsert = NULL;
    toInsert = (card*)malloc(sizeof(card));
    strcpy(toInsert->suit, temp->suit);
    strcpy(toInsert->displayChar, temp->displayChar);
    toInsert->value = temp->value;

    if (*head == NULL) { 
        // In this case, the deck of cards is empty
        *head = toInsert;
        *tail = toInsert;
        toInsert->next = NULL;
        toInsert->prev = NULL;
    }

    // The deck of cards is not empty
    else {
        (*tail)->next = toInsert;
        toInsert->prev = (*tail);
        *tail = toInsert;
        toInsert->next = NULL;
    }
}

/*
PURPOSE:
    This method calculates the length of the dealer's hand to print all but one card if he hits 27 or more

PARAMETERS / INPUTS:
    - The head pointer of the player's hand list

RETURN VALUES / OUTPUT:
    - One less than the length of the dealer's hand to print all but one card if he hits 27 or more
*/

int allButOne(card* head)
{
    if (getLength(head) > 13)
        return getLength(head) - 1;
    else
        return getLengthRow(head, 0) - 1;
}


/*
PURPOSE:
    This function is the central backbone of the game and controls the entire flow of the game
    and all its mechanics

PARAMETERS / INPUTS:
    - Head and tail double pointers of the deck of cards
    - A reference to the head of the deck of cards
    - References to the head and tail of the dealer's hand
    - References to the head and tail of the player's hand

RETURN VALUES / OUTPUT:
    - Since this function controls the entire game, it displays the card game, game statistics and
    other options pertaining to hit or miss selection and the results of each of the rounds, etc
*/
void mainGame(card** hdl, card** tl, card* head, card* dealer_head, card* dealer_tail, card* player_head, card* player_tail) {
    int wallet = 100, bet, sum_player = 0, sum_dealer = 0, ace_val = 1, count_games = 0;
    float player_win = 0.0, dealer_win = 0.0, hands_27 = 0, player_14 = 0;
    char choice;
    char quit = 'n';
    card* new_hdl = NULL;
    card* new_tl = NULL;

BEGINNING:
    // This is where each round of the game ends
    // Once a round is completed, they are directed back to this section of the program
    // This section handles crucial mechanics like whether the user wishes to continue

    // Increments the number of games in which the player or the dealer hits 27 points
    // This is for the calculation of game statistics
    if (sum_dealer == 27)
        hands_27++;

    if (sum_player == 27)
        hands_27++;

    choice = 'h';

    if (dealer_head != NULL) { // Makes sure that this option isn't shown in the first round

         // The player can only continue if they have the ability to bet more than the minimun amount
        if (wallet >= 20) {
            do {
                printf("Do you want to continue (y or n): ");
                scanf(" %c", &quit);
            } while (quit != 'y' && quit != 'n');
        }

        // In case the user wishes to end the game
        if (quit == 'n'){

        STATS:

            //      ****************************************************************
            //      ----                      EXTRA CREDIT                      ----
            //      ****************************************************************
            //
            // This is the part of the function which is executed when the entire game is completed
            // The code is redirected here when the user wants to quit or has run out of money
            // It displays the final statistics of all the games played

            printf("\n--------------- Final Game Statistics --------------");
            printf("\nThe number of rounds played: %d\n", count_games);
            printf("Winning percentage of dealer: %.2f percent", (dealer_win / count_games) * 100);
            printf("\nWinning percentage of player: %.2f percent", (player_win / count_games) * 100);
            printf("\nPercentage of hands being equal to 27: %.2f percent", (hands_27 / (count_games * 2.0)) * 100);
            printf("\nPercentage of hands being equal to 14 when the player wins: %.2f percent \n", (player_14 / count_games) * 100);
            goto END_CREDITS; // Pass control to the end-credits of the game
        }
    }
    // When the numbers of cards in the deck goes below 30 cards, the entire deck
    // is re-initialized and shuffled

    if (getLength(head) < 30) {
        free(head);
        head = NULL;
        card* tail = NULL;

        // Setting the values for the double pointers for the head and tail
        hdl = &head;
        tl = &tail;

        createDefaultDeck(hdl, tl);
        srand((int)time(0));

        for (int i = 0; i < 10000; i++) // Run the shuffle algorithm 10000 times
            shuffleDeck(head);

        new_hdl = NULL;
        new_tl = NULL;

    }

    dealer_head = NULL;
    dealer_tail = NULL;
    player_head = NULL;
    player_tail = NULL;
    sum_player = head->value;
    sum_dealer = head->next->value;

    // The player can only proceed further if they have enough money to bet atleast the minimum amount
    if (wallet >= 20) {

        if (quit != 'n') {

            //      ****************************************************************
            //      ----                      EXTRA CREDIT                      ----
            //      ****************************************************************

            // If the user wishes to continue playing the next round, display the game statistics up to this point
            printf("\n------------ Current Game Statistics ------------");
            printf("\nThe number of rounds played: %d\n", count_games);
            printf("Winning percentage of dealer: %.2f percent", (dealer_win / count_games) * 100);
            printf("\nWinning percentage of player: %.2f percent", (player_win / count_games) * 100);
            printf("\nPercentage of hands being equal to 27: %.2f percent", (hands_27 / (count_games * 2.0)) * 100);
            printf("\nPercentage of hands being equal to 14 when the player wins: %.2f percent\n", (player_14 / count_games) * 100);
            printf("-------------------------------------------------\n");
            printf("\nNext Round: \n\n");

        }

        count_games++; // Increment the number of games played by one

        // Deal card to the player first
        insertAtHead(head, &player_head, &player_tail);
        insertAtHead(head, &new_hdl, &new_tl);
        head = deleteCard(head, hdl, tl);

        // Then deal a card to the player
        insertAtHead(head, &dealer_head, &dealer_tail);
        insertAtHead(head, &new_hdl, &new_tl);
        head = deleteCard(head, hdl, tl);

        // The first move of the round
        // Show the player's card face up and the dealer's card face down
        printf("Dealer's: \n");
        printHand(dealer_head, allButOne(dealer_head));
        printf("Player's: \n");
        printHand(player_head, getLength(player_head));
        printf("\n");

        // Get the player's bet after they see the card they have
        bet = getBet(wallet);
        printf("\n");

        // Now start the dealer's moves
        while (sum_dealer != 14 && sum_dealer < 27) {

            // As long as the points aren't 14 or over 27, keep drawing cards
            insertAtHead(head, &dealer_head, &dealer_tail);
            sum_dealer += head->value;
            insertAtHead(head, &new_hdl, &new_tl);
            head = deleteCard(head, hdl, tl);

            // The following code is to intelligently help the dealer determine the value
            // of the ace card that would benefit him
            if (sum_dealer < 27) 
                sum_dealer = decideAceDealer(dealer_head, sum_dealer);
        }

        // If the dealer hits exactly 14 points, a special case is executed in which the
        // player HAS to hit 31 points to win, else they lose
        if (sum_dealer == 14) {

            printf("Dealer's: \n");
            printHand(dealer_head, getLength(dealer_head));
            printf("Player's: \n");
            printHand(player_head, getLength(player_head));
            printf("\n\nDealer hit 14. You have to hit 31 to win.\n\n");

            // As long as the player doesn't cross 31
            while (sum_player < 31) {
                goto DRAW; // Go to the part of the function in which the player needs to draw cards
            }

            // If this condition is true, the player wins
            if (sum_player == 31)
                // Go to the part of the function which handles the sequence of events in the case of a player's win
                goto WIN; 

            // In this case, the player has crossed 31 and has lost the round
            else
                // Go to the part of the function which handles the sequence of events in the case of a player's loss
                goto BUST;
        }

        // If the dealer hits 31 points, he wins instantly
        else if (sum_dealer == 31) {

            dealer_win++; // Increment the number of wins of the dealer by one

            printf("Dealer's: \n");
            printHand(dealer_head, getLength(dealer_head));
            printf("Player's: \n");
            printHand(player_head, getLength(player_head));

            moneyCalculations(&wallet, bet, 2); // Calculate the money in the player's wallet after the round
            printf("\n\nDealer hits 31! Dealer wins. Player total $%d\n\n", wallet);

            goto BEGINNING; // Go back to the start to begin a new round
        }

        // If the dealer crosses 31 points, he instantly loses and the player wins
        else if (sum_dealer > 31) {

            player_win++; // Increment the number of wins of the player by one

            printf("Dealer's: \n");
            printHand(dealer_head, getLength(dealer_head));
            printf("Player's: \n");
            printHand(player_head, getLength(player_head));

            moneyCalculations(&wallet, bet, 1); // Calculate the money in the player's wallet after the round
            printf("\n\nDealer busts! Player wins $%d. Player total $%d\n\n", bet * 2, wallet);

            goto BEGINNING; // Go back to the start to begin a new round
        }

        else {
            printf("Dealer's: \n");
            printHand(dealer_head, allButOne(dealer_head));
            printf("Player's: \n");
            printHand(player_head, getLength(player_head));
            printf("\n");
        }

    PLAYER:
        // This is the part of the code that executes once the dealer completes his moves
        // This is only executed if the dealer hits 14 points or if he is between 27 and 31 points

        // If the dealer hasn't hit 14 points and the player does, then the player wins
        if (sum_player == 14 && sum_dealer != 14) {
            player_14++;
            player_win++; // Increment the number of wins of the player by one

            printf("Dealer's: \n");
            printHand(dealer_head, getLength(dealer_head));
            printf("Player's: \n");
            printHand(player_head, getLength(player_head));

            moneyCalculations(&wallet, bet, 1); // Calculate the amount of money left after the round
            printf("\n\nPlayer hits 14! Player wins $%d. Player total $%d\n\n", bet * 2, wallet);
            goto BEGINNING; // Go back to the start to begin a new round
        }

        // If the player hits 31 points, then they win instantly
        else if (sum_player == 31) {
        WIN:
            player_win++; // Increment player wins by one

            printf("Dealer's: \n");
            printHand(dealer_head, getLength(dealer_head));
            printf("Player's: \n");
            printHand(player_head, getLength(player_head));

            moneyCalculations(&wallet, bet, 1); // Calculate the amount of money left after the round
            printf("\n\nPlayer hits 31! Player wins $%d. Player total $%d\n\n", bet * 2, wallet);
            goto BEGINNING; // Go back to the start to begin a new round
        }

        // If the player goes above 31 points, then they lose
        else if (sum_player > 31) {
        BUST:
            dealer_win++; // Increment the dealer wins by one

            printf("\n\nDealer's: \n");
            printHand(dealer_head, getLength(dealer_head));
            printf("Player's: \n");
            printHand(player_head, getLength(player_head));
            printf("\n");

            moneyCalculations(&wallet, bet, 2); // Calculate the amount of money left after the round
            printf("\nBust! Dealer wins. Player total $%d\n\n", wallet);
            goto BEGINNING; // Go back to the start to begin a new round
        }

        // In case none of the above conditions are satisfied, the player needs to decide whether
        // they want to hit or stand. The following piece of code handles this sequence of events
        else {

            // If the player has an ace, ask them what they want the value of the ace to be
            if (allButOne(player_head) == 0 && hasAce(player_head)) {
                printf("Enter the value of ace: ");
                scanf("%d", &ace_val);
            }

            // Ask the user if they want to hit or stand
            if (sum_dealer != 14) {
                printf("\nHit(h) or Stand(s): ");
                scanf(" %c", &choice);
                printf("\n");
            }

            if (choice == 'h') {
            DRAW:
                // This is the part of the code in which the player draws a card from the deck
                insertAtHead(head, &player_head, &player_tail);
                sum_player += head->value;

                // This code is to display the dealer's hand in two cases 
                // (1) When the dealer has 14 points and 
                // (2) When the dealer doesn't have 14 points

                if (sum_dealer != 14) {
                    printf("Dealer's: \n");
                    printHand(dealer_head, allButOne(dealer_head));
                }

                else if (sum_dealer == 14) {
                    printf("Dealer's: \n");
                    printHand(dealer_head, getLength(dealer_head));
                }

                // Display the player's hand
                printf("Player's: \n");
                printHand(player_head, getLength(player_head));

                // If there is an ace in the player's hand, ask them the point value for the ace
                if (hasAce(player_head) == 1) {
                    do {
                        printf("Enter the value of ace: ");
                        scanf("%d", &ace_val);
                        printf("\n");
                    } while (ace_val != 1 && ace_val != 11);

                    sum_player = decideAcePlayer(player_head, sum_player, ace_val);
                }

                insertAtHead(head, &new_hdl, &new_tl);
                head = deleteCard(head, hdl, tl);

                goto PLAYER; // Go back to the part of the code in which the user picks a card
            }

            // In this case, the user is satisfied with their total and wishes to stand
            else if (choice == 's')
                goto EVALUATE; // Go to the part of the code where the points of the player and dealer are evaluated
        }

    EVALUATE:
        // This is the part of the function which compares the hands of both the player and the dealer
        // We are guaranteed that the sum of their points are both below 31
        // Whoever has the higher sum wins the round
        printf("Dealer's: \n");
        printHand(dealer_head, getLength(dealer_head));
        printf("Player's: \n");
        printHand(player_head, getLength(player_head));

        // If the dealer scores more than the player, the dealer wins
        if (sum_dealer > sum_player) {
            dealer_win++;
            moneyCalculations(&wallet, bet, 2); // Calculate the amount left in the player's wallet
            printf("\n\nDealer: %d, Player: %d. Dealer wins. Player total: $%d\n\n", sum_dealer, sum_player, wallet);

            goto BEGINNING; // Go back to the start of the game and begin a new round
        }

        // In case the player has a higher score than the dealer, the player wins
        else if (sum_player > sum_dealer) {
            player_win++;
            moneyCalculations(&wallet, bet, 1); // Calculate the amount left in the player's wallet
            printf("\n\nDealer: %d, Player: %d. Player wins $%d. Player total: $%d\n\n", sum_dealer, sum_player, bet * 2, wallet);

            goto BEGINNING; // Go back to the start of the game and begin a new round
        }
        
        // In this case, the player and the dealer score the same amount and the bet is pushed
        else {
            printf("\n\nNo result. The bet was pushed. Player total: $%d\n\n", wallet);
            // There is no need to re-calculate the amount in the player's wallet since no changes are made

            goto BEGINNING; // Go back to the start of the game and begin a new round
        }

    }

    // Once this point is reached, the player has run out of money and the game ends
    // Now we need to ask the player if they wish to play the game once again
    printf("\nGame over, you have run out of money!\n\nDo you wish to play again? (y/n) ");

    scanf(" %c", &choice); // Take input to play again

    // If the user wishes to play again, redirect them to the start of the program where the game begins again
    if (choice == 'y') {
        printf("\n\n\n");
        main();
    }

    // In case the user doesn't wish to play again, roll the end credits and terminate the program
    else {
        // First display the final statistics of the game
        goto STATS;

    END_CREDITS:
        // End credits of the game
        printf("\nThank you for playing Hit Me, 31 style! Hope you enjoyed!\n\n\n");
    }
}

/*
PURPOSE:
    This function checks if a given hand of cards does or doesn't contain an ace in it

PARAMETERS / INPUTS:
    - The head of a hand of cards that we need to check

RETURN VALUES / OUTPUT:
    - 1 is an ace is present
    - 0 if there aren't any aces in the hand
*/
int hasAce(card* head) {
    while (head != NULL) {

        if (strcmp(head->displayChar, "A") == 0)
            return 1;

        head = head->next;
    }
    return 0;
}

/*
PURPOSE:
    This function calculates the amount in the player's wallet after a round ends

PARAMETERS / INPUTS:
    - A reference to the player's wallet
    - The amount that the player had bet for the round
    - An integer value denoting whether the player won or lost the round

RETURN VALUES / OUTPUT:
    - This function only changes the amount in the player's wallet
*/
void moneyCalculations(int* wallet, int bet, int winLoss) {
    switch (winLoss) {
    case 1: // This is a win for the player
        (*wallet) += bet * 2;
        break;

    case 2: // This is a win for the dealer
        (*wallet) -= bet;
        break;
    }
}

/*
PURPOSE:
    This function accepts the bet that the player wants to enter and displays the appropriate error messages
    in case the bet amount entered is invalic

PARAMETERS / INPUTS:
    - The wallet of the player to check if the amount entered can be paid by the player

RETURN VALUES / OUTPUT:
    - The appropriate error messages in case the bet amount entered is invalid
    - The amount that the player wishes to bet
*/
int getBet(int wallet) {

    int bet;
    printf("\nPlace your bet: ");
    scanf("%d", &bet);

    // Handles the various cases when the bet place is not valid
    // If it is invalid, the function is called again
    if (bet < 20) {
        printf("Bet placed is too small. The minimum value you can bet is $20. Try again.\n");
        return getBet(wallet);
    }

    else if (bet > 200) {
        printf("Bet placed is too large. The maximum value you can bet is $200. Try again.\n");
        return getBet(wallet);
    }

    else if (bet > wallet) {
        printf("The amount you wish to bet is greater than what you have left. Try again.\n");
        return getBet(wallet);
    }

    return bet;
}

/*
PURPOSE:
    This function intelligently assigns the point value of the ace card in the dealer's hand
    to benefit the dealer as much as possible

PARAMETERS / INPUTS:
    - The head of the dealer's hand
    - The sum of the dealer's hand

RETURN VALUES / OUTPUT:
    - The newly calculated value of the dealer's hand
*/
int decideAceDealer(card* head, int s) {
    int temp_sum = s;
    // Calculates the sum when all the values of ace in the hand is changed to 11 
    // The default value of the ace is 1
    while (head != NULL) {
        if (head->value == 1) {
            temp_sum += 10;
        }
        head = head->next;
    }
    if (temp_sum == 31 || temp_sum == 14) {

        while (head != NULL) {
            if (head->value == 1) {
                head->value = 11;
            }
            head = head->prev;
        }
        return temp_sum; // Returns the new sum if the ace is changed
    }

    // This means that the ace hasn't changed and the original value is returned
    return s; 
}

/*
PURPOSE:
    This method changes the value of ace at every play if the player requests it

PARAMETERS / INPUTS:
    - The head pointer of the player's hand list
    - The sum of all the values in the nodes of the dealer's hand

RETURN VALUES / OUTPUT:
    - The changed or not changed sum all the values in the nodes of the player's hand
*/

int decideAcePlayer(card* head, int s, int ace_val) {

    while (head != NULL) {

        // Checks if value of ace is 1 to change it to 11 incase the player requests it
        if (ace_val == 11 && head->value == 1) {
            head->value = 11;
            s += 10;
        }
        else if (ace_val == 1 && head->value == 11 && strcmp(head->displayChar, "J") != 0) {
            head->value = 1;
            s -= 10;
        }
        head = head->next;
    }

    return s; // Returns the changes or unchanged sum of the player's hand
}

/*
PURPOSE:
    This function prints the hand of a player or the dealer.
    This handles the cases when both the card is to be face-up or face-down
    ** This is done under extra credit **

PARAMETERS / INPUTS:
    - A reference to the head of the list of cards
    - The length of the list of cards

RETURN VALUES / OUTPUT:
    - Face-Up card (N: Card number, S: Card suit)
         ----
        |NS  |
        |    |
        |    |
        |  NS|
         ----
    - Face-Down card
         ----
        |    |
        |    |
        |    |
        |    |
         ----
*/


void printHand(card* head, int length) {

    //      ****************************************************************
    //      ----                      EXTRA CREDIT                      ----
    //      ****************************************************************

    card* temp = head;

    for (int i = 0; i < getLength(head) / 13; i++) {

        // Prints out the top element of the card
        for (int j = 0; j < getLengthRow(head, i * 13); j++)
            printf(" ----  ");
        printf("\n");

        // Print out the value and the suit of the card
        for (int j = 0; j < getLengthRow(head, i * 13); j++) {

            // In case the card to be displayed is a face down card
            if (length + 1 == getLengthRow(head, i * 13) && i == (getLength(head) / 13) - 1 && j == getLengthRow(head, i * 13) - 1)
                printf("|    | ");

            // If the card to be displayed has a 2 digit number, ex. 10
            else if (strcmp(temp->displayChar, "10") != 0)
                printf("|%s%s  | ", temp->displayChar, temp->suit);
            
            // When the card to be displayed has a single digit number
            else
                printf("|%s%s | ", temp->displayChar, temp->suit);

            temp = temp->next;
        }
        printf("\n");

        // The next three loops just print out the middle of the card
        for (int j = 0; j < getLengthRow(head, i * 13); j++)
            printf("|    | ");
        printf("\n");

        for (int j = 0; j < getLengthRow(head, i * 13); j++)
            printf("|    | ");
        printf("\n");

        for (int j = 0; j < getLengthRow(head, i * 13); j++)
            printf("|    | ");
        printf("\n");

        temp = goToNode(head, i * 13);

        // Prints the bottom half of the card which includes the number and the suit of the card
        for (int j = 0; j < getLengthRow(head, i * 13); j++) {

            // This is in case the card to be displayed is face down
            if (length + 1 == getLengthRow(head, i * 13) && i == (getLength(head) / 13) - 1 && j == getLengthRow(head, i * 13) - 1)
                printf("|    | ");

            // If the card to be displayed has a 2 digit number, ex. 10
            else if (strcmp(temp->displayChar, "10") != 0)
                printf("|  %s%s| ", temp->displayChar, temp->suit);

            // When the card to be displayed has a single digit number
            else
                printf("| %s%s| ", temp->displayChar, temp->suit);

            temp = temp->next;
        }
        printf("\n");

        // Prints the last row of the card
        for (int j = 0; j < getLengthRow(head, i * 13); j++)
            printf(" ----  ");

        printf("\n\n");
    }
}

/*
PURPOSE: 
    This function simply prints out the banner displayed at the start of the game

PARAMETERS / INPUTS: 
    No parameters

RETURN VALUES / OUTPUT: 
    No return values. Simply prints out the banner
*/
void printIntro() {
    printf("\n\n                /------------------------------------------------------------------\\\n");
    printf("                |                   Welcome to Hit Me, 31 style!                   |\n ");
    printf("               \\------------------------------------------------------------------/\n\n\n");
}
