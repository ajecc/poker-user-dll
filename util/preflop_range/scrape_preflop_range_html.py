import requests

positions = ['UTG', 'HJ', 'CO', 'BTN', 'SB', 'BB']

# open
for pos in positions:
    file_name = f'{pos}_Open.html'
    response = requests.get(f'https://www.pokersnowie.com/pftapp/tables/{file_name}')
    if response.status_code != 404:
        open(f'html\\{file_name}', 'w').write(response.text)

# facing_raise
for hero_pos, i in zip(positions, range(len(positions))):
    for villain_pos in positions[:i]:
        file_name = f'{hero_pos}_{villain_pos}R3BB.html'
        response = requests.get(f'https://www.pokersnowie.com/pftapp/tables/{file_name}')
        if response.status_code != 404:
            open(f'html\\{hero_pos}_{villain_pos}_Facing_Raise.html', 'w').write(response.text)

# facing 3bet
for hero_pos, i in zip(positions, range(len(positions))):
    for villain_pos in positions[i + 1:]:
        filen_name = f'{hero_pos}_{villain_pos}RPOT.html'
        response = requests.get(f'https://www.pokersnowie.com/pftapp/tables/{file_name}')
        if response.status_code != 404:
            open(f'html\\{hero_pos}_{villain_pos}_Facing_3bet.html', 'w').write(response.text)

# facing 4bet
for hero_pos, i in zip(positions, range(len(positions))):
    for villain_pos in positions[:i]:
        filen_name = f'{hero_pos}_{villain_pos}RPOT.html'
        response = requests.get(f'https://www.pokersnowie.com/pftapp/tables/{file_name}')
        if response.status_code != 404:
            open(f'html\\{hero_pos}_{villain_pos}_Facing_4bet.html', 'w').write(response.text)
