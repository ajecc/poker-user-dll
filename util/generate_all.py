bb_check = open("BB_Check.csv", "w")
cards = ['2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A']
cards = cards[::-1]

for i in range(len(cards)):
    for j in range(i, len(cards)):
        card_0 = cards[i] + cards[j] + 'o'
        card_1 = cards[i] + cards[j] + 's'
        bb_check.write(card_0 + '\n')
        if i != j:
            bb_check.write(card_1 + '\n')
