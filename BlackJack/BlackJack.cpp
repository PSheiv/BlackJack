#include <iostream>
#include <string>
#include <ostream>
#include <random>
#include <algorithm>


using namespace std;


class GenericPlayer;


enum class Rank
{

    ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING,
    first = ACE,
    last = KING
};

enum class Suit
{
    CLUBS = 1,    //♣️
    DIAMONDS,    //♦️
    HEARTS,        //♥️
    SPADES,        //♠️
    first = CLUBS,
    last = SPADES
};

class Card
{
public:
    Card(Rank r = Rank::ACE, Suit s = Suit::SPADES, bool ifu = true);

    void SetValue(Rank den, Suit suit);

    Rank GetRank() { return rank; };
    Suit GetSuit() { return suit; };
    void Flip() { isFaceUp = !isFaceUp; };
    uint16_t GetValue() const;

    friend ostream& operator<<(ostream& os, const Card& aCard);

private:
    Rank rank;
    Suit suit;
    bool isFaceUp;


};
ostream& operator<<(ostream& os, const Card& aCard);

class Hand
{
public:
    Hand();
    virtual ~Hand();

    //Добавляет карту в руку. Добавляет указатель на объект типа Сard в вектор cards
    void Add(Card* pCard);
    //Очищает руку от карт. Удаляет все указатели из вектора cards, устраняя все связанные с ними объекты в куче
    void Clear();
    //Возвращает сумму очков карт руки, присваивая тузу значение 1 или 11 в зависимости от ситуации
    uint16_t GetTotal() const;

protected:
    vector<Card*> cards;
};

class Deck : public Hand
{
public:
    Deck();
    virtual ~Deck() = default;

    //Создает стандартную колоду из 52 карт
    void Populate();
    //Тасует карты
    void Shuffle();
    //Раздает в руку одну карту
    void Deal(Hand& aHand);
    //Раздает игроку дополнительные карты до тех пор, пока он может и хочет их получать
    void AdditionalCards(GenericPlayer& aGenerlcPlayer);

};


enum class Chip
{
    White = 1,
    Red = 5,
    Blue = 10,
    Green = 25,
    Black = 100,
    Magenta = 500,
    Orange = 1000,

    count = 7
};

class ChipsOnHand
{
public:

private:
    Chip chips[(int)Chip::count]{};
};

class GenericPlayer : public Hand
{
public:
    GenericPlayer(const string& name = "");
    virtual ~GenericPlayer() = default;

    // показывает, хочет ли игрок продолжать брать карты
    virtual bool IsHitting() const = 0;
    // возвращает значение, если у игрока перебор
    bool IsBoosted() const;
    // объявляет, что игрок имеет перебор
    void Bust() const;

    friend ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer);

protected:
    string name;

};

ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer);

class Player : public GenericPlayer
{
public:
    Player(const string& name = "");
    virtual ~Player() = default;

    //спрашивает у пользователя, нужна ли ему еще одна карта
    virtual bool IsHitting() const override;
    void Win() const;
    void Lose() const;
    //сыграл вничью
    void Push() const;
private:

};

class Dealer : public GenericPlayer
{
public:
    Dealer(const string& name = "Дилер");
    virtual ~Dealer() = default;

    //проыеряет, нужна ли диллеру еще одна карта
    virtual bool IsHitting() const override;
    void FlipFirstCard();
private:


};


class Game
{
public:
    Game(const vector<string>& names);
    ~Game() = default;

    void Play();

private:
    Deck deck;
    Dealer dealer;
    vector<Player> players;
};

Rank operator++(Rank& den) { return den = (Rank)(underlying_type<Rank>::type(den) + 1); };
Rank operator*(Rank den) { return den; };
Rank begin(Rank den) { return Rank::first; };
Rank end(Rank den)
{
    Rank de = Rank::last;
    return ++de;
};

Suit operator++(Suit& suit) { return suit = (Suit)(underlying_type<Suit>::type(suit) + 1); };
Suit operator*(Suit suit) { return suit; };
Suit begin(Suit suit) { return Suit::first; };
Suit end(Suit suit)
{
    Suit su = Suit::last;
    return ++su;
};


Card::Card(Rank r, Suit s, bool ifu) : suit(s), rank(r), isFaceUp(ifu)
{};

void Card::SetValue(Rank den, Suit suit)
{
    rank = den;
    this->suit = suit;
};

uint16_t Card::GetValue() const
{
    uint16_t val = 0;
    if (isFaceUp)
    {
        val = underlying_type<Rank>::type(rank);
        if (val > 10) val = 10;
    }
    return val;
};

Deck::Deck()
{
    cards.reserve(52);
    Populate();
}

void Deck::Populate()
{
    Clear();
    for (const auto s : Suit())
    {
        for (const auto r : Rank())
        {
            Add(new Card(r, s));
        }
    }
}

void Deck::Shuffle()
{
    auto rng = default_random_engine{};
    shuffle(cards.begin(), cards.end(), rng);

    // random_shuffle(cards_.begin(), cards_.end());

}

void Deck::Deal(Hand& aHand)
{
    if (!cards.empty())
    {
        aHand.Add(cards.back());
        cards.pop_back();
    }
    else
    {
        cout << "Нет карт. Невозможно дать карту.";
    }
}

void Deck::AdditionalCards(GenericPlayer& aGenerlcPlayer)
{
    cout << endl;
    while (!aGenerlcPlayer.IsBoosted() && aGenerlcPlayer.IsHitting())
    {
        Deal(aGenerlcPlayer);
        cout << aGenerlcPlayer << endl;

        if (aGenerlcPlayer.IsBoosted())
        {
            aGenerlcPlayer.Bust();
        }
    }
}

Game::Game(const vector<string>& names)
{
    for (auto& name : names)
    {
        players.push_back(Player(name));
    }

    srand(static_cast<uint32_t>(time(0)));
    deck.Populate();
    deck.Shuffle();
}

void Game::Play()
{
    // раздает каждому по две стартовые карты
    for (size_t i = 0; i < 2; i++)
    {
        for (auto& player : players)
        {
            deck.Deal(player);
        }
        deck.Deal(dealer);
    }

    // прячет первую карту дилера
    dealer.FlipFirstCard();

    // открывает руки всех игроков
    for (auto& player : players)
    {
        cout << player << endl;
    }
    cout << dealer << endl;

    // раздает игрокам дополнительные карты
    for (auto& player : players)
    {
        deck.AdditionalCards(player);
    }

    // показывает первую карту дилера
    dealer.FlipFirstCard();
    cout << endl << dealer;

    // раздает дилеру дополнительные карты
    deck.AdditionalCards(dealer);

    if (dealer.IsBoosted())
    {
        // все, кто остался в игре, побеждают
        for (auto& player : players)
        {
            if (!player.IsBoosted())
            {
                player.Win();
            }
        }
    }
    else
    {
        // сравнивает суммы очков всех оставшихся игроков с суммой очков дилера
        for (auto& player : players)
        {
            if (!player.IsBoosted())
            {
                if (player.GetTotal() > dealer.GetTotal())
                {
                    player.Win();
                }
                else if (player.GetTotal() < dealer.GetTotal())
                {
                    player.Lose();
                }
                else
                {
                    player.Push();
                }
            }
        }
    }

    // очищает руки всех игроков
    for (auto& player : players)
    {
        player.Clear();
    }
    dealer.Clear();
}

Hand::Hand()
{
    cards.reserve(7);
}

Hand::~Hand()
{
    Clear();
}

void Hand::Add(Card* pCard)
{
    cards.push_back(pCard);
}

void Hand::Clear()
{
    for (auto& cr : cards)
    {
        delete cr;
    }
    cards.clear();
}

uint16_t Hand::GetTotal() const
{
    // если карт в руке нет, возвращает значение 0
    if (cards.empty())
    {
        return 0;
    }
    //если первая карта имеет значение 0, то она лежит рубашкой вверх:
    // вернуть значение 0
    if (cards[0]->GetValue() == 0)
    {
        return 0;
    }
    // находит сумму очков всех карт, каждый туз дает 1 очко
    uint16_t total = 0;
    for (const auto& cr : cards)
    {
        total += cr->GetValue();
    }
    // определяет, держит ли рука туз
    bool containsAce = false;
    for (const auto& cr : cards)
    {
        if (cr->GetValue() == static_cast<uint16_t>(Rank::ACE)) containsAce = true;
    }
    // если рука держит туз и сумма довольно маленькая, туз дает 11 очков

    if (containsAce && total <= 11)
    {
        // добавляем только 10 очков, поскольку мы уже добавили
        // за каждый туз по одному очку
        total += 10;
    }
    return total;
}

GenericPlayer::GenericPlayer(const string& name) : name(name)
{}

bool GenericPlayer::IsBoosted() const
{
    return (GetTotal() > 21);
}
void GenericPlayer::Bust() const
{
    cout << name << " перебор" << endl;
}

Dealer::Dealer(const string& name) : GenericPlayer(name)
{}

bool Dealer::IsHitting() const
{
    return (GetTotal() <= 16);
}

void Dealer::FlipFirstCard()
{
    if (!(cards.empty()))
    {
        cards[0]->Flip();
    }
    else
    {
        cout << "Нет карт для переворота!\n";
    }
}

ostream& operator<<(ostream& os, const Card& aCard)
{
    const string RANKS[] = { "A", "2", "3", "4", "5", "6", "7", "8", "9","10", "J", "Q", "K" };
    // { "♣️", "♦️", "♥️", "♠️" };
    const char SUITS[] = { 5, 4, 3, 6 };
    if (aCard.isFaceUp)
    {
        os << RANKS[static_cast<uint16_t>(aCard.rank) - 1] << SUITS[static_cast<uint16_t>(aCard.suit) - 1];
    }
    else
    {
        os << "XX";
    }
    return os;
}

ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer)
{
    os << aGenericPlayer.name << ":\t";
    if (!aGenericPlayer.cards.empty())
    {
        if (aGenericPlayer.GetTotal() != 0)
        {
            cout << "(" << aGenericPlayer.GetTotal() << ")\t";
        }
        for (const auto& pCard : aGenericPlayer.cards)
        {
            os << *pCard << "\t";
        }

    }
    else
    {
        os << "<empty>";
    }
    return os;
}

Player::Player(const string& name) : GenericPlayer(name)
{}

bool Player::IsHitting() const
{
    cout << name << ", нужна еще карта? (Y/N): ";
    char response;
    cin >> response;
    return (response == 'y' || response == 'Y');
}

void Player::Win() const
{
    cout << name << " выиграл." << endl;
}

void Player::Lose() const
{
    cout << name << " проиграл." << endl;
}

void Player::Push() const
{
    cout << name << " сыграл вничью." << endl;
}



void Task345()
{
    cout << "Сыграем в Blackjack!" << endl << endl;
    int numPlayers = 0;
    while (numPlayers < 1 || numPlayers > 7)
    {

        cout << "На сколько игроков раздаем? (1 - 7): ";
        cin >> numPlayers;

    }
    vector<string> names;
    names.reserve(numPlayers);
    string name;
    for (int i = 0; i < numPlayers; ++i)
    {
        cout << "Введите имя " << i + 1 << " игрока: ";
        cin >> name;

        names.push_back(name);
    }
    cout << endl;

    Game aGame(names);
    char again = 'y';
    while (again != 'n' && again != 'N')
    {
        aGame.Play();
        cout << "\nХотите сыграть заново? (Y/N): ";
        cin >> again;
    }
}




int main()
{
    setlocale(LC_ALL, "RU");

    Task345();


}