import requests

positions = ['UTG', 'HJ', 'CO', 'BTN', 'SB', 'BB']

# open
for pos in positions:
    file_name = f'{pos}_Open.html'
    response = requests.get(f'https://www.pokersnowie.com/pftapp/tables/{file_name}')
    text = f'https://www.pokersnowie.com/pftapp/tables/{file_name}\n'
    text += response.text
    if response.status_code != 404:
        open(f'html\\{file_name}', 'w').write(text)

# facing_raise
for hero_pos, i in zip(positions, range(len(positions))):
    for villain_pos in positions[:i]:
        file_name = f'{hero_pos}_{villain_pos}R3BB.html'
        response = requests.get(f'https://www.pokersnowie.com/pftapp/tables/{file_name}')
        text = f'https://www.pokersnowie.com/pftapp/tables/{file_name}\n'
        text += response.text
        if response.status_code != 404:
            open(f'html\\{hero_pos}_{villain_pos}_Facing_Raise.html', 'w').write(text)

# facing 3bet
for hero_pos, i in zip(positions, range(len(positions))):
    for villain_pos in positions[i + 1:]:
        file_name = f'{hero_pos}_{villain_pos}RPOT.html'
        response = requests.get(f'https://www.pokersnowie.com/pftapp/tables/{file_name}')
        text = f'https://www.pokersnowie.com/pftapp/tables/{file_name}\n'
        text += response.text
        if response.status_code != 404:
            open(f'html\\{hero_pos}_{villain_pos}_Facing_3bet.html', 'w').write(text)

# facing 4bet
for hero_pos, i in zip(positions, range(len(positions))):
    for villain_pos in positions[:i]:
        file_name = f'{hero_pos}_{villain_pos}RPOT.html'
        response = requests.get(f'https://www.pokersnowie.com/pftapp/tables/{file_name}')
        text = f'https://www.pokersnowie.com/pftapp/tables/{file_name}\n'
        text += response.text
        if response.status_code != 404:
            open(f'html\\{hero_pos}_{villain_pos}_Facing_4bet.html', 'w').write(text)
