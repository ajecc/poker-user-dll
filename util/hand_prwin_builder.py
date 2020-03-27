import copy
import functools
import pickle

@functools.total_ordering
class Card:
    def __init__(self, rank=None, suit=None):
        self.rank = rank
        self.suit = suit

    def __eq__(self, other):
        return self.rank == other.rank and self.suit == other.suit

    def __lt__(self, other):
        if self.rank < other.rank:
            return True
        elif self.rank > other.rank:
            return False
        return self.suit < other.suit

    def __str__(self):
        return str(self.rank) + '_' + str(self.suit)


def to_str(lst):
    ret = ''
    for elem in lst:
        elem_str = str(elem)
        if 'object at ' in elem_str:
            elem_str = to_str(elem) + '\n'
        ret += elem_str + ','
    return ret


def get_next_card(card):
    new_card = copy.deepcopy(card)
    if new_card.suit == 3:
        new_card.rank += 1
        new_card.suit = 0
    else:
        new_card.suit += 1
    if new_card.rank > 14:
        return None
    return new_card


def get_next_board(board):
    if len(board) == 0:
        return [Card(2, 0), Card(2, 0), Card(2, 0)]
    new_board = copy.deepcopy(board)
    for i in range(len(new_board)):
        new_board[i] = get_next_card(new_board[i])
        if new_board[i] is None:
            new_board[i] = Card(2, 0)
        else:
            return new_board
    if len(new_board) == 5:
        return None
    new_board.append(Card(2, 0))
    return new_board


def get_next_hand(hand):
    if hand[0] == Card(14, 3) and hand[1] == Card(14, 3):
        return None
    hand = copy.deepcopy(hand)
    hand[0] = get_next_card(hand[0])
    if hand[0] is None:
        hand[0] = Card(2, 0)
        hand[1] = get_next_card(hand[1])
    return hand


def get_hands():
    try:
        pickled_hands = open(r'D:\hands.pickle', 'rb')
        return pickle.load(pickled_hands)
    except Exception as e:
        print(str(e))
    used_hands = {}
    hands = []
    hand = [Card(2, 0), Card(2, 0)]
    hands.append(hand)
    while True:
        hand = get_next_hand(hand)
        if hand is None:
            break
        if to_str(sorted(hand)) in used_hands:
            continue
        used_hands[to_str(sorted(hand))] = 1
        if is_valid_game([], [], hand):
            hands.append(sorted(hand))
    hands = sorted(hands)
    pickle.dump(hands, open(r'D:\hands.pickle', 'wb'))
    return hands


def get_boards():
    print('Getting boards')
    try:
        print('Found pickled boards')
        pickled_boards = open(r'D:\boards.pickle', 'rb')
        return pickle.load(pickled_boards)
    except Exception as e:
        print(str(e))
    print('Generating the boards')
    used_boards = {}
    boards = []
    board = []
    boards.append(board)
    while True:
        board = get_next_board(board)
        if board is None:
            break
        if to_str(sorted(board)) in used_boards:
            continue
        used_boards[to_str(sorted(board))] = 1
        if is_valid_game([], [], board):
            boards.append(sorted(board))
    boards = sorted(boards)
    pickle.dump(boards, open(r'D:\boards.pickle', 'wb'))
    return boards


def get_extended_board(hand, board):
    board_aux = []
    board_aux.extend(hand)
    board_aux.extend(board)
    return board_aux


def has_straight_flush(board):
    suits = [[] for _ in range(4)]
    for hand in board:
        suits[hand.suit].append(hand.rank)
    for suit in suits:
        if len(suit) >= 5:
            consec = 1
            mmax = 0
            if 14 in suit:
                suit.append(1)
            suit = sorted(suit)
            for i in range(1, len(suit)):
                if suit[i] == suit[i - 1] + 1:
                    consec += 1
                    if consec >= 5:
                        mmax = suit[i]
                else:
                    consec = 1
            return [10, mmax]
    return 0


def has_quads(board):
    board = sorted(board)
    board = [elem.rank for elem in board]
    for i in range(len(board) - 3):
        if board[i] == board[i + 1] and board[i + 1] == board[i + 2] and board[i + 2] == board[i + 3]:
            ret = board[i]
            board = [elem for elem in board if elem != board[i]]
            return [9, ret, board[-1]]
    return 0


def has_fullhouse(board):
    trips = has_trips(board)
    if trips == 0:
        return 0
    board = [elem.rank for elem in board if elem.rank != trips[0]]
    board.extend([-1, -2, -3])
    pair = has_pair(board)
    if pair == 0:
        return 0
    return [8, trips[0], pair[0]]


def has_flush(board):
    suits = [[] for _ in range(4)]
    for hand in board:
        suits[hand.suit].append(hand.rank)
    for suit in suits:
        if len(suit) >= 5:
            suit = sorted(suit, reverse=True)
            return [7, suit[:5]]
    return 0


def has_straight(board):
    if 14 in board:
        board.append(1)
    board = sorted(board)
    consec = 1
    mmax = 0
    for i in range(1, len(board)):
        if board[i].rank == board[i - 1].rank + 1:
            consec += 1
            if consec >= 5:
                mmax = board[i].rank
        else:
            consec = 1
    if mmax == 0:
        return 0
    return [6, mmax]


def has_trips(board):
    board = sorted(board, reverse=True)
    board = [elem.rank for elem in board]
    for i in range(len(board) - 2):
        if board[i] == board[i + 1] and board[i + 1] == board[i + 2]:
            ret = board[i]
            board = [elem for elem in board if elem != board[i]]
            return [5, ret, board[0], board[1]]
    return 0

def has_twopair(board):
    first_pair = has_pair(board)
    if first_pair == 0:
        return 0
    board = [elem.rank for elem in board if elem.rank != first_pair[0]]
    board.extend([-1, -2])
    second_pair = has_pair(board)
    if second_pair == 0:
        return 0
    return [4, first_pair, second_pair, max(board)]


def has_pair(board):
    board = sorted(board, reverse=True)
    board = [elem.rank for elem in board]
    for i in range(len(board) - 2):
        if board[i] == board[i + 1]:
            ret = board[i]
            board = [elem for elem in board if elem != board[i]]
            return [3, ret, board[0], board[1], board[3]]
    return 0


def has_highcard(board):
    board = sorted(board, reverse=True)
    board = [elem.rank for elem in board]
    ret = [2]
    ret.extend(board[:5])
    return ret


def get_made_hand(hand, board):
    board_ext = get_extended_board(hand, board)

    made_hand = has_straight_flush(board_ext)
    if made_hand != 0:
        return made_hand 

    made_hand = has_quads(board_ext)
    if made_hand != 0:
        return made_hand 

    made_hand = has_fullhouse(board_ext)
    if made_hand != 0:
        return made_hand 

    made_hand = has_flush(board_ext)
    if made_hand != 0:
        return made_hand 

    made_hand = has_straight(board_ext)
    if made_hand != 0:
        return made_hand 

    made_hand = has_trips(board_ext)
    if made_hand != 0:
        return made_hand 

    made_hand = has_twopair(board_ext)
    if made_hand != 0:
        return made_hand 

    made_hand = has_pair(board_ext)
    if made_hand != 0:
        return made_hand 

    return has_highcard(board_ext)


def listcmp(lhs, rhs):
    for lhs_elem, rhs_elem in zip(lhs, rhs):
        if lhs_elem < rhs_elem:
            return -1
        elif lhs_elem > rhs_elem:
            return 1
    if len(lhs) < len(rhs):
        return -1
    elif len(lhs) > len(rhs):
        return 1
    else:
        return 0


def find_winner(lhs_hand, rhs_hand, board):
    lhs_made_hand = get_made_hand(lhs_hand, board)
    rhs_made_hand = get_made_hand(rhs_hand, board)
    return listcmp(lhs_made_hand, rhs_made_hand)


def is_valid_game(lhs, rhs, board):
    board_ext = get_extended_board(lhs, board)
    board_ext = get_extended_board(rhs, board_ext)
    board_ext = [str(elem) for elem in board_ext]
    board_ext_aux = list(set(board_ext))
    if len(board_ext_aux) != len(board_ext):
        return False
    return True


if __name__ == '__main__':
    #print(to_str(get_next_board([Card(13, 1), Card(13, 2), Card(14, 2)])))
    boards = get_boards()
    print(len(boards))
    hands = get_hands()
    #print(len(hands))
    #print(to_str(hands))
    #hands = get_hands()
    #for lhs in hands:
    #    for rhs in hands:
    #        for board in boards:
    #            if len(board) == 5 and valid_game(lhs, rhs, board):
    #                print(to_str(lhs) + ' ' + to_str(rhs) + ' ' + to_str(board) + ' ' + str(find_winner(lhs, rhs, board)))
